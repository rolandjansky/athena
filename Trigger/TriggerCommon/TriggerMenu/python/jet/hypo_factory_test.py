# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import copy
import unittest
from mock import MagicMock
from hypo_factory import (hypo_factory,
                          HypoAlg,
                          JetRecHypoAlg,
                          HTHypoAlg)

class Test_hypo_factory(unittest.TestCase):

    def setUp(self):

        jet_attribute = MagicMock()
        jet_attribute.threshold = 50
        jet_attribute.eta_range = '0eta320'
        
        self.hypo_args = {
            'chain_name': 'chain_name',
            'eta_str': '',   # '' for now
            'jet_attributes': [jet_attribute],
            'isCaloFullScan': True,
            'triggertower': False}

                
        self.ht_hypo_args = {
            'chain_name': 'chain_name',
            'eta_range': '',   # '' for now
            'ht_threshold': 250,
            'jet_et_threshold': 30.}

    def test_0(self):
        """test factory function"""

        hypo = hypo_factory('standard', self.hypo_args)
        self.assertTrue(hypo.__class__.__name__ == 'JetRecHypoAlg')

    def test_1(self):
        """test HypoAlg construction"""

        hypo = hypo_factory('standard', self.hypo_args)

        self.assertTrue(hypo.__class__.__name__ == 'JetRecHypoAlg')
        str(hypo)  # exercise string method
        self.assertTrue(hypo.eta_range() == '0eta320')
        
        self.assertTrue(hypo.jet_attributes_tostring() == '1_0eta320_50')

    def test_2(self):
        """test HypoAlg construction, bad arguments"""

        del self.hypo_args['chain_name']
        self.assertRaises(RuntimeError, JetRecHypoAlg, self.hypo_args)

    def test_3(self):
        """test HypoAlg construction, bad arguments"""

        self.hypo_args['jet_attributes'] = []
        self.assertRaises(RuntimeError, JetRecHypoAlg, self.hypo_args)

    def test_4(self):
        """test HypoAlg construction, bad arguments"""

        self.hypo_args['jet_attributes'][0].threshold = 50.1
        self.assertRaises(RuntimeError, JetRecHypoAlg, self.hypo_args)

    def test_6(self):
        """test HypoAlg construction, > 1 region"""

        jet_attribute = MagicMock()
        jet_attribute.threshold = 50
        jet_attribute.region = '320eta500'

        self.hypo_args['jet_attributes'].append(jet_attribute)
        self.assertRaises(RuntimeError, JetRecHypoAlg, self.hypo_args)

    def test_8(self):
        """test  construction HTHypoAlg"""

        hypo = HTHypoAlg(self.ht_hypo_args)
        self.assertTrue(hypo.__class__.__name__ == 'HTHypoAlg')
        
    def test_9(self):
        """test JetRecHypoAlg construction"""

        hypo = JetRecHypoAlg(self.hypo_args)
        self.assertTrue(hypo.__class__.__name__ == 'JetRecHypoAlg')
        
    def test_10(self):
        """test JetRecHypoAlg construction, bad args"""

        del self.hypo_args['triggertower']
        self.assertRaises(RuntimeError, JetRecHypoAlg, self.hypo_args)


if __name__ == '__main__':
    unittest.main()
