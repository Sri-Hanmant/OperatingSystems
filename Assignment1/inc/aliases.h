/**
 * @file aliases.h
 * @brief aliases.h
 * @author
 * @date 17-Sep-2019l
 *
 */

#ifndef ALIASES_H_
#define ALIASES_H_

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 * constants
 ***************************************************************************/
#define ALIAS_STR_LENGTH                24              //!< total size of aliases to store

/***************************************************************************
 * typedefs
 ***************************************************************************/
typedef struct {
    int id;
    char alias_name[ALIAS_STR_LENGTH];
    char alias_eqvi_cmd[ALIAS_STR_LENGTH];
} aliases_hold_t;

/***************************************************************************
 * prototypes
 ***************************************************************************/

/**
 * @brief updates the duplicate entry if present
 * @param alias_name, string alias name to update the entry
 * @param alias_eqvi_name, string command equivalent to alias
 * @param alias_hold, structure that holds the aliases
 * @param total_entries, total aliases to store
 * @return 0 if success otherwise fail
 */
int update_duplicate_entry(char *alias_name, char *alias_eqvi_name, aliases_hold_t *alias_hold, int total_entries);

/**
 * @brief to add the new alias to structure
 * @param alias_name, string alias name to add
 * @param alias_eqvi_name, string command equivalent to alias to add
 * @param alias_hold, structure that holds the aliases
 * @param total_entries, total aliases to store
 * @return 0 if success otherwise fail
 */
int register_new_alias(char *alias_name, char *alias_eqvi_name, aliases_hold_t *alias_hold, int total_entries);

/**
 * @brief display the aliases stored
 * @param alias_hold, structure that holds the aliases
 * @param total_entries, total aliases to store
 */
void display_aliases(aliases_hold_t *alias_hold, int total_entries);

/**
 * @brief delete alias if present in array
 * @param alias_del, string alias to delete
 * @param alias_hold, structure that holds the aliases
 * @param total_entries, total aliases to store
 * @return 0 if success otherwise fail
 */
int delete_alias_entry(char *alias_del, aliases_hold_t *alias_hold, int total_entries);

/**
 * @brief to check if entered command is alias
 * @param cmd, string command to check
 * @param alias_eqvi_command[out], if command is a alias store the equivalent command
 * @param alias_hold, structure that holds the aliases
 * @return 0 if success otherwise fail
 */
int check_cmd_is_alias(char *cmd, char *alias_eqvi_command, aliases_hold_t *alias_hold);

/**
 * @brief store aliases to a file
 * @param filename, name of the file where aliases to be stored
 * @param alias_hold, structure that holds the aliases
 * @return 0 if success otherwise fail
 */
int write_aliases_to_file(char *filename, aliases_hold_t *alias_hold);

/**
 * @brief read aliases from file
 * @param filename, name of the file where aliases to be read and restored
 * @param alias_hold, structure to hold the aliases
 * @param total_entries, total aliases to be stored in structure
 * @return 0 if success otherwise fail
 */
int read_aliases_from_file(char *filename, aliases_hold_t *alias_hold, int total_entries);

#ifdef __cplusplus
}
#endif

#endif /* ALIASES_H_ */
