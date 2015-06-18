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

static unsigned char FULL_BLOCK_PADDING[16] = { 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 };

AesNI_StatusCode aesni_encrypt_buffer_ecb128(
    const void* src,
    size_t src_size,
    void* dest,
    size_t* dest_size,
    AesNI_Aes128_RoundKeys* encryption_keys,
    AesNI_ErrorDetails* err_details)
{
    if (dest_size == NULL)
        return aesni_error_null_argument(err_details, "dest_size");

    const size_t rem_size = src_size % 16;
    const size_t padding_size = 16 - rem_size;
    *dest_size = src_size + padding_size;

    if (dest == NULL)
        return AESNI_SUCCESS;
    if (src == NULL)
        return aesni_error_null_argument(err_details, "src");
    if (encryption_keys == NULL)
        return aesni_error_null_argument(err_details, "encryption_keys");

    const size_t src_len = src_size / 16;

    for (size_t i = 0; i < src_len; ++i, (char*) src += 16, (char*) dest += 16)
    {
        AesNI_Block128 plaintext = aesni_load_block128(src);
        AesNI_Block128 ciphertext = aesni_aes128_encrypt_block_ecb(plaintext, encryption_keys);
        aesni_store_block128(dest, ciphertext);
    }

    unsigned char padding[16];

    if (rem_size == 0)
    {
        memcpy(padding, FULL_BLOCK_PADDING, 16);
    }
    else
    {
        memcpy(padding, src, rem_size);
        memset(padding + rem_size, padding_size, padding_size);
    }

    AesNI_Block128 plaintext = aesni_load_block128(padding);
    AesNI_Block128 ciphertext = aesni_aes128_encrypt_block_ecb(plaintext, encryption_keys);
    aesni_store_block128(dest, ciphertext);

    return AESNI_SUCCESS;
}

static unsigned char get_pkcs7_padding_size(const unsigned char* padding)
{
    if (padding[15] < 0x01 || padding[15] > 0x10)
        return 0;

    for (int i = 16 - padding[15]; i < 15; ++i)
        if (padding[i] != padding[15])
            return 0;

    return padding[15];
}

AesNI_StatusCode aesni_decrypt_buffer_ecb128(
    const void* src,
    size_t src_size,
    void* dest,
    size_t* dest_size,
    AesNI_Aes128_RoundKeys* decryption_keys,
    AesNI_ErrorDetails* err_details)
{
    if (dest_size == NULL)
        return aesni_error_null_argument(err_details, "dest_size");

    *dest_size = src_size;

    if (dest == NULL)
        return 0;
    if (src == NULL)
        return aesni_error_null_argument(err_details, "src");
    if (decryption_keys == NULL)
        return aesni_error_null_argument(err_details, "decryption_keys");

    const size_t src_len = src_size / 16;

    for (size_t i = 0; i < src_len - 1; ++i, (char*) src += 16, (char*) dest += 16)
    {
        AesNI_Block128 ciphertext = aesni_load_block128(src);
        AesNI_Block128 plaintext = aesni_aes128_decrypt_block_ecb(ciphertext, decryption_keys);
        aesni_store_block128(dest, plaintext);
    }

    AesNI_Block128 ciphertext = aesni_load_block128(src);
    AesNI_Block128 plaintext = aesni_aes128_decrypt_block_ecb(ciphertext, decryption_keys);
    unsigned char padding[16];
    aesni_store_block128(padding, plaintext);

    unsigned char padding_size = get_pkcs7_padding_size(padding);

    if (padding_size == 0)
        return aesni_error_invalid_pkcs7_padding(err_details);

    memcpy(dest, padding, 16 - padding_size);
    *dest_size -= padding_size;
    return AESNI_SUCCESS;
}
