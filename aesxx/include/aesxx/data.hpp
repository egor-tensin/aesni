// Copyright (c) 2015 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "AES tools" project.
// For details, see https://github.com/egor-tensin/aes-tools.
// Distributed under the MIT License.

#pragma once

#include "error.hpp"

#include <aes/all.h>

namespace aes
{
    typedef AES_Block128 Block128;

    inline void make_block(Block128& dest, int hi3, int hi2, int lo1, int lo0)
    {
        dest = aes_make_block128(hi3, hi2, lo1, lo0);
    }

    inline void load_block(Block128& dest, const void* src)
    {
        dest = aes_load_block128(src);
    }

    inline void load_block_aligned(Block128& dest, const void* src)
    {
        dest = aes_load_block128_aligned(src);
    }

    inline void store_block(void* dest, Block128& src)
    {
        aes_store_block128(dest, src);
    }

    inline void store_block_aligned(void* dest, Block128& src)
    {
        aes_store_block128_aligned(dest, src);
    }

    inline Block128 xor_blocks(Block128& a, Block128& b)
    {
        return aes_xor_block128(a, b);
    }

    inline Block128 reverse_byte_order(Block128& block)
    {
        return aes_reverse_byte_order_block128(block);
    }

    inline Block128 inc_block(Block128& block)
    {
        return aes_inc_block128(block);
    }
}
