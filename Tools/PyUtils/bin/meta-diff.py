#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""This script reads metadata from a given file"""

from __future__ import print_function

import argparse
import os
import sys

from PyUtils.MetaDiff import meta_diff

# escape sequence [?1034h which appear on several runs due to smm capability
# (Meta Mode On) for xterm.
if "TERM" in os.environ:
    del os.environ["TERM"]


def main():
    """Handle command line arguments and call meta_diff"""

    parser = argparse.ArgumentParser(
        description="Compare the metadata content of two files"
    )

    parser.add_argument(
        "files",
        nargs=2,
        metavar="FILE",
        help="The names of two files to compare",
    )

    parser.add_argument(
        "-v",
        "--verbose",
        action="store_true",
        help="print detailed output on screen",
    )

    parser.add_argument(
        "-s",
        "--ordered",
        action="store_true",
        help="When comparing lists, check the element order too.",
    )

    parser.add_argument(
        "-d",
        "--drop",
        nargs="*",
        default=None,
        metavar="KEY",
        help="Keys to drop from metadata retrieved from file",
    )

    parser.add_argument(
        "-m",
        "--mode",
        default="lite",
        metavar="MODE",
        type=str,
        choices=["tiny", "lite", "full", "peeker"],
        help="""\
             This flag provides the user capability to select the amount of
             metadata retrieved. There three options:
                         tiny (only those values used in PyJobTransforms),
                         lite (same output as dump-athfile)
                         and full ( all  available data found)
             """,
    )

    parser.add_argument(
        "-t",
        "--type",
        default=None,
        metavar="TYPE",
        type=str,
        choices=["POOL", "BS"],
        help="""\
             The file type of the input filename. By default, it tries to
             determine itself the file type of the input.
             """,
    )

    parser.add_argument(
        "-f",
        "--filter",
        default=[],
        metavar="FILTER",
        nargs="+",
        type=str,
        help="Expression to select specific metadata fields to retrieve.",
    )

    parser.add_argument(
        "-x",
        "--diff-format",
        default="simple",
        type=str,
        choices=["simple", "diff"],
        help="Switch between 'simple' or 'diff' style differences ",
    )

    parser.add_argument(
        "--promote",
        default=None,
        type=bool,
        help="Force promotion or not of the metadata keys ",
    )

    args = parser.parse_args()

    try:
        diff = meta_diff(
            args.files,
            verbose=args.verbose,
            ordered=args.ordered,
            drop=args.drop,
            mode=args.mode,
            meta_key_filter=args.filter,
            file_type=args.type,
            promote=args.promote,
            diff_format=args.diff_format,
        )
    except (ValueError, IndexError):
        print("you must supply two files to compare")
        sys.exit(1)
    except ReferenceError:
        print("no such file")
        sys.exit(1)

    if diff:
        print("\n".join(diff))
        sys.exit(1)

    sys.exit(0)


if __name__ == "__main__":
    main()
