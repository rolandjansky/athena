#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @Package test_trfExceptions.py
#  @brief Unittests for trfExceptions.py
#  @author graeme.andrew.stewart@cern.ch
#  @version $Id: test_trfExceptions.py 570543 2013-11-14 21:52:16Z graemes $

import unittest

import logging
msg = logging.getLogger(__name__)

# Allowable to import * from the package for which we are the test suite
from PyJobTransforms.trfExceptions import *



## Unittests for this module
class trfExceptionTests(unittest.TestCase):

    def test_baseException(self):
        e = TransformException(1, 'A simple exception message')
        self.assertTrue(isinstance(e, TransformException))

    def test_inheritedException(self):
        e = TransformLogfileErrorException(1, 'A simple exception message')
        self.assertTrue(isinstance(e, TransformLogfileErrorException))
        self.assertTrue(isinstance(e, TransformException))

    def test_getters(self):
        e = TransformLogfileErrorException(1, 'A simple exception message')
        self.assertEqual(e.errCode, 1)
        self.assertEqual(e.errMsg, 'A simple exception message')
        
    def test_errMsgSetter(self):
        e = TransformLogfileErrorException(1, 'A simple exception message')
        e.errMsg = 'A new message'
        self.assertEqual(e.errMsg, 'A new message')
        

if __name__ == '__main__':
    unittest.main()
    