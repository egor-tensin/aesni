/**
 * \file
 * \author Egor Tensin <Egor.Tensin@gmail.com>
 * \date 2015
 * \copyright This file is licensed under the terms of the MIT License.
 *            See LICENSE.txt for details.
 */

#pragma once

#include "data.h"
#include "raw.h"

#ifdef __cplusplus
extern "C"
{
#endif

static __inline void __fastcall aes128_expand_key_schedule(
    AesBlock128 key,
    Aes128KeySchedule* key_schedule)
{
    raw_aes128_expand_key_schedule(key, key_schedule);
}

static __inline void __fastcall aes128_invert_key_schedule(
    Aes128KeySchedule* key_schedule,
    Aes128KeySchedule* inverted_schedule)
{
    raw_aes128_invert_key_schedule(key_schedule, inverted_schedule);
}

static __inline AesBlock128 __fastcall aes128ecb_encrypt_block(
    AesBlock128 plain,
    Aes128KeySchedule* key_schedule)
{
    return raw_aes128_encrypt_block(plain, key_schedule);
}

static __inline AesBlock128 __fastcall aes128ecb_decrypt_block(
    AesBlock128 cipher,
    Aes128KeySchedule* inverted_schedule)
{
    return raw_aes128_decrypt_block(cipher, inverted_schedule);
}

static __inline AesBlock128 __fastcall aes128cbc_encrypt_block(
    AesBlock128 plain,
    Aes128KeySchedule* key_schedule,
    AesBlock128 init_vector,
    AesBlock128* next_init_vector)
{
    AesBlock128 cipher = raw_aes128_encrypt_block(_mm_xor_si128(plain, init_vector), key_schedule);
    *next_init_vector = cipher;
    return cipher;
}

static __inline AesBlock128 __fastcall aes128cbc_decrypt_block(
    AesBlock128 cipher,
    Aes128KeySchedule* inverted_schedule,
    AesBlock128 init_vector,
    AesBlock128* next_init_vector)
{
    AesBlock128 plain = _mm_xor_si128(raw_aes128_decrypt_block(cipher, inverted_schedule), init_vector);
    *next_init_vector = cipher;
    return plain;
}

static __inline AesBlock128 __fastcall aes128cfb_encrypt_block(
    AesBlock128 plain,
    Aes128KeySchedule* key_schedule,
    AesBlock128 init_vector,
    AesBlock128* next_init_vector)
{
    AesBlock128 cipher = _mm_xor_si128(raw_aes128_encrypt_block(init_vector, key_schedule), plain);
    *next_init_vector = cipher;
    return cipher;
}

static __inline AesBlock128 __fastcall aes128cfb_decrypt_block(
    AesBlock128 cipher,
    Aes128KeySchedule* key_schedule,
    AesBlock128 init_vector,
    AesBlock128* next_init_vector)
{
    AesBlock128 plain = _mm_xor_si128(raw_aes128_encrypt_block(init_vector, key_schedule), cipher);
    *next_init_vector = cipher;
    return plain;
}

static __inline AesBlock128 __fastcall aes128ofb_encrypt_block(
    AesBlock128 plain,
    Aes128KeySchedule* key_schedule,
    AesBlock128 init_vector,
    AesBlock128* next_init_vector)
{
    AesBlock128 tmp = raw_aes128_encrypt_block(init_vector, key_schedule);
    *next_init_vector = tmp;
    return _mm_xor_si128(tmp, plain);
}

static __inline AesBlock128 __fastcall aes128ofb_decrypt_block(
    AesBlock128 cipher,
    Aes128KeySchedule* key_schedule,
    AesBlock128 init_vector,
    AesBlock128* next_init_vector)
{
    AesBlock128 tmp = raw_aes128_encrypt_block(init_vector, key_schedule);
    *next_init_vector = tmp;
    return _mm_xor_si128(tmp, cipher);
}

static __inline AesBlock128 __fastcall aes128ctr_encrypt_block(
    AesBlock128 plain,
    Aes128KeySchedule* key_schedule,
    AesBlock128 init_vector,
    int counter)
{
    init_vector = aes128_le2be(init_vector);
    init_vector = _mm_add_epi32(init_vector, make_aes_block128(0, 0, 0, counter));
    init_vector = aes128_be2le(init_vector);
    return _mm_xor_si128(plain, raw_aes128_encrypt_block(init_vector, key_schedule));
}

static __inline AesBlock128 __fastcall aes128ctr_decrypt_block(
    AesBlock128 cipher,
    Aes128KeySchedule* key_schedule,
    AesBlock128 init_vector,
    int counter)
{
    init_vector = aes128_le2be(init_vector);
    init_vector = _mm_add_epi32(init_vector, make_aes_block128(0, 0, 0, counter));
    init_vector = aes128_be2le(init_vector);
    return _mm_xor_si128(cipher, raw_aes128_encrypt_block(init_vector, key_schedule));
}

static __inline void __fastcall aes192_expand_key_schedule(
    AesBlock192* key,
    Aes192KeySchedule* key_schedule)
{
    raw_aes192_expand_key_schedule(key->lo, key->hi, key_schedule);
}

static __inline void __fastcall aes192_invert_key_schedule(
    Aes192KeySchedule* key_schedule,
    Aes192KeySchedule* inverted_schedule)
{
    raw_aes192_invert_key_schedule(key_schedule, inverted_schedule);
}

static __inline AesBlock128 __fastcall aes192ecb_encrypt_block(
    AesBlock128 plain,
    Aes192KeySchedule* key_schedule)
{
    return raw_aes192_encrypt_block(plain, key_schedule);
}

static __inline AesBlock128 __fastcall aes192ecb_decrypt_block(
    AesBlock128 cipher,
    Aes192KeySchedule* inverted_schedule)
{
    return raw_aes192_decrypt_block(cipher, inverted_schedule);
}

static __inline AesBlock128 __fastcall aes192cbc_encrypt_block(
    AesBlock128 plain,
    Aes192KeySchedule* key_schedule,
    AesBlock128 init_vector,
    AesBlock128* next_init_vector)
{
    AesBlock128 cipher = raw_aes192_encrypt_block(_mm_xor_si128(plain, init_vector), key_schedule);
    *next_init_vector = cipher;
    return cipher;
}

static __inline AesBlock128 __fastcall aes192cbc_decrypt_block(
    AesBlock128 cipher,
    Aes192KeySchedule* inverted_schedule,
    AesBlock128 init_vector,
    AesBlock128* next_init_vector)
{
    AesBlock128 plain = _mm_xor_si128(raw_aes192_decrypt_block(cipher, inverted_schedule), init_vector);
    *next_init_vector = cipher;
    return plain;
}

static __inline AesBlock128 __fastcall aes192cfb_encrypt_block(
    AesBlock128 plain,
    Aes192KeySchedule* key_schedule,
    AesBlock128 init_vector,
    AesBlock128* next_init_vector)
{
    AesBlock128 cipher = _mm_xor_si128(raw_aes192_encrypt_block(init_vector, key_schedule), plain);
    *next_init_vector = cipher;
    return cipher;
}

static __inline AesBlock128 __fastcall aes192cfb_decrypt_block(
    AesBlock128 cipher,
    Aes192KeySchedule* key_schedule,
    AesBlock128 init_vector,
    AesBlock128* next_init_vector)
{
    AesBlock128 plain = _mm_xor_si128(raw_aes192_encrypt_block(init_vector, key_schedule), cipher);
    *next_init_vector = cipher;
    return plain;
}

static __inline AesBlock128 __fastcall aes192ofb_encrypt_block(
    AesBlock128 plain,
    Aes192KeySchedule* key_schedule,
    AesBlock128 init_vector,
    AesBlock128* next_init_vector)
{
    AesBlock128 tmp = raw_aes192_encrypt_block(init_vector, key_schedule);
    *next_init_vector = tmp;
    return _mm_xor_si128(tmp, plain);
}

static __inline AesBlock128 __fastcall aes192ofb_decrypt_block(
    AesBlock128 cipher,
    Aes192KeySchedule* key_schedule,
    AesBlock128 init_vector,
    AesBlock128* next_init_vector)
{
    AesBlock128 tmp = raw_aes192_encrypt_block(init_vector, key_schedule);
    *next_init_vector = tmp;
    return _mm_xor_si128(tmp, cipher);
}

static __inline AesBlock128 __fastcall aes192ctr_encrypt_block(
    AesBlock128 plain,
    Aes192KeySchedule* key_schedule,
    AesBlock128 init_vector,
    int counter)
{
    init_vector = aes128_le2be(init_vector);
    init_vector = _mm_add_epi32(init_vector, make_aes_block128(0, 0, 0, counter));
    init_vector = aes128_be2le(init_vector);
    return _mm_xor_si128(plain, raw_aes192_encrypt_block(init_vector, key_schedule));
}

static __inline AesBlock128 __fastcall aes192ctr_decrypt_block(
    AesBlock128 cipher,
    Aes192KeySchedule* key_schedule,
    AesBlock128 init_vector,
    int counter)
{
    init_vector = aes128_le2be(init_vector);
    init_vector = _mm_add_epi32(init_vector, make_aes_block128(0, 0, 0, counter));
    init_vector = aes128_be2le(init_vector);
    return _mm_xor_si128(cipher, raw_aes192_encrypt_block(init_vector, key_schedule));
}

static __inline void __fastcall aes256_expand_key_schedule(
    AesBlock256* key,
    Aes256KeySchedule* key_schedule)
{
    raw_aes256_expand_key_schedule(key->lo, key->hi, key_schedule);
}

static __inline void __fastcall aes256_invert_key_schedule(
    Aes256KeySchedule* key_schedule,
    Aes256KeySchedule* inverted_schedule)
{
    raw_aes256_invert_key_schedule(key_schedule, inverted_schedule);
}

static __inline AesBlock128 __fastcall aes256ecb_encrypt_block(
    AesBlock128 plain,
    Aes256KeySchedule* key_schedule)
{
    return raw_aes256_encrypt_block(plain, key_schedule);
}

static __inline AesBlock128 __fastcall aes256ecb_decrypt_block(
    AesBlock128 cipher,
    Aes256KeySchedule* inverted_schedule)
{
    return raw_aes256_decrypt_block(cipher, inverted_schedule);
}

static __inline AesBlock128 __fastcall aes256cbc_encrypt_block(
    AesBlock128 plain,
    Aes256KeySchedule* key_schedule,
    AesBlock128 init_vector,
    AesBlock128* next_init_vector)
{
    AesBlock128 cipher = raw_aes256_encrypt_block(_mm_xor_si128(plain, init_vector), key_schedule);
    *next_init_vector = cipher;
    return cipher;
}

static __inline AesBlock128 __fastcall aes256cbc_decrypt_block(
    AesBlock128 cipher,
    Aes256KeySchedule* inverted_schedule,
    AesBlock128 init_vector,
    AesBlock128* next_init_vector)
{
    AesBlock128 plain = _mm_xor_si128(raw_aes256_decrypt_block(cipher, inverted_schedule), init_vector);
    *next_init_vector = cipher;
    return plain;
}

static __inline AesBlock128 __fastcall aes256cfb_encrypt_block(
    AesBlock128 plain,
    Aes256KeySchedule* key_schedule,
    AesBlock128 init_vector,
    AesBlock128* next_init_vector)
{
    AesBlock128 cipher = _mm_xor_si128(raw_aes256_encrypt_block(init_vector, key_schedule), plain);
    *next_init_vector = cipher;
    return cipher;
}

static __inline AesBlock128 __fastcall aes256cfb_decrypt_block(
    AesBlock128 cipher,
    Aes256KeySchedule* key_schedule,
    AesBlock128 init_vector,
    AesBlock128* next_init_vector)
{
    AesBlock128 plain = _mm_xor_si128(raw_aes256_encrypt_block(init_vector, key_schedule), cipher);
    *next_init_vector = cipher;
    return plain;
}

static __inline AesBlock128 __fastcall aes256ofb_encrypt_block(
    AesBlock128 plain,
    Aes256KeySchedule* key_schedule,
    AesBlock128 init_vector,
    AesBlock128* next_init_vector)
{
    AesBlock128 tmp = raw_aes256_encrypt_block(init_vector, key_schedule);
    *next_init_vector = tmp;
    return _mm_xor_si128(tmp, plain);
}

static __inline AesBlock128 __fastcall aes256ofb_decrypt_block(
    AesBlock128 cipher,
    Aes256KeySchedule* key_schedule,
    AesBlock128 init_vector,
    AesBlock128* next_init_vector)
{
    AesBlock128 tmp = raw_aes256_encrypt_block(init_vector, key_schedule);
    *next_init_vector = tmp;
    return _mm_xor_si128(tmp, cipher);
}

static __inline AesBlock128 __fastcall aes256ctr_encrypt_block(
    AesBlock128 plain,
    Aes256KeySchedule* key_schedule,
    AesBlock128 init_vector,
    int counter)
{
    init_vector = aes128_le2be(init_vector);
    init_vector = _mm_add_epi32(init_vector, make_aes_block128(0, 0, 0, counter));
    init_vector = aes128_be2le(init_vector);
    return _mm_xor_si128(plain, raw_aes256_encrypt_block(init_vector, key_schedule));
}

static __inline AesBlock128 __fastcall aes256ctr_decrypt_block(
    AesBlock128 cipher,
    Aes256KeySchedule* key_schedule,
    AesBlock128 init_vector,
    int counter)
{
    init_vector = aes128_le2be(init_vector);
    init_vector = _mm_add_epi32(init_vector, make_aes_block128(0, 0, 0, counter));
    init_vector = aes128_be2le(init_vector);
    return _mm_xor_si128(cipher, raw_aes256_encrypt_block(init_vector, key_schedule));
}

#ifdef __cplusplus
}
#endif