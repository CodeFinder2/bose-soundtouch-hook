#ifndef BOSE_HOOK_CONFIG_H
#define BOSE_HOOK_CONFIG_H

#include "bose-hook/log.h"

typedef struct {
    log_level_t log_level;    /* current logging level, see log.h */
    char target_server[256];  /* your instance of the Bose fake server */
    int ssl_target_port; /* target port for SSL connections */
    int default_target_port; /* target port for non-SSL connections */
} bose_hook_config_t;

void config_init(bose_hook_config_t *cfg);
int  config_load_file(bose_hook_config_t *cfg, const char *path);

#endif /* BOSE_HOOK_CONFIG_H */
