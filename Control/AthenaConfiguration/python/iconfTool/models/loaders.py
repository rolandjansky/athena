# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

import pickle
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

logger = logging.getLogger(__name__)


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
        "MuonEDMHelperSvc",
        "ExtraInputs",
        "ExtraOutputs",
        "DetStore",
        "EvtStore",
        "NeededResources",
    ],
    help="Ignore properties",
)
baseParser.add_argument(
    "--renameComps",
    nargs="*",
    help="Pass comps You want to rename as OldName=NewName.",
    action="append",
)

def loadConfigFile(fname, args) -> Dict:
    """loads config file into a dictionary, supports several modifications of the input switched on via additional arguments
    Supports reading: Pickled file with the CA or properties & JSON
    """
    conf = {}
    if fname.endswith(".pkl"):
        with open(fname, "rb") as input_file:
            # determine if there is a old or new configuration pickled
            cfg = pickle.load(input_file)
            print("... Read", cfg.__class__.__name__, "from", fname)
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
            cfg = json.load(input_file, object_hook=__keepPlainStrings)
            for c in cfg:
                conf.update(cfg)

            print("... Read", len(conf), "items from JSON file: ", fname)

    else:
        sys.exit("File format not supported.")

    if conf is None:
        sys.exit("Unable to load %s file" % fname)

    def flatten_list(l):
        return [item for elem in l for item in elem] if l else []

    if args.includeComps or args.excludeComps:

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

        dic = conf
        conf = {}
        for (key, value) in dic.items():
            if eligible(key):
                conf[key] = value

    if args.ignoreIrrelevant:
        def remove_irrelevant(val_dict):
            return (
                {
                    key: val
                    for key, val in val_dict.items()
                    if key not in args.ignore
                }
                if isinstance(val_dict, dict)
                else val_dict
            )
        dic = conf
        conf = {}
        for (key, value) in dic.items():
            conf[key] = remove_irrelevant(value)

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

        dic = conf
        conf = {}
        for (key, value) in dic.items():
            conf[rename_comps(key)] = value

    return conf

class ComponentsFileLoader:
    def __init__(self, file_path: str, checked_elements=set()) -> None:
        self.file_path: str = file_path
        self.checked_elements: Set[str] = checked_elements

    def _load_file_data(self) -> Dict:
        logger.info(f"Loading {self.file_path}")
        return loadConfigFile(self.file_path)
        with open(self.file_path, "rb") as f:
            return dict(pickle.load(f))

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
