"""The function in this module you should look to be using is meta_diff"""
# -*- coding: utf-8 -*-
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
# This script reads metadata from a given file
from __future__ import print_function

import logging

from PyUtils.MetaReader import read_metadata


def print_diff(parent_key, obj1, obj2, diff_format):
    """build comparison string for two non-dictionary objects"""
    result = "\n"

    if diff_format == "simple":
        if obj1 is None:
            result += "{} has been inserted".format(parent_key)
        elif obj2 is None:
            result += "{} has been deleted".format(parent_key)
        else:
            result += "{} has changed from '{}' to '{}'".format(
                parent_key, obj1, obj2
            )
        result += "\n"
    else:
        if parent_key is not None:
            result += "{}:\n".format(parent_key)
        result += """\
        > {}
        ----------
        < {}
        """.format(
            obj1, obj2
        )

    return result


def print_diff_type(parent_key, obj1, obj2, diff_format):
    """Build diff string for objet of different type"""
    result = "\n"

    if diff_format == "simple":
        if obj1 is None:
            result += "{} has been inserted".format(parent_key)
        elif obj2 is None:
            result += "{} has been deleted".format(parent_key)
        else:
            result += (
                "{} has changed changed type from {} (value: '{}') to "
                "{} (value: '{}')"
            ).format(parent_key, type(obj1), obj1, type(obj2), obj2)
        result += "\n"
    else:
        if parent_key is not None:
            result += "{}:\n".format(parent_key)
        result += """\
        > {} (type: {})
        ----------
        < {} (type: {})
        """.format(
            obj1, type(obj1), obj2, type(obj2)
        )

    return result


def print_diff_dict_keys(parent_key, obj1, obj2, diff_format):
    """build diff style string for dictionary objects"""
    result = '\n'

    value1 = ', '.join(['{}: {}'.format(k, '{...}' if isinstance(v, dict) else v)
                        for k, v in sorted(obj1.items())])
    value2 = ', '.join(['{}: {}'.format(k, '{...}' if isinstance(v, dict) else v)
                        for k, v in sorted(obj2.items())])
    
    if diff_format == 'simple':
        if obj1 is None:
            result += "{} has been inserted".format(parent_key)
        elif obj2 is None:
            result += "{} has been deleted".format(parent_key)
        else:
            result += "{} has changed from '{}' to '{}'".format(
                parent_key, value1, value2
            )
    else:
        if parent_key is not None:
            result += "{}:\n".format(parent_key)
        result += "> " + value1
        result += "\n----------\n"
        result += "< " + value2

    result += "\n"

    return result


def compare(obj1, obj2, parent_key=None, ordered=False, diff_format="simple"):
    """Caclulate difference between two objects

    Keyword arguments:
    obj1       -- first object in comparision
    obj2       -- second object in comparision
    parent_key -- the key of the objects in the parent, used in recursion
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
                result += [
                    print_diff_dict_keys(parent_key, obj1, obj2, diff_format)
                ]
            else:
                for key in sorted(set(obj1.keys() + obj2.keys())):
                    if parent_key:
                        child_key = "{}/{}".format(parent_key, key)
                    else:
                        child_key = key
                    result += compare(
                        obj1[key], obj2[key], child_key, ordered, diff_format
                    )

        else:
            result += [print_diff(parent_key, obj1, obj2, diff_format)]

    else:
        result += [print_diff_type(parent_key, obj1, obj2, diff_format)]

    return result


def meta_diff(
    files,
    verbose=False,
    ordered=False,
    drop=None,
    mode="lite",
    meta_key_filter=None,
    file_type=None,
    promote=False,
    diff_format="simple",
):
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
    file_type    -- Type of files, POOL or BS (default: auto-configure)
    promote      -- MetaReader argument (default: False)
    diff_format  -- Return 'simple' or 'diff' style string (default: 'simple')
    """
    if len(files) != 2:
        raise ValueError("Wrong number of files passes, need two")

    reader_msg = logging.getLogger("MetaReader")
    reader_msg.setLevel(logging.INFO if verbose else logging.WARNING)

    msg = logging.getLogger("MetaDiff")
    msg.setLevel(logging.DEBUG if verbose else logging.INFO)

    metadata = read_metadata(
        files,
        file_type,
        mode=mode,
        meta_key_filter=meta_key_filter,
        promote=promote,
    )

    try:
        for key in drop:
            for _, value in metadata.items():
                value.pop(key, None)
    except TypeError:
        pass

    result = compare(
        metadata[files[0]],
        metadata[files[1]],
        ordered=ordered,
        diff_format=diff_format,
    )

    if not result:
        msg.info("No differences found")

    return result
