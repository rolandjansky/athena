# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import copy
import xAODBrowserLogger as logging

logger = logging.get_logger()

class Element(object):
    def __init__(self, name, x_pos, parent=None, size = 0.0):
        self.name = name
        self.x_pos = x_pos
        self.size = size
        self.parent = parent
        self.children = []
        self.show_children = False
        self.selected = False
        self.selected_children_number = 0

    def set_size(self, size):
        self.size = size

    def check_filter(self, filter_text):
        if filter_text in self.name:
            return True
        for child in self.children:
            if child.check_filter(filter_text):
                return True
        return False

    def get_mark_character(self):
        if self.selected:
            return 'X'
        elif self.selected_children_number > 0:
            return 'O'
        else:
            return ' '

    def select(self):
        if not self.selected:     
            self.selected = True
            element = self
            while element.parent and (element.selected or element.selected_children_number > 0):
                parent = element.parent
                parent.selected_children_number += 1
                element = parent

    def diselect(self):
        if self.selected:
            self.selected = False
            element = self
            while element.parent and (not element.selected or element.selected_children_number == 0):
                parent = element.parent
                parent.selected_children_number -= 1
                element = parent

    def add_child(self, child, size = 0.0):
        if type(child) is str:
            child = Element(child, self.x_pos + 3, self)
            child.set_size(size)
        self.children.append(child)

    def get_selected(self):
        self.show_children = False
        if self.selected:
            return self
        for i in range(len(self.children)):
            self.children[i] = self.children[i].get_selected()
        self.children = filter(lambda child: child is not None, self.children)
        return self if len(self.children) > 0 else None

    def get_name_without_class(self):
        return self.name if '#' not in self.name else self.name.split('#')[1]

    def get_root_parent(self):
        root = self.parent if self.parent else self
        while root.parent:
            root = root.parent
        return root

    def get_names_path(self):
        names = [self.get_name_without_class()]
        element = self
        while element.parent is not None:
            names.append(element.parent.get_name_without_class())
            element = element.parent
        names.reverse()
        names_path = '.'.join(names)
        return names_path
        
    def get_summary_size(self):
        return self.size + Element.get_structure_size(self.children)

    @staticmethod
    def get_structure_size(structure = []):
        return sum(el.get_summary_size() for el in structure)



class StructureGenerator(object):   
    def __init__(self, size_dict):
        self.size_dict = size_dict

    def create_elements_structure(self, d, x_pos, parent=None):
        elements = []
        for i in d:
            element = Element(i, x_pos, parent)
            names_path = element.get_names_path()
            if names_path in self.size_dict:
                element.set_size(self.size_dict[names_path])
            elements.append(element)
            if parent is not None:
                parent.add_child(element)
            if d[i] is not None:
                if type(d[i]) is dict:
                    self.create_elements_structure(d[i], x_pos + 3, element)
                elif type(d[i]) is str:
                    element.add_child(d[i])
        return elements

    def generate(self, d, x_pos):
        structure = self.create_elements_structure(d, x_pos)
        structure.sort(key=lambda x: x.name)
        return structure


def get_selected(structure):
    chosen_items_structure = []
    structure_copy = copy.deepcopy(structure)
    for element in structure_copy:
        chosen = element.get_selected()
        if chosen:
            chosen_items_structure.append(chosen)
    return chosen_items_structure