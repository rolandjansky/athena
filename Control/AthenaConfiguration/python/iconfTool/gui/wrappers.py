# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import curses
import logging
from typing import Any, Tuple

from AthenaConfiguration.iconfTool.gui.modals import (
    DoubleHelpModal,
    SearchModal,
    SingleHelpModal,
)
from AthenaConfiguration.iconfTool.gui.pad import Pad
from AthenaConfiguration.iconfTool.models.loaders import (
    ComponentsDiffFileLoader,
    ComponentsFileLoader,
)
from AthenaConfiguration.iconfTool.models.structure import ComponentsStructure

logger = logging.getLogger(__name__)


class GuiLoader:
    def __init__(self, data_loader: ComponentsFileLoader) -> None:
        self.data_loader: ComponentsFileLoader = data_loader
        self.data_structure: ComponentsStructure = self.data_loader.get_data()
        self.width: int = 0
        self.height: int = 0
        self.pad_height: int = 0
        self.pad: Pad
        self.actual_offset: int = 0
        self.screen: Any

    def _initialize_window(self) -> None:
        self.height, self.width = self.screen.getmaxyx()
        self.window = curses.newwin(self.height, self.width, 0, 0)
        self.pad = Pad(self.data_structure, self.width, self.height)
        self.pad_height = self.height - 1
        self.pad.refresh()

    def _initialize(self, stdscreen) -> None:
        self.screen = stdscreen
        self.screen.refresh()
        self._initialize_window()
        self._start_event_loop()

    def load_gui(self) -> None:
        curses.wrapper(self._initialize)

    def search(self, strict: bool = False) -> None:
        search_size: int = 50
        b_starty: int = 0
        b_startx: int = self.width - search_size - 2
        b_width: int = search_size
        b_height: int = 3
        cursor_pos: Tuple[int, int] = curses.getsyx()
        search: SearchModal = SearchModal(
            b_startx, b_starty, b_width, b_height
        )
        text: str = search.edit()
        curses.setsyx(cursor_pos[0], cursor_pos[1])
        self.pad.filter(text)

    def refresh_whole_screen(self) -> None:
        self.window.refresh()
        self.pad.refresh()

    def show_help(self):
        search_size: int = 60
        b_width: int = search_size
        b_height: int = 15
        b_starty: int = self.screen.getmaxyx()[0] - b_height
        b_startx: int = self.width - search_size - 2
        cursor_pos: Tuple[int, int] = curses.getsyx()
        search: SingleHelpModal = SingleHelpModal(
            b_startx, b_starty, b_width, b_height
        )
        curses.setsyx(*cursor_pos)
        search.show_help()
        self.pad.initialize_cursor()

    def _start_event_loop(self) -> None:
        self.show_help()
        while True:
            event: int = self.screen.getch()
            logger.debug(f"Key pressed: [{event}, {chr(event)}]")
            if event == ord("q"):
                break
            elif event == ord("m"):
                self.search()
            elif event == ord("h"):
                self.show_help()
            elif event == ord("r"):
                self.data_structure = self.data_loader.get_data()
                self.pad.reload_data(self.data_structure)
                self.pad.redraw()
            else:
                self.pad.handle_event(event)


class DoublePad:
    def __init__(self, data_loader: ComponentsDiffFileLoader) -> None:
        self.data_loader: ComponentsDiffFileLoader = data_loader
        self.data_structure: ComponentsStructure
        self.diff_structure: ComponentsStructure
        self.data_structure, self.diff_structure = self.data_loader.get_data()
        self.width: int = 0
        self.height: int = 0
        self.pad_height: int = 0
        self.pad: Pad
        self.diff_pad: Pad
        self.current_pad: Pad
        self.inactive_pad: Pad
        self.actual_offset: int = 0
        self.screen: Any
        self.show_diff_only: bool = False

    def _initialize_window(self) -> None:
        self.height, self.width = self.screen.getmaxyx()
        self.window = curses.newwin(self.height, self.width, 0, 0)
        self.pad = Pad(
            self.data_structure, int(self.width / 2 - 1), self.height
        )
        self.diff_pad = Pad(
            self.diff_structure,
            int(self.width / 2) - 1,
            self.height,
            int(self.width / 2) + 2,
        )
        self.pad_height = self.height - 1
        self.current_pad, self.inactive_pad = self.pad, self.diff_pad
        self.diff_pad.hide_cursor()
        self.diff_pad.refresh()
        self.pad.refresh()

    def _initialize(self, stdscreen) -> None:
        self.screen = stdscreen
        self.screen.refresh()
        self._initialize_window()
        self._start_event_loop()

    def load_gui(self) -> None:
        curses.wrapper(self._initialize)

    def search(self):
        search_size: int = 50
        b_starty: int = 0
        b_startx: int = self.width - search_size - 2
        b_width: int = search_size
        b_height: int = 3
        cursor_pos: Tuple[int, int] = curses.getsyx()
        search: SearchModal = SearchModal(
            b_startx, b_starty, b_width, b_height
        )
        text: str = search.edit()
        curses.setsyx(cursor_pos[0], cursor_pos[1])
        self.current_pad.filter(text)
        self.inactive_pad.filter(text)

    def refresh_whole_screen(self) -> None:
        self.window.refresh()
        self.current_pad.refresh()
        self.inactive_pad.refresh()

    def change_actual_pad(self) -> None:
        if self.inactive_pad.lines_empty():
            return
        self.current_pad.hide_cursor()
        self.current_pad, self.inactive_pad = (
            self.inactive_pad,
            self.current_pad,
        )
        self.inactive_pad.refresh()
        self.current_pad.refresh()
        self.current_pad.show_cursor()

    def show_help(self):
        search_size: int = 60
        b_width: int = search_size
        b_height: int = 17
        b_starty: int = self.screen.getmaxyx()[0] - b_height
        b_startx: int = self.width - search_size - 2
        cursor_pos: Tuple[int, int] = curses.getsyx()
        search: DoubleHelpModal = DoubleHelpModal(
            b_startx, b_starty, b_width, b_height
        )
        curses.setsyx(*cursor_pos)
        search.show_help()
        self.current_pad.initialize_cursor()

    def _start_event_loop(self) -> None:
        self.show_help()
        while True:
            event = self.screen.getch()
            logger.debug(f"Key pressed: [{event}, {chr(event)}]")
            if event == ord("q"):
                break
            elif event == ord("\t"):
                self.change_actual_pad()
            elif event == ord("h"):
                self.show_help()
            elif event == ord("m"):
                self.search()
            elif event == ord("r"):
                self.pad.clear()
                self.diff_pad.clear()
                (
                    self.data_structure,
                    self.diff_structure,
                ) = self.data_loader.get_data()
                self.pad.reload_data(self.data_structure)
                self.diff_pad.reload_data(self.diff_structure)

                self.pad.redraw()
                self.diff_pad.redraw()

            else:
                self.current_pad.handle_event(event)
