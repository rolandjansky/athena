# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

import pickle
import ast
import logging
from typing import Dict, List, Set, Tuple, cast
import collections
import json
import sys
import re
import argparse

from AthenaConfiguration.iconfTool.models.element import (
    Element,
    GroupingElement,
)
from AthenaConfiguration.iconfTool.models.structure import ComponentsStructure

logger = logging.getLogger("confTool")
logger.setLevel(level=logging.INFO)
logger.addHandler(logging.FileHandler("confTool-last-run.log", mode='w'))

componentRenamingDict={}

baseParser = argparse.ArgumentParser()
baseParser.add_argument(
    "--includeComps",
    nargs="*",
    help="Report only component matching this string",
    action="append",
)
baseParser.add_argument(
    "--excludeComps",
    nargs="*",
    help="Exclude components matching this string",
    action="append",
)
baseParser.add_argument(
    "--ignoreIrrelevant",
    help="Ignore differences in e.g. outputlevel",
    action="store_true",
)
baseParser.add_argument(
    "--ignore",
    action="append",
    default= [
        "StoreGateSvc",
        "OutputLevel",
        "ExtraInputs",
        "ExtraOutputs",
        "DetStore",
        "EvtStore",
        "EventStore",
        "NeededResources",
        "GeoModelSvc",
        "MetaDataStore"
    ],
    help="Ignore properties",
)
baseParser.add_argument(
    "--renameComps",
    nargs="*",
    help="Pass comps You want to rename as OldName=NewName.",
    action="append",
)
baseParser.add_argument(
    "--renameCompsFile",
    help="Pass the file containing remaps",
)

baseParser.add_argument(
    "--ignoreDefaults",
    help="Ignore values that are identical to the c++ defaults. Use it only when when the same release is setup as the one used to generate the config.",
    action="store_true"
)

baseParser.add_argument(
    "--ignoreDefaultNamedComps",
    help="""Ignores default handles that have full type specified. That is, if the setting is actually: Tool/A and the default value was just A, the Tool/A is assumed to be default and eliminated.
    Beware that there is a caveat, the ignored class name may be actually different from the default (there is no way to check that in python).""",
    action="store_true"
)


baseParser.add_argument(
    "--shortenDefaultComponents",
    help="Automatically shorten componet names that have a default name i.e. ToolX/ToolX to ToolX. It helps comparing Run2 & Run3 configurations where these are handled differently",
    action="store_true",
)

baseParser.add_argument(
    "--debug",
    help="Enable tool debugging messages",
    action="store_true",
)

def __flatten_list(l):
    return [item for elem in l for item in elem] if l else []


def excludeIncludeComps(dic, args) -> Dict:
    compsToReport = __flatten_list(args.includeComps)
    compsToExclude = __flatten_list(args.excludeComps)
    def eligible(component):
        include = any(re.match(s, component) for s in compsToReport)
        exclude = any(re.match(s, component) for s in compsToExclude)
        if args.includeComps and args.excludeComps:
            return include and not exclude
        elif args.includeComps:
            return include
        elif args.excludeComps:
            return not exclude
    conf = {}
    for (key, value) in dic.items():
        if eligible(key):
            conf[key] = value
        else:
            logger.debug("Ignored component %s", key)
    return conf

def ignoreIrrelevant(dic, args) -> Dict:
    def remove_irrelevant(val_dict):
        return (
            { key: val for key, val in val_dict.items() if key not in args.ignore }
            if isinstance(val_dict, dict)
            else val_dict
        )
    conf = {}
    for (key, value) in dic.items():
        conf[key] = remove_irrelevant(value)
    return conf

def renameComps(dic, args) -> Dict:
    compsToRename = __flatten_list(args.renameComps)
    if args.renameCompsFile:
        with open( args.renameCompsFile, "r") as refile:
            for line in refile:
                if not (line.startswith("#") or line.isspace() ):
                    compsToRename.append( line.rstrip('\n') )
    global componentRenamingDict
    componentRenamingDict.update({
        old_name: new_name
        for old_name, new_name in [
            [e.strip() for e in element.split("=")] for element in compsToRename
        ]
    })
    for f,t in componentRenamingDict.items():
        logger.info("Renaming from: %s to %s", f, t)

    def rename_comps(comp_name):
        """Renames component if it is in the dict or, when name fragment is in the dict
        The later is for cases like: ToolSvc.ToolA.X.Y is renamed to ToolSvc.ToolB.X.Y
        """
        logger.debug("Trying renaming on, %s", comp_name)
        for k,v in componentRenamingDict.items():
            if k == comp_name:
#                logger.debug("Renamed comp %s to %s", k, v)
                return v

            old = f".{k}."
            if old in comp_name:
                return comp_name.replace(old, f".{v}.")

            old = f"{k}."
            if comp_name.startswith(old):
                return comp_name.replace(old, f"{v}.")


            old = f".{k}"
            if comp_name.endswith(old):
                return comp_name.replace(old, f".{k}")
        return comp_name

    conf = {}
    for (key, value) in dic.items():        
        renamed = rename_comps(key)
        if renamed != key:
             logger.debug("Renamed comp %s to %s", key, renamed)
        conf[renamed] = value
    return conf

def ignoreDefaults(allconf, args) -> Dict:
    name_to_type=dict()
    conf = {}

    def drop_defaults(component_name, val_dict):
        # try picking the name from the dict, if missing use last part of the name, if that fails use the componet_name (heuristic)
        component_name_last_part = component_name.split(".")[-1]
        component_type = name_to_type.get(component_name, name_to_type.get(component_name_last_part, component_name_last_part))
        comp_cls = None
        try:
            from AthenaConfiguration.ComponentFactory import CompFactory
            comp_cls = CompFactory.getComp(component_type)
        except Exception:
            logger.debug("Could not find the configuration class %s no defaults for it can be eliminated", component_name)
            return val_dict
        c = {}

        for k,v in val_dict.items():
            if k not in comp_cls._descriptors: # property not in descriptors (for instance, removed from component now)
                c[k] = v
            else:    
                default = str(comp_cls._descriptors[k].default)
                sv = str(v)
                if default == sv or default.replace("StoreGateSvc+", "") == sv.replace("StoreGateSvc+", ""): 
                    logger.debug("Dropped default value %s of property %s in %s because the default is %s", sv, k, component_name, str(default))
                elif args.ignoreDefaultNamedComps and isinstance(v, str) and sv.endswith(f"/{default}"):
                    logger.debug("Dropped speculatively value %s of property %s in %s because the default it ends with %s", sv, k, component_name, str(default))
                else:
                    c[k] = v
                    logger.debug("Keep value %s of property %s in %s because it is different from default %s", str(v), str(k), component_name, str(comp_cls._descriptors[k].default))
        return c

     # collect types for all componets (we look for A/B or lost of A/B strings)
    def collect_types(value):
        """Updates name_to_type mapping"""
        parseable = False
        try:
            s = ast.literal_eval(str(value))
            parseable = True
            if isinstance(s, list):
                for el in s:
                    collect_types(el)
        except Exception:
            pass
        if isinstance(value,str) and "/" in value and not parseable:
            comp = value.split("/")
            if len(comp) == 2:
                name_to_type[comp[1]] = comp[0]
        if isinstance(value, dict):
            [ collect_types(v) for v in value.values() ]

    for (comp_name, comp_settings) in allconf.items():
        collect_types(comp_settings)
    for (comp_name, comp_settings) in allconf.items():
        remaining = drop_defaults(comp_name, comp_settings)
        if len(remaining) != 0: # ignore components that have only default settings
            conf[comp_name] = remaining
    return conf

def shortenDefaultComponents(dic, args) -> Dict:
    conf = {}
    def shorten(val):
        value = val
        # the value can possibly be a serialized object (like a list)
        try:
            value = ast.literal_eval(str(value))
        except Exception:
            pass

        if isinstance(value, str):
            svalue = value.split("/")
            if len(svalue) == 2 and svalue[0] == svalue[1]:
                logger.debug("Shortened %s", svalue)
                return svalue[0]
        if isinstance(value, list):
            return [shorten(el) for el in value]
        if isinstance(value, dict):
            return shorten_defaults(value)

        return value

    def shorten_defaults(val_dict):
        if isinstance(val_dict, dict):
            return { key: shorten(val) for key,val in val_dict.items() }

    for (key, value) in dic.items():
        conf[key] = shorten_defaults(value)
    return conf


def loadConfigFile(fname, args) -> Dict:
    """loads config file into a dictionary, supports several modifications of the input switched on via additional arguments
    Supports reading: Pickled file with the CA or properties & JSON
    """
    if args.debug:
        print("Debugging info from reading ", fname, " in ", logger.handlers[0].baseFilename)
        logger.setLevel(logging.DEBUG)

    conf = {}
    if fname.endswith(".pkl"):
        with open(fname, "rb") as input_file:
            # determine if there is a old or new configuration pickled
            cfg = pickle.load(input_file)
            logger.info("... Read %s from %s", cfg.__class__.__name__, fname)
            from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
            if isinstance(cfg, ComponentAccumulator):  # new configuration
                props = cfg.gatherProps()
                # to make json compatible with old configuration
                jos_props = props[2]
                to_json = {}
                for comp, name, value in jos_props:
                    to_json.setdefault(comp, {})[name] = value
                    to_json[comp][name] = value
                conf.update(to_json)
                conf.update(props[0])
                conf.update(props[1])

            elif isinstance(
                cfg, (collections.defaultdict, dict)
            ):  # old configuration
                conf.update(cfg)
                conf.update(pickle.load(input_file)) # special services
                # FIXME: there's a third pickle object with python components
            elif isinstance(cfg, (collections.Sequence)):
                for c in cfg:
                    conf.update(c)
        logger.info("... Read %d items from python pickle file: %s", len(conf), fname)

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
            cfg = json.load(input_file, object_hook=__keepPlainStrings)
            for c in cfg:
                conf.update(cfg)

        logger.info("... Read %d items from python pickle file: %s", len(conf), fname)

    else:
        sys.exit("File format not supported.")

    if conf is None:
        sys.exit("Unable to load %s file" % fname)


    if args.includeComps or args.excludeComps:
        conf = excludeIncludeComps(conf, args)

    if args.ignoreIrrelevant:
        conf = ignoreIrrelevant(conf, args)

    if args.renameComps or args.renameCompsFile:
        conf = renameComps(conf, args)

    if args.ignoreDefaults:
        conf = ignoreDefaults(conf, args)

    if args.shortenDefaultComponents:
        conf = shortenDefaultComponents(conf, args)
    return conf

class ComponentsFileLoader:
    def __init__(self, file_path: str, args, checked_elements=set()) -> None:
        self.file_path: str = file_path
        self.checked_elements: Set[str] = checked_elements
        self.args = args

    def _load_file_data(self) -> Dict:
        logger.info(f"Loading {self.file_path}")
        return loadConfigFile(self.file_path, self.args)


    def load_structure(self) -> ComponentsStructure:
        data = self._load_file_data()
        structure = ComponentsStructure(data, self.checked_elements)
        structure.generate()
        return structure

    def get_data(self) -> ComponentsStructure:
        return self.load_structure()


class ComponentsDiffFileLoader:
    def __init__(
        self,
        file_path: str,
        diff_file_path: str,
        checked_elements: Set[str],
    ) -> None:
        self.main_loader: ComponentsFileLoader = ComponentsFileLoader(
            file_path, checked_elements
        )
        self.diff_loader: ComponentsFileLoader = ComponentsFileLoader(
            diff_file_path, checked_elements
        )

    def get_data(self) -> Tuple[ComponentsStructure, ComponentsStructure]:
        structure = self.main_loader.load_structure()
        diff_structure = self.diff_loader.load_structure()
        self.mark_differences(structure.get_list(), diff_structure.get_list())
        return structure, diff_structure

    def equal(self, first: Element, second: Element) -> bool:
        return (
            first.get_name() == second.get_name()
            and first.x_pos == second.x_pos
            and type(first) == type(second)
        )

    def mark_differences(
        self, structure: List[Element], diff_structure: List[Element]
    ) -> None:
        i, j = 0, 0
        while i < len(structure) and j < len(diff_structure):
            if self.equal(structure[i], diff_structure[j]):
                if isinstance(structure[i], GroupingElement):
                    self.mark_differences(
                        cast(GroupingElement, structure[i]).children,
                        cast(GroupingElement, diff_structure[j]).children,
                    )
                i += 1
                j += 1
                continue

            # Find equal element in diff structure
            for tmp_j in range(j, len(diff_structure)):
                if self.equal(structure[i], diff_structure[tmp_j]):
                    for marking_j in range(j, tmp_j):
                        diff_structure[marking_j].mark()
                    j = tmp_j
                    break
            else:
                # Not found equal element in diff structure
                # Find equal element in first structure
                for tmp_i in range(i, len(structure)):
                    if self.equal(structure[tmp_i], diff_structure[j]):
                        for marking_i in range(i, tmp_i):
                            structure[marking_i].mark()
                        i = tmp_i
                        break
                else:
                    structure[i].mark()
                    diff_structure[j].mark()
                    i += 1
                    j += 1

        # Mark remaining elements in both structures
        while i < len(structure):
            structure[i].mark()
            i += 1

        while j < len(diff_structure):
            diff_structure[j].mark()
            j += 1


def loadDifferencesFile(fname) -> Dict:
    """
    Read differences file
    Format:
    full_component_name.property oldvalue=newvalue
    example:
    AlgX.ToolA.SubToolB.SettingZ 45=46
    It is possible to specify missing values, e.g:
    AlgX.ToolA.SubToolB.SettingZ 45=    means that now the old value should be ignored
    AlgX.ToolA.SubToolB.SettingZ =46    means that now the new value should be ignored
    AlgX.ToolA.SubToolB.SettingZ =      means that any change of the value should be ignored 

    """
    from collections import defaultdict
    differences = defaultdict(dict)
    count=0
    with open(fname, "r") as f:
        for line in f:
            if line[0] == "#" or line == "\n":
                continue
            line = line.strip()
            compAndProp, values = line.split(" ")           
            comp, prop = compAndProp.rsplit(".", 1)
            o,n = values.split("=")
            oldval,newval = o if o else None, n if n else None
                
            differences[comp][prop] = (oldval,newval)
            count+=1
    logger.info("... Read %d known differences from file: %s", count, fname)
    logger.info("..... %s", str(differences))

    return differences

