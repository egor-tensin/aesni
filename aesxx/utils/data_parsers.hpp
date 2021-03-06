// Copyright (c) 2015 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "AES tools" project.
// For details, see https://github.com/egor-tensin/aes-tools.
// Distributed under the MIT License.

#pragma once

#include <aesxx/all.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>

#include <istream>
#include <string>
#include <unordered_map>

inline std::istream& operator>>(std::istream& is, aes::Mode& dest)
{
    std::string src;
    is >> src;

    static const std::unordered_map<std::string, aes::Mode> lookup_table =
    {
        {"ecb", AES_ECB},
        {"cbc", AES_CBC},
        {"cfb", AES_CFB},
        {"ofb", AES_OFB},
        {"ctr", AES_CTR},
    };

    const auto it = lookup_table.find(boost::algorithm::to_lower_copy(src));

    if (it == lookup_table.cend())
        throw boost::program_options::invalid_option_value(src);

    dest = it->second;
    return is;
}

inline std::istream& operator>>(std::istream& is, aes::Algorithm& dest)
{
    std::string src;
    is >> src;

    static const std::unordered_map<std::string, aes::Algorithm> lookup_table =
    {
        {"aes128", AES_AES128},
        {"aes192", AES_AES192},
        {"aes256", AES_AES256},
    };

    const auto it = lookup_table.find(boost::algorithm::to_lower_copy(src));

    if (it == lookup_table.cend())
        throw boost::program_options::invalid_option_value(src);

    dest = it->second;
    return is;
}
