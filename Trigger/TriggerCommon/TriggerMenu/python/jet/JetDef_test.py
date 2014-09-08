# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import copy
import unittest
from mock import MagicMock
from JetDef import (generateHLTChainDef,
                    run_strawman_test,
                    run_test)

from ChainConfigMaker import JetAttributes


class TestJetDef(unittest.TestCase):

    def setUp(self):
         self.from_central = {'EBstep': 1,
                             'signatures': '',
                             'stream': 'Jet',
                             'chainParts': [{'dataType': 'tc',
                                             'signature': 'Jet',
                                             'trigType': 'j',
                                             'extra': '',
                                             'multiplicity': '1',
                                             'scan': 'FS',
                                             'etaRange':  '0eta320',
                                             'topo': [],
                                             'calib': 'had',
                                             'L1item': '',
                                             'threshold': '30',
                                             'addInfo': [],
                                             'chainPartName': 'j30_em',
                                             'recoAlg': 'a4'}],
                             'topo': '',
                             'chainCounter': 890,
                             'signature': 'Jet',
                             'L1item': 'L1_J20',
                             'chainName': 'not_used'}


    def test_0(self):
        """Create ChainDef instances with full instantiation"""

        updates = [('j30', {'threshold': '30'}),
                   ('j400', {'threshold': '400'}),
                   # ('fj200', {'threshold': '200', 'etaRange': '320eta640'}),
                   ('j60', {'threshold': '60'}),
                   ('j80', {'threshold': '80'}),
                   ('j110', {'threshold': '110'}),
                   ('j150', {'threshold': '150'}),
                   ('j200', {'threshold': '200'}),
                   ('j260', {'threshold': '260'}),
                   ('j330', {'threshold': '330'}),
                   ('3j175', {'threshold': '175', 'multiplicity': '3'}),
                   ('4j100', {'threshold': '100', 'multiplicity': '4'}),
                   # {'threshold': '85', 'multiplicity': '5'},
                   ]

        counter = 0
        for dd in updates:
            counter += 1
            fc = copy.deepcopy(self.from_central)
            fc['chainParts'][0].update(dd[1])
            # make chain name unique - this is used as an Alg Argument
            # and some central code checks it is unique
            fc['chainName'] = dd[0]
            chain_def = generateHLTChainDef(fc)
            if chain_def.__class__.__name__ != 'ChainDef':
                print chain_def
            self.assertTrue(chain_def.__class__.__name__ == 'ChainDef')

    def test_1(self):
        """Create ChainDef instances with Alg to string conversion"""

        updates = [('j30', {'threshold': '30'}),
                   ('j400', {'threshold': '400'}),
                   # ('fj200', {'threshold': '200', 'etaRange': '0eta320'}),
                   ('j60', {'threshold': '60'}),
                   ('j80', {'threshold': '80'}),
                   ('j110', {'threshold': '110'}),
                   ('j150', {'threshold': '150'}),
                   ('j200', {'threshold': '200'}),
                   ('j260', {'threshold': '260'}),
                   ('j330', {'threshold': '330'}),
                   ('3j175', {'threshold': '175', 'multiplicity': '3'}),
                   ('4j100', {'threshold': '100', 'multiplicity': '4'}),
                   # {'threshold': '85', 'multiplicity': '5'},
                   ]

        n_em = len(updates)
        updates_had = copy.copy(updates)
        for u in updates_had:
            u[1]['calib'] = 'had'
        updates.extend(updates_had)
        self.assertTrue(len(updates) == 2 * n_em)
            

        counter = 0
        for dd in updates:
            counter += 1
            fc = copy.deepcopy(self.from_central)
            fc['chainParts'][0].update(dd[1])
            # make chain name unique - this is used as an Alg Argument
            # and some central code checks it is unique
            fc['chainName'] = dd[0]
            chain_def = generateHLTChainDef(fc, False)
            if chain_def.__class__.__name__ != 'ChainDef':
                print chain_def
            self.assertTrue(chain_def.__class__.__name__ == 'ChainDef')

    def test_2(self):
        'run exerciser function'
        run_strawman_test()

    def test_3(self):
        'run exerciser function'
        run_test()

    def test_4(self):
        'test error handling: missing entry in input dict'

        fc = copy.deepcopy(self.from_central)
        del fc['chainParts']
        chain_def = generateHLTChainDef(fc, False)
        # if chain_def.__class__.__name__ != 'ChainDef':
        #    print chain_def
        self.assertTrue(chain_def.__class__.__name__ == 'ErrorChainDef')
        
    def test_5(self):
        'test error handling: too many chain parts'

        fc = copy.deepcopy(self.from_central)
        fc['chainParts'].extend([{}, {}])
        chain_def = generateHLTChainDef(fc, False)
        # if chain_def.__class__.__name__ != 'ChainDef':
        #    print chain_def
        self.assertTrue(chain_def.__class__.__name__ == 'ErrorChainDef')
        
    def test_6(self):
        'test error handling: unexpected chain part signature'

        fc = copy.deepcopy(self.from_central)
        fc['chainParts'][0]['signature'] = 'junk'
        chain_def = generateHLTChainDef(fc, False)
        # if chain_def.__class__.__name__ != 'ChainDef':
        #    print chain_def
        self.assertTrue(chain_def.__class__.__name__ == 'ErrorChainDef')
        
    def test_7(self):
        'test error handling: missing entry in chain part dict'

        fc = copy.deepcopy(self.from_central)
        del fc['chainParts'][0]['etaRange']
        chain_def = generateHLTChainDef(fc, False)
        # if chain_def.__class__.__name__ != 'ChainDef':
        #    print chain_def
        self.assertTrue(chain_def.__class__.__name__ == 'ErrorChainDef')
        
    def test_8(self):
        'test error handling: unexpected dataType in chain part dict'

        fc = copy.deepcopy(self.from_central)
        fc['chainParts'][0]['dataType'] = 'junk'
        chain_def = generateHLTChainDef(fc, False)
        # if chain_def.__class__.__name__ != 'ChainDef':
        #    print chain_def
        self.assertTrue(chain_def.__class__.__name__ == 'ErrorChainDef')
        
    def test_9(self):
        'test error handling: topo clusters (tc) not one of the dataTypes'

        fc = copy.deepcopy(self.from_central)
        fc['chainParts'][0]['dataType'] = 'TT'
        chain_def = generateHLTChainDef(fc, False)
        # if chain_def.__class__.__name__ != 'ChainDef':
        #    print chain_def
        self.assertTrue(chain_def.__class__.__name__ == 'ErrorChainDef')

                
    def test_11(self):
        'excercise JetAttribute __str__ method'

        ja = JetAttributes('t', '0eta320')
        str(ja)

    def _test_12(self):
        """excercise with input data having both trigger tower and
        topo cluster input data.

        HANDLING of TT not decided 16/6/14
        """


        fc = copy.deepcopy(self.from_central)
        fc['chainName'] = 'TT_and_tc_chainParts'
        d = copy.deepcopy(fc['chainParts'][0])
        d['dataType'] = 'TT'
        d['scan'] = ''
        fc['chainParts'].append(d)
        chain_def = generateHLTChainDef(fc, False)
        print chain_def
        # TT not implemented 11/5/2014
        self.assertTrue(chain_def.__class__.__name__ == 'ChainDef')


    def test_13(self):
        """error handling: unknown reco alg"""

        fc = copy.deepcopy(self.from_central)
        fc['chainName'] = 'bad reco alg'
        fc['chainParts'][0]['recoAlg'] = 'junk'
        chain_def = generateHLTChainDef(fc, False)
        # TT not implemented 11/5/2014
        self.assertTrue(chain_def.__class__.__name__ == 'ErrorChainDef')


    def test_14(self):
        """error handling: bad scan type"""

        fc = copy.deepcopy(self.from_central)
        fc['chainName'] = 'bad scan type'
        fc['chainParts'][0]['scan'] = 'junk'
        chain_def = generateHLTChainDef(fc, False)
        # TT not implemented 11/5/2014
        self.assertTrue(chain_def.__class__.__name__ == 'ErrorChainDef')

    def test_15(self):
        """error handling: scan type set for TT"""

        fc = copy.deepcopy(self.from_central)
        fc['chainName'] = 'scan type set for TT'
        d = copy.deepcopy(fc['chainParts'][0])
        fc['chainParts'].append(d)
        d['scan'] = 'PS'
        d['dataType'] = 'TT'
        chain_def = generateHLTChainDef(fc, False)
        self.assertTrue(chain_def.__class__.__name__ == 'ErrorChainDef')

    def test_16(self):
        """error handling: bad region"""

        fc = copy.deepcopy(self.from_central)
        fc['chainName'] = 'bad region'
        fc['chainParts'][0]['etaRange'] = 'junk'
        chain_def = generateHLTChainDef(fc, False)
        self.assertTrue(chain_def.__class__.__name__ == 'ErrorChainDef')


if __name__ == '__main__':
    unittest.main()
