#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# Simple test wrapper for PyUtils.RootUtils
#

# Make errors visible.
import os
os.environ['PYUTILS_SHUTUP_DEBUG']='1'
import ctypes
dso=ctypes.cdll.LoadLibrary('libCxxUtils.so')
dso.CxxUtils_installFatalHandler()

import sys
import unittest

class rootUtilsTests(unittest.TestCase):

    def test_rootUtilsBasic(self):
        import PyUtils.RootUtils as ru
        self.assertEqual(ru._test_main(), True)

if __name__ == '__main__':
    unittest.main()
