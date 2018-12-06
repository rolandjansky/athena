#!/usr/bin/env python2.7
# -*- coding: utf-8 -*-

# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

import curses
import logging

from AthenaConfiguration.iconfTool.gui.modals import SearchModal
from AthenaConfiguration.iconfTool.gui.pad import Pad

logger = logging.getLogger('iconfTool.' + __name__)


class GuiLoader(object):
    def __init__(self, data_loader):
        self.data_loader = data_loader
        self.data_structure = self.data_loader.get_data()
        self.width, self.height, self.pad_height = (0, 0, 0)
        self.pad = None
        self.actual_offset = 0
        self.screen = None

    def _initialize_window(self):
        self.height, self.width = self.screen.getmaxyx()
        self.window = curses.newwin(self.height, self.width, 0, 0)
        self.pad = Pad(self.data_structure, self.width, self.height)
        self.pad_height = self.height - 1
        self.pad.refresh()

    def _initialize(self, stdscreen):
        self.screen = stdscreen
        self.screen.refresh()
        self._initialize_window()
        self._start_event_loop()

    def load_gui(self):
        curses.wrapper(self._initialize)

    def search(self, strict=False):
        search_size = 50
        b_starty = 0
        b_startx = self.width - search_size - 2
        b_width = search_size
        b_height = 3
        cursor_pos = curses.getsyx()
        search = SearchModal(b_startx, b_starty, b_width, b_height)
        text = search.edit()
        curses.setsyx(cursor_pos[0], cursor_pos[1])
        self.pad.filter(text, strict)

    def refresh_whole_screen(self):
        self.window.refresh()
        self.pad.refresh()

    def _start_event_loop(self):
        while True:
            event = self.screen.getch()
            if event == ord('q'):
                break
            elif event == ord('f'):
                self.search()
            elif event == ord('m'):
                self.search(True)
            elif event == ord('r'):
                self.data_structure = self.data_loader.get_data()
                self.pad.reload_data(self.data_structure)
            else:
                self.pad.handle_event(event)


class DoublePad(object):
    def __init__(self, data_loader):
        self.data_loader = data_loader
        self.data_structure, self.diff_structure = self.data_loader.get_data()
        self.width, self.height, self.pad_height = (0, 0, 0)
        self.pad = None
        self.diff_pad = None
        self.current_pad = None
        self.inactive_pad = None
        self.actual_offset = 0
        self.screen = None
        self.show_diff_only = False

    def _initialize_window(self):
        self.height, self.width = self.screen.getmaxyx()
        self.window = curses.newwin(self.height, self.width, 0, 0)
        self.pad = Pad(self.data_structure, self.width / 2 - 1, self.height)
        self.diff_pad = Pad(self.diff_structure, self.width / 2 - 1,
                            self.height, self.width / 2 + 2)
        self.pad_height = self.height - 1
        self.current_pad, self.inactive_pad = self.pad, self.diff_pad
        self.diff_pad.hide_cursor()
        self.diff_pad.refresh()
        self.pad.refresh()

    def _initialize(self, stdscreen):
        self.screen = stdscreen
        self.screen.refresh()
        self._initialize_window()
        self._start_event_loop()

    def load_gui(self):
        curses.wrapper(self._initialize)

    def search(self, strict=False):
        search_size = 50
        b_starty = 0
        b_startx = self.width - search_size - 2
        b_width = search_size
        b_height = 3
        cursor_pos = curses.getsyx()
        search = SearchModal(b_startx, b_starty, b_width, b_height)
        text = search.edit()
        curses.setsyx(cursor_pos[0], cursor_pos[1])
        self.current_pad.filter(text, strict)
        self.inactive_pad.filter(text, strict)

    def refresh_whole_screen(self):
        self.window.refresh()
        self.current_pad.refresh()
        self.inactive_pad.refresh()

    def change_actual_pad(self):
        if self.inactive_pad.lines_empty():
            return
        self.current_pad.hide_cursor()
        self.current_pad, self.inactive_pad = self.inactive_pad, \
            self.current_pad
        self.inactive_pad.refresh()
        self.current_pad.refresh()
        self.current_pad.show_cursor()

    def switch_to_diff(self):
        if self.show_diff_only:
            self.current_pad.show_all()
            self.inactive_pad.show_all()
            self.show_diff_only = False
        else:
            self.current_pad.show_diff_only()
            self.inactive_pad.show_diff_only()
            self.show_diff_only = True

    def _start_event_loop(self):
        while True:
            event = self.screen.getch()
            if event == ord('q'):
                break
            elif event == ord('\t'):
                self.change_actual_pad()
            elif event == ord('f'):
                self.search()
            elif event == ord('m'):
                self.search(True)
            elif event == ord('d'):
                self.switch_to_diff()
            elif event == ord('r'):
                self.pad.clear()
                self.diff_pad.clear()
                self.data_structure, self.diff_structure = \
                    self.data_loader.get_data()
                self.pad.reload_data(self.data_structure)
                self.diff_pad.reload_data(self.diff_structure)
            else:
                self.current_pad.handle_event(event)
