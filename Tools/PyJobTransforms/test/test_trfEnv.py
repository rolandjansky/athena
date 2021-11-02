#! /usr/bin/env python

# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

## @Package test_trfEnv.py
#  @brief Unittests for trfEnv.py
#  @author atlas-comp-transforms-dev@cern.ch

import sys
import os
import unittest

from PyJobTransforms.trfLogger import msg
msg.info('logging set in %s' % sys.argv[0])

# Allowable to import * from the package for which we are the test suite
from PyJobTransforms.trfEnv import *

# Unittests for this module
class trfEnvTests(unittest.TestCase):
    
    def test_trfEnvinit(self):
        # Test we can get an environmentUpdate object
        envUp = environmentUpdate()
        self.assertTrue(isinstance(envUp, environmentUpdate))
        pVar = pathVar('PATH')
        self.assertTrue(isinstance(pVar, pathVar))
        
    def test_pathAdd(self):
        pVar = pathVar('PATH')
        pVar.add('/tmp')
        self.assertEqual(pVar._value[0], '/tmp')
        self.assertEqual(pVar._value[1], pVar._currentEnvironmentValue)
        pVar.add('/somepaththatdoesnotexist')
        self.assertNotEqual(pVar._value[0], '/somepaththatdoesnotexist')

    def test_pathStrValue(self):
        pVar = pathVar('foo', testForExistance=False)
        pVar.add('bar')
        pVar.add('baz')
        self.assertEqual(str(pVar), "baz:bar")
        
    def testEnvUp(self):
        from PyJobTransforms.trfArgClasses import argSubstepList, argSubstepBool
        envUp = environmentUpdate()
        argDict = {'env': argSubstepList(["KEY1=VALUE1","KEY2=VALUE2","KEY3=VALUE3"]),
                   'imf': argSubstepBool('True')}
        envUp.setStandardEnvironment(argDict)
        print(envUp.values)
        print(envUp._envdict)
        self.assertTrue("KEY1" in envUp._envdict)
        if 'ATLASMKLLIBDIR_PRELOAD' in os.environ:
            self.assertTrue("LD_PRELOAD" in envUp._envdict)
        

if __name__ == '__main__':
    unittest.main()

