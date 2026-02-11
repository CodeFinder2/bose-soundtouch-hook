#!/bin/sh

# Configuration
TARGET_BIN="/opt/Bose/STSCertified"
BIN_NAME=$(basename "$TARGET_BIN")
REAL_BIN_COPY="/tmp/${BIN_NAME}_real"  # The "real" copy to execute
WRAPP_BIN="/tmp/${BIN_NAME}_wrapper"
HOOK_LIB="/tmp/libbose_hook.so"
LOG_FILE="/tmp/hook_${BIN_NAME}.log"

if [ ! -f "$HOOK_LIB" ]; then
    echo "error: file '$HOOK_LIB' not found." >&2
    exit 1
fi

log_message() {
    echo "$(date '+%Y-%m-%d %H:%M:%S') - $1" >> "$LOG_FILE"
    echo "$1"
}

get_pid() {
    # We now search for the copy in /tmp
    ps | grep "$REAL_BIN_COPY" | grep -v grep | awk '{print $1}' | head -n 1
}

uninstall_hook() {
    log_message "[!] EMERGENCY UNINSTALL: Removing mount and cleaning up..."
    umount "$TARGET_BIN" 2>/dev/null
    rm -f "$WRAPP_BIN"
    rm -f "$REAL_BIN_COPY"
    killall "$BIN_NAME" 2>/dev/null
    killall "$(basename $REAL_BIN_COPY)" 2>/dev/null
    log_message "[!] Cleanup finished. System should be back to original state."
}

install_hook() {
    log_message "[+] Installing hook ..."

    if [ ! -f "$TARGET_BIN" ]; then
        log_message "[-] Error: Binary $TARGET_BIN not found!"
        exit 1
    fi

    if [ ! -f "$HOOK_LIB" ]; then
        log_message "[-] Error: Library $HOOK_LIB not found!"
        exit 1
    fi

    cp "$TARGET_BIN" "$REAL_BIN_COPY"
    chmod +x "$REAL_BIN_COPY"

    # Create wrapper script
    cat << EOF > "$WRAPP_BIN"
#!/bin/sh
export LD_PRELOAD="$HOOK_LIB"
log_message "[*] Starting LD_PRELOAD hook for $HOOK_LIB..."
exec "$REAL_BIN_COPY" "\$@" >> "$LOG_FILE" 2>&1
EOF
    chmod +x "$WRAPP_BIN"

    # Mount-bind the wrapper to the target location
    mount --bind "$WRAPP_BIN" "$TARGET_BIN"
    log_message "[+] Mount-bind $WRAPP_BIN -> $TARGET_BIN applied."

    # Kill the process so it restarts with the hook
    killall "$BIN_NAME"
    log_message "[+] Process killed. Waiting for Shepherd to restart it..."
    
    # Wait a bit to ensure the process has started
    sleep 3
    INITIAL_PID=$(get_pid)

    if [ -z "$INITIAL_PID" ]; then
        log_message "[!!!] CRASH: Process did not start at all!"
        uninstall_hook
        exit 1
    fi

    log_message "[+] Process started with PID: $INITIAL_PID. Monitoring stability..."

    # Watchdog phase: check for 3 seconds
    COUNT=0
    while [ $COUNT -lt 3 ]; do
        sleep 1
        CURRENT_PID=$(get_pid)

        if [ -z "$CURRENT_PID" ]; then
            log_message "[!!!] CRASH: Process vanished!"
            uninstall_hook
            exit 1
        fi

        if [ "$CURRENT_PID" != "$INITIAL_PID" ]; then
            log_message "[!!!] CRASH-LOOP DETECTED: PID changed from $INITIAL_PID to $CURRENT_PID! See $LOG_FILE for details."
            uninstall_hook
            exit 1
        fi

        COUNT=$((COUNT + 1))
        log_message "[.] Stability check ($COUNT/3s) - PID $CURRENT_PID seems healthy."
    done

    log_message "[+] SUCCESS: Hook is stable and running."
}

case "$1" in
    install|i)
        install_hook
        ;;
    uninstall|u)
        uninstall_hook
        ;;
    *)
        echo "Usage: $0 {install|i|uninstall|u}"
        exit 1
        ;;
esac
