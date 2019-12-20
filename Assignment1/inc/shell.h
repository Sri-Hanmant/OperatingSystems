/**
 * @file shell.h
 * @brief shell.h
 * @author
 * @date 06-Sep-2019
 *
 */

#ifndef SHELL_H_
#define SHELL_H_

extern char **environ;

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 * headers
 ***************************************************************************/

#include <stdbool.h>

/***************************************************************************
 * constants
 ***************************************************************************/

#define POLLDELAY                   100                             //!< main loop delay in ms
#define MAXSTRING                   256                             //!< maximun input string that can be processed
#define ClearScreen()               printf("\033[H\033[J")          //!< clears screen without history

#define CLEAR_SCREEN_WITH_HISTORY   true                            //!< clears screen with history
#define STORE_TOTAL_HISTORY         10                              //!< Total history to hold
#define STORE_TOTAL_ALIASES         10                              //!< Total aliases to hold
#define TERMINATOR_SIZE             8                               //!< Length of terminators to accept
#define SHELLNAME_SIZE              16                              //!< Length of shell name to accept
#define SHELL_CONFIG_FILE           "./shell_config.ini"            //!< Filename to store the shell configs

/***************************************************************************
 * typedefs
 ***************************************************************************/
typedef struct {
    char*       res_command;
    bool        (*funcptr)(char** token);
    char*       arguments;
    const char* description;
} command_t;

typedef struct {
    char terminator[TERMINATOR_SIZE];
    char shell_name[SHELLNAME_SIZE];
} shell_info_t;

/***************************************************************************
 * function prototypes
 ***************************************************************************/
/**
 * @brief, To exit the shell
 * @param[in] token input string
 * @return true if success otherwise fail
 */
static bool cmd_stop(char** token);

/**
 * @brief, To set the shell name
 * @param[in] token input string
 * @return true if success otherwise fail
 */
static bool cmd_setshellname(char** token);

/**
 * @brief, To set terminator
 * @param[in] token input string
 * @return true if success otherwise fail
 */
static bool cmd_setterminator(char** token);

/**
 * @brief, display the commands supported
 * @param[in] token input string
 * @return true if success otherwise fail
 */
static bool cmd_help(char** token);

/**
 * @brief, displays the command history executed
 * @param[in] token input string
 * @return true if success otherwise fail
 */
static bool cmd_history(char** token);

/**
 * @brief, To clear the command histories executed
 * @param[in] token input string
 * @return true if success otherwise fail
 */
static bool cmd_clear_history(char** token);

/**
 * @brief, To execute the history command
 * @param[in] token input string
 * @return true if success otherwise fail
 */
static bool cmd_history_execute(char** token);

/**
 * @brief, To set the new aliases
 * @param[in] token input string
 * @return true if success otherwise fail
 */
static bool cmd_newname(char** token);

/**
 * @brief, displays the aliases stored
 * @param[in] token input string
 * @return true if success otherwise fail
 */
static bool cmd_display_newname(char** token);

/**
 * @brief, stores aliases to a file
 * @param[in] token input string
 * @return true if success otherwise fail
 */
static bool cmd_store_newname(char **token);

/**
 * @brief, Restores the aliases from a file
 * @param[in] token input string
 * @return true if success otherwise fail
 */
static bool cmd_restore_newname(char **token);

/**
 * @brief, executes the command entered
 * @param[in] token input string
 * @return 0 if success otherwise fail
 */
int tsh_execute_command(char **command_entered, char *line);

/**
 * @brief, To remove tokens from the command
 * @param line, string to remove the tokens
 * @param split, to store the strings after removing the tokens
 * @param 0 if success otherwise fail
 */
int tsh_split_line(char *line, char ***split, int *total_token);

/**
 * @brief, To fetch the commands from user
 * @param line, to store the commands
 * @param total_char, to store the number of char entered
 * @return 0 if true otherwise false
 */
int tsh_read_line (char** line, int* total_char);

/**
 * @brief, To clear the display line colours
 */
void clear_colour();

/**
 * @brief, To free the allocated memory for the shell,
 * @param line, address to free the memory
 * @param token, address to free the memory
 * @param command_reserve, address to free the memory
 */
void clear_memory(char *line, char ***token, char* command_reserve);

/**
 * @brief, Keeps looping for the new commands from user
 */
void loop();

/**
 * @brief, To print the Emblem
 */
void Emblem();

/**
 * @brief, Initialize the shell
 */
int init_shell();

/**
 * @brief, To enable the debug
 * @param[in] debug. set true or false
 */
void shell_debug(bool debug);

/**
 * @brief start the shell
 */
void start();

/**
 * @brief stop the shell
 */
void stop();

#ifdef __cplusplus
}
#endif

#endif /* SHELL_H_ */
