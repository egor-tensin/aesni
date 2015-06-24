/**
 * \file
 * \author Egor Tensin <Egor.Tensin@gmail.com>
 * \date 2015
 * \copyright This file is licensed under the terms of the MIT License.
 *            See LICENSE.txt for details.
 */

#include <aesni/all.h>

#include <stdlib.h>
#include <string.h>

static AesNI_StatusCode aesni_extract_padding_size_pkcs7(
    const void* src,
    size_t src_size,
    size_t* padding_size,
    AesNI_ErrorDetails* err_details)
{
    const unsigned char* cursor = (const unsigned char*) src + src_size - 1;
    *padding_size = *cursor;

    for (size_t i = 1; i < *padding_size; ++i)
        if (cursor[0 - i] != *padding_size)
            return aesni_error_invalid_pkcs7_padding(err_details);

    return AESNI_SUCCESS;
}

AesNI_StatusCode aesni_extract_padding_size(
    AesNI_PaddingMethod method,
    const void* src,
    size_t src_size,
    size_t* padding_size,
    AesNI_ErrorDetails* err_details)
{
    assert(src);
    assert(padding_size);

    if (src == NULL)
        return aesni_error_null_argument(err_details, "src");
    if (padding_size == NULL)
        return aesni_error_null_argument(err_details, "padding_size");

    switch (method)
    {
        case AESNI_PADDING_PKCS7:
            return aesni_extract_padding_size_pkcs7(
                src, src_size, padding_size, err_details);

        default:
            return aesni_error_not_implemented(
                err_details, "unsupported padding method");
    }
}

static AesNI_StatusCode aesni_fill_with_padding_pkcs7(
    void* dest,
    size_t padding_size,
    AesNI_ErrorDetails* err_details)
{
    memset(dest, padding_size, padding_size);
    return AESNI_SUCCESS;
}

AesNI_StatusCode aesni_fill_with_padding(
    AesNI_PaddingMethod method,
    void* dest,
    size_t padding_size,
    AesNI_ErrorDetails* err_details)
{
    assert(dest);

    if (dest == NULL)
        return aesni_error_null_argument(err_details, "dest");

    switch (method)
    {
        case AESNI_PADDING_PKCS7:
            return aesni_fill_with_padding_pkcs7(
                dest, padding_size, err_details);

        default:
            return aesni_error_not_implemented(
                err_details, "unsupported padding method");
    }
}