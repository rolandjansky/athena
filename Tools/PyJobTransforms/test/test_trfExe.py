#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @Package test_trfExe.py
#  @brief Unittests for trfExe.py
#  @author graeme.andrew.stewart@cern.ch
#  @version $Id: test_trfExe.py 542755 2013-03-27 15:11:16Z graemes $

import unittest

import logging
msg = logging.getLogger(__name__)

# Allowable to import * from the package for which we are the test suite
from PyJobTransforms.trfExe import *

import PyJobTransforms.trfExceptions as trfExceptions

## Unittests for this module
class trfExeTests(unittest.TestCase):
    
    def test_basicExecutor(self):
        exe = transformExecutor()
        self.assertTrue(isinstance(exe, transformExecutor))
        
    def test_badExecutor(self):
        # Executors are not allowed to consume and produce the same data type
        self.assertRaises(trfExceptions.TransformSetupException, transformExecutor, inData=['RAW'], outData=['RAW'])
    
if __name__ == '__main__':
    unittest.main()
    