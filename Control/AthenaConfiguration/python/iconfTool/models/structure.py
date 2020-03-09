#!/usr/bin/env python2.7
# -*- coding: utf-8 -*-

# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

import ast
import copy
import sys
import logging

from AthenaConfiguration.iconfTool.models.element import SingleElement, GroupingElement, ValueElement

logger = logging.getLogger('iconfTool.' + __name__)
INDENT = 3


class Structure(object):
    def __init__(self, flags_dict, checked_elements=set()):
        self.flags_dict = flags_dict
        self.structure = {}
        self.checked_elements = checked_elements  # Set of checked elements
        # hashed

    def generate_structure_dict(self):
        """ Split dictionary keys into nested structure
        e.g. Test1.Test2: Value -> Test1: {Test2: Value}
        """
        structure_dict = {}
        for flag, value in self.flags_dict.iteritems():
            groups = flag.split('.')
            structure_point = structure_dict
            for i in range(len(groups) - 1):
                if groups[i] not in structure_point:
                    structure_point[groups[i]] = {}
                elif type(structure_point[groups[i]]) is str:
                    structure_point[groups[i]] = {
                        structure_point[groups[i]]: {}}
                structure_point = structure_point[groups[i]]
            if groups:
                last = groups[-1]
                if last in structure_point and type(structure_point[last]) is dict and type(value) is dict:
                    structure_point[last].update(value)
                else:
                    structure_point[last] = value
        return structure_dict

    def generate_for_element(self, grouping_element, structure):
        child_x_pos = grouping_element.x_pos + INDENT
        for child in structure:
            try:
                # Handles situation when list or dict is provided as a string
                structure[child] = ast.literal_eval(structure[child])
            except (ValueError, SyntaxError):
                pass

            if type(structure[child]) is dict:
                group = GroupingElement(child, child_x_pos, grouping_element)
                if group.hash in self.checked_elements:
                    group.set_as_checked()
                self.generate_for_element(group, structure[child])
                grouping_element.add_child(group)
            elif type(structure[child]) is list:
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
                flag = ValueElement(child, child_x_pos, structure[child],
                                    grouping_element)
                if flag.hash in self.checked_elements:
                    flag.set_as_checked()
                grouping_element.add_child(flag)

    def generate(self, x_pos=0):
        structure_dict = self.generate_structure_dict()
        roots = []
        for root in structure_dict:
            if type(structure_dict[root]) == dict:
                root_group = GroupingElement(root, x_pos)
                self.generate_for_element(root_group, structure_dict[root])
                roots.append(root_group)
            else:
                flag = ValueElement(root, x_pos, structure_dict[root])
                roots.append(flag)
        return sorted(roots, key=lambda r: r.name.lower())

    def add_to_checked(self, element):
        hash_value = element.hash
        self.checked_elements.add(hash_value)

    def remove_from_checked(self, element):
        hash_value = element.hash
        if hash_value in self.checked_elements:
            self.checked_elements.remove(hash_value)


class ComponentsStructure(Structure):
    def __init__(self, flags_dict, no_replace, checked_elements):
        super(ComponentsStructure, self).__init__(flags_dict, checked_elements)
        self.roots_dict = {}
        self.structure_list = []
        self.no_replace = no_replace
        self.filter = None  # Filter object used in search

    def replace_references(self, element, nesting=0):
        element.replaced = True
        if element.type != 'GROUP':
            return
        for i, child in enumerate(element.children):
            if element.children[i].type == 'SINGLE':
                ref_name = element.children[i].get_reference_name()
                if ref_name in self.roots_dict and not element.has_parent(
                        self.roots_dict[ref_name].get_name()):
                    element.children[i] = copy.deepcopy(
                        self.roots_dict[ref_name])
                    element.children[i].parent = element
                    element.update_xpos(element.x_pos)
            if element.children[i].type == 'GROUP' \
                    and not element.children[i].replaced:
                # Single element can have been transformed into grouping
                # element in previous section
                self.replace_references(element.children[i], nesting + 1)

    def generate(self, x_pos=0):
        structure_dict = self.generate_structure_dict()
        self.structure_list = []
        for root_name in structure_dict:
            if type(structure_dict[root_name]) == dict:
                root_group = GroupingElement(root_name, x_pos)
                if root_group.hash in self.checked_elements:
                    root_group.set_as_checked()
                self.generate_for_element(root_group,
                                          structure_dict[root_name])
                self.structure_list.append(root_group)
                self.roots_dict[root_name] = root_group
            else:
                flag = ValueElement(root_name, x_pos,
                                    structure_dict[root_name])
                if flag.hash in self.checked_elements:
                    flag.set_as_checked()
                self.structure_list.append(flag)
        if not self.no_replace:
            counter = 1
            print('{} structures to reconstruct'.format(
                len(self.structure_list)))
            for root in self.structure_list:
                sys.stdout.write('\rReconstructing {}...'.format(counter))
                sys.stdout.flush()
                counter += 1
                self.replace_references(root)
            sys.stdout.write('\rStructures reconstructed\n')
            sys.stdout.flush()
        self.sort()
        self.filter = ComponentsStructureFilter(self.structure_list)

    def get_list(self):
        self.filter.reset()
        return self.structure_list

    def get_as_flat_list(self):
        self.filter.reset()
        return [root.get_as_flat_list() for root in self.structure_list]

    def get_filtered_by_text(self, filter_text):
        return self.filter.generate_by_text(filter_text)

    def get_filtered_by_mark(self):
        return self.filter.generate_by_mark()

    def sort(self):
        self.structure_list.sort(key=lambda el: el.get_name().lower())
        for element in self.structure_list:
            element.sort_children()


class ComponentsStructureFilter(object):
    def __init__(self, structure):
        self.structure = structure
        self.output = []  # Found elements with their parents
        self.found_elements = []  # Elements which are stored in the original
        self.filter_called = False  # Used to restore found elements to
        # previous state

    def reset(self):
        if not self.filter_called:
            return
        for element in self.found_elements:
            if element.parent:
                element.update_xpos(element.parent.x_pos + INDENT)
        self.output = []
        self.found_elements = []
        self.filter_called = False

    def generate_by_text(self, filter_text):
        self.reset()
        self.browse_and_save_by_text(self.structure, filter_text)
        for element in self.output:
            element.update_xpos(0)
        self.filter_called = True
        return self.output

    def browse_and_save_by_text(self, elements, filter_text):
        filtered = filter(lambda el: filter_text in el.get_name(),
                          elements)
        if len(filtered) > 0:
            # All elements are on the same level and therefore have
            # the same parents
            first = filtered[0]
            if first.parent and first.parent.parent:
                root_parent = copy.copy(first.parent.parent)
                direct_parent = copy.copy(first.parent)
                direct_parent.children = filtered
                root_parent.children = [direct_parent]
                self.output.append(root_parent)
                self.found_elements += filtered
            elif first.parent:
                direct_parent = copy.copy(first.parent)
                direct_parent.children = filtered
                self.output.append(direct_parent)
                self.found_elements += filtered
            else:
                self.output += filtered
                self.found_elements += filtered
        for element in elements:
            if element.type == 'GROUP':
                self.browse_and_save_by_text(element.children, filter_text)

    def generate_by_mark(self):
        self.reset()
        self.browse_and_save_by_mark(self.structure)
        for element in self.output:
            element.update_xpos(0)
        self.filter_called = True
        return self.output

    def browse_and_save_by_mark(self, elements):
        filtered = [el for el in elements if el.is_marked()]
        to_check = [el for el in elements if not el.is_marked()]
        if len(filtered) > 0:
            # All elements are on the same level and therefore have
            # the same parents
            first = filtered[0]
            if first.parent and first.parent.parent:
                root_parent = copy.copy(first.parent.parent)
                direct_parent = copy.copy(first.parent)
                direct_parent.children = filtered
                root_parent.children = [direct_parent]
                self.output.append(root_parent)
                self.found_elements += filtered
            elif first.parent:
                direct_parent = copy.copy(first.parent)
                direct_parent.children = filtered
                self.output.append(direct_parent)
                self.found_elements += filtered
            else:
                self.output += filtered
                self.found_elements += filtered
        for element in to_check:
            if element.type == 'GROUP':
                self.browse_and_save_by_mark(element.children)
