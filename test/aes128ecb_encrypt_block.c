/**
 * \file
 * \author Egor Tensin <Egor.Tensin@gmail.com>
 * \date 2015
 * \copyright This file is licensed under the terms of the MIT License.
 *            See LICENSE.txt for details.
 */

#include <aesni/all.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void exit_with_usage()
{
    puts("Usage: aes128ecb_encrypt_block.exe KEY0 [PLAIN0...] [-- KEY1 [PLAIN1...]...]");
    exit(EXIT_FAILURE);
}

int main(int argc, char** argv)
{
    for (--argc, ++argv; argc > -1; --argc, ++argv)
    {
        AesNI_Block128 plain, key, cipher;
        AesNI_KeySchedule128 key_schedule;

        if (argc < 1)
            exit_with_usage();

        if (aesni_is_error(aesni_parse_block128(&key, *argv, NULL)))
        {
            fprintf(stderr, "Invalid 128-bit AES block '%s'\n", *argv);
            exit_with_usage();
        }

        aesni_expand_key_schedule128(key, &key_schedule);

        for (--argc, ++argv; argc > 0; --argc, ++argv)
        {
            if (strcmp("--", *argv) == 0)
                break;

            if (aesni_is_error(aesni_parse_block128(&plain, *argv, NULL)))
            {
                fprintf(stderr, "Invalid 128-bit AES block '%s'\n", *argv);
                continue;
            }
            cipher = aesni_encrypt_block_ecb128(plain, &key_schedule);
            aesni_print_block128(&cipher);
        }
    }

    return 0;
}
