#!/usr/bin/env python
# Copyright 2015, The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from __future__ import print_function

from argparse import ArgumentParser, FileType, Action
from hashlib import sha1
from os import fstat
import re
from struct import pack


BOOT_IMAGE_HEADER_V3_PAGESIZE = 4096

def filesize(f):
    if f is None:
        return 0
    try:
        return fstat(f.fileno()).st_size
    except OSError:
        return 0


def update_sha(sha, f):
    if f:
        sha.update(f.read())
        f.seek(0)
        sha.update(pack('I', filesize(f)))
    else:
        sha.update(pack('I', 0))


def pad_file(f, padding):
    pad = (padding - (f.tell() & (padding - 1))) & (padding - 1)
    f.write(pack(str(pad) + 'x'))

def write_header(args):
    BOOT_MAGIC = 'ANDROID!'.encode()

    args.output.write(pack('8s', BOOT_MAGIC))
    final_ramdisk_offset = (args.base + args.ramdisk_offset) if filesize(args.ramdisk) > 0 else 0
    if not args.recovery:
        args.output.write(pack(
            '10I',
            filesize(args.kernel),                          # size in bytes
            args.base + args.kernel_offset,                 # physical load addr
            filesize(args.ramdisk),                         # size in bytes
            final_ramdisk_offset,                           # physical load addr
            0,                                              # size in bytes
            0,                                              # physical load addr
            args.base + args.tags_offset,                   # physical addr for kernel tags
            args.pagesize,                                  # flash page size we assume
            filesize(args.dt),                              # dt size in bytes
            (args.os_version << 11) | args.os_patch_level)) # os version and patch level
    else:
        args.output.write(pack(
            '10I',
            filesize(args.kernel),                          # size in bytes
            args.base + args.kernel_offset,                 # physical load addr
            0,                                              # size in bytes
            0,                                              # physical load addr
            filesize(args.ramdisk),                         # size in bytes
            final_ramdisk_offset,                           # physical load addr
            args.base + args.tags_offset,                   # physical addr for kernel tags
            args.pagesize,                                  # flash page size we assume
            filesize(args.dt),                              # dt size in bytes
            (args.os_version << 11) | args.os_patch_level)) # os version and patch level
    args.output.write(pack('16s', args.board.encode())) # asciiz product name
    args.output.write(pack('512s', args.cmdline[:512].encode()))

    sha = sha1()
    update_sha(sha, args.kernel)
    update_sha(sha, args.ramdisk)
    update_sha(sha, args.dt)
    update_sha(sha, args.binfo)

    img_id = pack('32s', sha.digest())

    args.output.write(img_id)
    args.output.write(pack('1024s', args.cmdline[512:].encode()))

    pad_file(args.output, args.pagesize)

    args.output.seek(-12, 1)
    args.output.write("VENDOR!?")
    args.output.write(pack('I', filesize(args.binfo)))
        
    return img_id

class ValidateStrLenAction(Action):
    def __init__(self, option_strings, dest, nargs=None, **kwargs):
        if 'maxlen' not in kwargs:
            raise ValueError('maxlen must be set')
        self.maxlen = int(kwargs['maxlen'])
        del kwargs['maxlen']
        super(ValidateStrLenAction, self).__init__(option_strings, dest, **kwargs)

    def __call__(self, parser, namespace, values, option_string=None):
        if len(values) > self.maxlen:
            raise ValueError(
                'String argument too long: max {0:d}, got {1:d}'.format(self.maxlen, len(values)))
        setattr(namespace, self.dest, values)


def write_padded_file(f_out, f_in, padding):
    if f_in is None:
        return
    f_out.write(f_in.read())
    pad_file(f_out, padding)


def parse_int(x):
    return int(x, 0)


def parse_os_version(x):
    match = re.search(r'^(\d{1,3})(?:\.(\d{1,3})(?:\.(\d{1,3}))?)?', x)
    if match:
        a = int(match.group(1))
        b = c = 0
        if match.lastindex >= 2:
            b = int(match.group(2))
        if match.lastindex == 3:
            c = int(match.group(3))
        # 7 bits allocated for each field
        assert a < 128
        assert b < 128
        assert c < 128
        return (a << 14) | (b << 7) | c
    return 0


def parse_os_patch_level(x):
    match = re.search(r'^(\d{4})-(\d{2})(?:-(\d{2}))?', x)
    if match:
        y = int(match.group(1)) - 2000
        m = int(match.group(2))
        # 7 bits allocated for the year, 4 bits for the month
        assert 0 <= y < 128
        assert 0 < m <= 12
        return (y << 4) | m
    return 0


def parse_cmdline():
    parser = ArgumentParser()
    parser.add_argument('--kernel', help='path to the kernel', type=FileType('rb'))
    parser.add_argument('--ramdisk', help='path to the ramdisk', type=FileType('rb'))
    parser.add_argument('--cmdline', help='extra arguments to be passed on the '
                        'kernel command line', default='', action=ValidateStrLenAction, maxlen=1536)
    parser.add_argument('--base', help='base address', type=parse_int, default=0x10000000)
    parser.add_argument('--kernel_offset', help='kernel offset', type=parse_int, default=0x00008000)
    parser.add_argument('--ramdisk_offset', help='ramdisk offset', type=parse_int,
                        default=0x01000000)
    parser.add_argument('--os_version', help='operating system version', type=parse_os_version,
                        default=0)
    parser.add_argument('--os_patch_level', help='operating system patch level',
                        type=parse_os_patch_level, default=0)
    parser.add_argument('--tags_offset', help='tags offset', type=parse_int, default=0x00000100)
    parser.add_argument('--board', help='board name', default='', action=ValidateStrLenAction,
                        maxlen=16)
    parser.add_argument('--pagesize', help='page size', type=parse_int,
                        choices=[2**i for i in range(11, 15)], default=2048)
    parser.add_argument('--dt', help='path to the device tree image', type=FileType('rb'))
    parser.add_argument('-o', '--output', help='output file name', type=FileType('wb'))
    parser.add_argument('--binfo', help='BlackBerry binfo file', type=FileType('rb'))
    parser.add_argument('--recovery', help='mkbooting puts ramdisk into second with this flag set',
                        action='store_true')

    return parser.parse_args()

def write_data(args, pagesize):
    write_padded_file(args.output, args.kernel, pagesize)
    write_padded_file(args.output, args.ramdisk, pagesize)
    write_padded_file(args.output, args.dt, args.pagesize)
    write_padded_file(args.output, args.binfo, args.pagesize)

def main():
    args = parse_cmdline()
    if args.output is not None:
        if args.kernel is None:
            raise ValueError('kernel must be supplied when creating a boot image')
        if args.binfo is None:
            raise ValueError('binfo must be supplied for blackberry boot image')
        img_id = write_header(args)
        write_data(args, args.pagesize)

if __name__ == '__main__':
    main()
