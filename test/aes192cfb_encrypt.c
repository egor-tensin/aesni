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

static void exit_with_usage(const char* argv0)
{
    printf("Usage: %s KEY INIT_VECTOR [PLAIN...]\n", argv0);
    exit(EXIT_FAILURE);
}

int main(int argc, char** argv)
{
    __declspec(align(16)) AesBlock128 plain, cipher, iv;
    __declspec(align(16)) AesBlock192 key;
    __declspec(align(16)) Aes192KeySchedule key_schedule;

    if (argc < 3)
        exit_with_usage(argv[0]);

    if (parse_aes_block192(&key, argv[1]) != 0)
    {
        fprintf(stderr, "Invalid 192-bit AES block '%s'\n", argv[1]);
        exit_with_usage(argv[0]);
    }

    if (parse_aes_block128(&iv, argv[2]) != 0)
    {
        fprintf(stderr, "Invalid 128-bit AES block '%s'\n", argv[2]);
        exit_with_usage(argv[0]);
    }

    aes192_expand_key_schedule(&key, &key_schedule);

    for (int i = 3; i < argc; ++i)
    {
        if (parse_aes_block128(&plain, argv[i]) != 0)
        {
            fprintf(stderr, "Invalid 128-bit AES block '%s'\n", argv[i]);
            continue;
        }
        cipher = aes192cfb_encrypt(plain, &key_schedule, iv, &iv);
        print_aes_block128(&cipher);
    }

    return 0;
}