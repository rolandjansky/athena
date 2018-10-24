#!/usr/bin/env python2.7
# -*- coding: utf-8 -*-

# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

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
