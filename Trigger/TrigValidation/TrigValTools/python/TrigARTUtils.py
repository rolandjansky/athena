#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

'''Functions useful for dealing with Trigger ART test scripts'''

import os
import re
from contextlib import contextmanager


def package_prefix(package):
    '''Returns a prefix included in names of all tests from the given package'''
    from TrigValTools.TrigValSteering.Common import package_prefix_dict
    if package == 'ALL':
        return '({})'.format('|'.join(package_prefix_dict.values()))
    elif package in package_prefix_dict.keys():
        return package_prefix_dict[package]
    else:
        return None


def duplicate_filename(list, filename):
    for path in list:
        if os.path.basename(path) == filename:
            return True
    return False


def find_scripts(patterns):
    scripts = []
    for path in os.environ['PATH'].split(os.pathsep):
        try:
            files = os.listdir(path)
        except OSError:
            continue
        for filename in files:
            matched = True
            for patt in patterns:
                if re.search(patt, filename) is None:
                    matched = False
                    break
            if matched and not duplicate_filename(scripts, filename):
                scripts.append(os.path.join(path, filename))
    scripts.sort()
    return scripts


@contextmanager
def remember_cwd():
    '''Simple pushd/popd replacement from https://stackoverflow.com/a/169112'''
    curdir = os.getcwd()
    try:
        yield
    finally:
        os.chdir(curdir)


def first_existing_file(file_list):
    '''
    Returns the first file name from the list which corresponds to an existing file.
    Returns None if none of the files in the list exist.
    '''
    for file_name in file_list:
        if os.path.isfile(file_name):
            return file_name
    return None


def newest_file(pattern):
    '''
    Returns the newest file (by modification date) in the current directory
    with a name matching the pattern. Returns None if no file is matched.
    '''
    all_files = os.listdir('.')
    rx = re.compile(pattern)
    matched_files = [f for f in all_files if re.search(rx, f)]
    if not matched_files:
        return None
    matched_files.sort(key=lambda f: os.stat(f).st_mtime)
    return matched_files[-1]
