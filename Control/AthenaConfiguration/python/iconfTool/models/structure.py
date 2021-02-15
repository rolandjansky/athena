# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import ast
import logging
from typing import Callable, Dict, List, Sequence, Set, Tuple

from AthenaConfiguration.iconfTool.models.element import (
    Element,
    SingleElement,
    GroupingElement,
    ValueElement,
)

logger = logging.getLogger(__name__)
INDENT = 3


class Structure:
    def __init__(self, flags_dict: Dict, checked_elements=set()) -> None:
        self.flags_dict: Dict = flags_dict
        self.structure_dict: Dict = {}
        self.checked_elements: Set = (
            checked_elements  # Set of checked elements
        )
        # hashed

    def generate_structure_dict(self) -> Dict:
        """Split dictionary keys into nested structure
        e.g. Test1.Test2: Value -> Test1: {Test2: Value}
        """
        structure_dict: Dict = {}

        for flag, value in self.flags_dict.items():
            groups = flag.split(".")
            structure_point = structure_dict
            for i in range(len(groups) - 1):
                if groups[i] not in structure_point:
                    structure_point[groups[i]] = {}
                elif isinstance(structure_point[groups[i]], str):
                    structure_point[groups[i]] = {
                        structure_point[groups[i]]: {}
                    }
                structure_point = structure_point[groups[i]]
            if groups:
                last = groups[-1]
                if (
                    last in structure_point
                    and isinstance(structure_point[last], dict)
                    and isinstance(value, dict)
                ):
                    structure_point[last].update(value)
                else:
                    structure_point[last] = value
        return structure_dict

    def generate_for_element(
        self, grouping_element: GroupingElement, structure: Dict
    ) -> None:
        child_x_pos = grouping_element.x_pos + INDENT
        for child in structure:
            try:
                # Handles situation when list or dict is provided as a string
                structure[child] = ast.literal_eval(structure[child])
            except (ValueError, SyntaxError):
                pass

            if isinstance(structure[child], dict):
                group = GroupingElement(child, child_x_pos, grouping_element)
                if group.hash in self.checked_elements:
                    group.set_as_checked()
                self.generate_for_element(group, structure[child])
                grouping_element.add_child(group)
            elif isinstance(structure[child], list):
                group = GroupingElement(child, child_x_pos, grouping_element)
                if group.hash in self.checked_elements:
                    group.set_as_checked()
                for el in structure[child]:
                    no_value_child = SingleElement(el, child_x_pos + INDENT)
                    if no_value_child.hash in self.checked_elements:
                        no_value_child.set_as_checked()
                    group.add_child(no_value_child)
                grouping_element.add_child(group)
            else:
                flag = ValueElement(
                    child, child_x_pos, structure[child], grouping_element
                )
                if flag.hash in self.checked_elements:
                    flag.set_as_checked()
                grouping_element.add_child(flag)

    def add_to_checked(self, element: Element) -> None:
        hash_value = element.hash
        self.checked_elements.add(hash_value)

    def remove_from_checked(self, element: Element) -> None:
        hash_value = element.hash
        if hash_value in self.checked_elements:
            self.checked_elements.remove(hash_value)


class ComponentsStructure(Structure):
    def __init__(self, flags_dict: Dict, checked_elements: Set) -> None:
        super().__init__(flags_dict, checked_elements)
        self.roots_dict: Dict = {}
        self.structure_list: List[Element] = []
        self.filter: ComponentsStructureFilter  # Filter object used in search

    def replace_references(self, element: GroupingElement) -> None:
        element.replaced = True
        for i, child in enumerate(element.children):
            if isinstance(child, SingleElement):
                ref_name = child.get_reference_name()
                if ref_name in self.roots_dict and not element.has_parent(
                    self.roots_dict[ref_name].get_name()
                ):
                    element.children[i] = self.roots_dict[ref_name]

                    element.children[i].parent = element
                    element.update_xpos(element.x_pos)

    def generate(self, x_pos: int = 1, structure_dict=None) -> None:

        self.structure_dict = structure_dict or self.generate_structure_dict()
        for root_name in self.structure_dict:
            if isinstance(self.structure_dict[root_name], dict):
                root_group = GroupingElement(root_name, x_pos)
                if root_group.hash in self.checked_elements:
                    root_group.set_as_checked()
                self.generate_for_element(
                    root_group, self.structure_dict[root_name]
                )
                self.structure_list.append(root_group)
                self.roots_dict[root_name] = root_group

        self.structure_list = self.group_elements(
            self.structure_list, x_pos - 1
        )

        self.sort()
        self.filter = ComponentsStructureFilter(self.structure_dict)

    def group_elements(
        self, structure_list: List[Element], x_pos: int
    ) -> List[Element]:

        algs = GroupingElement("Algorithms and Sequences", x_pos)
        tools = GroupingElement("Public tools", x_pos)
        settings = GroupingElement("Application Settings and Services", x_pos)

        new_structure_list: List[Element] = [algs, tools, settings]

        seq_names = []

        def get_ath_sequences(el) -> None:
            if isinstance(el, GroupingElement):
                members_obj = el.get_child_by_name("Members")
                if members_obj:
                    assert isinstance(members_obj, GroupingElement)
                    for member in members_obj.children:
                        ref_name = member.get_reference_name()
                        seq_names.append(ref_name)
                        get_ath_sequences(self.roots_dict.get(ref_name))

        seq_names.append("AthMasterSeq")
        get_ath_sequences(self.roots_dict["AthMasterSeq"])

        for el in structure_list:
            if el.name == "AthMasterSeq":
                algs.add_child(el)
            elif (
                el.name == "ToolSvc"
            ):  # any of ToolSvc children is not present in roots dict
                tools.add_child(el)
            elif el.name not in seq_names:
                settings.add_child(el)

        return new_structure_list

    def get_list(self) -> List[Element]:
        self.filter.reset()
        return self.structure_list

    def filter_by_text(self, filter_text: str) -> None:
        self.filter.generate_by_text(filter_text)
        for element in self.structure_list:
            assert isinstance(element, GroupingElement)
            element.children = list(
                filter(
                    lambda child: child.get_reference_name()
                    in self.filter.comps_to_save
                    or child not in self.roots_dict,
                    element.children,
                )
            )

    def sort(self) -> None:
        self.structure_list.sort(key=lambda el: el.get_name().lower())
        for element in self.structure_list:
            element.sort_children()


class ComponentsStructureFilter:
    def __init__(self, structure_dict: Dict[str, str]) -> None:
        self.structure_dict: Dict[str, str] = structure_dict
        self.comps_to_save: List[str] = []

    def reset(self) -> None:
        self.comps_to_save = []

    def generate_by_text(self, filter_text: str) -> None:
        self.reset()
        found_elements: List[str] = []
        self.search_elements(filter_text, found_elements)
        if not found_elements:
            return
        found_elements.extend(
            [
                "Algorithms and Sequences",
                "Public tools",
                "Application Settings and Services",
            ]
        )
        self.comps_to_save = found_elements
        logger.debug(f"SEARCH - found elements: {found_elements}")

    @staticmethod
    def find_text_in_dict(
        filter_text: str, structure_dict: Dict
    ) -> List[Tuple[str, str]]:
        return list(
            filter(lambda el: filter_text in str(el), structure_dict.items())
        )

    def search_elements(
        self, filter_text: str, found_elements: List[str]
    ) -> None:
        for el in self.find_text_in_dict(filter_text, self.structure_dict):
            if el[0] not in found_elements:
                found_elements.append(el[0])
                self.search_elements(el[0], found_elements)

    def browse_and_save_by_text(
        self, elements: Sequence[Element], filter_lambda: Callable
    ) -> None:
        filtered = list(filter(filter_lambda, elements))
        self.filter_children(filtered)

    @staticmethod
    def filter_children(filtered: Sequence[Element]) -> None:
        if len(filtered) > 0:
            first = filtered[0]
            if first.parent:
                first.parent.children = list(filtered)
