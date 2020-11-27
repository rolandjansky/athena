# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import curses
import logging
import sys
from typing import Dict, List, Set, Tuple

from AthenaConfiguration.iconfTool.models.element import (
    Element,
    GroupingElement,
)
from AthenaConfiguration.iconfTool.models.structure import ComponentsStructure

logger = logging.getLogger(__name__)


def report():
    logger.error(
        "Can not init textGUI, probably wrong terminal. Try setting export TERM=xterm-256color"
    )
    sys.exit(-1)


class Pad:
    def __init__(
        self,
        data_structure: ComponentsStructure,
        width: int,
        height: int,
        start_x: int = 0,
    ) -> None:
        self.structure: ComponentsStructure = data_structure
        self.structure_list: List[Element] = self.structure.get_list()
        self.root_items_names: Set = {
            element.name for element in self.structure_list
        }
        self.width: int = width
        self.start_x: int = start_x
        self.MAX_PAD_HEIGHT: int = 32000
        self.pad = curses.newpad(self.MAX_PAD_HEIGHT, width)
        self.actual_offset: int = 0
        self.pad_height: int = height - 1

        self.mark_character: str = " "
        self.styles: Dict
        self.lines: List[Element] = []
        self.search_text: str = ""
        try:
            self.initialize_styles()
        except Exception:
            report()
        self.draw_all_structures()
        self.actual_y: int = 0
        try:
            self.initialize_cursor()
        except Exception:
            report()
        self.refresh()

    def initialize_styles(self) -> None:
        curses.init_pair(
            1, curses.COLOR_BLACK, curses.COLOR_WHITE
        )  # Sets up color pair
        highlighted = curses.color_pair(1)  # highlighted menu option
        normal = curses.A_NORMAL  # non-highlighted menu option
        curses.init_pair(3, curses.COLOR_GREEN, curses.COLOR_BLACK)
        marked = curses.color_pair(3)
        self.styles = {
            "normal": normal,
            "highlighted": highlighted,
            "marked": marked,
        }

    def reload_data(self, data_structure: ComponentsStructure) -> None:
        self.structure = data_structure
        self.structure_list = self.structure.get_list()
        self.root_items_names = {
            element.name for element in self.structure_list
        }

    def initialize_cursor(self) -> None:
        # Highlight cursor on initial position
        if self.lines_empty():
            return
        try:
            if self.actual_y >= len(self.lines):
                self.actual_y = 0
            element = self.lines[self.actual_y]
            curses.setsyx(self.actual_y, element.x_pos + 1)
            self.pad.addch(
                curses.getsyx()[0],
                curses.getsyx()[1],
                element.get_mark_character(),
                self.styles["highlighted"],
            )
            curses.curs_set(0)
        except IndexError:
            # Handle situation when self.lines list is empty
            pass

    def draw_structure(self, structure: Element, start_y: int) -> int:
        """start_y - starting vertical position where elements will be drawn"""
        style = self.styles["marked" if structure.is_marked() else "normal"]
        logger.debug(
            f"Adding str: x:{structure.x_pos}, y:{start_y}/{self.MAX_PAD_HEIGHT}"
        )
        try:
            self.pad.addstr(
                start_y,
                structure.x_pos,
                structure.get_line(),
                style,
            )
        except Exception as e:
            logger.error(
                f"Error {e} loading str y:{start_y}, x: {structure.x_pos}"
            )
        self.lines.insert(start_y, structure)
        start_y += 1
        if isinstance(structure, GroupingElement) and structure.show_children:
            for child in structure.children:
                if start_y < self.MAX_PAD_HEIGHT:
                    start_y = self.draw_structure(child, start_y)
                else:
                    logger.error(
                        f"Too many structures: {start_y}. Max count: {self.MAX_PAD_HEIGHT}"
                    )
        return start_y

    def draw_all_structures(self, start_y=0) -> None:
        self.lines = []
        start_y = 0
        for i in self.structure_list:
            if start_y >= self.MAX_PAD_HEIGHT:
                break
            start_y = self.draw_structure(i, start_y)
        self.refresh()

    def refresh(self) -> None:
        self.pad.refresh(
            self.actual_offset,
            0,
            0,
            self.start_x,
            self.pad_height,
            self.width + self.start_x - 2,
        )
        cursor_pos = curses.getsyx()
        curses.setsyx(cursor_pos[0], int(cursor_pos[1] - self.start_x))

    def lines_empty(self) -> bool:
        return len(self.lines) == 0

    def clear(self) -> None:
        self.pad.clear()
        self.refresh()

    def redraw(self, reinit_cursor: bool = False) -> None:
        cursor_pos = curses.getsyx()
        self.pad.clear()
        self.draw_all_structures()
        if self.lines_empty():
            return
        if reinit_cursor or cursor_pos[1] == 0:
            self.actual_offset = 0
            self.actual_y = 0
            self.initialize_cursor()
        else:
            logger.debug(
                f"y: {self.actual_y}, lines length: {len(self.lines)}"
            )
            element = self.lines[self.actual_y]
            self.pad.move(self.actual_y, cursor_pos[1])
            self.pad.addch(
                self.actual_y,
                cursor_pos[1] - 1,
                element.get_mark_character(),
                self.styles["highlighted"],
            )
        self.refresh()

    def filter(self, text: str) -> None:
        self.search_text = text if text else ""
        self.collapse_all()

        if len(self.search_text) > 0:
            self.structure.filter_by_text(text)
            if self.structure.filter.comps_to_save:
                self.expand_all(filter_comps=True)
                for element in self.lines:
                    if text in element.get_name() and not element.is_checked():
                        element.set_as_checked()
        self.redraw()

    def show_all(self) -> None:
        self.structure_list = self.structure.get_list()
        self.redraw(True)

    def get_character(self, y: int, x: int) -> str:
        current_character = chr(
            int(self.pad.inch(y, x)) & 0xFF
        )  # get only the first 8 bits of a number
        return current_character

    def move_cursor(self, number: int) -> None:
        cursor_pos = self.hide_cursor()
        self.actual_y += number
        if self.actual_y < len(self.lines):
            self.set_cursor(
                cursor_pos[0] + number, self.lines[self.actual_y].x_pos + 1
            )

    def hide_cursor(self) -> Tuple[int, int]:
        cursor_pos = curses.getsyx()
        current_character = self.get_character(
            cursor_pos[0] + self.actual_offset, cursor_pos[1] - 1
        )
        self.pad.addch(
            cursor_pos[0] + self.actual_offset,
            cursor_pos[1] - 1,
            current_character,
            self.styles["normal"],
        )
        return cursor_pos

    def show_cursor(self) -> None:
        cursor_pos = curses.getsyx()
        current_character = self.get_character(
            cursor_pos[0] + self.actual_offset, cursor_pos[1] - 1
        )
        self.pad.addch(
            cursor_pos[0] + self.actual_offset,
            cursor_pos[1] - 1,
            current_character,
            self.styles["highlighted"],
        )
        self.refresh()

    def set_cursor(self, y: int, x: int) -> None:
        curses.setsyx(y, x)
        cursor_pos = curses.getsyx()
        current_character = self.get_character(
            cursor_pos[0] + self.actual_offset, cursor_pos[1]
        )
        self.pad.addch(
            cursor_pos[0] + self.actual_offset,
            cursor_pos[1],
            current_character,
            self.styles["highlighted"],
        )
        self.refresh()

    def scroll(self, number: int) -> None:
        self.actual_offset += number
        self.refresh()

    def scroll_to_element(self, new_y: int) -> None:
        self.actual_y = new_y
        self.hide_cursor()
        # Handle the situation when element is above current view
        if new_y < self.actual_offset:
            self.actual_offset = new_y - 1
            if self.actual_offset < 0:
                self.actual_offset = 0
        elif new_y > self.actual_offset + self.pad_height:
            self.actual_offset = new_y - 1
            if self.actual_offset + self.pad_height > len(self.lines):
                self.actual_offset = len(self.lines) - self.pad_height
        self.set_cursor(
            new_y - self.actual_offset, self.lines[new_y].x_pos + 1
        )

    def get_index_by_name(self, name: str) -> int:
        for index, element in enumerate(self.lines):
            if element.parent is None and element.name == name:
                return index
        return -1

    def move_down(self) -> None:
        if self.actual_y < len(self.lines) - 1:
            self.move_cursor(1)
            cursor_pos = curses.getsyx()
            if cursor_pos[0] > self.pad_height - 4:
                self.scroll(1)

    def move_up(self) -> None:
        if len(self.lines) == 0:
            return
        if self.actual_y >= 1:
            self.move_cursor(-1)
            cursor_pos = curses.getsyx()
            if cursor_pos[0] < 4 and self.actual_offset > 0:
                self.scroll(-1)

    def expand(self) -> None:
        if self.actual_y >= len(self.lines):
            return
        element = self.lines[self.actual_y]
        if isinstance(element, GroupingElement):
            self.structure.replace_references(element)
            element.show_children = True
            self.redraw()

    def expand_all(self, filter_comps=False) -> None:
        for element in self.structure_list:
            self.expand_children(element, filter_comps)
        self.structure.filter.comps_to_save = []
        self.redraw()

    def expand_children(self, element: Element, filter_comps) -> None:
        if (
            filter_comps
            and element.get_reference_name()
            not in self.structure.filter.comps_to_save
            and element.get_reference_name() in self.structure.roots_dict
        ):
            logger.debug(
                f"Skipped expanding for: {element.get_reference_name()}"
            )
            return
        if isinstance(element, GroupingElement):
            self.structure.replace_references(element)
            element.show_children = True
            for child in element.children:
                self.expand_children(child, filter_comps)

    def collapse(self) -> None:
        if self.actual_y >= len(self.lines):
            return
        element = self.lines[self.actual_y]
        if isinstance(element, GroupingElement):
            element.show_children = False
            self.redraw()

    def collapse_all(self):
        for element in self.structure_list:
            self.collapse_children(element)
        self.actual_y = 0
        self.redraw()

    def collapse_children(self, element: Element) -> None:
        if isinstance(element, GroupingElement):
            element.show_children = False
            for child in element.children:
                self.collapse_children(child)

    def set_as_checked(self) -> None:
        element = self.lines[self.actual_y]
        if element.checked:
            element.set_as_unchecked()
            self.structure.remove_from_checked(element)
        else:
            element.set_as_checked()
            self.structure.add_to_checked(element)
        self.redraw()

    def handle_event(self, event: int) -> None:
        if event == curses.KEY_DOWN:
            self.move_down()
        elif event == curses.KEY_UP:
            self.move_up()
        elif event == curses.KEY_RIGHT:
            self.expand()
        elif event == curses.KEY_LEFT:
            self.collapse()
        elif event == ord(" "):
            self.set_as_checked()
