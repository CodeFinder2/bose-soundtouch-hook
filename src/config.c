#include "bose-hook/config.h"
#include "bose-hook/const.h"
#include "bose-hook/log.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

static char *trim(char *s)
{
    while (*s && isspace((unsigned char)*s)) s++;
    char *end = s + strlen(s);
    while (end > s && isspace((unsigned char)*(end-1))) *(--end) = 0;
    return s;
}

static void config_set(bose_hook_config_t *cfg, const char *key, const char *val)
{
    if (strcmp(key, "log_level") == 0) {
        cfg->log_level = atoi(val);
    } else if (strcmp(key, "target_server") == 0) {
        strncpy(cfg->target_server, val, sizeof(cfg->target_server)-1);
        cfg->target_server[sizeof(cfg->target_server)-1] = 0;
    } else if (strcmp(key, "ssl_target_port") == 0) {
        cfg->ssl_target_port = atoi(val);
    } else if (strcmp(key, "default_target_port") == 0) {
        cfg->default_target_port = atoi(val);
    }
}

static int check_required_fields(bose_hook_config_t *cfg)
{
    if (cfg->target_server[0] == 0) {
        log_msg(LOG_ERROR, "\"target_server\" not set in config file.");
        return -1;
    }
    return 0;
}

void config_init(bose_hook_config_t *cfg)
{
    cfg->log_level = LOG_INFO;
    cfg->target_server[0] = 0;
    cfg->ssl_target_port = DEFAULT_SSL_TARGET_PORT;
    cfg->default_target_port = DEFAULT_TARGET_PORT;
}

int config_load_file(bose_hook_config_t *cfg, const char *path)
{
    config_init(cfg);
    FILE *f = fopen(path, "r");
    if (!f) {
        log_msg(LOG_ERROR, "could not open config file %s", path);
        return -1;
    }

    char line[512];
    while (fgets(line, sizeof(line), f)) {
        char *p = trim(line);
        if (*p == 0 || *p == '#') continue;

        char *eq = strchr(p, '=');
        if (!eq) continue;

        *eq = 0;
        char *key = trim(p);
        char *val = trim(eq + 1);

        config_set(cfg, key, val);
    }

    fclose(f);
    return check_required_fields(cfg);
}
