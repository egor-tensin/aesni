/*
 * Copyright (c) 2015 Egor Tensin <Egor.Tensin@gmail.com>
 * This file is part of the "AES tools" project.
 * For details, see https://github.com/egor-tensin/aes-tools.
 * Distributed under the MIT License.
 */

#include <aes/all.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_MSC_VER) && _MSC_VER < 1900

#include <stdarg.h>

#define snprintf c99_snprintf
#define vsnprintf c99_vsnprintf

static int c99_vsnprintf(char *dest, size_t dest_size, const char *fmt, va_list ap)
{
    int dest_len = -1;

    if (dest_size != 0)
        dest_len = _vsnprintf_s(dest, dest_size, _TRUNCATE, fmt, ap);
    if (dest_len == -1)
        dest_len = _vscprintf(fmt, ap);

    return dest_len;
}

static int c99_snprintf(char *dest, size_t dest_size, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    int dest_len = c99_vsnprintf(dest, dest_size, fmt, ap);

    va_end(ap);
    return dest_len;
}

#endif

static void aes_fill_string(char* dest, size_t dest_size, const char* src)
{
    strncpy(dest, src, dest_size);
    dest[dest_size - 1] = '\0';
}

static const char* aes_strerror_messages[] =
{
    "Success",
    "Invalid argument value NULL",
    "Couldn't parse",
    "Invalid PKCS7 padding (wrong key?)",
    "Not implemented",
    "Missing padding",
    "Couldn't allocate memory",
};

const char* aes_strerror(AES_StatusCode ec)
{
    return aes_strerror_messages[ec];
}

static size_t aes_format_error_strerror(
    const AES_ErrorDetails* err_details,\
    char* dest,
    size_t dest_size)
{
    const AES_StatusCode ec = aes_get_error_code(err_details);
    const char* const msg = aes_strerror(ec);

    if (dest == NULL)
        return strlen(msg) + 1;

    aes_fill_string(dest, dest_size, msg);
    return strlen(dest);
}

static size_t aes_format_null_argument_error(
    const AES_ErrorDetails* err_details,
    char* dest,
    size_t dest_size)
{
    static const char* const fmt = "Invalid argument value NULL for parameter '%s'";
    const char* const param_name = err_details->params.null_arg.param_name;

    if (dest == NULL && dest_size == 0)
        return snprintf(NULL, 0, fmt, param_name) + 1;

    snprintf(dest, dest_size, fmt, param_name);
    return strlen(dest);
}

static size_t aes_format_parse_error(
    const AES_ErrorDetails* err_details,
    char* dest,
    size_t dest_size)
{
    static const char* const fmt = "Couldn't parse '%s' (possibly not complete input) as %s";
    const char* const src = err_details->params.parse_error.src;
    const char* const what = err_details->params.parse_error.what;

    if (dest == NULL)
        return snprintf(NULL, 0, fmt, src, what) + 1;

    snprintf(dest, dest_size, fmt, src, what);
    return strlen(dest);
}

static size_t aes_format_not_implemented_error(
    const AES_ErrorDetails* err_details,
    char* dest,
    size_t dest_size)
{
    static const char* const fmt = "Not implemented: %s";
    const char* const src = err_details->params.not_implemented.what;

    if (dest == NULL)
        return snprintf(NULL, 0, fmt, src) + 1;

    snprintf(dest, dest_size, fmt, src);
    return strlen(dest);
}

typedef size_t (*AES_ErrorFormatter)(const AES_ErrorDetails*, char*, size_t);

static AES_ErrorFormatter err_formatters[] =
{
    &aes_format_error_strerror,
    &aes_format_null_argument_error,
    &aes_format_parse_error,
    &aes_format_error_strerror,
    &aes_format_not_implemented_error,
    &aes_format_error_strerror,
    &aes_format_error_strerror,
};

size_t aes_format_error(
    const AES_ErrorDetails* err_details,
    char* dest,
    size_t dest_size)
{
    assert(err_details);

    return err_formatters[err_details->ec](err_details, dest, dest_size);
}

#ifdef WIN32
#include <windows.h>

static void aes_collect_call_stack(AES_ErrorDetails* err_details)
{
    err_details->call_stack_len = CaptureStackBackTrace(
        1, AES_MAX_CALL_STACK_LENGTH, err_details->call_stack, NULL);
}
#else
static void aes_collect_call_stack(AES_ErrorDetails* err_details)
{
    err_details->call_stack_len = 0;
}
#endif

static AES_StatusCode aes_make_error(
    AES_ErrorDetails* err_details,
    AES_StatusCode ec)
{
    if (err_details == NULL)
        return ec;

    if (aes_is_error(ec))
        aes_collect_call_stack(err_details);

    return err_details->ec = ec;
}

AES_StatusCode aes_success(
    AES_ErrorDetails* err_details)
{
    return aes_make_error(err_details, AES_SUCCESS);
}

AES_StatusCode aes_error_null_argument(
    AES_ErrorDetails* err_details,
    const char* param_name)
{
    AES_StatusCode status = aes_make_error(err_details, AES_NULL_ARGUMENT_ERROR);

    if (err_details != NULL)
        aes_fill_string(
            err_details->params.null_arg.param_name,
            sizeof(err_details->params.null_arg.param_name), param_name);

    return status;
}

AES_StatusCode aes_error_parse(
    AES_ErrorDetails* err_details,
    const char* src,
    const char* what)
{
    AES_StatusCode status = aes_make_error(err_details, AES_PARSE_ERROR);

    if (err_details != NULL)
    {
        aes_fill_string(
            err_details->params.parse_error.src,
            sizeof(err_details->params.parse_error.src), src);
        aes_fill_string(
            err_details->params.parse_error.what,
            sizeof(err_details->params.parse_error.what), what);
    }

    return status;
}

AES_StatusCode aes_error_invalid_pkcs7_padding(
    AES_ErrorDetails* err_details)
{
    return aes_make_error(err_details, AES_INVALID_PKCS7_PADDING_ERROR);
}

AES_StatusCode aes_error_not_implemented(
    AES_ErrorDetails* err_details,
    const char* what)
{
    AES_StatusCode status = aes_make_error(err_details, AES_NOT_IMPLEMENTED_ERROR);

    if (err_details != NULL)
        aes_fill_string(
            err_details->params.not_implemented.what,
            sizeof(err_details->params.not_implemented.what), what);

    return status;
}

AES_StatusCode aes_error_missing_padding(
    AES_ErrorDetails* err_details)
{
    return aes_make_error(err_details, AES_MISSING_PADDING_ERROR);
}

AES_StatusCode aes_error_memory_allocation(
    AES_ErrorDetails* err_details)
{
    return aes_make_error(err_details, AES_MEMORY_ALLOCATION_ERROR);
}
