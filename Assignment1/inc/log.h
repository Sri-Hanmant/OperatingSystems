/**
 * @file log.h
 * @brief log.h
 * @author
 * @date 09-Sep-2019
 *
 *
 */


#ifndef LOG_H_
#define LOG_H_

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 * headers
 ***************************************************************************/

#include <syslog.h>
#include <stdarg.h>

#define COLOUR          1
#define TIMESTAMP       1

/** Custom log for file, function and line  */
#define log_debug(FMT, ...) log_msg(LOG_DEBUG, "[%s:%d] " FMT, __func__, __LINE__, ##__VA_ARGS__)
#define log_info(FMT, ...) log_msg(LOG_INFO, "[%s:%d] " FMT, __func__, __LINE__, ##__VA_ARGS__)
#define log_warn(FMT, ...) log_msg(LOG_WARNING, "[%s:%d] " FMT, __func__, __LINE__, ##__VA_ARGS__)
#define log_err(FMT, ...) log_msg(LOG_ERR, "[%s:%d] " FMT, __func__, __LINE__, ##__VA_ARGS__)

/***************************************************************************
 * function prototypes
 ***************************************************************************/

extern void log_msg(int priority, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* LOG_H_ */
