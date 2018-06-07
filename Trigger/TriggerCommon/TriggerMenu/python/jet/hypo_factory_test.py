# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import unittest
from mock import MagicMock
from hypo_factory import (hypo_factory,
                          JetStandardHypo,
                          JetSingleEtaRegionHypo,
                          JetMaximumBipartiteHypo,
                          HTHypo)

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


                        
        self.tla_hypo_args = {
            'chain_name': 'chain_name',
            'tla_string': '',   # '' for now
            'indexlo': 0,
            'indexhi': 2,
            'mass_min': 0.,
            'mass_max': 1000.,
        }

    def test_0(self):
        """test factory function"""

        hypo = hypo_factory('run1hypo', self.hypo_args)
        self.assertTrue(hypo.__class__.__name__ == 'JetStandardHypo')

        hypo = hypo_factory('HLTSRhypo', self.hypo_args)
        self.assertTrue(hypo.__class__.__name__ == 'JetSingleEtaRegionHypo')

        hypo = hypo_factory('HLThypo', self.hypo_args)
        self.assertTrue(hypo.__class__.__name__ == 'JetMaximumBipartiteHypo')

        hypo = hypo_factory('ht', self.ht_hypo_args)
        self.assertTrue(hypo.__class__.__name__ == 'HTHypo')

        hypo = hypo_factory('tla', self.tla_hypo_args)
        self.assertTrue(hypo.__class__.__name__ == 'TLAHypo')

    def test_1(self):
        """test HypoAlg construction"""

        hypo = hypo_factory('run1hypo', self.hypo_args)

        self.assertTrue(hypo.__class__.__name__ == 'JetStandardHypo')
        str(hypo)  # exercise string method
        self.assertTrue(hypo.eta_range() == '0eta320')
        
        self.assertTrue(hypo.jet_attributes_tostring() == '1_0eta320_50')

    def test_2(self):
        """test HypoAlg construction, bad arguments"""

        del self.hypo_args['chain_name']
        self.assertRaises(RuntimeError, JetStandardHypo, self.hypo_args)

    def test_3(self):
        """test HypoAlg construction, bad arguments"""

        self.hypo_args['jet_attributes'] = []
        self.assertRaises(RuntimeError, JetStandardHypo, self.hypo_args)

    def test_4(self):
        """test HypoAlg construction, bad arguments"""

        self.hypo_args['jet_attributes'][0].threshold = 50.1
        self.assertRaises(RuntimeError, JetStandardHypo, self.hypo_args)

    def test_6(self):
        """test HypoAlg construction, > 1 region"""

        jet_attribute = MagicMock()
        jet_attribute.threshold = 50
        jet_attribute.region = '320eta500'

        self.hypo_args['jet_attributes'].append(jet_attribute)
        self.assertRaises(RuntimeError, JetStandardHypo, self.hypo_args)

    def test_8(self):
        """test  construction HTHypoAlg"""

        hypo = HTHypo(self.ht_hypo_args)
        self.assertTrue(hypo.__class__.__name__ == 'HTHypo')
        
    def test_9(self):
        """test JetStandardHypo construction"""

        hypo = JetStandardHypo(self.hypo_args)
        self.assertTrue(hypo.__class__.__name__ == 'JetStandardHypo')
        
    def test_10(self):
        """test JetStandardHypo construction, bad args"""

        del self.hypo_args['triggertower']
        self.assertRaises(RuntimeError, JetStandardHypo, self.hypo_args)

    def test_11(self):
        """test JetSingleEtaRegionHypo construction"""

        hypo = JetSingleEtaRegionHypo(self.hypo_args)
        self.assertTrue(hypo.__class__.__name__ == 'JetSingleEtaRegionHypo')

    def test_12(self):
        """test JetMaximumBipartiteHypo construction"""

        hypo = JetMaximumBipartiteHypo(self.hypo_args)
        print hypo.__class__.__name__
        self.assertTrue(hypo.__class__.__name__ == 'JetMaximumBipartiteHypo')


if __name__ == '__main__':
    unittest.main()
