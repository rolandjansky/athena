#!/usr/bin/env python

#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from __future__ import print_function

import argparse
import ast
import collections
import json
import pickle
import pprint
import re
import sys

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def parse_args():
    parser = argparse.ArgumentParser(
        description="Utility to transform/display athena configurations"
    )
    parser.add_argument(
        "-p", "--printConf", action="store_true", help="Prints"
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
        "--includeComps",
        nargs="*",
        help="Report only component matching this string",
        action="append",
    )
    parser.add_argument(
        "--excludeComps",
        nargs="*",
        help="Exclude components matching this string",
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
    parser.add_argument(
        "--renameComps",
        nargs="*",
        help="Pass comps You want to rename as OldName=NewName.",
        action="append",
    )

    args = parser.parse_args()
    main(args)


def main(args):
    if args.ignoreIrrelevant:
        args.ignoreList = [
            "StoreGateSvc",
            "OutputLevel",
            "MuonEDMHelperSvc",
            "ExtraInputs",
            "ExtraOutputs",
            "DetStore",
            "EvtStore",
            "NeededResources",
        ]
        print(f"Components to ignore: {args.ignoreList}")

    if args.printComps:
        for fileName in args.file:
            conf = _loadSingleFile(fileName, args)
            _printComps(conf)

    if args.printConf:
        for fileName in args.file:
            conf = _loadSingleFile(fileName, args)
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
        conf = _loadSingleFile(args.file[0], args)
        with open(args.toPickle, "wb") as oFile:
            for item in conf:
                pickle.dump(item, oFile)

    if args.diff:
        if len(args.file) != 2:
            sys.exit(
                "ERROR, can diff exactly two files at a time, got: %s"
                % args.file
            )
        configRef = _loadSingleFile(args.file[0], args)
        configChk = _loadSingleFile(args.file[1], args)
        flattenedRef = {}
        flattenedChk = {}
        for ref in configRef:
            if isinstance(ref, dict):
                flattenedRef.update(ref)
        for chk in configChk:
            if isinstance(chk, dict):
                flattenedChk.update(chk)

        _compareConfig(flattenedRef, flattenedChk, args)


def _loadSingleFile(fname, args):
    conf = []
    if fname.endswith(".pkl"):
        with open(fname, "rb") as input_file:
            # determine if there is a old or new configuration pickled
            cfg = pickle.load(input_file)
            if isinstance(cfg, ComponentAccumulator):  # new configuration
                props = cfg.gatherProps()
                # to make json compatible with old configuration
                jos_props = props[2]
                to_json = {}
                for comp, name, value in jos_props:
                    to_json.setdefault(comp, {})[name] = value
                    to_json[comp][name] = value
                conf = [to_json, props[0], props[1]]

            elif isinstance(
                cfg, (collections.defaultdict, dict)
            ):  # old configuration
                cfg.update(pickle.load(input_file))
                conf.append(pickle.load(input_file))
                conf.append(cfg)
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

    def flatten_list(l):
        return [item for elem in l for item in elem] if l else []

    if (
        args.includeComps or args.excludeComps
    ):  # returning only wanted components

        compsToReport = flatten_list(args.includeComps)
        compsToExclude = flatten_list(args.excludeComps)

        def eligible(component):
            include = any(re.match(s, component) for s in compsToReport)
            exclude = any(re.match(s, component) for s in compsToExclude)
            if args.includeComps and args.excludeComps:
                return include and not exclude
            elif args.includeComps:
                return include
            elif args.excludeComps:
                return not exclude

        conf = [
            {key: value for (key, value) in dic.items() if eligible(key)}
            for dic in conf
            if isinstance(dic, dict)
        ]

    if args.ignoreIrrelevant:

        def remove_irrelevant(val_dict):
            return (
                {
                    key: val
                    for key, val in val_dict.items()
                    if key not in args.ignoreList
                }
                if isinstance(val_dict, dict)
                else val_dict
            )

        conf = [
            {key: remove_irrelevant(value) for (key, value) in dic.items()}
            for dic in conf
            if isinstance(dic, dict)
        ]

    if args.renameComps:
        compsToRename = flatten_list(args.renameComps)
        splittedCompsNames = {
            old_name: new_name
            for old_name, new_name in [
                element.split("=") for element in compsToRename
            ]
        }

        def rename_comps(comp_name):
            return (
                splittedCompsNames[comp_name]
                if comp_name in splittedCompsNames
                else comp_name
            )

        conf = [
            {rename_comps(key): value for (key, value) in dic.items()}
            for dic in conf
            if isinstance(dic, dict)
        ]

    return conf


def _print(conf):
    for item in conf:
        pprint.pprint(dict(item))


def _printComps(conf):
    for item in conf:
        if isinstance(item, dict):
            for compName in item.keys():
                print(compName)


def _compareConfig(configRef, configChk, args):
    # Find superset of all components:
    allComps = list(set(configRef.keys()) | set(configChk.keys()))
    allComps.sort()

    print("Step 1: reference file #components:", len(configRef))
    print("Step 2: file to check  #components:", len(configChk))

    for component in allComps:

        if component not in configRef:
            if not args.ignoreMissing:
                print(
                    "\n\033[91m Component ",
                    component,
                    " \033[94m exists only in 2nd file \033[0m \033[0m \n",
                )
            continue

        if component not in configChk:
            if not args.ignoreMissing:
                print(
                    "\n\033[91m Component",
                    component,
                    " \033[92m exists only in 1st file \033[0m  \033[0m \n",
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
