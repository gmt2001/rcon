#include "config.h"
#include "rcon.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <glib.h>

#define CONFIG_KEY_HOSTNAME "hostname"
/* It could be either a port number or a service from /etc/services.
 * But it is more intuitive if it is called "port"
 */
#define CONFIG_KEY_SERVICE  "port"
#define CONFIG_KEY_PASSWORD "password"
#define CONFIG_KEY_MINECRAFT "minecraft"

static GKeyFile *config = NULL;

int config_load(char const *filename, bool isDefault)
{
    GError *error = NULL;

    config_free();

    config = g_key_file_new();
    if (config == NULL) {
        return -1;
    }

    if (!g_key_file_load_from_file(config, filename, G_KEY_FILE_NONE, &error)) {
        if (!isDefault) {
            fprintf(stderr, "Failed to load configuration file: %s: %s\n",
                filename, error->message
            );
        }
        g_clear_error(&error);
        config_free();
        return -2;
    }

    return 0;
}

void config_free(void)
{
    if (config) {
        g_key_file_free(config);
        config = NULL;
    }
}

int config_host_data(char const *name, char **hostname,
                     char **service, char **passwd,
                     bool *minecraft)
{
    gchar *h = NULL, *s = NULL, *p = NULL;
    gboolean mc = FALSE;

    return_if_true(config == NULL, -1);

    if (!g_key_file_has_group(config, name)) {
        return -2;
    }

    h = g_key_file_get_string(config, name, CONFIG_KEY_HOSTNAME, NULL);
    if (h == NULL) {
        return -3;
    }

    s = g_key_file_get_string(config, name, CONFIG_KEY_SERVICE, NULL);
    if (s == NULL) {
        g_free(h);
        return -3;
    }

    p = g_key_file_get_string(config, name, CONFIG_KEY_PASSWORD, NULL);
    mc = g_key_file_get_boolean(config, name, CONFIG_KEY_MINECRAFT, NULL);

    if (hostname) {
        *hostname = strdup(h);
    }

    if (service) {
        *service = strdup(s);
    }

    if (passwd && p) {
        *passwd = strdup(p);
    }

    if (minecraft) {
        *minecraft = mc;
    }

    g_free(h);
    g_free(s);
    g_free(p);

    return 0;
}
