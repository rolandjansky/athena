# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import unittest
import random
import copy
from PyUtils.xAODBrowserElement import Element, StructureGenerator

class ElementTests(unittest.TestCase):

    def setUp(self):
        self.structure_generator = StructureGenerator({})

    def get_example_dict(self):
        return {
            'b_element': {
                'cc': None
            },
            'a_element': {
                'aa': None, 
                'bb': None
            }
        }

    def generate_example_structure(self):
        x_pos = 0
        d = self.get_example_dict()
        return self.structure_generator.generate(d, x_pos)

    def test_init(self):
        ''' Element object initialization '''
        name = 'test name'
        x_position = random.randint(0, 10)
        element = Element(name, x_position, None)
        self.assertEqual(name, element.name)
        self.assertEqual(x_position, element.x_pos)

    def test_selection(self):
        ''' Selecting and diselecting elements '''
        name = 'test name'
        x_position = random.randint(0, 10)
        element = Element(name, x_position, None)
        self.assertFalse(element.selected)
        element.select()
        self.assertTrue(element.selected)
        element.diselect()
        self.assertFalse(element.selected)

    def test_structure_creation(self):
        ''' Creating structure from dict '''
        x_position = random.randint(0, 10)
        d = self.get_example_dict()
        structures = self.structure_generator.generate(d, x_position)
        element = structures[0]
        self.assertEqual(element.name, 'a_element')
        self.assertEqual(element.x_pos, x_position)
        self.assertIsNone(element.parent)
        child = element.children[0]
        for child in element.children:
            self.assertEqual(child.x_pos, x_position + 3)
            self.assertEqual(child.parent, element)
        element = structures[1]
        self.assertEqual(element.name, 'b_element')
        self.assertEqual(element.x_pos, x_position)
        self.assertIsNone(element.parent)
        child = element.children[0]
        for child in element.children:
            self.assertEqual(child.x_pos, x_position + 3)
            self.assertEqual(child.parent, element)

    def test_mark_character(self):
        ''' Retrieving mark character depending on selection '''
        structures = self.generate_example_structure()
        first, second = structures[0], structures[1]
        first.select()
        second_child = second.children[0]
        second_child.select()
        self.assertEqual(first.get_mark_character(), 'X')
        self.assertEqual(first.children[0].get_mark_character(), ' ')
        self.assertEqual(second.get_mark_character(), 'O')
        self.assertEqual(second_child.get_mark_character(), 'X')
        second_child.diselect()
        self.assertEqual(second_child.get_mark_character(), ' ')
        self.assertEqual(second.get_mark_character(), ' ')

    def test_get_selected(self):
        ''' Retrieving selected part of element '''
        structures = self.generate_example_structure()
        first = structures[0]
        first_copy = copy.deepcopy(first)
        self.assertIsNone(first_copy.get_selected())
        first_copy = copy.deepcopy(first)
        first_copy.select()
        self.assertEqual(len(first.children), len(first_copy.get_selected().children))
        first_copy = copy.deepcopy(first)
        first_copy.children[0].select()
        self.assertEqual(len(first_copy.get_selected().children), 1)

    def test_check_filter(self):
        ''' Filtering structure by input text '''
        structures = self.generate_example_structure()
        first = structures[0]
        second = structures[1]
        self.assertTrue(first.check_filter('element'))
        self.assertTrue(first.check_filter('a'))
        self.assertTrue(first.check_filter('aa'))
        self.assertTrue(first.check_filter('bb'))
        self.assertFalse(first.check_filter('cc'))
        self.assertTrue(second.check_filter('element'))
        self.assertTrue(second.check_filter('b'))
        self.assertTrue(second.check_filter('cc'))
        self.assertFalse(second.check_filter('dd'))
        
    def test_get_root_parent(self):
        ''' Retrieving root parent for element '''
        nested = {
            'aa': {
                'bb': {
                    'cc': {}
                }
            }
        }
        x_pos = 0
        structure = self.structure_generator.generate(nested, x_pos)
        first = structure[0]
        first_child = first.children[0]
        nested_child = first_child.children[0]
        self.assertTrue(first.get_root_parent(), first)
        self.assertTrue(first_child.get_root_parent(), first)
        self.assertTrue(nested_child.get_root_parent(), first)