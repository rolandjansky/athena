# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
import os
from cStringIO import StringIO
import copy
import unittest
from mock import MagicMock
from JetDef import generateHLTChainDef, usage
from test_functions import run_strawman_test

from ChainConfigMaker import JetAttributes

from MC_pp_V5_dicts import MC_pp_V5_dicts

class TestJetDef(unittest.TestCase):

    def output_off(self):
        self.oldout, self.olderr = sys.stdout, sys.stderr
        self.out=[StringIO(), StringIO()]
        sys.stdout,sys.stderr = self.out

    def output_on(self):
        sys.stdout, sys.stderr = self.oldout, self.olderr
        
    def setUp(self):
        # single dictionary to play with
        self.from_central =  copy.deepcopy(MC_pp_V5_dicts[0])

        
    def test_0(self):
        """Create ChainDef instances with full instantiation"""

        for dd in MC_pp_V5_dicts:
            chain_def = generateHLTChainDef(dd)
            if chain_def.__class__.__name__ != 'ChainDef':
                print 'real error ---> ', chain_def
            self.assertTrue(chain_def.__class__.__name__ == 'ChainDef')

    def _test_3(self):
        'run exerciser function'
        run_test()

    def test_4(self):
        'test error handling: missing entry in input dict'

        del self.from_central['chainParts']
        self.output_off()
        chain_def = generateHLTChainDef(self.from_central)
        self.output_on()
        # if chain_def.__class__.__name__ != 'ChainDef':
        #    print chain_def
        self.assertTrue(chain_def.__class__.__name__ == 'ErrorChainDef')
        
    def test_5(self):
        'test error handling: too many chain parts'

        self.from_central['chainParts'].extend([{}, {}])
        self.output_off()
        chain_def = generateHLTChainDef(self.from_central)
        # if chain_def.__class__.__name__ != 'ChainDef':
        #    print chain_def
        self.assertTrue(chain_def.__class__.__name__ == 'ErrorChainDef')
        
    def test_6(self):
        'test error handling: unexpected chain part signature'

        self.from_central['chainParts'][0]['signature'] = 'junk'
        self.output_off()
        chain_def = generateHLTChainDef(self.from_central)
        # if chain_def.__class__.__name__ != 'ChainDef':
        #    print chain_def
        self.assertTrue(chain_def.__class__.__name__ == 'ErrorChainDef')
        
    def test_7(self):
        'test error handling: missing entry in chain part dict'

        del self.from_central['chainParts'][0]['etaRange']
        self.output_off()
        chain_def = generateHLTChainDef(self.from_central)
        # if chain_def.__class__.__name__ != 'ChainDef':
        #    print chain_def
        self.assertTrue(chain_def.__class__.__name__ == 'ErrorChainDef')
        
    def test_8(self):
        'test error handling: unexpected dataType in chain part dict'

        self.from_central['chainParts'][0]['dataType'] = 'junk'
        self.output_off()
        chain_def = generateHLTChainDef(self.from_central)
        # if chain_def.__class__.__name__ != 'ChainDef':
        #    print chain_def
        self.assertTrue(chain_def.__class__.__name__ == 'ErrorChainDef')
        
    def test_9(self):
        'test error handling: topo TT is one of the dataTypes'

        self.from_central['chainParts'][0]['dataType'] = 'TT'
        chain_def = generateHLTChainDef(self.from_central)
        #if chain_def.__class__.__name__ != 'ChainDef':
        #    print chain_def
        # No TT alg yet
        if 'JETDEF_NO_INSTANTIATION' in os.environ:
            self.assertTrue(chain_def.__class__.__name__ == 'ChainDef')
        else:
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
        self.output_off()
        chain_def = generateHLTChainDef(self.from_central)
        # TT not implemented 11/5/2014
        self.assertTrue(chain_def.__class__.__name__ == 'ErrorChainDef')


    def test_14(self):
        """error handling: bad scan type"""

        self.from_central['chainName'] = 'bad scan type'
        self.from_central['chainParts'][0]['scan'] = 'junk'
        self.output_off()
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
        self.output_off()
        chain_def = generateHLTChainDef(self.from_central)
        self.assertTrue(chain_def.__class__.__name__ == 'ErrorChainDef')

    def test_17(self):
        """excercise usage mesage"""
        usage()

if __name__ == '__main__':
    unittest.main()
