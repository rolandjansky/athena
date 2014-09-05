#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @Package test_transform.py
#  @brief Unittests for transform.py
#  @author graeme.andrew.stewart@cern.ch
#  @version $Id: test_transform.py 529035 2012-12-05 15:45:24Z graemes $

import unittest

import logging
msg = logging.getLogger(__name__)

# Allowable to import * from the package for which we are the test suite
from PyJobTransforms.transform import *

# Unittests for this module
class transformsTests(unittest.TestCase):
    
    def test_transformSetup(self):
        # Test we can get a transform
        from PyJobTransforms.trfExe import echoExecutor 
        tf = transform(executor = echoExecutor())
        self.assertTrue(isinstance(tf, transform))

if __name__ == '__main__':
    unittest.main()

