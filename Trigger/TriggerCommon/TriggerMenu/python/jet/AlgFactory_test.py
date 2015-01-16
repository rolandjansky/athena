# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import unittest
from mock import MagicMock
from AlgFactory import AlgFactory
import inspect

class TestAlgFactory(unittest.TestCase):
    
    def setUp(self):

        class AntiKt:
            fex_label = 'amtikt'
            def __init__(self):
                self.merge_param = 0.4
                self.jet_calib = 'subjes'
                pass

        menu_data = MagicMock()
        menu_data.fex_params = AntiKt()
        menu_data.fex.fex_key = ''

        jet_attribute = MagicMock()
        jet_attribute.eta_min = 0.
        menu_data.hypo_params = MagicMock()
        menu_data.hypo_params.jet_attributes = [jet_attribute]
        
        menu_data.scantype = 'FS'
        chain_config = MagicMock()
        chain_config.menu_data = menu_data
        chain_config.menu_data.scan_type = ''
        
        self.alg_factory = AlgFactory(chain_config)

    def test_0(self):
        "call all the factory methods"
        for name, m in inspect.getmembers(self.alg_factory,
                                          predicate=inspect.ismethod):

            if name.startswith ('__'):
                continue
            print 'running Algfactory methods', name
            a = m()
            self.assertTrue(a.__class__.__name__ == 'Alg')

    def test_1(self):
        "call methods of "
        for name, m in inspect.getmembers(self.alg_factory,
                                          predicate=inspect.ismethod):

            if name.startswith ('__'):
                continue
            print 'running Algfactory methods', name
            a = m()
            str(a)
            a.asString()
            return

if __name__ == '__main__':
    unittest.main()
