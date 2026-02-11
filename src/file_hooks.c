#define _GNU_SOURCE
#include "bose-hook/const.h"

#ifdef ENABLE_FILE_HOOKS

#include "bose-hook/log.h"
#include <fcntl.h> // for open flags
#include <stdarg.h>
#include <dlfcn.h>
#include <string.h>
#include <stdio.h>

typedef int (*open_func_t)(const char *, int, ...);
typedef FILE* (*fopen_func_t)(const char *, const char *);

// Hook for low-level open()
int open(const char *pathname, int flags, ...)
{
    static open_func_t orig_open = NULL;
    if (!orig_open) {
        orig_open = (open_func_t)dlsym(RTLD_NEXT, "open");
    }

    mode_t mode = 0;
    // Handle variable arguments: mode is only present if O_CREAT is set.
    // We use a safe check for O_TMPFILE which might not be defined on older systems.
    int needs_mode = (flags & O_CREAT);
#ifdef O_TMPFILE
    if (flags & O_TMPFILE) needs_mode = 1;
#endif

    if (needs_mode) {
        va_list args;
        va_start(args, flags);
        mode = va_arg(args, mode_t);
        va_end(args);
    }

    // Log file access (filtering system noise)
    if (pathname && strncmp(pathname, "/proc", 5) != 0 && 
        strncmp(pathname, "/dev", 4) != 0 && 
        strncmp(pathname, "/lib", 4) != 0) {
        
        const char *m_str = (flags & O_RDWR) ? "RW" : (flags & O_WRONLY) ? "W" : "R";
        log_msg(LOG_INFO, "File Open: %s [%s]", pathname, m_str);
    }

    // Forward to original function with or without mode
    if (needs_mode) {
        return orig_open(pathname, flags, mode);
    } else {
        return orig_open(pathname, flags);
    }
}

// Hook for high-level fopen()
FILE* fopen(const char *pathname, const char *mode)
{
    static fopen_func_t orig_fopen = NULL;
    if (!orig_fopen) {
        orig_fopen = (fopen_func_t)dlsym(RTLD_NEXT, "fopen");
    }

    if (!pathname || strncmp(pathname, "/proc", 5) == 0 ||
        strncmp(pathname, "/dev", 4) == 0 ||
        strncmp(pathname, "/lib", 4) == 0 ||
        strncmp(pathname, "/etc/hosts", 11) == 0 ||
        strncmp(pathname, DEFAULT_CONFIG_PATH, strlen(DEFAULT_CONFIG_PATH)) == 0) {
        return orig_fopen(pathname, mode);
    }

  #ifdef ENABLE_SSL_SEND_RECV_HOOKS
    // Special case for SSL traffic log file
    if (strcmp(pathname, SSL_TRAFFIC_LOGFILE_PATH) == 0) {
        return orig_fopen(pathname, mode);
    }
  #endif /* ENABLE_SSL_SEND_RECV_HOOKS */

  #ifdef ENABLE_SEND_RECV_HOOKS
    // Special case for general traffic log file
    if (strcmp(pathname, TRAFFIC_LOGFILE_PATH) == 0) {
        return orig_fopen(pathname, mode);
    }
  #endif /* ENABLE_SEND_RECV_HOOKS */
        
    log_msg(LOG_INFO, "File open (stream): %s [%s]", pathname, mode);

    return orig_fopen(pathname, mode);
}

#endif /* ENABLE_FILE_HOOKS */