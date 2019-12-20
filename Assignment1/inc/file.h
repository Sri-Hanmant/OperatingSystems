/**
 * @file file.h
 * @brief file.h
 * @author
 * @date 11-Sep-2019
 */


#ifndef FILE_H_
#define FILE_H_

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 * headers
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shell.h"

/***************************************************************************
 * function prototypes
 ***************************************************************************/

/**
 * @brief Restores the configuration from the file
 * @param shell_info, address of structure to store the configs
 * @param shell_config_file, filename from where the configs should be read
 * @return 0 success otherwise fail
 */
int restore_config(shell_info_t *shell_info, char* shell_config_file);

/**
 * @brief stores the configuration to file
 * @param shell_config_file, filename to store the configs
 * @param type, which type of the config
 * @param data, data for the type specified
 * @return 0 success otherwise fail
 */
int store_config(char *shell_config_file, char *type, char *data);


#ifdef __cplusplus
}
#endif

#endif /* FILE_H_ */
