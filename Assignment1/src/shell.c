/**
 * @file shell.c
 * @brief shell.c
 * @author
 * @date 06-Sep-2019
 *
 */

/********************************************************************
 * Headers
 *******************************************************************/

#include <stdio.h>
#include <error.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/types.h>

#include "shell.h"
#include "file.h"
#include "aliases.h"

#include "history.h"
#include "log.h"

/********************************************************************
 * Global Variables
 *******************************************************************/
bool tsh_debug;
static bool exit_signal;

static char *hist[STORE_TOTAL_HISTORY];
static int current = 0;
static int history_count = 0;
static char *preserve_string = NULL;
static bool re_execution_command = false;
static aliases_hold_t alias_hold[STORE_TOTAL_ALIASES];

shell_info_t s_info = {
    .terminator = ">",
    .shell_name = "toyshell"
};

/********************************************************************
 * Decelerations
 *******************************************************************/

/* List of commands registered */
command_t commands[] = {
        {"stop",            cmd_stop,               "NILL",             "exit a command shell"},
        {"setshellname",    cmd_setshellname,       "string",           "To rename the shell"},
        {"setterminator",   cmd_setterminator,      "string",           "set the terminator"},
        {"history",         cmd_history,            "NILL",             "display the history of current session"},
        {"clear_history",   cmd_clear_history,      "NILL",             "clear the history of current session"},
        {"!",               cmd_history_execute,    "int",              "re-execute the history command"},
        {"newname",         cmd_newname,            "strings",          "store the new aliases"},
        {"newnames",        cmd_display_newname,    "NILL",             "display the aliases stored"},
        {"savenewnames",    cmd_store_newname,      "NILL",             "save the aliases to a file"},
        {"readnewnames",    cmd_restore_newname,    "NILL",             "restore the aliases from a file"},
        {"help",            cmd_help,               "NILL",             "display the command supported"}
};

/********************************************************************
 * Function Definitions
 *******************************************************************/

void display_warning()
{
    printf("\tTHIS SHELL HAS A ISSUES EXECUTING ALIASES, STORED IN HISTORY, i.e '!' \n");
    printf("\tIT OVERWRITES ALL THE ALIASES WHEN readnewnames COMMAND IS EXECUTED\n");
    printf("\tEXECUTE help FOR THE LIST OF COMMANDS SUPPORTED BY %s\n",s_info.shell_name);

}

static bool cmd_store_newname(char **token)
{
    if(NULL == *(token+1)) {
        printf("please specify the filename where aliases should be stored\n");
        return false;
    }

    if (write_aliases_to_file(*(token+1), alias_hold) != 0) {
        printf("Failed to write aliases to file\n");
        return false;
    }

    return true;
}

static bool cmd_restore_newname(char **token)
{
    if (NULL == *(token+1)) {
        printf("please specify the filename from where aliases should be restored");
        return false;
    }

    if (read_aliases_from_file(*(token+1), alias_hold, STORE_TOTAL_ALIASES) != 0 ) {
        printf("Failed to restore the aliases from file\n");
        return false;
    }

    return true;
}

static bool cmd_newname(char** token)
{
    int ret;
    if(NULL == *(token+1) && NULL == *(token+2)) {
        printf("Enter proper alias substitution\n");
        return false;
    }

    if(NULL == *(token+2) && *(token+1)) {
        if (delete_alias_entry(*(token+1), alias_hold, STORE_TOTAL_ALIASES) != 0 ) {
             printf("FAILED TO DELETE THE ENTRY\n");
             return false;
        }
        return true;
    }

    ret = update_duplicate_entry(*(token+1), *(token+2), alias_hold, STORE_TOTAL_ALIASES);
    if(!ret) {
        printf("Updated Duplicate Entry\n");
    }
    else {
        register_new_alias(*(token+1), *(token+2), alias_hold, STORE_TOTAL_ALIASES);
        printf("Added new alias to list\n");
    }

    return false;
}

static bool cmd_display_newname(char** token)
{
    aliases_hold_t *alias_ptr = NULL;
    alias_ptr = alias_hold;
    display_aliases(alias_hold, STORE_TOTAL_ALIASES);
    return true;
}

static bool cmd_history_execute(char** token)
{
    char *history_cmd;
    int history_num;
    int final_val;

    /* check whether it is alphabet */
    if(isalpha(*(*(token+1)))) {
        printf("Entered value is a character Enter proper value\n");
        return false;
    }

    /* converting alphabet to int */
    history_num = atoi(*(token+1));

    if(tsh_debug) {
        printf("history num is %d\n",history_num);
    }

    if (history_num > history_count || 0 == (history_count -1) || history_num <= 0) {
        printf("%s: ! %d: event not found \n",s_info.shell_name, history_num);
        return false;
    }

    if (history_num == 1 && (STORE_TOTAL_HISTORY + 1) == history_count) {
         history_cmd = preserve_string;
    }
    else if((STORE_TOTAL_HISTORY + 1) > history_count) {
        history_cmd = hist[history_num -1];
    }
    else {
        /* IMPLIMENT TO GET THE CURRENT HISTORY COUNT */

        final_val = (history_num -1) + (current -1);
        if(final_val >= 10) {
            final_val = final_val % STORE_TOTAL_HISTORY;
        }

        history_cmd = hist[final_val];
    }

    tsh_execute_command(&history_cmd,NULL);

    return true;
}

static bool cmd_history(char** token)
{
    display_history(hist, current, STORE_TOTAL_HISTORY);
    return true;
}

static bool cmd_clear_history(char** token)
{
    clear_history(hist, STORE_TOTAL_HISTORY);
    history_count = 0;
    current = 0;
    return true;
}

static bool cmd_stop(char** token)
{
    cmd_clear_history(NULL);
    exit_signal = true;
    return true;
}

static bool cmd_setshellname(char** token)
{
    if(NULL == *(token +1)) {
        printf("please enter the shellname\n");
        return false;
    }
    strncpy(s_info.shell_name, *(token+1),SHELLNAME_SIZE - 1);
    s_info.shell_name[SHELLNAME_SIZE - 1] = '\0';

    store_config(SHELL_CONFIG_FILE, "Shellname", s_info.shell_name);

    if(tsh_debug) {
        printf("set shell name: %s",*token);
        printf("set shell name: %s",*(token+1));
    }

    return true;
}

static bool cmd_setterminator(char** token)
{
    if(NULL == *(token +1)) {
        printf("please enter the terminator\n");
        return false;
    }
    strncpy(s_info.terminator, *(token+1), TERMINATOR_SIZE -1);
    s_info.terminator[TERMINATOR_SIZE -1] = '\0';

    store_config(SHELL_CONFIG_FILE, "Terminator", s_info.terminator);
    return true;
}

static bool tsh_use_linux_shell(char *line)
{
    /* we can use fork to handle the code well, NEED TO IMPLIMENT */
    if(tsh_debug) {
        log_debug("line is %s",line);
    }

    char **split_line = NULL, *env = NULL, **split_env=NULL, *env_read=NULL, validate[256]={};
    char *env_stored = NULL;
    int status, inc_env, total_env_token = 0, total_line_token = 0;
    bool is_background = false;
    pid_t pid;

    env_stored = (char*) calloc (sizeof(char), MAXSTRING);
    if (env_stored == NULL) {
            printf(" memory allocation error");
            return 1;
    }

    if(NULL == (env_read = getenv("PATH"))) {
        printf("FAILED TO READ ENV\n");
        if(env_stored)
            free(env_stored);
        return 1;
    }

    /* copying to heap memory, strtok excepts */
    strcpy(env_stored,env_read);
    //printf("Env is [%s]\n",env_stored);
    /* Token ths env */
    if(tsh_split_line(env_stored, &split_env, &total_env_token) != 0) {
        printf("Failed to token the env\n");
        if(env_stored)
            free(env_stored);
        return 1;
    }

    if(env_stored)
        free(env_stored);
    /* Token the input line*/
    if(tsh_split_line(line, &split_line, &total_line_token) != 0) {
        printf("Failed to token the input\n");
        return 1;
    }
    /* To check if a command is a background or not */
    if(strcmp(*(split_line + total_line_token -1), "-") == 0) {
        is_background = true;
        //printf("Background command\n");
    }
    //printf("what is background char %s\n",*(split_line + total_line_token-1));
    //printf("Total line token %d\n",total_line_token);
    //printf("Total token for shell %d\n",total_token);
    //printf("value is %s\n",*split_line);
    //printf("value is %s\n",*(split_line + 1));

    inc_env = 0;
    while( *(split_env + inc_env)) {
        sprintf(validate,"%s/%s",*(split_env + inc_env),*split_line);
        //printf("VALIDATE %s\n",validate);
        if(0 != access(validate,F_OK|X_OK)) {
            //printf("Error while searching the exe path\n");
            inc_env++;
            continue;
        }
        /* breaks when we get env */
        //printf("EXE FOUND %s\n",validate);
        break;
    }
    if(total_env_token <= inc_env) {
      printf("command not found\n");
      return 1;
    }
    //printf("INC value %d total token %d\n",inc_env, total_token);

    if(split_env)
        free(split_env);

    //printf("EXECUTING LINUX SHELL\n");
/* can use system call instead of fork */
    //system(line);
    pid = fork();
    if(pid < 0 ) {
        printf("FAILED TO FORK\n");
        return false;
    }
    else if(pid == 0) {
        //printf("CHILD PROCESS\n");
        /* we can use char *args[] = {"ls","-l",NULL};
         * execv(args[0],args,NULL);
         */
        char *args[4];
        //char path[128];
        //sprintf(path,"%s",*split_line);
        //sprintf(path,"%s/%s",binpath,*split_line);
        args[0] = validate;
        char* envp[] = {NULL};
        int i =1, neg_backgrouns_str;
#if 1
        while (*(split_line + i)) {
            if(is_background) {
                printf("BACK\n");
                if(!(i < total_line_token-1)) {
                    printf("BREAK\n");
                    break;
                }
            }
            args[i] = *(split_line + i);
            i++;
        }
#endif
        printf("inc %d total %d\n",i, total_line_token);
        args[i] = NULL;
        printf("VALUES ASRE %s\n",args[0]);
        printf("VALUES ASRE %s\n",args[1]);
        if(execve(args[0],args,envp) == -1 ) {
        //if(execvp(args[0],args) == -1 ) {
            //printf("FAILED TO EXECUTE THE COMMAND\n");
            exit(0);
        }
    }
    else {
        //printf("PARENT PROCESS\n");
        if(!is_background) {
            if (waitpid(pid, &status, WUNTRACED | WCONTINUED) < 0) {
                printf("FAILED TO WAIT FOR CHILD\n");
                return false;
            }

            if (WIFEXITED(status)) {
                printf("exited, status=%d\n", WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                printf("killed by signal %d\n", WTERMSIG(status));
            } else if (WIFSTOPPED(status)) {
                printf("stopped by signal %d\n", WSTOPSIG(status));
            } else if (WIFCONTINUED(status)) {
                printf("continued\n");
            }

            //printf("CHILD EXECUTED\n");
        }
    }

    /* To free the memory */
    if(split_line)
        free(split_line);

    return true;
}

static bool cmd_help(char** token)
{
    int i,j;
    for(i=0;i<=2;i++) {
        for(j=0;j<=88;j++) {
            if(i == 0 || i == 2 ) {
                printf("-");
            }
            else if(i == 1 && j == 1) {
                printf("Command_supported\t|\targuments\t|\tDescription\t\t\t|");
            }
        }
        printf("\n");
    }
    for(i=0;i<(int)(sizeof(commands)/sizeof(command_t));i++) {
        printf("%-10s\t\t\t%s\t\t%s\n",commands[i].res_command, commands[i].arguments, commands[i].description);
    }
    for(j=0;j<=88;j++) {
        printf("-");
    }
    printf("\n");

    return true;
}

void handle_ctrl_l()
{
    /* to handle ctrl + L signal to clear the screen */
    system("clear");
}

int tsh_execute_command(char **command_entered, char *line)
{
    int search = 0;
    bool alias_found = false;
    char alias_eqvi_command[ALIAS_STR_LENGTH]= {};
    if(*command_entered == NULL) {
        if(tsh_debug) {
            log_debug("No command entered\n");
        }
        return 0;
    }

    //printf("command entered is %s\n",*command_entered);
    //printf("line is %s\n",line);

    /* check for alias substitution and get the eqvivalent command if found*/
    if(check_cmd_is_alias(*command_entered, alias_eqvi_command, alias_hold) == 0) {
        alias_found = true;
        strcpy(*command_entered, alias_eqvi_command);
    }
    else {
        //printf("THIS is not a alias\n");
    }

    /* To handle the alias without any arg */
    if(line == NULL) {
        int count = 1;
        char *token;
        token = strtok(*command_entered, "' '");
        while (token != NULL) {
            token = strtok(NULL, "' '");
            count++;
       }

        /* NO arg found */
       if(count == 2) {
           tsh_use_linux_shell(*command_entered);
           return 0;
       }
    }

    /* To replace alias and make a entire command */
    if (alias_found) {

        int inc = 1;
        char final_command[256] = { };

        strcat (final_command, alias_eqvi_command);

        while (*(command_entered + inc)) {
            strcat (final_command, " ");
            strcat (final_command, *(command_entered + inc));
            inc++;
        };

        strcpy(*command_entered, final_command);

    }

    for(search = 0; search < (int)(sizeof(commands)/sizeof(command_t)); search++) {
        if(strcmp(commands[search].res_command, *command_entered) == 0) {
            if(commands[search].funcptr(command_entered) != true ) {
                if(tsh_debug) {
                    log_err("Command execution failed");
                }
                return 1;
            }
            return 0;
        }
    }

    /* To handle cd command */
    if(0 == strcmp("cd",*command_entered)) {
        chdir(*(command_entered+ 1));
        return 0;
    }

/* To execute the alias command and the history re-execution command, if it is not a builtin command */
    if(line == NULL || alias_found) {
        tsh_use_linux_shell(*command_entered);
        return 0;
    }
    else {
        tsh_use_linux_shell(line); /* To handle the shell command directly*/
        return 0;
    }

    if(tsh_debug) {
        log_warn("\t** COMMAND NOT SUPPORTED PLEASE TRY help **");
    }

    return 1;
}

int tsh_split_line(char *line, char ***split, int *total_token)
{
    char* token;
    int position = 0, count = 0;
    *split = (char**)calloc(MAXSTRING, sizeof(char*));
    if(!split) {
        log_err("Error allocating memory");
        return 1;
    }

    token = strtok(line, "\n\r\t':'' '");
    while(token != NULL) {
        *(*(split)+position++) = token;
        count++;
        if(tsh_debug) {
            log_debug("split tokens are %s",token);
        }
        token = strtok(NULL, "\n\r\t':'' '");
    }
    /* storing total tokens got */
    if(total_token != NULL)
        *total_token = count;

    return 0;
}

int tsh_read_line (char** line, int* total_char)
{
    char ch;
    int position = 0;

    *line = (char*) calloc (sizeof(char), MAXSTRING);

    if (!*line) {
        log_err(" memory allocation error");
        return 1;
    }

    for (position = 0; position < MAXSTRING;) {

        ch = getchar ();
        if(ch == 0x0C) {
            handle_ctrl_l();
            /* to read the next line char left in the buffer */
            getchar();
            /* handling -1 */
            return -1;
        }

        if('!' == ch) {
            re_execution_command = true; /* To know if it is history command, helps not to store the history commands */
        }

        if (ch == '\n' || ch == '$') {
            *(*(line) + position) = '\0';
            *total_char = position;
            if(ch == '$') {
                while(getchar() != '\n');
            }
            break;
        }  else {
            *(*(line) + position) = ch;
        }
        position++;
    }

    if(tsh_debug) {
        printf("READ LINE IS %s \n",*line);
    }

    return 0;

}
void clear_colour()
{
    /* To clear the colour */
    printf("\033[0m");

}

void clear_memory(char *line, char ***token, char* command_reserve)
{
    if(line) {
        free(line);
        line = NULL;
    }
    if(command_reserve) {
        free(command_reserve);
        command_reserve = NULL;
    }
    if(preserve_string) {
        free(preserve_string);
        preserve_string = NULL;
    }
    if(*token) {
        free(*token);
        token = NULL;
    }

}

void loop()
{
    char *line = NULL, **token = NULL, *command_reserve = NULL;
    int total_char = 0, cmd_num = 1, ret;

    do{
        re_execution_command = false; /* clearing flag */
        printf("\033[1;32m%s[%d]%s ",s_info.shell_name, cmd_num, s_info.terminator);
        clear_colour();
        ret = tsh_read_line(&line, &total_char);
        printf("%d", ret);

        
        if (ret) {
            if (ret != -1)
                log_warn("failed to read the line");
            continue;
        }

        command_reserve = (char*) calloc (sizeof(char), MAXSTRING);
        if(NULL == command_reserve) {
            log_err("failed to allocate memory to reserve the command");
            free(line);
            line = NULL;
            continue;
        }

        preserve_string = (char*) calloc (sizeof(char), MAXSTRING);
        if(NULL == preserve_string) {
            log_err("failed to allocate memory to preserve_string the command");
            free(line);
            line = NULL;
            continue;
        }

        /* To take a backup of the command */
        strcpy(command_reserve,line);

        if(total_char) {
            cmd_num++;

            if(history_count <= STORE_TOTAL_HISTORY)
                history_count++;
            /* This is used only when we try to execute the first history */
            if(hist[current]) {
                strcpy(preserve_string, hist[current]);
            }
            /* This will not store the the re-execution command */
            if(!re_execution_command)
                add_history(command_reserve, hist, &current, STORE_TOTAL_HISTORY);

        }

        if(tsh_debug) {
            log_debug("entered command is %s",line);
        }

        if(tsh_split_line(line, &token, NULL) != 0) {
            log_warn("failed to get token");
            /* we dont loose the commnad */
            clear_memory(line, &token, command_reserve);
            continue;
        }

        if(tsh_debug && total_char) {
            log_debug("%s\n",*token);
            //printf("%s\n",*(token + 1));
        }

        if(tsh_execute_command(token, command_reserve) != 0) {
            if(tsh_debug) {
                log_warn("failed to execute the command");
            }
        }
        clear_memory(line, &token, command_reserve);

    }while(!exit_signal);

}

void Emblem()
{
   printf("\t\t \033[0;32m WELCOME TO %s\n", s_info.shell_name);
   /* To reset the colour */
   clear_colour();
}

int init_shell()
{
    if (0 != restore_config(&s_info, SHELL_CONFIG_FILE)) {
        printf("Failed to restore config file\n");
    }

    if(CLEAR_SCREEN_WITH_HISTORY) {
        system("clear");
        usleep(10000);
    } else {
        ClearScreen();
    }

    Emblem();

    return 0;
}

void shell_debug(bool debug)
{
    tsh_debug = debug;
}

void start()
{
    shell_debug(false);
    if(0 != init_shell())
        return;
    display_warning();
    loop();

}

void stop()
{

}

