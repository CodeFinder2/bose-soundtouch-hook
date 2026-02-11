#ifndef LOG_H
#define LOG_H

#include "bose-hook/const.h"
#include <stddef.h>

typedef enum {
    LOG_ERROR = 0,
    LOG_WARN  = 1,
    LOG_INFO  = 2,
    LOG_DEBUG = 3
} log_level_t;

void log_set_level(log_level_t lvl);
log_level_t log_get_level(void);

/* printf-like */
void log_msg(log_level_t lvl, const char *fmt, ...);

#ifdef ENABLE_SEND_RECV_HOOKS
void log_traffic_data(int fd, const void *buf, size_t count, const char *direction);
#endif
#ifdef ENABLE_SSL_SEND_RECV_HOOKS
void log_ssl_traffic_data(const char *prefix, const void *buf, int num);
#endif

#endif /* LOG_H */
