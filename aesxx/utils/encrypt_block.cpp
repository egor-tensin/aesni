// Copyright (c) 2015 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "AES tools" project.
// For details, see https://github.com/egor-tensin/aes-tools.
// Distributed under the MIT License.

#include "block_cmd_parser.hpp"
#include "block_dumper.hpp"
#include "block_input.hpp"

#include <aesxx/all.hpp>

#include <boost/program_options.hpp>

#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

namespace
{
    template <aes::Algorithm algorithm, aes::Mode mode>
    void encrypt_with_mode(
        const Input& input,
        bool verbose = false)
    {
        typename aes::Types<algorithm>::Block iv;

        if (aes::ModeRequiresInitVector<mode>::value)
        {
            aes::from_string<algorithm>(iv, input.iv);
            if (verbose)
                dump_iv<algorithm>(iv);
        }

        typename aes::Types<algorithm>::Key key;
        aes::from_string<algorithm>(key, input.key);
        if (verbose)
            dump_key<algorithm>(key);

        aes::EncryptWrapper<algorithm, mode> encrypt{key, iv};
        if (verbose)
            dump_wrapper<algorithm, mode>(encrypt);

        for (const auto& input_block_string : input.blocks)
        {
            typename aes::Types<algorithm>::Block plaintext, ciphertext;
            aes::from_string<algorithm>(plaintext, input_block_string);

            encrypt.encrypt_block(plaintext, ciphertext);

            if (verbose)
            {
                dump_plaintext<algorithm>(plaintext);
                dump_ciphertext<algorithm>(ciphertext);
                dump_next_iv<algorithm, mode>(encrypt);
            }
            else
            {
                std::cout << aes::to_string<algorithm>(ciphertext) << '\n';
            }
        }
    }

    template <aes::Algorithm algorithm>
    void encrypt_with_algorithm(
        aes::Mode mode,
        const Input& input,
        bool verbose = false)
    {
        switch (mode)
        {
            case AES_ECB:
                encrypt_with_mode<algorithm, AES_ECB>(input, verbose);
                break;

            case AES_CBC:
                encrypt_with_mode<algorithm, AES_CBC>(input, verbose);
                break;

            case AES_CFB:
                encrypt_with_mode<algorithm, AES_CFB>(input, verbose);
                break;

            case AES_OFB:
                encrypt_with_mode<algorithm, AES_OFB>(input, verbose);
                break;

            case AES_CTR:
                encrypt_with_mode<algorithm, AES_CTR>(input, verbose);
                break;

            default:
                throw std::runtime_error("the selected mode of operation is not implemented");
                break;
        }
    }

    void encrypt_using_cxx_api(
        aes::Algorithm algorithm,
        aes::Mode mode,
        const Input& input,
        bool verbose = false)
    {
        switch (algorithm)
        {
            case AES_AES128:
                encrypt_with_algorithm<AES_AES128>(mode, input, verbose);
                break;

            case AES_AES192:
                encrypt_with_algorithm<AES_AES192>(mode, input, verbose);
                break;

            case AES_AES256:
                encrypt_with_algorithm<AES_AES256>(mode, input, verbose);
                break;

            default:
                throw std::runtime_error("the selected algorithm is not implemented");
                break;
        }
    }

    void encrypt_using_particular_box(
        aes::Box& box,
        const std::vector<std::string>& input_block_strings)
    {
        for (const auto& input_block_string : input_block_strings)
        {
            aes::Box::Block plaintext;
            box.parse_block(plaintext, input_block_string);

            aes::Box::Block ciphertext;
            box.encrypt_block(plaintext, ciphertext);
            std::cout << box.format_block(ciphertext) << '\n';
        }
    }

    void encrypt_using_boxes(
        aes::Algorithm algorithm,
        aes::Mode mode,
        const Input& input)
    {
        aes::Box::Key key;
        aes::Box::parse_key(key, algorithm, input.key);

        if (aes::mode_requires_init_vector(mode))
        {
            aes::Box::Block iv;
            aes::Box::parse_block(iv, algorithm, input.iv);
            aes::Box box{algorithm, key, mode, iv};

            encrypt_using_particular_box(box, input.blocks);
        }
        else
        {
            aes::Box box{algorithm, key};
            encrypt_using_particular_box(box, input.blocks);
        }
    }
}

int main(int argc, char** argv)
{
    try
    {
        BlockSettings settings{argv[0]};

        try
        {
            settings.parse(argc, argv);
        }
        catch (const boost::program_options::error& e)
        {
            settings.usage_error(e);
            return 1;
        }

        if (settings.exit_with_usage)
        {
            settings.usage();
            return 0;
        }

        for (const auto& input : settings.inputs)
        {
            if (settings.use_boxes)
            {
                encrypt_using_boxes(
                    settings.algorithm,
                    settings.mode,
                    input);
            }
            else
            {
                encrypt_using_cxx_api(
                    settings.algorithm,
                    settings.mode,
                    input,
                    settings.verbose);
            }
        }
    }
    catch (const aes::Error& e)
    {
        std::cerr << e;
        return 1;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << "\n";
        return 1;
    }
    return 0;
}
