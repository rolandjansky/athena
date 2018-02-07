#!/usr/bin/env python
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# This script reads metadata from a given file

import sys
import json
import argparse
import time

import logging
msg = logging.getLogger('MetaReader')

from PyUtils.MetaReader import read_metadata

def __pretty_print(content, indent=4, fd=sys.stdout, level=0):
#    print(type(content))
    if isinstance(content, dict):
        for key, value in content.items():
            print >> fd,' ' * indent  * level + str(key)
            __pretty_print(value, indent, fd, level + 1)

    elif isinstance(content, list):
        for value in content:
            __pretty_print(value, indent, fd, level + 1)

    elif isinstance(content, tuple):

        for value in content:
            __pretty_print(value, indent, fd, level + 1)
        
    else:
        print >> fd, ' ' * indent * (level + 1) + str(content)


def __main():
    # Parsing the arguments provided by user
    parser = argparse.ArgumentParser(description='This script reads metadata from a given file')
    parser.add_argument('filenames',
                        nargs = '+',
                        help= 'The filenames to read. User can provide a single file or a list of files.')
    parser.add_argument('-v',
                        '--verbose',
                        action='store_true',
                        help='print detailed output on screen')
    parser.add_argument('-o',
                        '--output',
                        metavar='FILE',
                        default=None,
                        help="Saves the output in a file. By default, the output is written on the screen (stdout) in a prettier format for better readabiilty.")
    parser.add_argument('--json',
                        action='store_true',
                        help="Sets the output file format as json.")
    parser.add_argument('--indent',
                        metavar='N',
                        type=int,
                        default=2,
                        help="Sets the indent spaces in the output either on screen (without -o flag) either on file (with -o flag). By default, uses two spaces as indent.")
    parser.add_argument('-f',
                        '--full',
                        action='store_true',
                        default=False,
                        help="Retrieve the full set of metadata from file.")
    parser.add_argument('-t',
                        '--type',
                        default= None,
                        metavar='TYPE',
                        type=str,
                        choices=['POOL', 'BS'],
                        help="The file type of the input filename. By default, it tries to determine itself the file type of the input.")
    args = parser.parse_args()

    verbose = args.verbose
    filenames = args.filenames
    output = args.output
    is_json = args.json
    indent = args.indent
    is_full = args.full
    file_type = args.type

    if verbose:
        msg.setLevel(logging.INFO)
        # create a stream handler
        handler = logging.StreamHandler()
        handler.setLevel(logging.INFO)
        # create a logging format
        formatter = logging.Formatter('%(name)s                       %(levelname)s %(message)s')
        handler.setFormatter(formatter)
        # add the handlers to the logger
        msg.addHandler(handler)

    startTime = time.time()
    msg.info('Imported headers in: {0} miliseconds'.format((time.time() - startTime) * 1e3)) 
    msg.info('The output file is: {0}'.format(output))

    d = read_metadata(filenames, file_type, full= is_full)
    
    if output is None:
        if is_json:
            print json.dumps(d, indent=indent)
        else:
            __pretty_print(d, indent=indent)
    else:
        if is_json:
            with open(output, 'w') as fd:
                print >> fd, json.dumps(d, indent=indent)
        else:
            with open(output, 'w') as fd:
                __pretty_print(d, indent=indent, fd=fd)
    
    msg.info('Done!')

if __name__ == '__main__':
    __main()
