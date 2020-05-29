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

msg = logging.getLogger('MetaDiff')

from PyUtils.MetaReader import read_metadata

def print_diff(parent_key, obj1, obj2, diff_format):
    
    if diff_format == 'simple':
        if obj1 is None:
            print('{} has been inserted'.format(parent_key))
        elif obj2 is None:
            print('{} has been deleted'.format(parent_key))
        else:
            print('{} has changed from \'{}\' to \'{}\''.format(parent_key, obj1, obj2))
    else:
        print('')
        if parent_key is not None:
            print('{}:'.format(parent_key))
        print('> {}'.format(obj1))
        print('----------')
        print('< {}'.format(obj2))

def print_diff_type(parent_key, obj1, obj2, diff_format):
    
    if diff_format == 'simple':
        if obj1 is None:
            print('{} has been inserted'.format(parent_key))
        elif obj2 is None:
            print('{} has been deleted'.format(parent_key))
        else:
            print('{} has changed changed type from {} (value: \'{}\') to {} (value: \'{}\')'.format(parent_key, type(obj1), obj1,  type(obj2), obj2))
    else:
        print('')
        if parent_key is not None:
            print('{}:'.format(parent_key))
        print('> {} (type: {})'.format(obj1, type(obj1)))
        print('----------')
        print('< {} (type: {})'.format(obj2, type(obj2)))
    
def print_diff_dict_keys(parent_key, obj1, obj2, diff_format):

    value1 = None
    value2 = None
    
    if obj1 is not None:
        value1 = ', '.join(['{}: {}'.format(k, '{...}' if isinstance(v, dict) else v) for k,v in sorted(obj1.items())])
        
    if obj2 is not None:
        value2 = ', '.join(['{}: {}'.format(k, '{...}' if isinstance(v, dict) else v) for k,v in sorted(obj2.items())])
        
    if diff_format == 'simple':
        if obj1 is None:
            print('{}  has been inserted'.format(parent_key))
        elif obj2 is None:
            print('{} has been deleted'.format(parent_key))
        else:
            print('{} has changed from \'{}\' to \'{}\''.format(parent_key, value1, value2))
    else:
        print('')
        if parent_key is not None:
            print('{}:'.format(parent_key))
            
        print('> ' + value1)
        print('----------')
        print('< ' + value2)
        
def print_same(parent_key, obj1, obj2, diff_format):
    if diff_format == 'simple':
        print('{}  is the same'.format(parent_key))
    else:
        print('')
        print('{}: same'.format(parent_key))

def compare(obj1, obj2, parent_key=None, ordered=False, show_same=False, diff_format='simple'):
    
 
    same = True;
    
    if isinstance(obj1, dict) and isinstance(obj2, dict):
        if sorted(obj1.keys()) != sorted(obj2.keys()):
            print_diff_dict_keys(parent_key, obj1, obj2, diff_format)
            same = False
        else:
            for key in sorted(set(obj1.keys() + obj2.keys())):
                
                child_key = ''
                if parent_key is not None:
                    child_key += parent_key + '/'
                child_key += key
                
                same &= compare(obj1[key] if key in obj1 else None, obj2[key] if key in obj2 else None, child_key, ordered, show_same, diff_format)
                
    elif  isinstance(obj1, list) and isinstance(obj2, list):
        if ordered:
            if sorted(obj1) != sorted(obj2):
                print_diff(parent_key, obj1, obj2, diff_format)
                same = False
            else:
                if show_same:
                    print_same(parent_key, obj1, obj2, diff_format)
        else:
            if obj1 != obj2:
                print_diff(parent_key, obj1, obj2, diff_format) 
                same = False
            else:
                if show_same:
                    print_same(parent_key, obj1, obj2, diff_format)
    elif  isinstance(obj1, set) and isinstance(obj2, set):
        if obj1 != obj2:
            print_diff(parent_key, obj1, obj2, diff_format)
            same = False
        else:
            if show_same:
                print_same(parent_key, obj1, obj2, diff_format)
    elif type(obj1) == type(obj2):
        if obj1 != obj2:
            print_diff(parent_key, obj1, obj2, diff_format)
            same = False
        else:
            if show_same:
                print_same(parent_key, obj1, obj2, diff_format)
    else:
        print_diff_type(parent_key, obj1, obj2, diff_format)
        same = False
        
    return same
        
    
    
    
def _main():
    # Parsing the arguments provided by user
    parser = argparse.ArgumentParser(description='This script reads metadata from a given file')
    
    parser.add_argument('-v',
                        '--verbose',
                        action='store_true',
                        help='print detailed output on screen')
    parser.add_argument('--ordered',
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
                             
    parser.add_argument('-c',
                    '--check',
                    action='store_true',
                    help='If difference are found, produce a non-zero return code')
                             
    parser.add_argument('-s',
                    '--show-same',
                    action='store_true',
                    help='Show also values that are the same')
                    
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
    parser.add_argument('-d',
                        '--diff-format',
                        default= 'simple',
                        type=str,
                        choices=['simple', 'diff'],
                        help="Show 'simple' or 'diff' style differences ")
    parser.add_argument('--promote',
                        default=None,
                        type=bool,
                        help="Force promotion or not of the metadata keys ")
                        
            
                        
    parser.add_argument('filename1', help='First file to compare.')
    parser.add_argument('filename2', help='Second file to compare.')
                        
    args = parser.parse_args()

    check = args.check
    show_same = args.show_same
    verbose = args.verbose
    ordered = args.ordered
    filename1 = args.filename1
    filename2 = args.filename2
    output = args.output
    mode = args.mode
    diff_format = args.diff_format
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

    metadatas = read_metadata([filename1, filename2], file_type, mode=mode, meta_key_filter=meta_key_filter, promote=args.promote)
    metadata1 = metadatas[filename1]
    metadata2 = metadatas[filename2]
    
    del metadata1['file_guid']
    del metadata2['file_guid']
    
    del metadata1['file_size']
    del metadata2['file_size']
    
    same = compare(metadata1, metadata2, ordered=ordered, diff_format=diff_format, show_same=show_same)
    
    
    if check and not same:
        sys.exit(1)


if __name__ == '__main__':
    res = _main()
    
    






