# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import unittest
import copy
from mock import MagicMock
from ChainConfigMaker import ChainConfigMaker
from ChainConfigMaker import JetAttributes


class Test_ChainConfigMaker(unittest.TestCase):
    
    def setUp(self):
        self.from_central0 = {'EBstep': -1,
                              'signatures': '',
                              'stream': ['Main'],
                              'run_rtt_diags': False,
                              'chainParts': [{'bTracking': '',
                                              'bTag': '',
                                              'scan': 'FS',
                                              'dataType': 'tc',
                                              'jetCalib': 'subjes',
                                              'etaRange': '0eta320',
                                              'topo': [],
                                              'threshold': '110',
                                              'chainPartName': 'j110',
                                              'recoAlg': 'a4',
                                              'trigType': 'j',
                                              'bConfig': [],
                                              'multiplicity': '1',
                                              'extra': '',
                                              'dataScouting': '',
                                              'signature': 'Jet',
                                              'calib': 'em',
                                              'addInfo': [],
                                              'cleaning': 'noCleaning',
                                              'TLA': '',
                                              'L1item': ''}],
                              'topo': [],
                              'chainCounter': 412,
                              'groups': ['RATE:SingleJet',
                                         'BW:Jets'],
                              'signature': 'Jet',
                              'topoThreshold': None,
                              'topoStartFrom': False,
                              'L1item': 'L1_J30',
                              'chainName': 'j110'}

        self.from_central1 = {'EBstep': -1,
                              'signatures': '',
                              'run_rtt_diags': False,
                              'stream': ['Main'],
                              'chainParts': [
                                  {'bTracking': '',
                                   'bTag': '',
                                   'scan': 'FS',
                                   'dataType': 'tc',
                                   'jetCalib': 'jes',
                                   'etaRange': '0eta320',
                                   'topo': [],
                                   'threshold': '135',
                                   'chainPartName': 'j135_test3_lcw_jes_L1J12',
                                   'recoAlg': 'a4',
                                   'trigType': 'j',
                                   'bConfig': [],
                                   'multiplicity': '1',
                                   'extra': 'test1',
                                   'dataScouting': '',
                                   'signature': 'Jet',
                                   'calib': 'lcw',
                                   'TLA': '',
                                   'cleaning': 'noCleaning',
                                   'addInfo': [],
                                   'L1item': ''}],
                              'topo': [],
                              'chainCounter': 18,
                              'groups': ['RATE:SingleJet',
                                         'BW:Jets'],
                              'signature': 'Jet',
                              'topoThreshold': None,
                            'topoStartFrom': False,
                              'L1item': 'L1_J12',
                              'chainName': 'j135_test3_lcw_jes_L1J12'}

    def test_0(self):
        """Create ChainConfig instance"""

        ccm = ChainConfigMaker(self.from_central0)
        chainConfig = ccm()
        print chainConfig
        self.assertTrue(chainConfig.__class__.__name__ == 'ChainConfig')

    def test_1(self):
        """Create ChainConfig instance"""

        ccm = ChainConfigMaker(self.from_central1)
        chainConfig = ccm()
        print chainConfig
        self.assertTrue(chainConfig.__class__.__name__ == 'ChainConfig')

    def test_2(self):
        """Create ChainConfig instance"""

        for c in self.from_central1['chainParts']:
            c['extra'] = 'test2'

        ccm = ChainConfigMaker(self.from_central1)
        chainConfig = ccm()
        print chainConfig
        self.assertTrue(chainConfig.__class__.__name__ == 'ChainConfig')




if __name__ == '__main__':
    unittest.main()
