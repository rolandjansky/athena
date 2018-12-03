# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import unittest
from PyUtils.xAODBrowserRoot import OutputElement, OutputGenerator
from PyUtils.xAODBrowserElement import StructureGenerator, get_selected


class OutputElementTests(unittest.TestCase):

    def generate_aux_element(self):
        name = 'Container#TestElementAux'
        return OutputElement(name)

    def test_init(self):
        ''' Initializes ordinary output element'''
        name = 'Container#TestElement'
        element = OutputElement(name)
        self.assertEqual(element.container_name, 'Container')
        self.assertEqual(element.name, 'TestElement')
        self.assertFalse(element.is_aux)

    def test_aux_init(self):
        ''' Initializes aux output element ''' 
        element = self.generate_aux_element()
        self.assertEqual(element.container_name, 'Container')
        self.assertEqual(element.name, 'TestElementAux')
        self.assertTrue(element.is_aux)

    def test_add_child(self):
        ''' Adds child to output element '''
        element = self.generate_aux_element()
        child_name = 'child'
        element.add_child(child_name)
        self.assertEqual(len(element.children), 1)
        self.assertEqual(element.children[0], child_name)

    def test_prepare_name(self):
        ''' Adds children names to object name '''
        element = self.generate_aux_element()
        child_name = 'child'
        element.add_child(child_name)
        element.prepare_name()
        self.assertEqual(element.name, 'TestElementAux.child')
        
    def test_get_names(self):
        ''' Generates two object names: with original container and with Aux base container '''
        element = self.generate_aux_element()
        names = element.get_names()
        expected_names = ['Container#TestElementAux.', 'xAOD::AuxContainerBase#TestElementAux.']
        self.assertListEqual(names, expected_names)


class OutputGeneratorTests(unittest.TestCase):
    
    def get_test_dict(self):
        return {
            'container#a_elementAux': {
                'aa': None, 
                'bb': None
            },
            'container#b_element': None
        }

    def test_save_to_output_file(self):
        ''' Generates output python script from set of chosen items '''
        structure_generator = StructureGenerator({})
        structures = structure_generator.generate(self.get_test_dict(), 0)
        structures[0].children[0].select()
        structures[0].children[1].select()
        structures[1].select()
        selected_items_structure = get_selected(structures)
        output_generator = OutputGenerator(selected_items_structure)
        path = '/tmp/output.py'
        output_generator.save_to_output_file(path)
        with open(path) as f:
            lines = f.readlines()
            self.assertEqual(lines[0], 'obj = MSMgr.GetStream(0)\n')
            self.assertEqual(lines[1], 'del obj.GetItems()[:]\n')
            self.assertTrue(('obj.AddItem("container#a_elementAux.aa.bb")\n' in lines))
            self.assertTrue(('obj.AddItem("xAOD::AuxContainerBase#a_elementAux.aa.bb")\n' in lines))
            self.assertTrue(('obj.AddItem("container#b_element")\n' in lines))