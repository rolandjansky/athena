# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from __future__ import annotations
import logging
from typing import Any, List, Optional, Sequence

logger = logging.getLogger(__name__)

INDENT = 3


class Element:
    def __init__(self, name: str, x_pos: int, parent=None) -> None:
        self.name: str = f"{name}"  # handles situation when name is not str
        self.x_pos: int = x_pos
        self.parent: GroupingElement = parent
        self.show_children: bool = False
        self.marked: bool = False  # For diff displaying
        self.checked: bool = False  # For disabling diff highlight
        self.replaced: bool = False  # To prevent replacing multiple times
        self.hash: str = ""
        self.generate_hash()

    def show(self) -> None:
        if self.parent:
            self.parent.show_children = True
            self.parent.show()

    def get_name(self) -> str:
        return self.name

    def update_xpos(self, new_xpos: int) -> None:
        self.x_pos = new_xpos

    def get_as_flat_list(self) -> Sequence[Element]:
        return [self]

    def mark(self) -> None:
        self.marked = True

    def has_parent(self, name_string: str) -> bool:
        parent = self.parent
        while parent:
            if parent.get_name() == name_string:
                return True
            parent = parent.parent  # Go level up
        return False

    def generate_hash(self) -> None:
        base = self.get_name()
        if self.parent:
            base = self.parent.hash + base
        self.hash = str(hash(base))

    def is_checked(self) -> bool:
        if self.checked:
            return True
        parent = self.parent
        while parent and parent.marked:
            if parent.checked:
                return True
            parent = parent.parent
        return False

    def is_marked(self) -> bool:
        return self.marked and not self.is_checked()

    def set_as_checked(self) -> None:
        self.checked = True

    def get_line(self) -> str:
        pass

    def set_as_unchecked(self) -> None:
        self.checked = False

    def get_mark_character(self) -> str:
        return " " if not self.checked else "X"

    def sort_children(self) -> None:
        pass

    def check_filter(self, filter_text: str) -> bool:
        pass

    def get_reference_name(self) -> str:
        pass


class GroupingElement(Element):
    def __init__(
        self, name: str, x_pos: int, parent: GroupingElement = None
    ) -> None:
        super().__init__(name, x_pos, parent)
        self.children: List[Element] = []

    def get_names(self) -> List[str]:
        return [el.name for el in self.children]

    def get_child_by_name(self, name: str) -> Optional[Element]:
        matched = list(filter(lambda el: el.name == name, self.children))
        if matched:
            return matched[0]
        return None

    def check_filter(self, filter_text: str) -> bool:
        if filter_text in self.get_name():
            return True
        for child in self.children:
            if child.check_filter(filter_text):
                return True
        return False

    def add_child(self, child: Element) -> None:
        self.children.append(child)
        child.parent = self

    def update_xpos(self, new_xpos: int) -> None:
        self.x_pos = new_xpos
        for child in self.children:
            child.update_xpos(new_xpos + INDENT)

    def mark(self) -> None:
        self.marked = True
        for child in self.children:
            child.mark()

    def sort_children(self) -> None:
        self.children.sort(key=lambda c: c.get_name().lower())
        for child in self.children:
            if isinstance(child, GroupingElement):
                child.sort_children()

    def get_reference_name(self) -> str:
        return self.get_name()

    def get_line(self) -> str:
        return f"[{self.get_mark_character()}] {self.get_name()}"


class SingleElement(Element):
    def check_filter(self, filter_text: str) -> bool:
        return filter_text in self.get_name()

    def get_reference_name(self) -> str:
        return str(self.name).split("/")[-1]

    def get_mark_character(self) -> str:
        return "-" if not self.checked else "*"

    def get_line(self) -> str:
        return f" {self.get_mark_character()} {self.get_name()}"


class ValueElement(SingleElement):
    def __init__(
        self, name: str, x_pos: int, value: Any, parent: Element = None
    ) -> None:
        """Value must be initialized first as it is used in hash generating"""
        self.value: Any = value
        super().__init__(name, x_pos, parent)

    def get_name(self) -> str:
        return f"{self.name} = {self.value}"

    def get_reference_name(self) -> str:
        return str(self.value).split("/")[-1]
