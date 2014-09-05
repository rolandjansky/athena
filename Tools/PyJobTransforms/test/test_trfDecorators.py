#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @Package test_trfDecorators.py
#  @brief Unittests for trfDecorators.py
#  @author graeme.andrew.stewart@cern.ch
#  @version $Id: test_trfDecorators.py 529035 2012-12-05 15:45:24Z graemes $

import unittest

import logging
msg = logging.getLogger(__name__)

# Allowable to import * from the package for which we are the test suite
from PyJobTransforms.trfDecorators import *

## Unittests for this module
class trfDecoratorTests(unittest.TestCase):
    pass

if __name__ == '__main__':
    unittest.main()

