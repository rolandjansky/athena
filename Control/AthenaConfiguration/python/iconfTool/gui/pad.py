#!/usr/bin/env python2.7
# -*- coding: utf-8 -*-

# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

import curses

import logging

logger = logging.getLogger('iconfTool.' + __name__)


class Pad(object):
    def __init__(self, data_structure, width, height, start_x=0):
        self.structure = data_structure
        self.structure_list = self.structure.get_list()
        self.root_items_names = {element.name for element in
                                 self.structure_list}
        self.width = width
        self.start_x = start_x
        self.MAX_PAD_HEIGHT = 32000
        self.pad = curses.newpad(self.MAX_PAD_HEIGHT, width)
        self.actual_offset = 0
        self.pad_height = height - 1

        self.mark_character = ' '
        self.styles = None
        self.lines = []
        self.search_text = ''

        self.initialize_styles()
        self.draw_all_structures()
        self.actual_y = 0
        self.initialize_cursor()
        self.refresh()

    def initialize_styles(self):
        curses.init_pair(1, curses.COLOR_BLACK,
                         curses.COLOR_WHITE)  # Sets up color pair
        highlighted = curses.color_pair(1)  # highlighted menu option
        normal = curses.A_NORMAL  # non-highlighted menu option
        curses.init_pair(3, curses.COLOR_GREEN, curses.COLOR_BLACK)
        marked = curses.color_pair(3)
        self.styles = {'normal': normal, 'highlighted': highlighted,
                       'marked': marked}

    def reload_data(self, data_structure):
        self.structure = data_structure
        self.structure_list = self.structure.get_list()
        self.root_items_names = {element.name for element in
                                 self.structure_list}
        self.filter(self.search_text)  # Filter also draws structures

    def initialize_cursor(self):
        # Highlight cursor on initial position
        if self.lines_empty():
            return
        try:
            if self.actual_y >= len(self.lines):
                self.actual_y = 0
            element = self.lines[self.actual_y]
            curses.setsyx(self.actual_y, element.x_pos + 1)
            self.pad.addch(curses.getsyx()[0], curses.getsyx()[1],
                           element.get_mark_character(),
                           self.styles['highlighted'])
            curses.curs_set(0)
        except IndexError:
            # Handle situation when self.lines list is empty
            pass

    def draw_structure(self, structure, start_y):
        """start_y - starting vertical position where elements will be drawn"""
        if start_y >= self.MAX_PAD_HEIGHT:
            return
        style = self.styles['marked' if structure.is_marked() else 'normal']
        self.pad.addstr(start_y, structure.x_pos,
                        '[{}] {}'.format(structure.get_mark_character(),
                                         structure.get_name()), style)
        self.lines.insert(start_y, structure)
        start_y += 1
        if structure.type == 'GROUP' and structure.show_children:
            for child in structure.children:
                start_y = self.draw_structure(child, start_y)
        return start_y

    def draw_all_structures(self):
        self.lines = []
        start_y = 0
        for i in self.structure_list:
            if start_y >= self.MAX_PAD_HEIGHT:
                break
            start_y = self.draw_structure(i, start_y)
        self.refresh()

    def refresh(self):
        self.pad.refresh(self.actual_offset, 0, 0, self.start_x,
                         self.pad_height, self.width + self.start_x - 2)
        cursor_pos = curses.getsyx()
        curses.setsyx(cursor_pos[0], cursor_pos[1] - self.start_x)

    def lines_empty(self):
        return len(self.lines) == 0

    def clear(self):
        self.pad.clear()
        self.refresh()

    def redraw(self, reinit_cursor=False):
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
            element = self.lines[self.actual_y]
            self.pad.move(self.actual_y, cursor_pos[1])
            self.pad.addch(self.actual_y, cursor_pos[1] - 1,
                           element.get_mark_character(),
                           self.styles['highlighted'])
        self.refresh()

    def filter(self, text, strict=False):
        """ If is strict then it leaves only elements which fits to text
        If not then it leaves whole structures in which text appear somewhere
        """
        self.search_text = text if text else ''
        if len(self.search_text) > 0:
            if strict:
                self.structure_list = self.structure.get_filtered_by_text(text)
            else:
                self.structure_list = filter(
                    lambda el: el.check_filter(self.search_text),
                    self.structure.get_list())
        else:
            self.structure_list = self.structure.get_list()
        self.redraw(True)

    def show_diff_only(self):
        self.structure_list = self.structure.get_filtered_by_mark()
        self.redraw(True)

    def show_all(self):
        self.structure_list = self.structure.get_list()
        self.redraw(True)

    def get_character(self, y, x):
        current_character = chr(self.pad.inch(y, x) & 0xFF)
        return current_character

    def move_cursor(self, number):
        cursor_pos = self.hide_cursor()
        self.actual_y += number
        if self.actual_y < len(self.lines):
            self.set_cursor(cursor_pos[0] + number,
                            self.lines[self.actual_y].x_pos + 1)

    def hide_cursor(self):
        cursor_pos = curses.getsyx()
        current_character = self.get_character(
            cursor_pos[0] + self.actual_offset, cursor_pos[1] - 1)
        self.pad.addch(cursor_pos[0] + self.actual_offset, cursor_pos[1] - 1,
                       current_character,
                       self.styles['normal'])
        return cursor_pos

    def show_cursor(self):
        cursor_pos = curses.getsyx()
        current_character = self.get_character(
            cursor_pos[0] + self.actual_offset, cursor_pos[1] - 1)
        self.pad.addch(cursor_pos[0] + self.actual_offset, cursor_pos[1] - 1,
                       current_character,
                       self.styles['highlighted'])
        self.refresh()

    def set_cursor(self, y, x):
        curses.setsyx(y, x)
        cursor_pos = curses.getsyx()
        current_character = self.get_character(
            cursor_pos[0] + self.actual_offset, cursor_pos[1])
        self.pad.addch(cursor_pos[0] + self.actual_offset, cursor_pos[1],
                       current_character,
                       self.styles['highlighted'])
        self.refresh()

    def scroll(self, number):
        self.actual_offset += number
        self.refresh()

    def scroll_to_element(self, new_y):
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
        self.set_cursor(new_y - self.actual_offset,
                        self.lines[new_y].x_pos + 1)

    def get_index_by_name(self, name):
        for index, element in enumerate(self.lines):
            if element.parent is None and element.name == name:
                return index
        return -1

    def move_down(self):
        if self.actual_y < len(self.lines) - 1:
            self.move_cursor(1)
            cursor_pos = curses.getsyx()
            if cursor_pos[0] > self.pad_height - 4:
                self.scroll(1)

    def move_up(self):
        if len(self.lines) == 0:
            return
        if self.actual_y >= 1:
            self.move_cursor(-1)
            cursor_pos = curses.getsyx()
            if cursor_pos[0] < 4 and self.actual_offset > 0:
                self.scroll(-1)

    def expand(self):
        if self.actual_y >= len(self.lines):
            return
        element = self.lines[self.actual_y]
        if element.type == 'GROUP':
            element.show_children = True
            self.redraw()

    def collapse(self):
        if self.actual_y >= len(self.lines):
            return
        element = self.lines[self.actual_y]
        if element.type == 'GROUP':
            element.show_children = False
            self.redraw()

    def set_as_checked(self):
        element = self.lines[self.actual_y]
        if element.checked:
            element.set_as_unchecked()
            self.structure.remove_from_checked(element)
        else:
            element.set_as_checked()
            self.structure.add_to_checked(element)
        self.redraw()

    def go_to_reference(self):
        if self.actual_y >= len(self.lines):
            return
        element = self.lines[self.actual_y]
        if not element.type == 'SINGLE':
            return
        reference_name = element.get_reference_name()
        if reference_name not in self.root_items_names:
            return
        # Cancel eventual search result
        self.filter(None)
        index = self.get_index_by_name(reference_name)
        if index != -1:
            self.scroll_to_element(index)
        else:
            # Scroll to first element if name was not found
            # (should never happen as the name is checked
            # in root_items_names first)
            self.scroll_to_element(0)

    def handle_event(self, event):
        if event == curses.KEY_DOWN:
            self.move_down()
        elif event == curses.KEY_UP:
            self.move_up()
        elif event == curses.KEY_RIGHT:
            self.expand()
        elif event == curses.KEY_LEFT:
            self.collapse()
        elif event == ord(' '):
            self.set_as_checked()
        elif event == ord('g'):
            self.go_to_reference()
