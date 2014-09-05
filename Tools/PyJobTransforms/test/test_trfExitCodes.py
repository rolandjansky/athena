#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @Package test_trfExitCodes.py
#  @brief Unittests for trfExitCodes.py
#  @author graeme.andrew.stewart@cern.ch
#  @version $Id: test_trfExitCodes.py 542702 2013-03-27 10:49:10Z graemes $

import unittest

import logging
msg = logging.getLogger(__name__)

# Allowable to import * from the package for which we are the test suite
from PyJobTransforms.trfExitCodes import *


## Unittests for this module
class trfExitTests(unittest.TestCase):
    
    def test_exitTestsNameToCode(self):
        self.assertEqual(trfExit.nameToCode('OK'), 0)
        self.assertEqual(trfExit.nameToCode('TRF_SETUP'), 3)

    def test_exitTestsCodeToName(self):
        self.assertEqual(trfExit.codeToName(0), 'OK') 
        self.assertEqual(trfExit.codeToName(3), 'TRF_SETUP')

    def test_exitTestsNameToDesc(self):
        self.assertEqual(trfExit.nameToDesc('OK'), 'Successful exit')
        self.assertEqual(trfExit.nameToDesc('TRF_SETUP'), 'Transform setup error')
        
if __name__ == '__main__':
    unittest.main()
    