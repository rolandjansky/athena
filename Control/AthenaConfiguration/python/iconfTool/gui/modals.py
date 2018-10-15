#!/usr/bin/env python2.7
# -*- coding: utf-8 -*-

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import curses.textpad


class Modal(object):
    def __init__(self, start_x, start_y, width, height):
        self.width = width
        self.height = height
        self.start_x = start_x
        self.start_y = start_y
        self.window = curses.newwin(height, width, start_y, start_x)
        self.window.box()
        self.refresh()

    def refresh(self):
        self.window.refresh()

    def destroy(self):
        del self.window


class SearchModal(Modal):
    def __init__(self, start_x, start_y, width, height):
        super(SearchModal, self).__init__(start_x, start_y, width, height)
        self.search_window = self.window.derwin(self.height - 2,
                                                self.width - 2, 1, 1)
        self.search = curses.textpad.Textbox(self.search_window)

    def edit(self):
        return self.search.edit().strip()


class InputModal(Modal):
    def __init__(self, start_x, start_y, width, height, message):
        super(InputModal, self).__init__(start_x, start_y, width, height)
        self.message_box = self.window.derwin((self.height / 2) - 2,
                                              self.width - 2,
                                              (self.height / 2) - 1, 1)
        self.message_box.addstr(0, (self.width / 2) - len(message) / 2,
                                message, curses.A_NORMAL)
        self.message_box.refresh()
        input_width = 30
        if input_width > (self.width - 2):
            input_width = self.width - 2
        input_start_y, input_start_x = (self.height / 2 + 1,
                                        self.width / 2 - input_width / 2)
        self.input_box = self.window.derwin(1, input_width, input_start_y,
                                            input_start_x)
        self.input = curses.textpad.Textbox(self.input_box)

    def get_input(self):
        return self.input.edit().strip()


class InfoModal(Modal):
    def __init__(self, start_x, start_y, width, height, message_lines):
        super(InfoModal, self).__init__(start_x, start_y, width, height)
        line_y = 1
        for line in message_lines:
            self.window.addstr(line_y, 1, line, curses.A_NORMAL)
            line_y += 1
        self.refresh()
        self.window.getch()
