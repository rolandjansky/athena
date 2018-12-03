#!/usr/bin/env python2.7
# -*- coding: utf-8 -*-

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import re
import os
import ROOT
import xAODBrowserLogger
import xAODBrowserUtils

logger = xAODBrowserLogger.get_logger()

class RootFileReader(object):
    def __init__(self, file_path, names_dict_path):
        self.input_file_path = file_path
        self.input_file = ROOT.TFile.Open(file_path)
        self.data_dict = {}
        self.splitting_regexp = '[.]'
        self.load_global_classes_dict(names_dict_path)
        self.load_input_classes()
        self.load_branches_with_size()
        self.load_data_dict()

    def get_class_name(self, obj_name):
        class_name = self.branches_classes[obj_name]
        if obj_name in self.input_classes:
            class_name = self.input_classes[obj_name]
        elif 'aux' in obj_name or 'Aux' in obj_name or class_name.startswith('vector'):
            class_name = 'xAOD::AuxContainerBase'
        elif class_name in self.global_classes_dict:
            class_name = self.global_classes_dict[class_name]
        return re.sub('_v[1-9]', '', class_name)

    def get_class_name_for_branch(self, branch):
        name = re.split(self.splitting_regexp, branch.GetName().replace('Dyn', ''))[0]
        return self.get_class_name(name)

    def load_branches_with_size(self):
        self.branches_list = self.input_file.CollectionTree.GetListOfBranches()
        self.branches_classes = {}
        self.size_dict = {}
        for branch in self.branches_list:
            name = branch.GetName().replace('Dyn', '')
            if name.endswith('.'):
                name = name[:-1]
            self.size_dict[name] = branch.GetZipBytes()
            branch_name = re.split(self.splitting_regexp, name)[0]
            self.branches_classes[branch_name] = branch.GetClassName()

    def load_names_arrays(self):
        return map(lambda x: re.split(self.splitting_regexp, self.get_class_name_for_branch(x) + '#' + x.GetName().replace('Dyn', '')), self.branches_list)
    
    def get_full_name(self, obj_name):
        if obj_name.endswith('.'):
            obj_name = obj_name[:-1]
        return self.get_class_name(obj_name) + '#' + obj_name

    def load_data_dict(self):
        try:
            read_stats = self.get_stats_from_file(self.input_file_path)
            for container in read_stats.containers():
                obj_name = self.get_full_name(container.first)
                self.data_dict[obj_name] = None
            for branch in read_stats.branches():
                obj_name = self.get_full_name(branch.first)
                self.data_dict[obj_name] = {}
                for var in branch.second:
                    if var:
                        var_name = re.split(self.splitting_regexp, var.GetName())[1]
                        self.data_dict[obj_name][var_name] = None
        except AttributeError as ae:
            logger.warning('Cannot load readStats. Objects will be loaded by getListOfBranches() function. Reason: {}'.format(ae.message))
            self.data_dict = xAODBrowserUtils.generate_data_dict(self.load_names_arrays())

    def load_global_classes_dict(self, file_path):
        self.global_classes_dict = {}
        if file_path:
            try:
                self.global_classes_dict = xAODBrowserUtils.load_object(file_path)
            except Exception as e:
                logger.warning('Data from file {} cannot be retrieved. Reason: {}'.format(file_path, e))     

    def get_stats_from_file(self, path):
        import_util = ReadStatsImportUtil()
        import_util.load_read_stats()
        import_util.cleanup()
        f = ROOT.TFile.Open(path)
        event = ROOT.xAOD.TEvent()
        return_code = event.readFrom(f)
        return_code.isSuccess()
        read_stats = ROOT.getStats()
        return read_stats

    def load_input_classes(self):
        try:
            from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
            athenaCommonFlags.FilesInput = [self.input_file_path]
            from RecExConfig.InputFilePeeker import inputFileSummary
            logger.info('Reading containers names from input file')
            if 'eventdata_itemsDic' in inputFileSummary:
                itemsdict = inputFileSummary['eventdata_itemsDic']
            else:
                self.input_classes = {}
                logger.warning('Failed to read {} by inputFileSummary. Classes will not be read'.format(self.input_file_path))
                return
            classes = {}
            for key in itemsdict:
                for n in itemsdict[key]:
                    name = n if not n.endswith('.') else n[:-1]
                    classes[name] = key 
            self.input_classes = classes
        except Exception as ie:
            self.input_classes = {}
            logger.warning('Cannot load class names from input file. Reason: ' + ie.message)


class OutputElement(object):
    def __init__(self, name, splitting_character='#'):
        self.container_name, self.name = name.split(splitting_character)
        self.children = []
        self.is_aux = ('aux' in self.name or 'Aux' in self.name)
        self.aux_base_container = 'xAOD::AuxContainerBase'
        self.split_char = splitting_character

    def add_child(self, child_name):
        self.children.append(child_name)

    def remove_dyn_substring(self):
        if 'dyn' in self.name:
            self.name = self.name.replace('dyn', '')
        if 'Dyn' in self.name:
            self.name = self.name.replace('Dyn', '')

    def prepare_name(self):
        if self.is_aux or len(self.children) > 0:
            children_string = '.'.join(self.children)
            self.name += '.{}'.format(children_string)

    def get_names(self):
        self.prepare_name()
        names = [self.split_char.join((self.container_name, self.name))]
        if self.container_name != self.aux_base_container and self.is_aux:
            names.append(self.split_char.join((self.aux_base_container, self.name)))
        return names


class OutputElementsDict(object):
    def __init__(self, chosen_elements_structure):
        self.elements_dict = {}
        for element in chosen_elements_structure:
            output_element = OutputElement(element.name)
            if not element.selected:
                for child in element.children:
                    output_element.add_child(child.name)
            self.elements_dict[element.name] = output_element 
                
    def get_elements(self):
        elems =  [value.get_names() for (key, value) in self.elements_dict.iteritems()]
        return elems


class OutputGenerator(object):
    def __init__(self, chosen_elements_structure):
        self.elements_dict = OutputElementsDict(chosen_elements_structure)

    def generate_beginning_lines(self):
        return ['obj = MSMgr.GetStream(0)', 'del obj.GetItems()[:]']

    def generate_adding_statement(self, name):
        return 'obj.AddItem("{}")'.format(name)

    def generate_elements_adding_statements(self):
        elements_adding_statements = []
        elements = self.elements_dict.get_elements()
        for names in elements:
            for name in names:
                elements_adding_statements.append(self.generate_adding_statement(name))
        return elements_adding_statements

    def save_to_output_file(self, file_path):
        beginning = self.generate_beginning_lines()
        adding = self.generate_elements_adding_statements()
        lines = beginning + adding
        file = open(file_path, 'w')
        file.write('\n'.join(lines) + '\n')
        file.close()


class ReadStatsImportUtil(object):
    
    def __init__(self):
        self.dir_path = os.path.abspath(os.path.dirname(__file__))

    def __get_path(self, filename):
        return os.path.join(self.dir_path, filename)

    def __generate_unique_path(self):
        ext = '.C'
        start_name = 'readStats';
        number = 1
        self.path = self.__get_path(start_name + str(number) + ext)
        while os.path.exists(self.path):
            number += 1
            logger.info('increasing number')
            self.path = self.__get_path(start_name + str(number) + ext)
        self.filename = self.path[:-2]

    def __generate_cpp_file(self):
        lines = [
            '#include <iostream>',
            '#include <xAODCore/tools/IOStats.h>',
            '#include <xAODCore/tools/ReadStats.h>',
            'xAOD::ReadStats& getStats() {',
            'return xAOD::IOStats::instance().stats();',
            '}'
        ]
        self.__generate_unique_path()
        file = open(self.path, 'w')
        file.write('\n'.join(lines) + '\n')
        file.close()

    def load_read_stats(self):
        self.__generate_cpp_file()
        ROOT.gROOT.ProcessLine('.L {}+'.format(self.path))

    def cleanup(self):
        extensions = [
            '_C.so',
            '_C_ACLiC_dict_rdict.pcm',
            '_C.d',
            '.C'
        ]
        for ext in extensions:
            try:
                os.unlink(self.filename + ext)        
            except Exception as e:
                logger.error(e)
