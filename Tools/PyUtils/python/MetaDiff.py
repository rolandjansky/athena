"""The function in this module you should look to be using is meta_diff"""
# -*- coding: utf-8 -*-
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
# This script reads metadata from a given file
from __future__ import print_function

import logging

from PyUtils.MetaReader import read_metadata


def print_diff(parent_key, obj1, obj2):
    """build comparison string for two non-dictionary objects"""
    result = '\n'

    if parent_key is not None:
        result += '{}:\n'.format(parent_key)
    result += '''\
    > {}
    ----------
    < {}
    '''.format(obj1, obj2)

    return result


def print_diff_type(parent_key, obj1, obj2):
    """Build diff string for objet of different type"""
    result = '\n'

    if parent_key is not None:
        result += '{}:\n'.format(parent_key)
    result += '''\
    > {} (type: {})
    ----------
    < {} (type: {})
    '''.format(obj1, type(obj1), obj2, type(obj2))

    return result


def print_diff_dict_keys(parent_key, obj1, obj2):
    """build diff style string for dictionary objects"""
    result = '\n'

    if parent_key is not None:
        result += '{}:\n'.format(parent_key)
    result += '> ' + ', '.join([
        '{}: {}'.format(k, '{...}' if isinstance(v, dict) else v)
        for k, v in sorted(obj1.items())])
    result += '\n----------\n'
    result += '< ' + ', '.join([
        '{}: {}'.format(k, '{...}' if isinstance(v, dict) else v)
        for k, v in sorted(obj2.items())])
    result += '\n'

    return result


def compare(obj1, obj2, parent_key=None, ordered=False):
    """Caclulate difference between two objects

    Keyword arguments:
    obj1       -- first object in comparision
    obj2       -- second object in comparision
    parent_key -- the key for in objects in the parent objects, used in recursion
    ordered    -- whether to check order of list content
    """
    result = list()

    if not ordered and isinstance(obj1, list):
        obj1.sort()

    if not ordered and isinstance(obj2, list):
        obj2.sort()

    if obj1 == obj2:
        return result

    if isinstance(obj1, type(obj2)):

        if isinstance(obj1, dict):

            if sorted(obj1.keys()) != sorted(obj2.keys()):
                result += [print_diff_dict_keys(parent_key, obj1, obj2)]
            else:
                for key in sorted(set(obj1.keys() + obj2.keys())):
                    if parent_key:
                        child_key = '{}/{}'.format(parent_key, key)
                    else:
                        child_key = key
                    result += compare(obj1[key], obj2[key], child_key, ordered)

        else:
            result += [print_diff(parent_key, obj1, obj2)]

    else:
        result += [print_diff_type(parent_key, obj1, obj2)]

    return result


def meta_diff(files, verbose=False, ordered=False, drop=None,
              mode='lite', meta_key_filter=None, file_type=None, promote=False):
    """
    Compare the in-file metadata in two given files. Uses PyUtils.MetaReader
    to obtain file content. Generates list of string that show difference.
    Returns empty list if no difference is found

    Keyword arguments:
    files   -- Names of two files to compare
    verbose -- toggle to get debug information
    ordered -- whether to check order of lists in the metadata
    drop    -- keys to drop from metadata retrieved by MetaReader
    mode    -- MetaReader argument setting amount of content (default 'lite').
               Allowed values are: tiny, lite, peeker, and full
    meta_key_filter -- MetaReader argument selecting keys to retrieve (default
                       get all)
    file_type -- Type of files, POOL or BS (default: auto-configure)
    promote   -- MetaReader argument (default: False)
    """
    if len(files) != 2:
        raise ValueError("Wrong number of files passes, need two")

    msg = logging.getLogger('MetaReader')
    msg.setLevel(logging.INFO if verbose else logging.WARNING)

    metadata = read_metadata(
        files, file_type, mode=mode,
        meta_key_filter=meta_key_filter, promote=promote)

    try:
        for key in drop:
            for _, value in metadata.items():
                value.pop(key, None)
    except TypeError:
        pass

    return compare(metadata[files[0]], metadata[files[1]], ordered=ordered)
