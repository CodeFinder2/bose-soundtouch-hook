#define _GNU_SOURCE
#include "bose-hook/log.h"
#include <time.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h> // for getpid()
#include <stdio.h>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

static log_level_t current_level = LOG_INFO;

static const char *level_str(log_level_t lvl)
{
    switch (lvl) {
        case LOG_ERROR: return "ERROR";
        case LOG_WARN:  return "WARN";
        case LOG_INFO:  return "INFO";
        case LOG_DEBUG: return "DEBUG";
        default:        return "?";
    }
}

void log_set_level(log_level_t lvl)
{
    current_level = lvl;
}

log_level_t log_get_level(void)
{
    return current_level;
}

/**
 * Log a message with the given log level.
 * @param lvl The log level of the message
 * @param fmt The printf-style format string
 * @param ... Additional arguments for the format string
 */
void log_msg(log_level_t lvl, const char *fmt, ...)
{
    if (lvl > current_level)
        return;

    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    char ts[32];
    strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", t);

    va_list args;
    va_start(args, fmt);

    const char prefix[] = "bose_hook";
    fprintf(stderr, "[%s] [%-5s] [PID:%-5d] [%-10s] ", 
        ts,             // Timestamp
        level_str(lvl), // Log Level (e.g., INFO, DEBUG)
        getpid(),       // Process ID
        prefix          // Prefix (e.g., bose_hook)
    );
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");

    va_end(args);
}

#ifdef ENABLE_SSL_SEND_RECV_HOOKS
/**
 * Log decrypted (SSL) traffic data to a file.
 * @param prefix A string prefix to indicate the direction (e.g., "SEND" or "RECV")
 * @param buf    Pointer to the data buffer
 * @param num    Number of bytes in the buffer
 */
void log_ssl_traffic_data(const char *prefix, const void *buf, int num)
{
    FILE *f = fopen(SSL_TRAFFIC_LOGFILE_PATH, "a");
    if (f) {
        fprintf(f, "\n--- %s (%d bytes) ---\n", prefix, num);
        fwrite(buf, 1, num, f);
        fprintf(f, "\n-------------------\n");
        fclose(f);
    } else {
        log_msg(LOG_ERROR, "Unable to create / append to logfile %s!", SSL_TRAFFIC_LOGFILE_PATH);
    }
}
#endif /* ENABLE_SSL_SEND_RECV_HOOKS */

#ifdef ENABLE_SEND_RECV_HOOKS
void log_traffic_data(int fd, const void *buf, size_t count, const char *direction)
{
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    // Check if the fd is an IPv4 socket
    if (getpeername(fd, (struct sockaddr *)&addr, &addr_len) == 0) {
        if (addr.sin_family == AF_INET) {
            char ip_str[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &addr.sin_addr, ip_str, INET_ADDRSTRLEN);
            int port = ntohs(addr.sin_port);

            // FILTERS:

            // Optional: Roughly filter out HTTPS/SSL traffic (ignore port 443)
            if (port == 443) {
                return;
            }
            // Optional: Filter out DNS traffic (port 53)
            if (port == 53) {
                return;
            }
            // Optional: Filter out XML traffic (simple check for "<?xml" at start), e.g., XML/UPnP
            //if (count > 5 && memcmp(buf, "<?xml", 5) == 0) {
            //    return;
            //}
            // Optional: Filter out known UPnP traffic (simple check for "urn:schemas-upnp-org" marker)
            const char *upnp_marker = "urn:schemas-upnp-org";
            if (memmem(buf, count, upnp_marker, strlen(upnp_marker)) != NULL) {
                return;
            }

            FILE *f = fopen(TRAFFIC_LOGFILE_PATH, "a");
            if (f) {
                fprintf(f, "\n--- %s (%d bytes, %s:%d) ---\n", direction, (int)count, ip_str, port);
                fwrite(buf, 1, count, f);
                fprintf(f, "\n-------------------\n");
                fclose(f);
            } else {
                log_msg(LOG_ERROR, "Unable to create / append to logfile %s!", TRAFFIC_LOGFILE_PATH);
            }

            //log_msg(LOG_INFO, "Intercepting %s %zu bytes to/from %s:%d", direction, count, ip_str, port);
            
            // // Only output if it looks like printable text (simple check)
            // const unsigned char *data = buf;
            // if (count > 0 && data[0] >= 32 && data[0] <= 126) {
            //     fwrite(buf, 1, count, stderr);
            //     fprintf(stderr, "\n------------------------------------------\n");
            // }
        }
    }
}
#endif /* ENABLE_SEND_RECV_HOOKS */
