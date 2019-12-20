/**
 * @file history.c
 * @brief history.c
 * @author
 * @date 10-Sep-2019
 *
 */

/***************************************************************************
 * headers
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "log.h"
#include "history.h"

/***************************************************************************
 * Global variables
 ***************************************************************************/
extern bool tsh_debug;

/***************************************************************************
 * functions
 ***************************************************************************/
void add_history(char *line, char *hist[], int *current, int history_length)
{
    if(hist[*current])
        free(hist[*current]);
    hist[*current] = strdup(line);
    /* added extra history length to handle the execution of history */
    *current = (*current + 1)  % (history_length);

}

void display_history(char *hist[], int current, int history_length)
{
    int i = current;
    int hist_num = 1;

    do {
        if (hist[i]) {
            printf ("%4d  %s\n", hist_num, hist[i]);
            hist_num++;
        }
        /* display up to history_length */
        i = (i + 1) % (history_length);

    } while (i != current);

}

void clear_history(char *hist[], int history_length)
{
    int i;

    for (i = 0; i < (history_length); i++) {
        free (hist[i]);
        hist[i] = NULL;
    }

}
