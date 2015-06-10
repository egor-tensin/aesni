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
    puts("Usage: aes192cfb_decrypt_block.exe KEY0 IV0 [CIPHER0...] [-- KEY1 IV1 [CIPHER1...]...]");
    exit(EXIT_FAILURE);
}

int main(int argc, char** argv)
{
    for (--argc, ++argv; argc > -1; --argc, ++argv)
    {
        AesBlock128 plain, cipher, iv;
        AesBlock192 key;
        Aes192KeySchedule key_schedule;

        if (argc < 2)
            exit_with_usage();

        if (parse_aes_block192(&key, *argv) != 0)
        {
            fprintf(stderr, "Invalid 192-bit AES block '%s'\n", *argv);
            exit_with_usage();
        }

        if (parse_aes_block128(&iv, argv[1]) != 0)
        {
            fprintf(stderr, "Invalid 128-bit AES block '%s'\n", argv[1]);
            exit_with_usage();
        }

        aes192_expand_key_schedule(&key, &key_schedule);

        for (argc -= 2, argv += 2; argc > 0; --argc, ++argv)
        {
            if (strcmp("--", *argv) == 0)
                break;

            if (parse_aes_block128(&cipher, *argv) != 0)
            {
                fprintf(stderr, "Invalid 128-bit AES block '%s'\n", *argv);
                continue;
            }
            plain = aes192cfb_decrypt_block(cipher, &key_schedule, iv, &iv);
            print_aes_block128(&plain);
        }
    }

    return 0;
}