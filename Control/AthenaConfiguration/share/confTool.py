#!/usr/bin/env python

#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

import argparse


def parse_args():
    parser = argparse.ArgumentParser(
        description="Utility to transform/display athena configurations"
    )
    parser.add_argument(
        "-n",
        "--newConfig",
        action="store_true",
        help="Specify to compare two ComponentAccumulator objects",
    )
    parser.add_argument("-p", "--printConf", action="store_true", help="Prints")
    parser.add_argument(
        "--printComps", action="store_true", help="Prints only the components"
    )
    parser.add_argument(
        "--diff", dest="diff", action="store_true", help="Diffs two files"
    )
    parser.add_argument("--toJSON", help="Convert to JSON file")
    parser.add_argument("--toPickle", help="Convert to pickle file")

    parser.add_argument("file", nargs="+", help="Files to work with")
    parser.add_argument(
        "--comps",
        nargs="*",
        help="Report only component containing this string",
        action="append",
    )
    parser.add_argument(
        "--ignoreMissing",
        help="Don't report components existing in only of the two configuartions",
        action="store_true",
    )
    parser.add_argument(
        "--allComponentPrint",
        help="Print all component if there are differences in any of its properties",
        action="store_true",
    )
    parser.add_argument(
        "--printIdenticalComponents",
        help="Print all components even, if there are no differences.",
        action="store_true",
    )
    parser.add_argument(
        "--printIdenticalPerParameter",
        help="Print all parameters in component with difference even, if there are no differences.",
        action="store_true",
    )
    parser.add_argument(
        "--ignoreIrrelevant",
        help="Ignore differences in e.g. outputlevel",
        action="store_true",
    )

    args = parser.parse_args()

    if args.newConfig:
        from AthenaConfiguration.new_confTool import main
    else:
        from AthenaConfiguration.old_confTool import main

    main(args)


if __name__ == "__main__":
    parse_args()
