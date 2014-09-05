#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @Package test_trfJobOptions.py
#  @brief Unittests for trfJobOptions.py
#  @author graeme.andrew.stewart@cern.ch
#  @version $Id: test_trfJobOptions.py 553586 2013-07-04 14:41:56Z graemes $

import unittest

import logging
msg = logging.getLogger(__name__)

# Allowable to import * from the package for which we are the test suite
from PyJobTransforms.trfJobOptions import *

## Unit tests
class jobOptionsTests(unittest.TestCase):

    def test_runArguments(self):
        ra = RunArguments()
        ra.foo = 'bar'
        ra.baz = ['some', 'list']
        self.assertTrue('foo' in dir(ra) and 'baz' in dir(ra))
        self.assertEqual(ra.foo, 'bar')
        self.assertEqual(ra.baz, ['some', 'list'])
        self.assertFalse(hasattr(ra, 'nothingToSee'))

    # It would be good to have a test of actually writing runargs,
    # but note this really requires a whole transform+executor

## Unittests for this module
if __name__ == '__main__':
    unittest.main()
