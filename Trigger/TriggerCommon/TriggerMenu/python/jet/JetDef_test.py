# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import copy
import unittest
from JetDef import generateHLTChainDef, usage

from ChainConfigMaker import JetAttributes

from MC_pp_V5_dicts import MC_pp_V5_dicts

class TestJetDef(unittest.TestCase):

    def setUp(self):
        self.from_central =  copy.deepcopy(MC_pp_V5_dicts[0])
        # self.old_stdout = sys.stdout
        # sys.stdout = StringIO()

    def tearDown(self):
        #sys.stdout = self.old_stdout
        pass
        
    def test_0(self):
        """Create ChainDef instances with full instantiation"""

        for dd in MC_pp_V5_dicts:
            chain_def = generateHLTChainDef(dd)
            if chain_def.__class__.__name__ != 'ChainDef':
                print 'real error ---> ', chain_def
                print chain_def
            self.assertTrue(chain_def.__class__.__name__ == 'ChainDef')

    def _test_3(self):
        'run exerciser function'
        #run_test()

    def test_4(self):
        'test error handling: missing entry in input dict'

        del self.from_central['chainParts']
        chain_def = generateHLTChainDef(self.from_central)
        # if chain_def.__class__.__name__ != 'ChainDef':
        #    print chain_def
        self.assertTrue(chain_def.__class__.__name__ == 'ErrorChainDef')
        
    def test_5(self):
        'test error handling: too many chain parts'

        self.from_central['chainParts'].extend([{}, {}])
        chain_def = generateHLTChainDef(self.from_central)
        # if chain_def.__class__.__name__ != 'ChainDef':
        #    print chain_def
        self.assertTrue(chain_def.__class__.__name__ == 'ErrorChainDef')
        
    def test_6(self):
        'test error handling: unexpected chain part signature'

        self.from_central['chainParts'][0]['signature'] = 'junk'
        chain_def = generateHLTChainDef(self.from_central)
        # if chain_def.__class__.__name__ != 'ChainDef':
        #    print chain_def
        self.assertTrue(chain_def.__class__.__name__ == 'ErrorChainDef')
        
    def test_7(self):
        'test error handling: missing entry in chain part dict'

        del self.from_central['chainParts'][0]['etaRange']
        chain_def = generateHLTChainDef(self.from_central)
        # if chain_def.__class__.__name__ != 'ChainDef':
        #    print chain_def
        self.assertTrue(chain_def.__class__.__name__ == 'ErrorChainDef')
        
    def test_8(self):
        'test error handling: unexpected dataType in chain part dict'

        self.from_central['chainParts'][0]['dataType'] = 'junk'
        chain_def = generateHLTChainDef(self.from_central)
        # if chain_def.__class__.__name__ != 'ChainDef':
        #    print chain_def
        self.assertTrue(chain_def.__class__.__name__ == 'ErrorChainDef')
        
    def test_9(self):
        'test error handling: topo TT is one of the dataTypes'

        d = {'EBstep': -1,
             'L1item': 'L1_J20',
             'chainCounter': 763,
             'chainName': 'j60_TT',
             'chainParts': [{'L1item': '',
                             'TLA': '',
                            'addInfo': [],
                             'bConfig': [],
                             'bMatching': [],
                             'bTag': '',
                             'bTracking': '',
                             'calib': 'em',
                             'chainPartName': 'j60_TT',
                             'cleaning': 'noCleaning',
                             'dataScouting': '',
                             'dataType': 'TT',
                             'etaRange': '0eta320',
                             'extra': '',
                             'jetCalib': 'subjes',
                             'multiplicity': '1',
                             'recoAlg': 'a4',
                             'scan': 'FS',
                             'signature': 'Jet',
                             'threshold': '60',
                             'topo': [],
                             'trigType': 'j'}],
             'groups': ['RATE:SingleJet', 'BW:Jets'],
             'signature': 'Jet',
             'signatures': '',
             'stream': ['Main'],
             'topo': [],
            'topoStartFrom': False,
             'topoThreshold': None}

        chain_def = generateHLTChainDef(d)
        if chain_def.__class__.__name__ != 'ChainDef':
            print 'real error ---> ', chain_def
            print chain_def
 
        self.assertTrue(chain_def.__class__.__name__ == 'ChainDef')
            

                
    def test_11(self):
        'excercise JetAttribute __str__ method'

        ja = JetAttributes('t', '0eta320')
        str(ja)

    def _test_12(self):
        """excercise with input data having both trigger tower and
        topo cluster input data.

        HANDLING of TT not decided 16/6/14
        """

        self.from_central['chainName'] = 'TT_and_tc_chainParts'
        d = copy.deepcopy(self.from_central['chainParts'][0])
        d['dataType'] = 'TT'
        d['scan'] = ''
        self.from_central['chainParts'].append(d)
        chain_def = generateHLTChainDef(self.from_central)
        print chain_def
        # TT not implemented 11/5/2014
        self.assertTrue(chain_def.__class__.__name__ == 'ChainDef')


    def test_13(self):
        """error handling: unknown reco alg"""

        self.from_central['chainName'] = 'bad reco alg'
        self.from_central['chainParts'][0]['recoAlg'] = 'junk'
        chain_def = generateHLTChainDef(self.from_central)
        # TT not implemented 11/5/2014
        self.assertTrue(chain_def.__class__.__name__ == 'ErrorChainDef')


    def test_14(self):
        """error handling: bad scan type"""

        self.from_central['chainName'] = 'bad scan type'
        self.from_central['chainParts'][0]['scan'] = 'junk'
        chain_def = generateHLTChainDef(self.from_central)
        # TT not implemented 11/5/2014
        self.assertTrue(chain_def.__class__.__name__ == 'ErrorChainDef')

    def test_15(self):
        """error handling: scan type set for TT"""

        self.from_central['chainName'] = 'scan type set for TT'
        d = copy.deepcopy(self.from_central['chainParts'][0])
        self.from_central['chainParts'].append(d)
        d['scan'] = 'PS'
        d['dataType'] = 'TT'
        chain_def = generateHLTChainDef(self.from_central)
        self.assertTrue(chain_def.__class__.__name__ == 'ErrorChainDef')

    def test_16(self):
        """error handling: bad region"""

        self.from_central['chainName'] = 'bad region'
        self.from_central['chainParts'][0]['etaRange'] = 'junk'
        chain_def = generateHLTChainDef(self.from_central)
        self.assertTrue(chain_def.__class__.__name__ == 'ErrorChainDef')

    def test_17(self):
        """excercise usage message"""
        usage()

    def test_18(self):
        """test exception thrown of two hypos specified"""
        d = {'EBstep': -1,
             'L1item': 'L1_J100',
             'chainCounter': 727,
             'chainName': '2hypochain',
            'chainParts': [{'L1item': '',
                            'TLA': '1i2c500m700TLA',
                            'addInfo': [],
                            'bConfig': ['split'],
                            'bMatching': [],
                            'bTag': 'bmv2c2060',
                            'bTracking': '',
                            'calib': 'em',
                            'chainPartName': 'j175_bmv2c2060_split',
                            'cleaning': 'noCleaning',
                            'dataScouting': '',
                            'dataType': 'tc',
                            'etaRange': '0eta320',
                            'extra': '',
                            'jetCalib': 'subjes',
                            'multiplicity': '1',
                            'recoAlg': 'a4',
                            'scan': 'FS',
                            'signature': 'Jet',
                            'threshold': '0',
                            'topo': [],
                            'trigType': 'j'},
                           {'L1item': '',
                            'TLA': '',
                            'addInfo': [],
                            'bConfig': ['split'],
                            'bMatching': [],
                            'bTag': 'bmv2c2050',
                            'bTracking': '',
                            'calib': 'em',
                            'chainPartName': 'j50_bmv2c2050_split',
                            'cleaning': 'noCleaning',
                            'dataScouting': '',
                            'dataType': 'tc',
                            'etaRange': '0eta320',
                            'extra': '',
                            'jetCalib': 'subjes',
                            'multiplicity': '1',
                            'recoAlg': 'a4',
                            'scan': 'FS',
                            'signature': 'Jet',
                            'threshold': '50',
                            'topo': [],
                            'trigType': 'j'}],
             'groups': ['RATE:MultiBJet', 'BW:Bjet'],
             'signature': 'Jet',
             'signatures': '',
             'stream': ['Main'],
             'topo': [],
             'topoStartFrom': False,
             'topoThreshold': None}
        
        generateHLTChainDef(d)
        

if __name__ == '__main__':
    unittest.main()
