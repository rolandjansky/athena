# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import pickle
import logging
from typing import Dict, List, Set, Tuple, cast

from AthenaConfiguration.iconfTool.models.element import (
    Element,
    GroupingElement,
)
from AthenaConfiguration.iconfTool.models.structure import ComponentsStructure

logger = logging.getLogger(__name__)


class ComponentsFileLoader:
    def __init__(self, file_path: str, checked_elements=set()) -> None:
        self.file_path: str = file_path
        self.checked_elements: Set[str] = checked_elements

    def _load_file_data(self) -> Dict:
        logger.info(f"Loading {self.file_path}")
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
