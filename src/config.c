#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int load_config(const char* path, AppConfig* cfg) {
    if (path == NULL || cfg == NULL) {
        return -1;
    }
    memset(cfg, 0, sizeof(AppConfig));
    FILE* f = fopen(path, "r");
    if (f == NULL) {
        return -1;
    }
    char line[64];
    char key[64], value[128];

    while (fgets(line, sizeof(line), f) != NULL) {
        if (sscanf(line, "%63[^=]=%127s", key, value) == 2) {
            if (strcmp(key, "threshold") == 0) {
                cfg->threshold = atoi(value);
            } else if (strcmp(key, "log_path") == 0) {
                snprintf(cfg->log_path, sizeof(cfg->log_path), "%s", value);
            } else if (strcmp(key, "enable_threads") == 0) {
                cfg->enable_threads = atoi(value);
            } else if (strcmp(key, "include_env") == 0) {
                char tmp[64];
                const char* resultado = getenv(value);
                if (resultado == NULL) {
                    fclose(f);
                    return -1;
                } 
                strncpy(tmp, resultado, sizeof(tmp));
                strncat(cfg->log_path, "/", sizeof(cfg->log_path) - strlen(cfg->log_path));
                strncat(cfg->log_path, tmp, sizeof(cfg->log_path) - strlen(cfg->log_path));
            }
        }
    }
    fclose(f);
    return 0;
}

int config_crc_view_serialize(const AppConfig* cfg, void* out_buf, size_t out_size) { //Función nunca usada
    if ((cfg == NULL) || (out_buf == NULL) || (out_size < sizeof(ConfigCrcView))) {
        return -1;
    }
    ConfigCrcView v;
    memset(&v, 0, sizeof v);
    v.threshold      = (int32_t)cfg->threshold;
    v.enable_threads = (int32_t)cfg->enable_threads;
    size_t len = strlen(cfg->log_path);
    strncpy(v.log_path, cfg->log_path, len);
    (void)memcpy(out_buf, &v, sizeof v);
    return (int)sizeof v;
}
