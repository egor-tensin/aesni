# Copyright 2015 Egor Tensin <Egor.Tensin@gmail.com>
# This file is licensed under the terms of the MIT License.
# See LICENSE.txt for details.

from contextlib import contextmanager
from datetime import datetime
from enum import Enum
from glob import iglob as glob
import filecmp
import logging
import os
import shutil
import sys
from tempfile import NamedTemporaryFile

from toolkit import *

class TestExitCode(Enum):
    SUCCESS, FAILURE, ERROR, SKIPPED = range(1, 5)

_KEY_EXT = 'key'
_IV_EXT = 'iv'
_PLAIN_EXT = 'plain'
_CIPHER_EXT = 'cipher'

def _list_dirs(root_path):
    xs = map(lambda x: os.path.join(root_path, x), os.listdir(root_path))
    return filter(os.path.isdir, xs)

def _list_files(root_path, ext):
    xs = glob(os.path.join(root_path, '*.{}'.format(ext)))
    return filter(os.path.isfile, xs)

def _list_keys(root_path):
    return _list_files(root_path, _KEY_EXT)

def _read_first_line(path):
    with open(path) as f:
        return f.readline()

def _read_key(key_path):
    return _read_first_line(key_path)

def _read_iv(iv_path):
    return _read_first_line(iv_path)

def _extract_test_name(key_path):
    return os.path.splitext(os.path.basename(key_path))[0]

def _replace_ext(path, new_ext):
    return '{}.{}'.format(os.path.splitext(path)[0], new_ext)

def _extract_iv_path(key_path):
    return _replace_ext(key_path, _IV_EXT)

def _extract_plaintext_path(key_path):
    return _replace_ext(key_path, _PLAIN_EXT)

def _extract_ciphertext_path(key_path):
    return _replace_ext(key_path, _CIPHER_EXT)

@contextmanager
def _make_output_file():
    with NamedTemporaryFile(delete=False) as tmp_file:
        tmp_path = tmp_file.name
        yield tmp_path
    os.remove(tmp_path)

def run_encryption_test(tools, algorithm, mode, key, plaintext_path,
                        ciphertext_path, iv=None, force=False):
    logging.info('Running encryption test...')
    logging.info('\tPlaintext file path: ' + plaintext_path)
    logging.info('\tExpected ciphertext file path: ' + ciphertext_path)
    logging.info('\tAlgorithm: ' + str(algorithm))
    logging.info('\tMode: ' + str(mode))

    with _make_output_file() as tmp_path:
        logging.info('\tEncrypted file path: ' + tmp_path)

        try:
            tools.run_encrypt_file(algorithm, mode, key, plaintext_path,
                                   tmp_path, iv)
            if force:
                logging.warn('Overwriting expected ciphertext file')
                shutil.copy(tmp_path, ciphertext_path)
                return TestExitCode.SKIPPED
            if filecmp.cmp(ciphertext_path, tmp_path):
                return TestExitCode.SUCCESS
            else:
                logging.error('The encrypted file doesn\'t match the ciphertext file')
                return TestExitCode.FAILURE
        except Exception as e:
            logging.error('Encountered an exception!')
            logging.exception(e)
            return TestExitCode.ERROR

def run_decryption_test(tools, algorithm, mode, key, plaintext_path,
                        ciphertext_path, iv=None):
    logging.info('Running decryption test...')
    logging.info('\tCiphertext file path: ' + ciphertext_path)
    logging.info('\tExpected plaintext file path: ' + plaintext_path)
    logging.info('\tAlgorithm: ' + str(algorithm))
    logging.info('\tMode: ' + str(mode))

    with _make_output_file() as tmp_path:
        logging.info('\tDecrypted file path: ' + tmp_path)

        try:
            tools.run_decrypt_file(algorithm, mode, key, ciphertext_path,
                                   tmp_path, iv)
            if filecmp.cmp(tmp_path, plaintext_path):
                return TestExitCode.SUCCESS
            else:
                logging.error('The decrypted file doesn\'t match the plaintext file')
                return TestExitCode.FAILURE
        except Exception as e:
            logging.error('Encountered an exception!')
            logging.exception(e)
            return TestExitCode.ERROR

def enum_tests(suite_dir):
    suite_dir = os.path.abspath(suite_dir)
    logging.info('Suite directory path: ' + suite_dir)
    for algorithm_dir in _list_dirs(suite_dir):
        algorithm = os.path.basename(algorithm_dir)
        maybe_algorithm = Algorithm.try_parse(algorithm)
        if maybe_algorithm is None:
            logging.warn('Unknown or unsupported algorithm: ' + algorithm)
            continue
        algorithm = maybe_algorithm
        for mode_dir in _list_dirs(algorithm_dir):
            mode = os.path.basename(mode_dir)
            maybe_mode = Mode.try_parse(mode)
            if maybe_mode is None:
                logging.warn('Unknown or unsupported mode: ' + mode)
                continue
            mode = maybe_mode
            for key_path in _list_keys(mode_dir):
                key = _read_key(key_path)
                logging.info('Key: ' + key)
                test_name = _extract_test_name(key_path)
                logging.info('Test name: ' + test_name)
                iv = None
                if mode.requires_init_vector():
                    iv_path = _extract_iv_path(key_path)
                    iv = _read_iv(iv_path)
                plaintext_path = _extract_plaintext_path(key_path)
                ciphertext_path = _extract_ciphertext_path(key_path)
                yield algorithm, mode, key, plaintext_path, ciphertext_path, iv

def _build_default_log_path():
    return datetime.now().strftime('{}_%Y-%m-%d_%H-%M-%S.log').format(
        os.path.splitext(os.path.basename(__file__))[0])

if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument('--path', '-p', nargs='*',
                        help='set path to file encryption utilities')
    parser.add_argument('--sde', '-e', action='store_true',
                        help='use Intel SDE to run *.exe files')
    parser.add_argument('--log', '-l', default=_build_default_log_path(),
                        help='set log file path')
    parser.add_argument('--force', '-f', action='store_true',
                        help='overwrite ciphertext files')
    parser.add_argument('--suite', '-s', default='file',
                        help='set test suite directory path')
    args = parser.parse_args()

    logging.basicConfig(filename=args.log,
                        format='%(asctime)s | %(module)s | %(levelname)s | %(message)s',
                        level=logging.DEBUG)

    tools = Tools(args.path, use_sde=args.sde)
    exit_codes = []

    for test in enum_tests(args.suite):
        exit_codes.append(run_encryption_test(tools, *test, args.force))
        exit_codes.append(run_decryption_test(tools, *test))

    logging.info('Test exit codes:')
    logging.info('\tSkipped:   {}'.format(exit_codes.count(TestExitCode.SKIPPED)))
    logging.info('\tError(s):  {}'.format(exit_codes.count(TestExitCode.ERROR)))
    logging.info('\tSucceeded: {}'.format(exit_codes.count(TestExitCode.SUCCESS)))
    logging.info('\tFailed:    {}'.format(exit_codes.count(TestExitCode.FAILURE)))
    if (exit_codes.count(TestExitCode.ERROR) == 0 and
            exit_codes.count(TestExitCode.FAILURE) == 0):
        sys.exit()
    else:
        sys.exit(1)
