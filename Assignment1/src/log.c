/**
 * @file log.c
 * @brief log module
 * @author
 * @date 2019-01-04
 *
 */

/***************************************************************************
 * headers
 ***************************************************************************/
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#include "log.h"

/***************************************************************************
 * objects
 ***************************************************************************/

static const char *level_names[] = {
    "DEBUG", "INFO", "WARN", "ERROR"
};

/* bold green, white, green, red, yellow */
static const char *level_colors[] = {
        "\033[1;32m", "\033[0m", "\033[0;32m", "\033[1;31m", "\033[1;33m"
};

/***************************************************************************
 * functions
 ***************************************************************************/

static void log_msgv(int priority, const char *fmt, va_list args)
{
    char str[1024] = { 0 };
    int n = 0, level = 1;

    level = (priority == LOG_DEBUG) ? 0 : (priority == LOG_INFO) ? 1 : (priority == LOG_WARNING) ? 2 : 3;
#if defined TIMESTAMP
    n += snprintf(&str[n],sizeof(str) - 2 - n, "[%s %s] %-5s",__DATE__, __TIME__, level_names[level]);
#endif

#if defined COLOUR
    n += snprintf(&str[n],sizeof(str) - 2 - n, "%s",level_colors[level]);
#endif

    if (n > sizeof(str) - 2) {
        //error check
        return;
    }

    n += vsnprintf(&str[n], sizeof(str) - 2 - n, fmt, args);
    if (n > sizeof(str) - 2) {
        return;
    }

    str[n++] = '\n';
    str[n] = '\0';

    fputs(str, stdout);
    /* To reset the colour */
    printf("%s",level_colors[1]);
}

void log_msg(int priority, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    log_msgv(priority, fmt, args);
    va_end(args);
}
