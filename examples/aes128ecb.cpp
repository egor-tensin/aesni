/**
 * \file
 * \author Egor Tensin <Egor.Tensin@gmail.com>
 * \date 2015
 * \copyright This file is licensed under the terms of the MIT License.
 *            See LICENSE.txt for details.
 */

#include "common.hpp"

#include <aesni/all.h>

#include <aesnixx/all.hpp>

#include <exception>
#include <iostream>

int main()
{
    try
    {
        aesni::Block128 plaintext;
        make_default_plaintext(plaintext);

        aesni::Block128 key;
        make_default_key(key);

        aesni::KeySchedule128 encryption_schedule;
        aesni_aes128_expand_key(key, &encryption_schedule);
        dump_encryption_schedule(encryption_schedule);

        const auto ciphertext = aesni_aes128_encrypt_block_ecb(plaintext, &encryption_schedule);
        dump_ciphertext(ciphertext);

        aesni::KeySchedule128 decryption_schedule;
        aesni_aes128_derive_decryption_keys(&encryption_schedule, &decryption_schedule);
        dump_decryption_schedule(decryption_schedule);

        const auto decrypted = aesni_aes128_decrypt_block_ecb(ciphertext, &decryption_schedule);
        dump_decrypted(decrypted);

        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << "\n";
        return 1;
    }
}