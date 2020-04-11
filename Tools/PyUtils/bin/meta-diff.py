#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
# This script reads metadata from a given file

from __future__ import print_function

import sys
import json
import argparse
import time
import logging
import os

# escape sequence [?1034h which appear on several runs due to smm capability (Meta Mode On) for xterm.
if 'TERM' in os.environ:
    del os.environ['TERM']

msg = logging.getLogger('MetaReaderDiff')

from PyUtils.MetaReader import read_metadata

def print_diff(parent_key, obj1, obj2):
    print('')
    if parent_key is not None:
        print('{}:'.format(parent_key))
    print('> {}'.format(obj1))
    print('----------')
    print('< {}'.format(obj2))

def print_diff_type(parent_key, obj1, obj2):
    print('')
    if parent_key is not None:
        print('{}:'.format(parent_key))
    print('> {} (type: {})'.format(obj1, type(obj1)))
    print('----------')
    print('< {} (type: {})'.format(obj2, type(obj2)))
    
def print_diff_dict_keys(parent_key, obj1, obj2):
    print('')
    if parent_key is not None:
        print('{}:'.format(parent_key))
    print('> ' + ', '.join(['{}: {}'.format(k, '{...}' if isinstance(v, dict) else v) for k,v in sorted(obj1.items())]))
    print('----------')
    print('< ' + ', '.join(['{}: {}'.format(k, '{...}' if isinstance(v, dict) else v) for k,v in sorted(obj2.items())]))

def compare(obj1, obj2, parent_key=None, ordered=False):
    
    if isinstance(obj1, dict) and isinstance(obj2, dict):
        if sorted(obj1.keys()) != sorted(obj2.keys()):
            print_diff_dict_keys(parent_key, obj1, obj2)
        else:
            for key in sorted(set(obj1.keys() + obj2.keys())):
                
                child_key = ''
                if parent_key is not None:
                    child_key += parent_key + '/'
                child_key += key
                
                compare(obj1[key], obj2[key], child_key, ordered)
                
    elif  isinstance(obj1, list) and isinstance(obj2, list):
        if ordered:
            if sorted(obj1) != sorted(obj2):
                print_diff(parent_key, obj1, obj2)
        else:
            if obj1 != obj2:
                print_diff(parent_key, obj1, obj2)      
    elif  isinstance(obj1, set) and isinstance(obj2, set):
        if obj1 != obj2:
            print_diff(parent_key, obj1, obj2)
    elif type(obj1) == type(obj2):
        if obj1 != obj2:
            print_diff(parent_key, obj1, obj2)
    else:
        print_diff_type(parent_key, obj1, obj2)
        
    
    
    
def _main():
    # Parsing the arguments provided by user
    parser = argparse.ArgumentParser(description='This script reads metadata from a given file')
    
    parser.add_argument('-v',
                        '--verbose',
                        action='store_true',
                        help='print detailed output on screen')
    parser.add_argument('-s',
                        '--ordered',
                        action='store_true',
                        help='When comparing lists, check the element order too.')
    parser.add_argument('-o',
                        '--output',
                        metavar='FILE',
                        default=None,
                        help="Saves the output in a file. By default, the output is written on the screen (stdout) in a prettier format for better readabiilty.")
    parser.add_argument('-m',
                        '--mode',
                        default= 'lite',
                        metavar='MODE',
                        type=str,
                        choices=['tiny', 'lite', 'full', 'peeker'],
                        help="This flag provides the user capability to select the amount of metadata retrieved. There three options: "
                             "tiny (only those values used in PyJobTransforms), "
                             "lite (same output as dump-athfile) "
                             "and full ( all  available data found) ")
    parser.add_argument('-t',
                        '--type',
                        default= None,
                        metavar='TYPE',
                        type=str,
                        choices=['POOL', 'BS'],
                        help="The file type of the input filename. By default, it tries to determine itself the file type of the input.")
                        
    parser.add_argument('-f',
                        '--filter',
                        default= [],
                        metavar='FILTER',
                        nargs = '+',
                        type=str,
                        help="The metadata keys to filter. ")
    parser.add_argument('--promote',
                        default=None,
                        type=bool,
                        help="Force promotion or not of the metadata keys ")
                        
    parser.add_argument('filename1', help='First file to compare.')
    parser.add_argument('filename2', help='Second file to compare.')
                        
    args = parser.parse_args()

    verbose = args.verbose
    ordered = args.ordered
    filename1 = args.filename1
    filename2 = args.filename2
    output = args.output
    mode = args.mode
    file_type = args.type
    meta_key_filter = args.filter

    msg.setLevel(logging.INFO if verbose else logging.WARNING)
    # create a stream handler
    handler = logging.StreamHandler()
    handler.setLevel(logging.INFO if verbose else logging.WARNING)
    # create a logging format
    formatter = logging.Formatter('%(name)s                       %(levelname)s %(message)s')
    handler.setFormatter(formatter)
    # add the handlers to the logger
    msg.addHandler(handler)

    metadatas = read_metadata([filename1, filename2], file_type, mode=mode, meta_key_filter= meta_key_filter, promote=args.promote)
    metadata1 = metadatas[filename1]
    metadata2 = metadatas[filename2]
    compare(metadata1, metadata2, ordered=ordered)
    


if __name__ == '__main__':
    _main()






