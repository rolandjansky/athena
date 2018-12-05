#!/usr/bin/env python2.7
# -*- coding: utf-8 -*-

# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

import pickle
import logging

from AthenaConfiguration.iconfTool.models.structure import ComponentsStructure

logger = logging.getLogger('iconfTool.' + __name__)


class ComponentsFileLoader(object):
    def __init__(self, file_path, no_replace, checked_elements=set()):
        self.file_path = file_path
        self.no_replace = no_replace
        self.checked_elements = checked_elements

    def _load_file_data(self):
        f = open(self.file_path, 'rb')
        st = pickle.load(f)
        f.close()
        return dict(st)

    def load_structure(self):
        data = self._load_file_data()
        structure = ComponentsStructure(data, self.no_replace,
                                        self.checked_elements)
        structure.generate()
        return structure

    def get_data(self):
        return self.load_structure()


class ComponentsDiffFileLoader(object):
    def __init__(self, file_path, diff_file_path, no_replace,
                 checked_elements):
        self.main_loader = ComponentsFileLoader(file_path, no_replace,
                                                checked_elements)
        self.diff_loader = ComponentsFileLoader(diff_file_path, no_replace,
                                                checked_elements)

    def get_data(self):
        structure = self.main_loader.load_structure()
        diff_structure = self.diff_loader.load_structure()
        self.mark_differences(structure.get_list(), diff_structure.get_list())
        return structure, diff_structure

    def equal(self, first, second):
        return first.get_name() == second.get_name() \
            and first.x_pos == second.x_pos and first.type == second.type

    def mark_differences(self, structure, diff_structure):
        i, j = 0, 0
        while i < len(structure) and j < len(diff_structure):
            if self.equal(structure[i], diff_structure[j]):
                if structure[i].type == 'GROUP':
                    self.mark_differences(structure[i].children,
                                          diff_structure[j].children)
                i += 1
                j += 1
                continue

            # Find equal element in diff structure
            for tmp_j in range(j, len(diff_structure)):
                if self.equal(structure[i], diff_structure[tmp_j]):
                    for marking_j in range(j, tmp_j):
                        diff_structure[marking_j].mark()
                    j = tmp_j
                    break
            else:
                # Not found equal element in diff structure
                # Find equal element in first structure
                for tmp_i in range(i, len(structure)):
                    if self.equal(structure[tmp_i], diff_structure[j]):
                        for marking_i in range(i, tmp_i):
                            structure[marking_i].mark()
                        i = tmp_i
                        break
                else:
                    structure[i].mark()
                    diff_structure[j].mark()
                    i += 1
                    j += 1

        # Mark remaining elements in both structures
        while i < len(structure):
            structure[i].mark()
            i += 1

        while j < len(diff_structure):
            diff_structure[j].mark()
            j += 1
