#!/usr/bin/env python

#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from __future__ import print_function

import ast
import json
import pickle
import pprint
import re
import sys

from AthenaConfiguration.iconfTool.models.loaders import loadConfigFile, baseParser, componentRenamingDict


def parse_args():
    parser = baseParser
    parser.add_argument(
        "-p", "--printConf", action="store_true", help="Prints entire configuration"
    )
    parser.add_argument(
        "--printComps", action="store_true", help="Prints only the components"
    )
    parser.add_argument(
        "--diff", dest="diff", action="store_true", help="Diffs two files"
    )
    parser.add_argument("--toJSON", action="store_true", help="Convert to JSON file")
    parser.add_argument("--toPickle", help="Convert to pickle file")

    parser.add_argument("file", nargs="+", help="Files to work with")
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


    args = parser.parse_args()
    main(args)


def main(args):
    if args.ignoreIrrelevant:
        print(f"Components to ignore: {args.ignore}")
    if args.printComps:
        for fileName in args.file:
            conf = loadConfigFile(fileName, args)
            _printComps(conf)

    if args.printConf:
        for fileName in args.file:
            conf = loadConfigFile(fileName, args)
            _print(conf)

    if args.toJSON:
        if len(args.file) != 1:
            sys.exit(
                "ERROR, can convert single file at a time, got: %s" % args.file
            )
        from TrigConfIO.JsonUtils import create_joboptions_json
        create_joboptions_json(args.file[0], args.file[0].replace("pkl","json"))

    if args.toPickle:
        if len(args.file) != 1:
            sys.exit(
                "ERROR, can convert single file at a time, got: %s" % args.file
            )
        conf = loadConfigFile(args.file[0], args)
        with open(args.toPickle, "wb") as oFile:
            for item in conf:
                pickle.dump(item, oFile)

    if args.diff:
        if len(args.file) != 2:
            sys.exit(
                "ERROR, can diff exactly two files at a time, got: %s"
                % args.file
            )
        configRef = loadConfigFile(args.file[0], args)
        configChk = loadConfigFile(args.file[1], args)

        _compareConfig(configRef, configChk, args)

def _print(conf):
    pprint.pprint(conf)


def _printComps(conf):
    for k, item in conf.items():
        if isinstance(item, dict):
            print(k)

def _compareConfig(configRef, configChk, args):
    # Find superset of all components:
    allComps = list(set(configRef.keys()) | set(configChk.keys()))
    allComps.sort()

    print("Step 1: reference file #components:", len(configRef))
    print("Step 2: file to check  #components:", len(configChk))

    componentReverseRenamig = {v: k for k, v in componentRenamingDict.items()} # need mapping from new name to old when renaming
    def _componentDescription(comp_name):
        return (comp_name+ " renamed from " + componentReverseRenamig[comp_name]) if comp_name in componentReverseRenamig else comp_name

    for component in allComps:
        if component not in configRef:
            if not args.ignoreMissing:
                print(
                    "\n\033[91m Component ",
                    _componentDescription(component),
                    " \033[94m exists only in 2nd file \033[0m \033[0m \n",
                )
            continue

        if component not in configChk:
            if not args.ignoreMissing:
                print(
                    "\n\033[91m Component",
                    _componentDescription(component),
                    " \033[92m exists only in 1st file \033[0m  \033[0m \n",
                )
            continue

        refValue = configRef[component]
        chkValue = configChk[component]

        if chkValue == refValue:
            if args.printIdenticalComponents:
                print("Component", _componentDescription(component), "identical")
        else:
            print("\033[91m Component", _componentDescription(component), "differ \033[0m")
            if not args.allComponentPrint:
                _compareComponent(refValue, chkValue, "\t", args, component)
            else:
                print(
                    "\t\033[92mRef\033[0m\t",
                    sorted(configRef[component].items(), key=lambda kv: kv[0]),
                )
                print(
                    "\t\033[94mChk\033[0m\t",
                    sorted(configChk[component].items(), key=lambda kv: kv[0]),
                )


def _parseNumericalValues(v1, v2):
    values = (v1, v2)
    if any(isinstance(val, float) for val in values):
        return float(v1), float(v2)
    elif all(isinstance(val, int) for val in values):
        return int(v1), int(v2)
    else:
        return values


def _compareComponent(compRef, compChk, prefix, args, component):

    if isinstance(compRef, dict):

        allProps = list(set(compRef.keys()) | set(compChk.keys()))
        allProps.sort()

        for prop in allProps:
            if prop not in compRef.keys():
                print(
                    "%s%s = %s: \033[94m exists only in 2nd file \033[0m \033[91m<< !!!\033[0m"
                    % (prefix, prop, compChk[prop])
                )
                continue

            if prop not in compChk.keys():
                print(
                    "%s%s = %s: \033[92m exists only in 1st file \033[0m \033[91m<< !!!\033[0m"
                    % (prefix, prop, compRef[prop])
                )
                continue

            refVal = compRef[prop]
            chkVal = compChk[prop]

            try:
                refVal = ast.literal_eval(str(refVal)) if refVal else ""
                chkVal = ast.literal_eval(str(chkVal)) if chkVal else ""
            except SyntaxError:
                pass
            except ValueError:
                pass  # literal_eval exception when parsing particular strings

            refVal, chkVal = _parseNumericalValues(refVal, chkVal)

            if str(chkVal) == str(refVal):
                if not args.printIdenticalPerParameter:
                    continue
                diffmarker = ""
            else:
                diffmarker = " \033[91m<< !!!\033[0m"

            if not (component == "IOVDbSvc" and prop == "Folders"):
                print(
                    "%s%s : \033[92m %s \033[0m vs \033[94m %s \033[0m %s"
                    % (prefix, prop, str(refVal), str(chkVal), diffmarker)
                )

            if refVal and (
                isinstance(refVal, list) or isinstance(refVal, dict)
            ):
                if component == "IOVDbSvc" and prop == "Folders":
                    _compareIOVDbFolders(refVal, chkVal, "\t", args)
                else:
                    _compareComponent(
                        refVal, chkVal, "\t" + prefix + ">> ", args, component
                    )

    elif isinstance(compRef, (list, tuple)) and len(compRef) > 1:

        if isinstance(compRef[0], list):  # to achieve hashability
            compRef = [tuple(el) for el in compRef]

        if len(compChk) > 0 and isinstance(compChk[0], list):
            compChk = [tuple(el) for el in compChk]

        diffRef = list(set(compRef) - set(compChk))
        diffChk = list(set(compChk) - set(compRef))

        if diffRef:
            print(
                "%s exists only in 1st file : \033[92m %s \033[0m \033[91m<< !!!\033[0m"
                % (prefix, str(diffRef))
            )
        if diffChk:
            print(
                "%s exists only in 2nd file : \033[94m %s \033[0m \033[91m<< !!!\033[0m"
                % (prefix, str(diffChk))
            )

        if len(compRef) == len(compChk):
            if sorted(compRef) == sorted(compChk):
                print(
                    "%s : \033[91m ^^ Different order ^^ !!!\033[0m" % (prefix)
                )
            else:
                for i, (refVal, chkVal) in enumerate(zip(compRef, compChk)):
                    if refVal != chkVal:
                        print(
                            "%s : \033[92m %s \033[0m vs \033[94m %s \033[0m \033[91m<< at index %s !!!\033[0m"
                            % (prefix, str(refVal), str(chkVal), str(i))
                        )
                        _compareComponent(
                            refVal, chkVal, "\t" + prefix + ">> ", args, ""
                        )


def _parseIOVDbFolder(definition):
    result = {}
    # db
    db_match = re.search(r"<db>(.*)</db>", definition)
    if db_match:
        result["db"] = db_match.group(1)
        definition = definition.replace(db_match.group(0), "")
    # key
    key_match = re.search(r"<key>(.*)</key>", definition)
    if key_match:
        result["key"] = key_match.group(1)
        definition = definition.replace(key_match.group(0), "")
    # tag
    tag_match = re.search(r"<tag>(.*)</tag>", definition)
    if tag_match:
        result["tag"] = tag_match.group(1)
        definition = definition.replace(tag_match.group(0), "")
    # cache -- ignore for now
    cache_match = re.search(r"<cache>(.*)</cache>", definition)
    if cache_match:
        definition = definition.replace(cache_match.group(0), "")
    # noover
    noover_match = re.search(r"<noover/>", definition)
    if noover_match:
        result["noover"] = True
        definition = definition.replace(noover_match.group(0), "")
    # name
    result["name"] = definition.strip()

    return json.dumps(result)


def _compareIOVDbFolders(compRef, compChk, prefix, args):
    refParsed = []
    chkParsed = []
    for item in compRef:
        refParsed.append(_parseIOVDbFolder(item))
    for item in compChk:
        chkParsed.append(_parseIOVDbFolder(item))
    _compareComponent(refParsed, chkParsed, prefix, args, "")


if __name__ == "__main__":
    parse_args()
