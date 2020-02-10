#!/usr/bin/env python

#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
from __future__ import print_function

import pickle
import pprint
import json
import sys
import argparse
import ast


def main():
    parser = argparse.ArgumentParser(
        description="Utility to transform/display athena configurations"
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

    if args.printComps:
        for fileName in args.file:
            conf = __loadSingleFile(fileName, args)
            __printComps(conf)

    if args.printConf:
        for fileName in args.file:
            conf = __loadSingleFile(fileName, args)
            __print(conf)

    if args.toJSON:
        if len(args.file) != 1:
            sys.exit("ERROR, can convert single file at a time, got: %s" % args.file)
        conf = __loadSingleFile(args.file[0], args)
        with open(args.toJSON, "w") as oFile:
            json.dump(conf, oFile, indent=2, ensure_ascii=True)

    if args.toPickle:
        if len(args.file) != 1:
            sys.exit("ERROR, can convert single file at a time, got: %s" % args.file)
        conf = __loadSingleFile(args.file[0], args)
        with open(args.toPickle, "wb") as oFile:
            for item in conf:
                pickle.dump(item, oFile)

    if args.diff:
        if len(args.file) != 2:
            sys.exit("ERROR, can diff exactly two files at a time, got: %s" % args.file)
        configRef = __loadSingleFile(args.file[0], args)
        configChk = __loadSingleFile(args.file[1], args)
        for ref, chk in zip(configRef, configChk):
            if isinstance(ref, dict) and isinstance(chk, dict):
                __compareConfig(ref, chk, args)
            else:
                print("Given list of size %d. Not looking for differences." % len(ref))


def __loadSingleFile(fname, args):
    conf = []
    if fname.endswith(".pkl"):
        with open(fname, "rb") as input_file:
            while True:
                try:
                    conf.append(pickle.load(input_file))
                except EOFError:
                    break
        print("... Read", len(conf), "items from python pickle file: ", fname)

    elif fname.endswith(".json"):

        def __keepPlainStrings(element):
            if isinstance(element, str):
                return str(element)
            if isinstance(element, list):
                return [__keepPlainStrings(x) for x in element]
            if isinstance(element, dict):
                return {
                    __keepPlainStrings(key): __keepPlainStrings(value)
                    for key, value in element.items()
                }
            return element

        with open(fname, "r") as input_file:
            conf = json.load(input_file, object_hook=__keepPlainStrings)
            print("... Read", len(conf), "items from JSON file: ", fname)

    else:
        sys.exit("File format not supported.")

    if conf is None:
        sys.exit("Unable to load %s file" % fname)

    if args.comps:  # returning only wanted components

        def is_component_from_list(component):
            return True in [s in component for s in compsToReport]

        compsToReport = [
            item for elem in args.comps for item in elem
        ]  # creates flat list of wanted components
        conf = [
            {key: value for (key, value) in dic.items() if is_component_from_list(key)}
            for dic in conf
            if isinstance(dic, dict)
        ]
    return conf


def __print(conf):
    for item in conf:
        pprint.pprint(dict(item))


def __printComps(conf):

    def __printDict(d):
        for key, value in dict(d).items():
            if key in "Histograms":
                continue

            if isinstance(value, dict):
                __printDict(value)
            elif isinstance(value, str) and "/" in value:
                if "[" not in value:  # not an array
                    if value.count("/") == 1:
                        print(value)
                else:  # handle arrays
                    actualType = ast.literal_eval(value)
                    if isinstance(actualType, list):
                        for el in actualType:
                            if el.count("/") == 1:  # ==1 eliminates COOL folders
                                print(el)
    for item in conf:
        __printDict(item)


def __compareConfig(configRef, configChk, args):
    # Find superset of all components:
    allComps = set(list(configRef.keys()) + list(configChk.keys()))
    print("Step 1: reference file #components:", len(configRef))
    print("Step 2: file to check  #components:", len(configChk))

    for component in allComps:

        if component not in configRef and not args.ignoreMissing:
            print(
                "\n\033[91m Component ",
                component,
                " \033[94m exists only in Chk \033[0m \033[0m \n",
            )
            continue

        if component not in configChk and not args.ignoreMissing:
            print(
                "\n\033[91m Component",
                component,
                " \033[92m exists only in Ref \033[0m  \033[0m \n",
            )
            continue
        refValue = configRef[component]
        chkValue = configChk[component]

        if chkValue == refValue:
            if args.printIdenticalComponents:
                print("Component", component, "identical")
        else:
            print("\033[91m Component", component, "differ \033[0m")
            if not args.allComponentPrint:
                __compareComponent(refValue, chkValue, "\t", args)
            else:
                print(
                    "\t\033[92mRef\033[0m\t",
                    sorted(configRef[component].items(), key=lambda kv: kv[0]),
                )
                print(
                    "\t\033[94mChk\033[0m\t",
                    sorted(configChk[component].items(), key=lambda kv: kv[0]),
                )


def __compareComponent(compRef, compChk, prefix, args):

    if isinstance(compRef, dict):

        allProps = set(list(compRef.keys()) + list(compChk.keys()))

        ignoreList = ["StoreGateSvc", "OutputLevel", "MuonEDMHelperSvc"]
        for prop in allProps:
            if prop not in compRef.keys():
                print(
                    "%s%s: \033[94m exists only in Chk \033[0m \033[91m<< !!!\033[0m"
                    % (prefix, prop)
                )
                continue

            if prop not in compChk.keys():
                print(
                    "%s%s: \033[92m exists only in Ref \033[0m \033[91m<< !!!\033[0m"
                    % (prefix, prop)
                )
                continue

            refVal = compRef[prop]
            chkVal = compChk[prop]

            if args.ignoreIrrelevant and chkVal in ignoreList:
                continue

            if chkVal == refVal:
                if not args.printIdenticalPerParameter:
                    continue
                diffmarker = ""
            else:
                diffmarker = " \033[91m<< !!!\033[0m"

            print(
                "%s%s : \033[92m %s \033[0m vs \033[94m %s \033[0m %s"
                % (prefix, prop, str(refVal), str(chkVal), diffmarker)
            )

            try:
                refVal = ast.literal_eval(str(refVal))
                chkVal = ast.literal_eval(str(chkVal))
            except ValueError:
                pass # literal_eval exception when parsing particular strings

            if refVal and (isinstance(refVal, list) or isinstance(refVal, dict)):
                __compareComponent(refVal, chkVal, "\t" + prefix + ">> ", args)

    elif isinstance(compRef, list) and len(compRef) > 1:
        if len(compRef) != len(compChk):
            diff = list(set(compRef) - set(compChk))
            inRef = all(elem in compRef for elem in compChk)
            missingIn = "Ref" if inRef else "Chk"
            color = "\033[92m" if inRef else "\033[94m"
            print(
                "%s exists only in %s : %s %s \033[0m \033[91m<< !!!\033[0m"
                % (prefix, missingIn, color, str(diff))
            )
        else:
            for refVal, chkVal in zip(compRef, compChk):
                if refVal != chkVal:
                    print(
                        "%s : \033[92m %s \033[0m vs \033[94m %s \033[0m \033[91m<< !!!\033[0m"
                        % (prefix, str(refVal), str(chkVal))
                    )
                    __compareComponent(refVal, chkVal, "\t" + prefix + ">> ", args)


if __name__ == "__main__":
    main()
