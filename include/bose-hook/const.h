#ifndef CONST_H
#define CONST_H

/* Run-time config file path to config file on the device (must be writable) */
#define DEFAULT_CONFIG_PATH         "/tmp/bose_hook.conf"
/* Logfile for intercepted traffic on the device (must be writable) */
#define TRAFFIC_LOGFILE_PATH        "/tmp/bose_traffic.log"
/* Logfile for intercepted SSL traffic on the device (must be writable) */
#define SSL_TRAFFIC_LOGFILE_PATH    "/tmp/bose_ssl_traffic.log"
/* Default SSL target port on custom (Python) server */
#define DEFAULT_SSL_TARGET_PORT     8443
/* Default target port on custom (Python) server to sniff all other traffic (non SSL) */
#define DEFAULT_TARGET_PORT         8080

/* Compile-time option to enable dynamic IP resolution on each connect() */
#define USE_DYNAMIC_REDIRECT
/* Compile-time option to enable file operation hooks */
#define ENABLE_FILE_HOOKS

/* Compile-time option to enable SSL hooks */
#define ENABLE_SSL_SEND_RECV_HOOKS

/* Compile-time option to enable connect() hook */
#define ENABLE_CONNECT_HOOK

/* Compile-time option to enable DNS hooks */
#define ENABLE_DNS_HOOK

/* Compile-time option to enable redirection of all non-private IPs to fake cloud */
//#define ENABLE_REDIRECT_TO_FAKE_CLOUD

/* Compile-time option to enable send()/recv() hooks */
#define ENABLE_SEND_RECV_HOOKS

/* Compile-time/consistency checks go here: */
#if defined ENABLE_REDIRECT_TO_FAKE_CLOUD && !defined ENABLE_CONNECT_HOOK
#error "ENABLE_REDIRECT_TO_FAKE_CLOUD requires ENABLE_CONNECT_HOOK to be enabled."
#endif

#endif /* CONST_H */
