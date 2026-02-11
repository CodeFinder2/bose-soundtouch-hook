#!/bin/sh
set -e

if [ -z "${TARGET_DEVICE:-}" ]; then
    echo "error: TARGET_DEVICE is not set" >&2
    exit 1
fi

if [ -z "${OUTPUT_FILE:-}" ]; then
    echo "error: OUTPUT_FILE is not set" >&2
    exit 1
fi

echo "Building $OUTPUT_FILE for Bose SoundTouch speaker $TARGET_DEVICE... "

gcc -shared -fPIC -ldl -O2 \
    -Werror -Wall -ansi -std=gnu99 \
    -o $OUTPUT_FILE \
    -Iinclude \
    src/bose_hook.c \
    src/utils.c \
    src/log.c \
    src/config.c \
    src/file_hooks.c
echo "Build complete. Transferring $OUTPUT_FILE to $TARGET_DEVICE at /tmp/$OUTPUT_FILE... "
scp $OUTPUT_FILE root@$TARGET_DEVICE:/tmp/$OUTPUT_FILE