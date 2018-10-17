#!/usr/bin/env python2.7
# -*- coding: utf-8 -*-

# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

import logging

logger = logging.getLogger('iconfTool.' + __name__)

INDENT = 3


class Element(object):
    def __init__(self, name, x_pos, parent=None):
        self.name = '{}'.format(name) # handles situation when name is not str
        self.x_pos = x_pos
        self.parent = parent
        self.show_children = False
        self.marked = False  # For diff displaying
        self.checked = False  # For disabling diff highlight
        self.replaced = False  # To prevent replacing multiple times
        self.hash = None
        self.generate_hash()

    def get_name(self):
        return self.name

    def update_xpos(self, new_xpos):
        self.x_pos = new_xpos

    def get_as_flat_list(self):
        return [self]

    def mark(self):
        self.marked = True

    def has_parent(self, name_string):
        parent = self.parent
        while parent:
            if parent.get_name() == name_string:
                return True
            parent = parent.parent  # Go level up
        return False

    def set_parent(self, parent):
        self.parent = parent
        self.generate_hash()

    def generate_hash(self):
        base = self.get_name()
        if self.parent:
            base = self.parent.hash + base
        self.hash = str(hash(base))

    def is_checked(self):
        if self.checked:
            return True
        parent = self.parent
        while parent and parent.marked:
            if parent.checked:
                return True
            parent = parent.parent
        return False

    def is_marked(self):
        return self.marked and not self.is_checked()

    def set_as_checked(self):
        self.checked = True

    def set_as_unchecked(self):
        self.checked = False

    def get_mark_character(self):
        return ' ' if not self.checked else 'X'

    def sort_children(self):
        pass


class GroupingElement(Element):
    def __init__(self, name, x_pos, parent=None):
        super(GroupingElement, self).__init__(name, x_pos, parent)
        self.type = 'GROUP'
        self.children = []

    def check_filter(self, filter_text):
        if filter_text in self.get_name():
            return True
        for child in self.children:
            if child.check_filter(filter_text):
                return True
        return False

    def add_child(self, child):
        self.children.append(child)
        child.parent = self

    def update_xpos(self, new_xpos):
        self.x_pos = new_xpos
        for child in self.children:
            child.update_xpos(new_xpos + INDENT)

    def get_as_flat_list(self):
        return [self] + [child.get_as_flat_list() for child in self.children]

    def mark(self):
        self.marked = True
        for child in self.children:
            child.mark()

    def sort_children(self):
        self.children.sort(key=lambda c: c.get_name().lower())
        for child in self.children:
            if child.type == 'GROUP':
                child.sort_children()


class SingleElement(Element):
    def __init__(self, name, x_pos, parent=None):
        super(SingleElement, self).__init__(name, x_pos, parent)
        self.type = 'SINGLE'
        self.value_type = False

    def check_filter(self, filter_text):
        return filter_text in self.get_name()

    def get_reference_name(self):
        return self.name.split('/')[-1] if type(self.name) is str \
            else str(self.name)


class ValueElement(SingleElement):
    def __init__(self, name, x_pos, value, parent=None):
        """ Value must be initialized first as it is used in hash generating
        """
        self.value = value
        super(ValueElement, self).__init__(name, x_pos, parent)
        self.value_type = True

    def get_name(self):
        return '{} = {}'.format(self.name, self.value)

    def get_reference_name(self):
        return self.value.split('/')[-1] if type(self.value) == str \
            else str(self.value)
