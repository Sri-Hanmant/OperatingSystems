/**
 * @file shell-main.c
 * @brief shell-main.c
 * @author
 * @date 06-Sep-2019
 */

/***************************************************************************
 * headers
 ***************************************************************************/
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <wait.h>
#include <signal.h>
#include <sys/types.h>

#include "shell.h"

/***************************************************************************
 * function prototypes
 ***************************************************************************/
void sig_handler(int sig);

/***************************************************************************
 * functions
 ***************************************************************************/

void sig_handler(int sig)
{
    /* Ignoring the caught siganls without processing it */
    //printf("Caught signal %d\n", sig);
}
#if 1
void childSignalHandler(int sig)
{
    int status;
    pid_t pid;
    pid = waitpid(-1, &status, WNOHANG);
}
#endif
int main(int argc, char **argv)
{
    //printf("starting shell\n");

    /* Registering the signals */
    /*
    signal(SIGTERM, sig_handler);
    signal(SIGINT, sig_handler);
    signal(SIGKILL, sig_handler);
    signal(SIGTSTP, sig_handler);
*/
    /* To handle the background process */
    signal(SIGCHLD, childSignalHandler);
    start(); /* start shell execution */

    /* Entered only when stop command is executed */
    stop(); /* stop shell execution */

    //printf("stopped shell\n");

    return 0; /* Exit success */
}
