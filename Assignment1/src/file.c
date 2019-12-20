/**
 * @file file.c
 * @brief file.c
 * @author
 * @date 11-Sep-2019
 *
 */

/***************************************************************************
 * headers
 ***************************************************************************/
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <string.h>

#include "file.h"
#include "iniparser.h"
#include "dictionary.h"

#include "log.h"

/***************************************************************************
 * Global variables
 ***************************************************************************/
extern bool tsh_debug;

/***************************************************************************
 * functions
 ***************************************************************************/
int store_config(char *shell_config_file, char *type, char *data)
{
    dictionary  *ini;
    char val[24], str[32];
    FILE *fp;

    ini = iniparser_load(shell_config_file);
    if (NULL == ini) {
        if(tsh_debug) {
            log_err("cannot parse file: %s", shell_config_file);
        }
        return 1 ;
    }

    snprintf(str, 32, "Shell:%s",type);

    if( 0 != iniparser_set(ini, (const char*)str, data)) {
        if(tsh_debug) {
            log_err("failed to set new value");
        }
        return 1;
    }

    if(!(fp = fopen(shell_config_file, "w"))){
        if(tsh_debug) {
            log_err("File open error %s",shell_config_file);
        }
        return 1;
    }

    iniparser_dump_ini(ini, fp);
    iniparser_freedict(ini);
    fclose(fp);

    return 0;
}

static int parse_ini_file(shell_info_t *shell_info, char *shell_config_file)
{
    dictionary  *ini;
    const char* read_string;

    ini = iniparser_load(shell_config_file);
    if (NULL == ini) {
        if(tsh_debug) {
            log_err("cannot parse file: %s", shell_config_file);
        }
        return 1 ;
    }

    read_string = iniparser_getstring(ini, "Shell:Shellname", NULL);
    strcpy(shell_info->shell_name, read_string);

    read_string = iniparser_getstring(ini, "Shell:Terminator", NULL);
    strcpy(shell_info->terminator, read_string);

    iniparser_freedict(ini);

    return 0;
}

static void save_default_config(FILE *ini)
{
    fprintf(ini,
    "#\n"
    "# This is shell config ini file\n"
    "#\n"
    "\n"
    "[Shell]\n"
    "\n"
    "Shellname       = toyshell\n"
    "Terminator      = >\n"
    "\n");

}

int restore_config(shell_info_t *shell_info, char* shell_config_file)
{
    int ret;
    if (0 != access(shell_config_file, F_OK)) {
        FILE *ini;
        if ( NULL == (ini=fopen(shell_config_file, "w"))) {
            if(tsh_debug)
                log_err("iniparser: cannot create %s",shell_config_file);
            return 1;
        }
        save_default_config(ini);
        fclose(ini);
    }

    if (0 != parse_ini_file(shell_info, shell_config_file)) {
        if(tsh_debug)
            log_err("failed to parse shell config file");
        return 1;
    }

    return 0;
}

