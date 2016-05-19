/**
 * \file
 * \author Egor Tensin <Egor.Tensin@gmail.com>
 * \date 2015
 * \copyright This file is licensed under the terms of the MIT License.
 *            See LICENSE.txt for details.
 */

#include "file_cmd_parser.hpp"

#include <aesxx/all.hpp>

#include <boost/program_options.hpp>

#include <cstdlib>
#include <cstring>

#include <exception>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include <Windows.h>

namespace
{
    std::ifstream::pos_type get_file_size(const std::string& path)
    {
        std::ifstream ifs;
        ifs.exceptions(std::ifstream::badbit | std::ifstream::failbit);
        ifs.open(path, std::ifstream::binary | std::ifstream::ate);
        return ifs.tellg();
    }

    std::vector<char> read_file(const std::string& path)
    {
        const auto size = static_cast<std::size_t>(get_file_size(path));

        std::ifstream ifs;
        ifs.exceptions(std::ifstream::badbit | std::ifstream::failbit);
        ifs.open(path, std::ifstream::binary);

        std::vector<char> ciphertext_buf;
        ciphertext_buf.reserve(size);
        ciphertext_buf.assign(
            std::istreambuf_iterator<char>(ifs),
            std::istreambuf_iterator<char>());
        return ciphertext_buf;
    }

    void write_file(
        const std::string& path,
        const std::vector<unsigned char>& src)
    {
        std::ofstream ofs;
        ofs.exceptions(std::ofstream::badbit | std::ofstream::failbit);
        ofs.open(path, std::ofstream::binary);
        ofs.write(reinterpret_cast<const char*>(src.data()), src.size());
    }

    void decrypt_bmp(
        aesni::Box& box,
        const std::string& ciphertext_path,
        const std::string& plaintext_path)
    {
        const auto ciphertext_buf = read_file(ciphertext_path);

        const auto bmp_header = reinterpret_cast<const BITMAPFILEHEADER*>(ciphertext_buf.data());

        const auto header_size = bmp_header->bfOffBits;
        const auto cipherpixels = ciphertext_buf.data() + header_size;
        const auto cipherpixels_size = ciphertext_buf.size() - header_size;

        const auto pixels = box.decrypt_buffer(
            cipherpixels, cipherpixels_size);

        std::vector<unsigned char> plaintext_buf(header_size + pixels.size());
        std::memcpy(plaintext_buf.data(), bmp_header, header_size);
        std::memcpy(plaintext_buf.data() + header_size, pixels.data(), pixels.size());

        write_file(plaintext_path, plaintext_buf);
    }

    void decrypt_bmp(const Settings& settings)
    {
        const auto algorithm = settings.get_algorithm();
        const auto mode = settings.get_mode();

        const auto& ciphertext_path = settings.get_input_path();
        const auto& plaintext_path = settings.get_output_path();

        aesni::Box::Key key;
        aesni::Box::parse_key(key, algorithm, settings.get_key_string());

        if (aesni::mode_requires_initialization_vector(mode))
        {
            aesni::Box::Block iv;
            aesni::Box::parse_block(iv, algorithm, settings.get_iv_string());

            decrypt_bmp(
                aesni::Box(algorithm, key, mode, iv),
                ciphertext_path,
                plaintext_path);
        }
        else
        {
            decrypt_bmp(
                aesni::Box(algorithm, key),
                ciphertext_path,
                plaintext_path);
        }
    }
}

int main(int argc, char** argv)
{
    try
    {
        CommandLineParser cmd_parser(argv[0]);
        try
        {
            Settings settings;
            cmd_parser.parse(settings, argc, argv);

            if (cmd_parser.exit_with_usage())
            {
                std::cout << cmd_parser;
                return 0;
            }

            decrypt_bmp(settings);
        }
        catch (const boost::program_options::error& e)
        {
            std::cerr << "Usage error: " << e.what() << "\n";
            std::cerr << cmd_parser;
            return 1;
        }
        catch (const aesni::Error& e)
        {
            std::cerr << e;
            return 1;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << "\n";
        return 1;
    }
    return 0;
}
