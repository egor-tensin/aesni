/**
 * \file
 * \author Egor Tensin <Egor.Tensin@gmail.com>
 * \date 2015
 * \copyright This file is licensed under the terms of the MIT License.
 *            See LICENSE.txt for details.
 */

#pragma once

#include <aes/all.h>

#include <type_traits>

namespace aes
{
    typedef AES_Mode Mode;

    template <Mode mode>
    struct ModeRequiresInitializationVector : public std::true_type
    { };

    template <>
    struct ModeRequiresInitializationVector<AES_ECB> : public std::false_type
    { };

    template <Mode mode>
    struct ModeUsesEncryptionKeysOnly : public std::true_type
    { };

    inline bool mode_requires_initialization_vector(Mode mode)
    {
        return mode != AES_ECB;
    }

    template <>
    struct ModeUsesEncryptionKeysOnly<AES_ECB> : public std::false_type
    { };

    template <>
    struct ModeUsesEncryptionKeysOnly<AES_CBC> : public std::false_type
    { };

    inline bool mode_uses_encryption_keys_only(Mode mode)
    {
        return mode != AES_ECB && mode != AES_CBC;
    }

#define AESXX_ENCRYPT_BLOCK_ECB(prefix) \
    template <> \
    inline void encrypt_block<AES_## prefix, AES_ECB>( \
        const typename Types<AES_## prefix>::Block& plaintext, \
        const typename Types<AES_## prefix>::RoundKeys& encryption_keys, \
        typename Types<AES_## prefix>::Block& ciphertext) \
    { \
        ciphertext = aes_## prefix ##_encrypt_block_ECB(plaintext, &encryption_keys); \
    }

#define AESXX_DECRYPT_BLOCK_ECB(prefix) \
    template <> \
    inline void decrypt_block<AES_## prefix, AES_ECB>( \
        const typename Types<AES_## prefix>::Block& ciphertext, \
        const typename Types<AES_## prefix>::RoundKeys& decryption_keys, \
        typename Types<AES_## prefix>::Block& plaintext) \
    { \
        plaintext = aes_## prefix ##_decrypt_block_ECB(ciphertext, &decryption_keys); \
    }

#define AESXX_ENCRYPT_BLOCK_CBC(prefix) \
    template <> \
    inline void encrypt_block<AES_## prefix, AES_CBC>( \
        const typename Types<AES_## prefix>::Block& plaintext, \
        const typename Types<AES_## prefix>::RoundKeys& encryption_keys, \
        typename Types<AES_## prefix>::Block& iv, \
        typename Types<AES_## prefix>::Block& ciphertext) \
    { \
        ciphertext = aes_## prefix ##_encrypt_block_CBC(plaintext, &encryption_keys, iv, &iv); \
    }

#define AESXX_DECRYPT_BLOCK_CBC(prefix) \
    template <> \
    inline void decrypt_block<AES_## prefix, AES_CBC>( \
        const typename Types<AES_## prefix>::Block& ciphertext, \
        const typename Types<AES_## prefix>::RoundKeys& decryption_keys, \
        typename Types<AES_## prefix>::Block& iv, \
        typename Types<AES_## prefix>::Block& plaintext) \
    { \
        plaintext = aes_## prefix ##_decrypt_block_CBC(ciphertext, &decryption_keys, iv, &iv); \
    }

#define AESXX_ENCRYPT_BLOCK_CFB(prefix) \
    template <> \
    inline void encrypt_block<AES_## prefix, AES_CFB>( \
        const typename Types<AES_## prefix>::Block& plaintext, \
        const typename Types<AES_## prefix>::RoundKeys& encryption_keys, \
        typename Types<AES_## prefix>::Block& iv, \
        typename Types<AES_## prefix>::Block& ciphertext) \
    { \
        ciphertext = aes_## prefix ##_encrypt_block_CFB(plaintext, &encryption_keys, iv, &iv); \
    }

#define AESXX_DECRYPT_BLOCK_CFB(prefix) \
    template <> \
    inline void decrypt_block<AES_## prefix, AES_CFB>( \
        const typename Types<AES_## prefix>::Block& ciphertext, \
        const typename Types<AES_## prefix>::RoundKeys& encryption_keys, \
        typename Types<AES_## prefix>::Block& iv, \
        typename Types<AES_## prefix>::Block& plaintext) \
    { \
        plaintext = aes_## prefix ##_decrypt_block_CFB(ciphertext, &encryption_keys, iv, &iv); \
    }

#define AESXX_ENCRYPT_BLOCK_OFB(prefix) \
    template <> \
    inline void encrypt_block<AES_## prefix, AES_OFB>( \
        const typename Types<AES_## prefix>::Block& plaintext, \
        const typename Types<AES_## prefix>::RoundKeys& encryption_keys, \
        typename Types<AES_## prefix>::Block& iv, \
        typename Types<AES_## prefix>::Block& ciphertext) \
    { \
        ciphertext = aes_## prefix ##_encrypt_block_OFB(plaintext, &encryption_keys, iv, &iv); \
    }

#define AESXX_DECRYPT_BLOCK_OFB(prefix) \
    template <> \
    inline void decrypt_block<AES_## prefix, AES_OFB>( \
        const typename Types<AES_## prefix>::Block& ciphertext, \
        const typename Types<AES_## prefix>::RoundKeys& encryption_keys, \
        typename Types<AES_## prefix>::Block& iv, \
        typename Types<AES_## prefix>::Block& plaintext) \
    { \
        plaintext = aes_## prefix ##_decrypt_block_OFB(ciphertext, &encryption_keys, iv, &iv); \
    }

#define AESXX_ENCRYPT_BLOCK_CTR(prefix) \
    template <> \
    inline void encrypt_block<AES_## prefix, AES_CTR>( \
        const typename Types<AES_## prefix>::Block& plaintext, \
        const typename Types<AES_## prefix>::RoundKeys& encryption_keys, \
        typename Types<AES_## prefix>::Block& iv, \
        typename Types<AES_## prefix>::Block& ciphertext) \
    { \
        ciphertext = aes_## prefix ##_encrypt_block_CTR(plaintext, &encryption_keys, iv, &iv); \
    }

#define AESXX_DECRYPT_BLOCK_CTR(prefix) \
    template <> \
    inline void decrypt_block<AES_## prefix, AES_CTR>( \
        const typename Types<AES_## prefix>::Block& ciphertext, \
        const typename Types<AES_## prefix>::RoundKeys& encryption_keys, \
        typename Types<AES_## prefix>::Block& iv, \
        typename Types<AES_## prefix>::Block& plaintext) \
    { \
        plaintext = aes_## prefix ##_decrypt_block_CTR(ciphertext, &encryption_keys, iv, &iv); \
    }
}
