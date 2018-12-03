# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import unittest
import PyUtils.xAODBrowserUtils as utils
from PyUtils.xAODBrowserElement import StructureGenerator

class UtilsTests(unittest.TestCase):

    def get_test_dict(self):
        return {
            'a_element': {
                'aa': None, 
                'bb': None
            },
            'b_element': {
                'cc': None
            }
        }

    def test_generate_data_dict(self):
        ''' Generates dictionary from list of lists of strings '''
        nested_arr = [
            ['aa', 'bb', 'cc'],
            ['dd', 'ee']
        ]
        expected_dict = {
            'aa': {
                'bb': {
                    'cc': {}
                }
            },
            'dd': {
                'ee': {}
            }
        }
        d = utils.generate_data_dict(nested_arr)
        self.assertDictEqual(d, expected_dict)

    def test_saving_and_loading_object(self):
        ''' Serializes and saves object to file and loads it back '''
        test_dict = {
            'aa': 'bb',
            'cc': None
        }
        path = '/tmp/saving_test.pkl'
        utils.save_object(path, test_dict)
        read_object = utils.load_object(path)
        self.assertDictEqual(read_object, test_dict)

    def test_saving_and_loading_configuration(self):
        ''' Saves and loads elements array '''
        path = '/tmp/saving_test.pkl'
        data_dict =  self.get_test_dict()
        structures = StructureGenerator({}).generate(data_dict, 0)
        structures[0].select()
        utils.save_configuration(path, structures)
        read_structures = utils.load_initial_data(path)
        for i in range(len(read_structures)):
            self.assertEqual(read_structures[i].name, structures[i].name)
            self.assertEqual(read_structures[i].selected, structures[i].selected)
            self.assertEqual(len(read_structures[i].children), len(structures[i].children))
            for j in range(len(read_structures[i].children)):
                self.assertEqual(read_structures[i].children[j].name, structures[i].children[j].name)
                self.assertEqual(read_structures[i].children[j].selected, structures[i].children[j].selected)

    @unittest.expectedFailure        
    def test_invalid_configuration_save(self):
        ''' Fails for invalid elements list '''
        path = '/tmp/saving_test.pkl'
        data = ['aa', 'bb', 'cc']
        utils.save_configuration(path, data)

    @unittest.expectedFailure
    def test_invalid_configuration_load(self):
        ''' Fails for invalid data structure in file '''
        path = '/tmp/saving_test.pkl'
        data = self.get_test_dict()
        utils.save_object(path, data)
        read_data = utils.load_initial_data(path)