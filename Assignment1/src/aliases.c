/**
 * @file aliases.c
 * @brief aliases.c
 * @author
 * @date 17-Sep-2019
 *
 */

/***************************************************************************
 * headers
 ***************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "aliases.h"
#include "log.h"

/***************************************************************************
 * Global variables
 ***************************************************************************/
extern bool tsh_debug;
static int total_aliases = 0; /* To know the total values stored */

/***************************************************************************
 * functions
 ***************************************************************************/
static void update_alias(aliases_hold_t *alias_ptr, char *new_evqi_cmd)
{
    strcpy(alias_ptr->alias_eqvi_cmd, new_evqi_cmd);
}

int update_duplicate_entry(char *alias_name, char *alias_eqvi_name, aliases_hold_t *alias_hold, int total_entries)
{
    int ret = 0, i = 0;
    aliases_hold_t *alias_ptr = alias_hold;

    for(i=0; i< total_entries; i++ ) {
        if(NULL == alias_hold->alias_name) {
            if(tsh_debug) {
                log_err("NO ENTRIES");
            }
            alias_ptr = NULL;
            return 1;
        }
        ret = strcmp(alias_name, alias_ptr->alias_name);
        if(ret == 0) {
            update_alias(alias_ptr, alias_eqvi_name);
            alias_ptr = NULL;
            return 0;
        }
        alias_ptr++;
    }
    alias_ptr = NULL;
    return 1;
}

int register_new_alias(char *alias_name, char *alias_eqvi_name, aliases_hold_t *alias_hold, int total_entries)
{
    aliases_hold_t *alias_ptr = alias_hold;

    (alias_ptr+total_aliases)->id = 1;
    strcpy((alias_ptr+total_aliases)->alias_name, alias_name);
    strcpy((alias_ptr+total_aliases)->alias_eqvi_cmd, alias_eqvi_name);
    total_aliases = (total_aliases + 1)  % (total_entries);

    alias_ptr = NULL;
    return 0;
}


void display_aliases(aliases_hold_t *alias_hold, int total_entries)
{
    int inc = 0;
    int i,j;
    bool found = false;
    aliases_hold_t *alias_ptr = alias_hold;

    if(alias_ptr->id != 0) {
        for(i=0;i<=2;i++) {
            for(j=0;j<=44;j++) {
                if(i == 0 || i == 2 ) {
                    printf("-");
                }
                else if(i == 1 && j == 1) {
                    printf("ALIAS_STORED  |  EQVIVALENT_ALIAS_COMMANDS  |");
                }
            }
            printf("\n");
        }
        found = true;
    }

    for(inc=0; (inc< total_entries) && (alias_ptr->id != 0) ; inc++ ) {
        //printf("\t%-10s\t%s\t%d\n",alias_ptr->alias_name, alias_ptr->alias_eqvi_cmd, alias_ptr->id);
        printf("\t%-10s\t%s\t\n",alias_ptr->alias_name, alias_ptr->alias_eqvi_cmd);
        alias_ptr++;
    }

    if(!found) {
        if(tsh_debug) {
            log_info("NO ENTRIES FOUND");
        }
    }
    else {
        for(j=0;j<=44;j++) {
            printf("-");
        }
        printf("\n\n");
    }
    alias_ptr = NULL;
}

int delete_alias_entry(char *alias_del, aliases_hold_t *alias_hold, int total_entries)
{
    bool found = false;
    int index = 0, i =0;
    aliases_hold_t *alias_ptr = alias_hold;

    if(0 == alias_ptr->id) {
        if(tsh_debug) {
            log_warn("No aliases stored to delete please create alias");
        }
        return 1;
    }

    for (i = 0; i < total_entries && found == false; i++) {
        if (!(strcmp(alias_del, alias_ptr->alias_name))) {
            found = true;
            index = i; /* Tells where the entry is found, helps to decrease the command */
        }
        alias_ptr++;
    }
    alias_ptr = alias_hold; /* Reinitialize the pointer */

    if (found == true) {
        int i;
        for (i = index; (i < total_entries -1) && ((alias_ptr+i)->id == 1); i++) {
            strcpy((alias_ptr+i)->alias_name, (alias_ptr+(i+1))->alias_name);
            strcpy((alias_ptr+i)->alias_eqvi_cmd, (alias_ptr+(i+1))->alias_eqvi_cmd);
            (alias_ptr+i)->id = 1;
        }
        (alias_ptr+(i-1))->id = 0;  /* To add zeros to the final array */
        total_aliases--;
        return 0;
    }
    if(tsh_debug) {
        log_warn("ALIAS ENTRY NOT FOUND");
    }
    return 1;

}

int check_cmd_is_alias(char *cmd, char *alias_eqvi_command, aliases_hold_t *alias_hold)
{
    int i;
    aliases_hold_t *alias_ptr = alias_hold;

    for(i=0; i<total_aliases; i++) {
        if(strcmp(cmd, alias_ptr->alias_name) == 0) {
            strcpy(alias_eqvi_command, alias_ptr->alias_eqvi_cmd);
            return 0;
        }
        alias_ptr++;
    }

    return 1;
}


int write_aliases_to_file(char *filename, aliases_hold_t *alias_hold)
{
    FILE *fptr;
    bool found= false;
    aliases_hold_t *alias_ptr = alias_hold;

    fptr = fopen(filename, "w");
    if (fptr == NULL) {
        if(tsh_debug) {
            log_err("Failed to create a file to store aliases!");
        }
      return 1;
    }

    while(alias_ptr->id) {
        fprintf(fptr, "%s %s\n", alias_ptr->alias_name, alias_ptr->alias_eqvi_cmd);
        alias_ptr++;
        found = true;
    }

    if(!found) {
        if(tsh_debug) {
            log_info("No data/aliases present to write into the file");
        }
        fclose(fptr);
        return 1;
    }
    alias_ptr = NULL;
    fclose(fptr);

    return 0;
}

int read_aliases_from_file(char *filename, aliases_hold_t *alias_hold, int total_entries)
{
    FILE *fptr;
    char alias_name[ALIAS_STR_LENGTH];
    char alias_eqvi_cmd[ALIAS_STR_LENGTH];
    aliases_hold_t *alias_ptr = alias_hold;

    fptr = fopen(filename, "r");
    if (fptr == NULL) {
        if(tsh_debug) {
            log_err("Failed to open a file to restore aliases, Maybe File not present!");
        }
      return 1;
    }
    printf("NEED TO IMPLIMENT, I AM JUST RESTORING THE VALUES FROM FILE\n");

    while(fscanf(fptr, "%s %[^\n]s", alias_name, alias_eqvi_cmd) == 2) {
        if(tsh_debug) {
            log_info("data read: %s %s",alias_name, alias_eqvi_cmd);
        }
        /* check for duplicate entry in array if found copy that to new array */
        /* if duplicate not found update the entry as it is to the new array */
        /* copy entire data from new array to old array */
        strcpy(alias_ptr->alias_name, alias_name);
        strcpy(alias_ptr->alias_eqvi_cmd, alias_eqvi_cmd);
        total_aliases ++;
        alias_ptr->id = 1;
        alias_ptr++;
    }

    fclose(fptr);
    return 0;
}
