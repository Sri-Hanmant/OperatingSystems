/**
 * @file history.h
 * @brief history.h
 * @author
 * @date 10-Sep-2019
 *
 */

#ifndef HISTORY_H_
#define HISTORY_H_

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 * headers
 ***************************************************************************/

/***************************************************************************
 * prototypes
 ***************************************************************************/
/**
 * @brief add commands to the array
 * @param line, command to be stored
 * @param hist, array where the commands is stored
 * @param history_length, total histories to be stored
 */
void add_history(char *line, char *hist[], int *current, int history_length);

/**
 * @brief shows the history stored in an array
 * @param hist, array where history is stored
 * @param current, current position where history is pointing
 * @param history_length, total histories to be stored
 */
void display_history(char *hist[], int current, int history_length);

/**
 * @brief clears the histories present in an array
 * @param hist, array where history is stored
 * @param history_length, total histories to be stored
 */
void clear_history(char *hist[], int history_length);

#ifdef __cplusplus
}
#endif

#endif /* HISTORY_H_ */
