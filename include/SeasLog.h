/*
+----------------------------------------------------------------------+
| SeasLog                                                              |
+----------------------------------------------------------------------+
| This source file is subject to version 2.0 of the Apache license,    |
| that is bundled with this package in the file LICENSE, and is        |
| available through the world-wide-web at the following url:           |
| http://www.apache.org/licenses/LICENSE-2.0.html                      |
| If you did not receive a copy of the Apache2.0 license and are unable|
| to obtain it through the world-wide-web, please send a note to       |
| license@php.net so we can mail you a copy immediately.               |
+----------------------------------------------------------------------+
| Author: Neeke.Gao  <neeke@php.net>                                   |
+----------------------------------------------------------------------+
*/

#ifndef _SEASLOG_H_
#define _SEASLOG_H_

#include "php.h"
#include "php_ini.h"
#include "zend_extensions.h"
#include "zend_exceptions.h"
#include "ext/standard/info.h"
#include "ext/standard/file.h"
#include "ext/standard/php_string.h"
#include "ext/date/php_date.h"
#include "php7_wrapper.h"

#if PHP_VERSION_ID >= 70000
#include "zend_smart_str.h"
#else
#include "ext/standard/php_smart_str.h"
#endif

#include <stdlib.h>

#define SEASLOG_RES_NAME                    "SeasLog"
#define SEASLOG_VERSION                     "1.7.0"
#define SEASLOG_AUTHOR                      "Chitao.Gao  [ neeke@php.net ]"

#define SEASLOG_ALL                         "all"
#define SEASLOG_DEBUG                       "debug"
#define SEASLOG_INFO                        "info"
#define SEASLOG_NOTICE                      "notice"
#define SEASLOG_WARNING                     "warning"
#define SEASLOG_ERROR                       "error"
#define SEASLOG_CRITICAL                    "critical"
#define SEASLOG_ALERT                       "alert"
#define SEASLOG_EMERGENCY                   "emergency"

#define SEASLOG_APPENDER_FILE               1
#define SEASLOG_APPENDER_TCP                2
#define SEASLOG_APPENDER_UDP                3

#define SEASLOG_DETAIL_ORDER_ASC            1
#define SEASLOG_DETAIL_ORDER_DESC           2

#define SEASLOG_EVENT_ERROR                 1
#define SEASLOG_EVENT_EXCEPTION             2

#define SEASLOG_DIR_MODE                    0755
#define SEASLOG_FILE_MODE                   0666

#define SEASLOG_BUFFER_RE_INIT_YES          1
#define SEASLOG_BUFFER_RE_INIT_NO           2

#define SEASLOG_PROCESS_LOGGER_LAST         1
#define SEASLOG_PROCESS_LOGGER_TMP          2

#define SL_S(s)                             s, sizeof(s) - 1

#define SEASLOG_HASH_VALUE_LOGGER           1
#define SEASLOG_HASH_VALUE_PATH             2
#define SEASLOG_HASH_VALUE_ACCESS           3

#define SEASLOG_BUFFER_MAX_SIZE             65535

#define SEASLOG_TRIM_WRAP                   26



#if PHP_VERSION_ID >= 70000

#define EX_ARRAY_DESTROY(arr) \
	do { \
		zval_ptr_dtor(arr); \
		ZVAL_UNDEF(arr); \
	} while(0)

#else

#define EX_ARRAY_DESTROY(arr) \
	do { \
		zval_ptr_dtor(arr);\
	} while(0)

#endif

typedef struct _logger_entry_t
{
    ulong logger_hash;
    char *logger;
    int logger_len;
    char *logger_path;
    int logger_path_len;
    int access;
} logger_entry_t;

typedef struct _last_sec_entry_t
{
    int  sec;
    char *real_time;
} last_sec_entry_t;

typedef struct _last_min_entry_t
{
    int  sec;
    char *real_time;
} last_min_entry_t;


//Common Toolkit
static char *str_replace(char *ori, char * rep, char * with);
static char *strreplace(char *src, const char *oldstr, const char *newstr, size_t len);
static char *delN(char *a);
static char *get_uniqid();
static int message_trim_wrap(char *message,int message_len TSRMLS_DC);

#if PHP_VERSION_ID >= 70000
static char *php_strtr_array(char *str, int slen, HashTable *pats);
#else
static char *php_strtr_array(char *str, int slen, HashTable *hash);
#endif


//Logger
static void seaslog_init_logger(TSRMLS_D);
static void seaslog_init_default_last_logger(TSRMLS_D);
static void seaslog_clear_logger(TSRMLS_D);
static void seaslog_init_logger_list(TSRMLS_D);
static void seaslog_clear_logger_list(TSRMLS_D);
static logger_entry_t *process_logger(char *logger, int logger_len, int last_or_tmp TSRMLS_DC);

//Datetime
static void seaslog_process_last_sec(int now TSRMLS_DC);
static void seaslog_process_last_min(int now TSRMLS_DC);
static char *seaslog_format_date(char *format, int format_len, time_t ts TSRMLS_DC);
static char *make_real_date(TSRMLS_D);
static char *make_real_time(TSRMLS_D);
static char *mic_time();


//Buffer
static void seaslog_init_buffer(TSRMLS_D);
static int real_php_log_buffer(zval *msg_buffer, char *opt TSRMLS_DC);
static int seaslog_buffer_set(char *log_info, int log_info_len, char *path, int path_len, zend_class_entry *ce TSRMLS_DC);
static void seaslog_shutdown_buffer(int re_init TSRMLS_DC);
static void seaslog_clear_buffer(TSRMLS_D);

//ErrorHook
void (*old_error_cb)(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args);
void seaslog_error_cb(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args);
static void process_event_error(int type, char * error_filename, uint error_lineno, char * msg TSRMLS_DC);
static void initErrorHooks(TSRMLS_D);
static void recoveryErrorHooks(TSRMLS_D);

//ExceptionHook
void (*old_throw_exception_hook)(zval *exception TSRMLS_DC);
void seaslog_throw_exception_hook(zval *exception TSRMLS_DC);
static void process_event_exception(int type, char * error_filename, uint error_lineno, char * msg TSRMLS_DC);
static void initExceptionHooks(TSRMLS_D);
static void recoveryExceptionHooks(TSRMLS_D);

//Request
static void seaslog_init_host_name(TSRMLS_D);
static void seaslog_clear_host_name(TSRMLS_D);
static void seaslog_init_request_id(TSRMLS_D);
static void seaslog_clear_request_id(TSRMLS_D);

//Appender
static php_stream *seaslog_stream_open_wrapper(char *opt TSRMLS_DC);
static int seaslog_real_log_ex(char *message, int message_len, char *opt TSRMLS_DC);
static int seaslog_log_content(int argc, char *level, char *message, int message_len, HashTable *content, char *module, int module_len, zend_class_entry *ce TSRMLS_DC);
static int seaslog_log_ex(int argc, char *level, char *message, int message_len, char *module, int module_len, zend_class_entry *ce TSRMLS_DC);
static int appender_handle_file(char *message, int message_len, char *level, logger_entry_t *logger, zend_class_entry *ce TSRMLS_DC);
static int appender_handle_tcp_udp(char *message, int message_len, char *level, logger_entry_t *logger, zend_class_entry *ce TSRMLS_DC);
static int check_log_level(char *level TSRMLS_DC);
static int make_log_dir(char *dir TSRMLS_DC);
static int check_log_dir(char *dir TSRMLS_DC);
static int seaslog_real_buffer_log_ex(char *message, int message_len, char *log_file_path, int log_file_path_len, zend_class_entry *ce TSRMLS_DC);

//Analyzer
static long get_type_count(char *log_path, char *level, char *key_word TSRMLS_DC);
static int get_detail(char *log_path, char *level, char *key_word, long start, long end, long order, zval *return_value TSRMLS_DC);


#endif /* _SEASLOG_H_ */