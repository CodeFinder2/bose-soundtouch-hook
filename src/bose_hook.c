#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h> // for gethostbyname
#include <stdbool.h>
#include <string.h>

#include "bose-hook/config.h"
#include "bose-hook/log.h"
#include "bose-hook/const.h"
#include "bose-hook/utils.h"

// The following global variables must be outside of init() to persist for connect(), etc.
static struct in_addr redirect_ip;
static bool ip_resolved = false;
static bose_hook_config_t cfg;

__attribute__((constructor))
void init(void)
{
    // The following will ALWAYS be shown:
    log_msg(LOG_INFO, "Bose/SSL spy library/hook loaded successfully! Let's liberate this speaker from the cloud!");
    // Initialize and load config:
    if (config_load_file(&cfg, DEFAULT_CONFIG_PATH) < 0) {
        log_msg(LOG_ERROR, "Failed to load config, exiting.");
        exit(1);
    } else {
        log_set_level(cfg.log_level);
        log_msg(LOG_INFO, "Configuration loaded from %s.", DEFAULT_CONFIG_PATH);
    }
  #ifdef USE_DYNAMIC_REDIRECT
    // Resolve the target server IP:
    struct hostent *he = gethostbyname(cfg.target_server);
    if (he) {
        // We copy the address into the global variable
        redirect_ip = *((struct in_addr *)he->h_addr_list[0]);
        ip_resolved = true;
        // TODO: if the IP changes at runtime, this will fail. However, we cannot simply resolve it dynamically
        //       because it may cause recursion or other issues.

        log_msg(LOG_INFO, "Resolved %s to %s.", cfg.target_server, inet_ntoa(redirect_ip));
    } else {
        log_msg(LOG_ERROR, "Could not resolve %s, exiting.", cfg.target_server);
        exit(1);
    }
  #endif /* USE_DYNAMIC_REDIRECT */
}

#ifdef ENABLE_DNS_HOOK

int getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res)
{
    typedef int (*getaddrinfo_func_t)(const char *, const char *, const struct addrinfo *, struct addrinfo **);
    static getaddrinfo_func_t orig_getaddrinfo = NULL;
    if (!orig_getaddrinfo) {
        orig_getaddrinfo = (getaddrinfo_func_t)dlsym(RTLD_NEXT, "getaddrinfo");
    }

    // Call the original function first to get the result
    int result = orig_getaddrinfo(node, service, hints, res);

    // Only log if the lookup was successful and we have a hostname
    if (result == 0 && node != NULL) {
        
        // 1. Skip local/internal lookups immediately
        if (strcmp(node, "localhost") == 0 || 
            strcmp(node, "127.0.0.1") == 0 || 
            strcmp(node, cfg.target_server) == 0) {
            return result;
        }

        // 2. Extract the resolved IP address for the log
        char ip_buf[INET_ADDRSTRLEN] = "unknown";
        if (*res && (*res)->ai_family == AF_INET) {
            struct sockaddr_in *addr = (struct sockaddr_in *)(*res)->ai_addr;
            inet_ntop(AF_INET, &(addr->sin_addr), ip_buf, INET_ADDRSTRLEN);
        }

        // 3. Filter based on the resolved IP (skip if it's our target server's IP)
        // This is a safety net in case the box uses an IP instead of a name
        if (strcmp(ip_buf, inet_ntoa(redirect_ip)) != 0) {
            log_msg(LOG_INFO, "DNS Lookup intercepted: %s -> %s.", node, ip_buf);
        }
    }

    return result;
}
#endif /* ENABLE_DNS_HOOK */

#ifdef ENABLE_CONNECT_HOOK
static __thread bool in_connect_hook = false; // must be global

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    typedef int (*connect_func_t)(int, const struct sockaddr *, socklen_t);
    static connect_func_t orig_connect = NULL;
    if (!orig_connect) {
        orig_connect = (connect_func_t)dlsym(RTLD_NEXT, "connect");
    }

    // Prevent recursion
    if (in_connect_hook) {
        return orig_connect(sockfd, addr, addrlen);
    }
    in_connect_hook = true;

    if (addr->sa_family == AF_INET) { // IPv4 only (sufficient?)
        struct sockaddr_in *addr_in = (struct sockaddr_in *)addr;

        log_msg(LOG_INFO, "connect() to %s:%u", inet_ntoa(addr_in->sin_addr), ntohs(addr_in->sin_port));

      #ifdef ENABLE_REDIRECT_TO_FAKE_CLOUD
        // Check if destination is a public cloud IP
        if (!is_private_addr(addr_in->sin_addr, true)) {
            
            // 1. Safe copy of the original IP for logging
            // Using a local buffer to avoid inet_ntoa pointer clashing
            char old_ip_str[INET_ADDRSTRLEN];
            strncpy(old_ip_str, inet_ntoa(addr_in->sin_addr), INET_ADDRSTRLEN);

            // 2. Port Adjustment
            uint16_t original_port = ntohs(addr_in->sin_port);
            uint16_t new_port = (original_port == 443) ? cfg.ssl_target_port : cfg.default_target_port;
            addr_in->sin_port = htons(new_port);

            // 3. IP Redirection
          #ifdef USE_DYNAMIC_REDIRECT
            struct hostent *he = gethostbyname(cfg.target_server);
            if (he && he->h_addr_list[0]) {
                addr_in->sin_addr = *(struct in_addr *)he->h_addr_list[0];
            }
          #else
            if (!ip_resolved) {
                log_msg(LOG_WARN, "Resolving target server IP for %s in connect().", cfg.target_server);
                struct hostent *he = gethostbyname(cfg.target_server);
                if (he) {
                    // We copy the address into the global variable
                    redirect_ip = *((struct in_addr *)he->h_addr_list[0]);
                    ip_resolved = true;
                } else {
                    log_msg(LOG_ERROR, "Could not resolve %s, exiting.", cfg.target_server);
                    exit(1);
                }
            }
            addr_in->sin_addr = redirect_ip;
          #endif /* USE_DYNAMIC_REDIRECT */

            log_msg(LOG_INFO, "Redirecting cloud traffic: %s:%u -> %s:%u (%s)",
                    old_ip_str, original_port, 
                    cfg.target_server, new_port, 
                    inet_ntoa(addr_in->sin_addr));
        } else {
            // This traffic stays original
            log_msg(LOG_DEBUG, "Bypassing hook for IP %s (private).", inet_ntoa(addr_in->sin_addr));
        }
      #endif /* ENABLE_REDIRECT_TO_FAKE_CLOUD */
    }

    int result = orig_connect(sockfd, addr, addrlen);
    in_connect_hook = false;
    return result;
}
#endif /* ENABLE_CONNECT_HOOK */

#ifdef ENABLE_SSL_SEND_RECV_HOOKS
// 2. The SSL verify hook: ignore certificate warnings
// In OpenSSL, this function returns 0 (X509_V_OK) on success.
long SSL_get_verify_result(const void *ssl)
{
    log_msg(LOG_INFO, "SSL_get_verify_result() caught - bypassing SSL certificate verification.");
    return 0;
}

// Force the verify callback to always return 1 (Success)
void SSL_set_verify(void *ssl, int mode, int (*verify_callback)(int, void *))
{
    log_msg(LOG_INFO, "SSL_set_verify() caught - forcing success mode.");
    // We call the original but ignore the provided callback or mode if we want
    static void (*orig_SSL_set_verify)(void *, int, int (*)(int, void *)) = NULL;
    if (!orig_SSL_set_verify) {
        typedef void (*ssl_set_verify_func_t)(void *, int, int (*)(int, void *));
        orig_SSL_set_verify = (ssl_set_verify_func_t)dlsym(RTLD_NEXT, "SSL_set_verify");
    }
    
    // SSL_VERIFY_NONE = 0
    orig_SSL_set_verify(ssl, 0, NULL); 
}

// Also hook the context version
void SSL_CTX_set_verify(void *ctx, int mode, int (*verify_callback)(int, void *))
{
    log_msg(LOG_INFO, "SSL_CTX_set_verify() caught - forcing success mode.");
    static void (*orig_SSL_CTX_set_verify)(void *, int, int (*)(int, void *)) = NULL;
    if (!orig_SSL_CTX_set_verify) {
        typedef void (*ssl_ctx_set_verify_func_t)(void *, int, int (*)(int, void *));
        orig_SSL_CTX_set_verify = (ssl_ctx_set_verify_func_t)dlsym(RTLD_NEXT, "SSL_CTX_set_verify");
    }
    orig_SSL_CTX_set_verify(ctx, 0, NULL);
}

int SSL_write(void *ssl, const void *buf, int num)
{
    typedef int (*ssl_write_func_t)(void *, const void *, int);
    static ssl_write_func_t orig_SSL_write = NULL;
    if (!orig_SSL_write) {
        typedef int (*ssl_write_func_t)(void *, const void *, int);
        orig_SSL_write = (ssl_write_func_t)dlsym(RTLD_NEXT, "SSL_write");
    }
    log_msg(LOG_INFO, "Intercepting SSL_write (%d bytes).", num);
    log_ssl_traffic_data("SEND (SSL_write)", buf, num);
    return orig_SSL_write(ssl, buf, num);
}

int SSL_read(void *ssl, void *buf, int num)
{
    typedef int (*ssl_read_func_t)(void *, void *, int);
    static ssl_read_func_t orig_SSL_read = NULL;
    if (!orig_SSL_read) {
        typedef int (*ssl_read_func_t)(void *, void *, int);
        orig_SSL_read = (ssl_read_func_t)dlsym(RTLD_NEXT, "SSL_read");
    }
    log_msg(LOG_INFO, "Intercepting SSL_read (%d bytes).", num);
    int n = orig_SSL_read(ssl, buf, num);
    if (n > 0) {
        log_ssl_traffic_data("RECV (SSL_read)", buf, n);
    }
    return n;
}
#endif /* ENABLE_SSL_SEND_RECV_HOOKS */

#ifdef ENABLE_SEND_RECV_HOOKS
ssize_t write(int fd, const void *buf, size_t count)
{
    typedef ssize_t (*write_func_t)(int, const void *, size_t);
    static write_func_t orig_write = NULL;
    if (!orig_write) {
        orig_write = dlsym(RTLD_NEXT, "write");
    }
    
    log_traffic_data(fd, buf, count, "WRITE");
    return orig_write(fd, buf, count);
}

ssize_t send(int sockfd, const void *buf, size_t len, int flags)
{
    typedef ssize_t (*send_func_t)(int, const void *, size_t, int);
    static send_func_t orig_send = NULL;
    if (!orig_send) {
        orig_send = dlsym(RTLD_NEXT, "send");
    }

    log_traffic_data(sockfd, buf, len, "SEND");
    return orig_send(sockfd, buf, len, flags);
}

ssize_t read(int fd, void *buf, size_t count)
{
    typedef ssize_t (*read_func_t)(int, void *, size_t);
    static read_func_t orig_read = NULL;
    if (!orig_read) {
        orig_read = dlsym(RTLD_NEXT, "read");
    }

    ssize_t ret = orig_read(fd, buf, count); // read first, so data is in the buffer!
    if (ret > 0) {
        log_traffic_data(fd, buf, ret, "READ");
    }
    return ret;
}

ssize_t recv(int sockfd, void *buf, size_t len, int flags)
{
    typedef ssize_t (*recv_func_t)(int, void *, size_t, int);
    static recv_func_t orig_recv = NULL;
    if (!orig_recv) {
        orig_recv = dlsym(RTLD_NEXT, "recv");
    }

    ssize_t ret = orig_recv(sockfd, buf, len, flags);
    if (ret > 0) {
        log_traffic_data(sockfd, buf, ret, "RECV");
    }
    return ret;
}
#endif /* ENABLE_SEND_RECV_HOOKS */
