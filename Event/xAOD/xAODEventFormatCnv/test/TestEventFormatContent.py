#!/usr/bin/env python
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""Read in a given file and return how often given metadata item appears

Use MetaReader to look up metadata items in a given file. Search for the given
key in each key found in the list of metadata items. If the number of matches
is 1 return success else return failure.

    Common usgage example:

        In a shell:
            if TestEventFormatContent.py file.pool.root metadata_key; then
                echo "found one"
            else
                echo "found none or many"
            fi
"""
import argparse
from functools import reduce
import logging
import sys
import PyUtils.MetaReader


def main():
    """parse aruments and return number of matches"""
    parser = argparse.ArgumentParser(
        description=""""
        Read in a given file and return the number of times metadata items with
        a name matching the given term appear.
        """
    )

    parser.add_argument(
        "file", metavar="FILE", help="the file to read"
    )

    parser.add_argument(
        "key", metavar="KEY", help="the key to search for"
    )

    args = parser.parse_args()

    # silence meta reader
    reader_msg = logging.getLogger("MetaReader")
    reader_msg.setLevel(logging.WARNING)

    # get the list of metadata items of the file
    try:
        metadata = next(
            iter(
                PyUtils.MetaReader.read_metadata([args.file]).items()
            )
        )[1]['metadata_items']
    except (OSError,):
        print("cannot read metadata from ", args.file)
        sys.exit(1)

    matches = reduce(
        lambda n, key: n + 1 if args.key in key else n,
        [0] + list(metadata)
    )

    if matches == 1:
        return 0

    return 1


if __name__ == "__main__":
    main()
