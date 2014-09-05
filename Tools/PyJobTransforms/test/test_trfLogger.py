#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @Package test_trfLogger.py
#  @brief Unittests for trfLogger.py
#  @author graeme.andrew.stewart@cern.ch
#  @version $Id: test_trfLogger.py 529035 2012-12-05 15:45:24Z graemes $

import unittest

import logging
msg = logging.getLogger(__name__)

# Allowable to import * from the package for which we are the test suite
from PyJobTransforms.trfLogger import *


## Unittests for this module
# Write me!
if __name__ == '__main__':
    unittest.main()
