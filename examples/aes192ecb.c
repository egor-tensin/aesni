/**
 * \file
 * \author Egor Tensin <Egor.Tensin@gmail.com>
 * \date 2015
 * \copyright This file is licensed under the terms of the MIT License.
 *            See LICENSE.txt for details.
 */

#include <aesni/all.h>

#include <stdio.h>

int main()
{
    __declspec(align(16)) AesBlock128 plain, cipher, decrypted;
    __declspec(align(16)) AesBlock192 key;
    __declspec(align(16)) Aes192KeySchedule key_schedule, inverted_schedule;

    plain = make_aes_block128(0xffeeddcc, 0xbbaa9988, 0x77665544, 0x33221100);
    key = make_aes_block192(0x17161514, 0x13121110, 0x0f0e0d0c, 0x0b0a0908, 0x07060504, 0x03020100);

    printf("Plain: %s\n", format_aes_block128(&plain).str);
    print_aes_block128_as_matrix(&plain);

    printf("\n");
    printf("Key: %s\n", format_aes_block192(&key).str);
    print_aes_block192_as_matrix(&key);

    aes192_expand_key_schedule(&key, &key_schedule);

    printf("\n");
    printf("Key schedule:\n");
    for (int i = 0; i < 13; ++i)
        printf("\t[%d]: %s\n", i, format_aes_block128(&key_schedule.keys[i]).str);

    cipher = aes192ecb_encrypt(plain, &key_schedule);
    printf("\n");
    printf("Cipher: %s\n", format_aes_block128(&cipher).str);
    print_aes_block128_as_matrix(&cipher);

    aes192_invert_key_schedule(&key_schedule, &inverted_schedule);

    printf("\n");
    printf("Inverted key schedule:\n");
    for (int i = 0; i < 13; ++i)
        printf("\t[%d]: %s\n", i, format_aes_block128(&inverted_schedule.keys[i]).str);

    decrypted = aes192ecb_decrypt(cipher, &inverted_schedule);
    printf("\n");
    printf("Decrypted: %s\n", format_aes_block128(&decrypted).str);
    print_aes_block128_as_matrix(&decrypted);

    return 0;
}