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

static void process_event_exception(int type, char * error_filename, uint error_lineno, char * msg TSRMLS_DC)
{
    char *event_str;
    int event_str_len;

    event_str_len = spprintf(&event_str, 0, "Exception - type:%d - file:%s - line:%d - msg:%s", type, error_filename, error_lineno, msg);

    seaslog_log_ex(1, SEASLOG_CRITICAL, event_str, event_str_len, NULL, 0, seaslog_ce TSRMLS_CC);
    efree(event_str);
}

void seaslog_throw_exception_hook(zval *exception TSRMLS_DC)
{
    zval *message, *file, *line, *code;
#if PHP_VERSION_ID >= 70000
    zval rv;
#endif
    zend_class_entry *default_ce;

    if (!exception)
    {
        return;
    }

#if PHP_VERSION_ID >= 70000
    default_ce = Z_OBJCE_P(exception);
#else
    default_ce = zend_exception_get_default(TSRMLS_C);
#endif

#if PHP_VERSION_ID >= 70000
    message = zend_read_property(default_ce, exception, "message", sizeof("message")-1, 0, &rv);
    file = zend_read_property(default_ce, exception, "file", sizeof("file")-1, 0, &rv);
    line = zend_read_property(default_ce, exception, "line", sizeof("line")-1, 0, &rv);
    code = zend_read_property(default_ce, exception, "code", sizeof("code")-1, 0, &rv);
#else
    message = zend_read_property(default_ce, exception, "message", sizeof("message")-1, 0 TSRMLS_CC);
    file = zend_read_property(default_ce, exception, "file", sizeof("file")-1, 0 TSRMLS_CC);
    line = zend_read_property(default_ce, exception, "line", sizeof("line")-1, 0 TSRMLS_CC);
    code = zend_read_property(default_ce, exception, "code", sizeof("code")-1, 0 TSRMLS_CC);
#endif

    process_event(SEASLOG_EVENT_EXCEPTION, Z_LVAL_P(code), Z_STRVAL_P(file), Z_LVAL_P(line), Z_STRVAL_P(message) TSRMLS_CC);

    if (old_throw_exception_hook)
    {
        old_throw_exception_hook(exception TSRMLS_CC);
    }
}

static void initExceptionHooks(TSRMLS_D)
{
    if (SEASLOG_G(trace_exception))
    {
        if (zend_throw_exception_hook)
        {
            old_throw_exception_hook = zend_throw_exception_hook;
        }

        zend_throw_exception_hook = seaslog_throw_exception_hook;
    }
}

static void recoveryExceptionHooks(TSRMLS_D)
{
    if (SEASLOG_G(trace_exception))
    {
        if (old_throw_exception_hook)
        {
            zend_throw_exception_hook = old_throw_exception_hook;
        }
    }
}
