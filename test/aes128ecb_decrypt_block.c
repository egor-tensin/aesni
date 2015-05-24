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
    printf("Usage: %s KEY [CIPHER...]\n", argv0);
    exit(EXIT_FAILURE);
}

int main(int argc, char** argv)
{
    __declspec(align(16)) AesBlock128 plain, key, cipher;

    if (argc < 2)
        exit_with_usage(argv[0]);

    if (parse_aes_block128(&key, argv[1]) != 0)
    {
        fprintf(stderr, "Invalid 128-bit AES block '%s'\n", argv[1]);
        exit_with_usage(argv[0]);
    }

    for (int i = 2; i < argc; ++i)
    {
        if (parse_aes_block128(&cipher, argv[i]) != 0)
        {
            fprintf(stderr, "Invalid 128-bit AES block '%s'\n", argv[i]);
            continue;
        }
        plain = aes128ecb_decrypt(cipher, key);
        print_aes_block128(&plain);
    }

    return 0;
}
