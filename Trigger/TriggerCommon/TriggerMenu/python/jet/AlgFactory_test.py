# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import unittest
from mock import MagicMock
from AlgFactory import AlgFactory
import inspect

class TestAlgFactory(unittest.TestCase):

    def setUp(self):
        self.special_cases = ('jr_hypo',
                              # follwoing 3 methods tested indirectly
                              'dataScoutingAlg1',  
                              'dataScoutingAlg2',
                              'dataScouting_TrigHLTJetTLASelector',
                          )

    def make_algfactory_singlejet(self):

        class AntiKt:
            fex_label = 'amtikt'
            def __init__(self):
                self.merge_param = 0.4
                self.jet_calib = 'subjes'
                self.fex_alg_name = 'a4'
                self.data_type = 'tc'
                pass

        menu_data = MagicMock()
        menu_data.fex_params = AntiKt()
        menu_data.fex.fex_key = ''

        jet_attribute = MagicMock()
        jet_attribute.eta_min = 0.
        menu_data.hypo_params = MagicMock()
        menu_data.hypo_params.jet_attributes = [jet_attribute]
        menu_data.hypo_params.eta_range = '0eta320'
        
        menu_data.scantype = 'FS'
        chain_config = MagicMock()
        chain_config.menu_data = menu_data
        chain_config.menu_data.scan_type = ''
        
        self.alg_factory = AlgFactory(chain_config)


    def make_algfactory_multijet(self):
        self.make_algfactory_singlejet()

        # make multiple jet attributes by copying
        self.alg_factory.hypo_params.jet_attributes.append(
            self.alg_factory.hypo_params.jet_attributes[0]
        )

    def test_0(self):
        "call all the factory methods"

        self.make_algfactory_singlejet()

        for name, m in inspect.getmembers(self.alg_factory,
                                          predicate=inspect.ismethod):

            if name.startswith ('__') or name == '_hlt_hypo':
                continue
            print 'running Algfactory methods', name

            if name in self.special_cases:
                continue

            algs = m()

            for alg in algs:
                self.assertTrue(alg.__class__.__name__ == 'Alg')
                str(alg)
                alg.asString()


    def test_1(self):
        "call special case factory methods"

        self.make_algfactory_multijet()

        algs = self.alg_factory.jr_hypo()
        for alg in algs:
            self.assertTrue(alg.__class__.__name__ == 'Alg')

            str(alg)
            alg.asString()


if __name__ == '__main__':
    unittest.main()
