#!/usr/bin/env python

#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

import ast
import json
import pickle
import re
import sys

from AthenaConfiguration.iconfTool.models.loaders import loadConfigFile, baseParser, componentRenamingDict, loadDifferencesFile, isReference
class fullColor:
    reset="\033[0m"
    difference="\033[91m"
    knowndifference="\033[35m"
    first="\033[92m"
    property="\033[94m"
    second="\033[35m"
    component="\33[92m"
    value="\33[91m"

class noColor:
    reset=""
    difference=""
    knowndifference=""
    first=""
    property=""
    second=""
    component=""
    value=""


def parse_args():
    parser = baseParser
    parser.add_argument(
        "-q", "--quiet", action="store_true", help="Don't print command arguments"
    )
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
        help="Don't report components existing in only of the two configurations",
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
    parser.add_argument("--knownDifferencesFile", 
        help="Ignore differences enlisted in file (to be used only with diffing)")
        
    parser.add_argument("--color",
        help="Use colored output even for file output (useful when piping to less -R to to HTML conversion", 
        action="store_true")
    
    parser.add_argument("-s", "--structured", 
        action='append',
        default=[],
        help="Print only a single component, in a structured manner (reflecting components parent children)")

    parser.add_argument("--includeClasses",
        action='append',
        default=[],
        help="Only list the components selected by the given classname (anchored regular expression)")

    parser.add_argument("--excludeClasses",
        action='append',
        default=[],
        help="Don't list the components excluded by the given classname (anchored regular expression)")

    parser.add_argument("--excludeComponents",
        action='append',
        default=[],
        help="Don't list these components (anchored regular expression)")

    parser.add_argument("--includeClassesSub",
        action='append',
        default=[],
        help="Below the top-level, also include the components selected by the given classname (anchored regular expression)")

    parser.add_argument("--includeSequences",
        help="Include sequences in the structured printout",
        action="store_true")

    parser.add_argument("--classes",
        action="store_true",
        help="Only show class names")

    parser.add_argument("--uniqueClasses",
        action="store_true",
        help="Only show unique classes")

    parser.add_argument("--showComponentName",
        help="Show component name with --classes",
        action="store_true")

    parser.add_argument("--maxDepth",
        help="Maximum depth for structured printout",
        type=int,
        default=None)

    args = parser.parse_args()
    if not args.quiet:
        print("Run with arguments:")
        print( "confTool.py", " ".join(sys.argv[1:]))

    main(args)

knownDifferences={}

def main(args):
    if not args.quiet and args.ignoreIrrelevant:
        print(f"Components to ignore: {args.ignore}")
    color = fullColor()
    if not sys.stdout.isatty() and not args.color: #Remove colors when writing to a file unless forced
        color = noColor()
    if args.printComps:
        for fileName in args.file:
            conf = loadConfigFile(fileName, args)
            _printComps(conf)

    if args.printConf:
        for fileName in args.file:
            conf = loadConfigFile(fileName, args)
            _print(conf, color)

    if args.structured:
        for fileName in args.file:
            conf = loadConfigFile(fileName, args)
            _structuredPrint(conf, args.structured, args)


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
        # renaming only applied on the "reference/1st" file
        configRef = loadConfigFile(args.file[0], args)
        args.renameComps=None 
        args.renameCompsFile=None
        configChk = loadConfigFile(args.file[1], args)
        global knownDifferences
        if args.knownDifferencesFile:
            knownDifferences = loadDifferencesFile(args.knownDifferencesFile)
        _compareConfig(configRef, configChk, args, color)





def _print(conf, color):
    for k, settings in conf.items():
        print(f"{color.component}{k}{color.reset}")
        if isinstance(settings, dict):
            for prop,val in settings.items():
                print(f"   {color.property}{prop} = {color.value}{val}")
        else:
            print(settings)

def _printComps(conf):
    for k, item in conf.items():
        if isinstance(item, dict):
            print(k)

def _structuredPrint(conf, start, args):
    showClasses = args.classes or args.uniqueClasses
    def _oneCompPrint(d, comp, cl, done={}, depth=0, indent = "") -> list:
        show = ((not showClasses or cl is not None) and
                (depth>0 and                 any([re.match(f"({regex})$",cl)   for regex in args.includeClassesSub])) or
                ((not args.includeClasses or any([re.match(f"({regex})$",cl)   for regex in args.includeClasses]))   and
                                         not any([re.match(f"({regex})$",cl)   for regex in args.excludeClasses])    and
                                         not any([re.match(f"({regex})$",comp) for regex in args.excludeComponents])))
        if show:
            if not args.uniqueClasses:
                if args.classes:
                    cc = cl if (not args.showComponentName) else f"{cl}/{comp}"
                    print(f"{indent}{cc}")
                else:
                    print(f"{indent}{comp}")
            newindent = indent + "   - "
            depth += 1
        elif args.includeSequences and ((len(d) == 1 and "Members" in d) or (comp == "ApplicationMgr" and "TopAlg" in d)):
            newindent = indent
        else:
            return []
        if showClasses and args.maxDepth is not None and depth > args.maxDepth:
            return []

        sub = []
        for prop,val in sorted(d.items()):
            if not showClasses and show:
                print(f"{indent}   {prop} = {val}")
            if args.maxDepth is not None and depth > args.maxDepth:
                continue
            if not args.includeSequences and (prop == "Members" or prop == "TopAlg"):
                continue
            for ref,c in isReference(val, comp, conf):
                if showClasses and not c:
                    continue
                if ref in conf and ref not in done:
                    r = _oneCompPrint(conf[ref], ref, c or ref, {**done, ref:True}, depth, newindent)
                    if args.uniqueClasses:
                        sub += r
        if show:
            return [[cl, comp, sub]]
        else:
            return sub

    sub = []
    for comp in start:
        for ref,cl in isReference(start, None, conf) or [(None,None)]:
            if ref and ref in conf:
                settings = conf.get(ref)
                if isinstance(settings, dict):
                    r = _oneCompPrint(settings, ref, cl or ref)
                    sub += r
                elif not showClasses:
                    print(settings)
            else:
                print(f"{comp} is absent in the config, unfortunately the name has to be exact")

    if not args.uniqueClasses:
        return

    def _nocomp(top):
        """copy structure without the component names, so equality tests for the same class names."""
        new = []
        for cl,_,sub in top:
            r = _nocomp(sub)
            new.append([cl,r])
        return new

    from collections import defaultdict
    cls = defaultdict(list)
    def _oneCompPrune(top):
        """Duplicate classes (with identical structure below) are listed, without substructure
        - or not listed at all if they already showed up at this level.
        Classes with the same name are grouped next to each other and shown with their component name."""

        # group classes with the same name together
        ord = defaultdict(list)
        for cl,comp,sub in top:
            ord[cl].append([cl,comp,sub])

        # remove duplicate class structures
        new = []
        dupcls = defaultdict(int)
        for o in ord.values():
            for cl,comp,sub in o:
                comp = comp.split('.')[-1]
                nsub = _nocomp(sub)
                if cl in cls and nsub in cls[cl]:
                    if cl in dupcls:
                        continue
                    r = []
                else:
                    cls[cl].append(nsub)
                    r = _oneCompPrune(sub)
                dupcls[cl] += 1
                new.append([cl,comp,r])

        # only show component for duplicated class names
        if not args.showComponentName:
            for e in new:
                if dupcls[e[0]] < 2:
                    e[1] = None
        return new

    def _structPrint(top, indent = ""):
        for cl,comp,sub in top:
            cc = f"{cl}/{comp}"   if args.showComponentName else \
                 f"{cl} ({comp})" if comp is not None       else cl
            print(f"{indent}{cc}")
            if sub:
                _structPrint(sub, indent + "   - ")

    _structPrint(_oneCompPrune(sub))


def _compareConfig(configRef, configChk, args, color):
    # Find superset of all components:
    allComps = list(set(configRef.keys()) | set(configChk.keys()))
    allComps.sort()

    print("Step 1: reference file #components:", len(configRef))
    print("Step 2: file to check  #components:", len(configChk))
    print("Legend:")
    print(f"{color.difference}Differences in components {color.first}Settings in 1st file {color.second}Settings in 2nd file{color.reset}")

    componentReverseRenamig = {v: k for k, v in componentRenamingDict.items()} # need mapping from new name to old when renaming
    def _componentDescription(comp_name):
        return (comp_name+ " renamed from " + componentReverseRenamig[comp_name]) if comp_name in componentReverseRenamig else comp_name

    for component in allComps:
        if component not in configRef:
            if not args.ignoreMissing:
                print(
                    f"\n{color.second} Component ",
                    _componentDescription(component),
                    f"{color.reset} only in 2nd file {color.reset} \n",
                )
            continue

        if component not in configChk:
            if not args.ignoreMissing:
                print(
                    f"\n{color.first} Component",
                    _componentDescription(component),
                    f"{color.reset}only in 1st file {color.reset} \n",
                )
            continue

        refValue = configRef[component]
        chkValue = configChk[component]

        if chkValue == refValue:
            if args.printIdenticalComponents:
                print("Component", _componentDescription(component), "identical")
        else:
            print(f"{color.difference}Component", _componentDescription(component), f"may differ{color.reset}")
            if not args.allComponentPrint:
                countDifferent = _compareComponent(refValue, chkValue, "\t", args, component, color)
                if countDifferent == 0:
                    print("   but all are suppressed by renaming/known differences/...") 
                else:
                    print(f"  {color.difference} {countDifferent} relevant differences{color.reset}") 
            else:
                print(
                    f"\t{color.first}Ref{color.reset}\t",
                    sorted(configRef[component].items(), key=lambda kv: kv[0]),
                )
                print(
                    f"\t{color.second}Chk{color.reset}\t",
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


def _knownDifference(comp, prop, chkVal, refVal):
    if comp in knownDifferences:
        if prop in knownDifferences[comp]:
            acceptedDifference = knownDifferences[comp][prop]
            if acceptedDifference == (None,None):
                return True
            if acceptedDifference[0] is None:
                return chkVal == acceptedDifference[1]
            if acceptedDifference[1] is None:
                return refVal == acceptedDifference[0]
            else:
                return refVal == acceptedDifference[0] and chkVal == acceptedDifference[1]
    return False

def _handleComponentsReanaming( refVal ):
    """ Rename values in reference as long as they are hashable (and in renamingDict)
        It is a bit of heuristics that is invoved, the assumption is that the property has value of the form A/B if it is name of the compoemnt or it is a just single string"""
    refList = refVal if isinstance(refVal, list) else [refVal]
    updatedRef = []
    for v in refList:
        if  isinstance(v, str):
            if "/" in v and len(v.split("/")) == 2:
                compType,compName = v.split("/")
                newName = componentRenamingDict.get(compName, compName)
                updatedRef.append( f"{compType}/{newName}" )
            else:
                updatedRef.append( componentRenamingDict.get(v, v) )
        else:
            updatedRef.append(v)
    return updatedRef if isinstance(refVal, list) else updatedRef[0]

def _compareComponent(compRef, compChk, prefix, args, component, color):
    countDifferent=0
    if isinstance(compRef, dict):

        allProps = list(set(compRef.keys()) | set(compChk.keys()))
        allProps.sort()

        for prop in allProps:
            if prop not in compRef.keys(): 
                if not _knownDifference(component, prop, compChk[prop], None):
                    print(f"{prefix}{color.property}{prop} = {color.second}{compChk[prop]} {color.reset} only in 2nd file {color.reset}")
                    countDifferent += 1
                else:
                    print(f"{prefix}known difference in: {prop}")
                continue

            if prop not in compChk.keys():
                if not _knownDifference(component, prop, compRef[prop], None):
                    print(f"{prefix}{color.property}{prop} = {color.first}{compRef[prop]} {color.reset} only in 1st file {color.reset}")
                    countDifferent += 1
                else:
                    print(f"{prefix}known difference in: {prop}")
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

            refVal = _handleComponentsReanaming( refVal )

            refVal, chkVal = _parseNumericalValues(refVal, chkVal)
            diffmarker = ""
            if str(chkVal) == str(refVal):
                if not args.printIdenticalPerParameter:
                    continue
            elif _knownDifference(component, prop, chkVal, refVal):
                print(f"{prefix}known difference in: {prop}")
                if not args.printIdenticalPerParameter:
                    continue
            else:
                diffmarker = f" {color.difference}<<{color.reset}"

            if not (component == "IOVDbSvc" and prop == "Folders"):
                print(f"{prefix}{color.property}{prop} = {color.first} {refVal} {color.reset} vs {color.second} {chkVal} {color.reset} {diffmarker}")

            if refVal and ( isinstance(refVal, list) or isinstance(refVal, dict) ):
                if component == "IOVDbSvc" and prop == "Folders":
                    countDifferent += _compareIOVDbFolders(refVal, chkVal, "\t", args, color)
                else:
                    countDifferent += _compareComponent(
                        refVal, chkVal, "\t" + prefix + ">> ", args, component, color
                    )

    elif isinstance(compRef, (list, tuple)) and len(compRef) > 1:

        if isinstance(compRef[0], list):  # to achieve hashability
            compRef = [tuple(el) for el in compRef]

        if len(compChk) > 0 and isinstance(compChk[0], list):
            compChk = [tuple(el) for el in compChk]

        diffRef = list(set(compRef) - set(compChk))
        diffChk = list(set(compChk) - set(compRef))

        if diffRef:
            print(f"{prefix} {color.reset}only in 1st file : {color.first} {diffRef} {color.reset}")
            countDifferent += 1
        if diffChk:
            print(f"{prefix} {color.reset}only in 2nd file :  {color.second} {diffChk} {color.reset}")
            countDifferent += 1

        if len(compRef) == len(compChk):
            if sorted(compRef) == sorted(compChk):
                print(
                    f"{prefix} : {color.difference} ^^ Different order ^^ {color.reset}"
                )
                countDifferent += 1
            else:
                for i, (refVal, chkVal) in enumerate(zip(compRef, compChk)):
                    if refVal != chkVal:
                        print(f"{prefix} : {color.first} {refVal} {color.reset} vs {color.second} {chkVal} {color.reset} {color.difference}<< at index {i} {color.reset}")
                        countDifferent += _compareComponent(
                            refVal, chkVal, "\t" + prefix + ">> ", args, "", color
                        )
    return countDifferent

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


def _compareIOVDbFolders(compRef, compChk, prefix, args, color):
    refParsed = []
    chkParsed = []
    for item in compRef:
        refParsed.append(_parseIOVDbFolder(item))
    for item in compChk:
        chkParsed.append(_parseIOVDbFolder(item))
    return _compareComponent(refParsed, chkParsed, prefix, args, "", color)


if __name__ == "__main__":
    parse_args()
