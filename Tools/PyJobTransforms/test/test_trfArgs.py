#! /usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

## @Package test_trfArgs.py
#  @brief Unittests for trfArgs.py
#  @author maddocks.harvey@gmail.com, graeme.andrew.stewart@cern.ch
#  @version $Id: test_trfArgs.py 748303 2016-05-19 09:46:48Z graemes $

import argparse
import json
import os
import os.path as path
import subprocess
import unittest

import logging
msg = logging.getLogger(__name__)

# Allowable to import * from the package for which we are the test suite
from PyJobTransforms.trfArgs import *
from PyJobTransforms.trfArgClasses import trfArgParser
from PyJobTransforms.trfDecorators import silent

## Unittests for trfArgs module
class trfArgsUnitTests(unittest.TestCase):
    
    def test_argsok(self):
        myParser = trfArgParser(description='Test parser for argsok, %s' % __name__)
        addTeaArguments(myParser)
        args = ['--cupsOfTea', '3']
        self.assertTrue(isinstance(myParser.parse_args(args), argparse.Namespace))
        
    # Stop this test from being noisy - argparse spits out to stderr
    # and here we expect a failure
    @silent
    def test_argsbad(self):
        myParser = trfArgParser(description='Test parser for argsbad, %s' % __name__)
        addStandardTrfArgs(myParser)
        args = ['--verbose', '--outputFile', 'someFile', '--runNumber', '1234', '--unknownoption', 'what']
        self.assertRaises(SystemExit, myParser.parse_args, args)

    @silent
    def test_help(self):
        myParser = trfArgParser(description='Test parser for trf help, %s' % __name__)
        addStandardTrfArgs(myParser)
        args = ['--help']
        self.assertRaises(SystemExit, myParser.parse_args, args)
      
    def test_subStep(self):
        myParser = trfArgParser()
        addStandardTrfArgs(myParser)
        addAthenaArguments(myParser)
        args = ['--preExec', 'r2e:stuff', 'e2e:something', 'r2e:somemorestuff', 'e2e:somethingElse']
        myArgDict = vars(myParser.parse_args(args))
        properArgDict = {'r2e': ['stuff', 'somemorestuff'], 'e2e': ['something', 'somethingElse']}
        self.assertTrue(isinstance(myArgDict, dict))
        self.assertEqual(myArgDict['preExec']._value, properArgDict)

    def test_triggerConfig(self):
        myParser = trfArgParser()
        addStandardTrfArgs(myParser)
        addTriggerArguments(myParser)
        args = ['--triggerConfig', 'r2e,e2e=MC:TRIGGERDB:124,154,132']
        myArgDict = vars(myParser.parse_args(args))
        properArgDict = {'r2e': 'MC:TRIGGERDB:124,154,132', 'e2e': 'MC:TRIGGERDB:124,154,132'}
        self.assertTrue(isinstance(myArgDict, dict))
        self.assertEqual(myArgDict['triggerConfig']._value, properArgDict)


class trfIntArgsUnitTests(unittest.TestCase):

    @silent
    def test_EchoInt(self):
        from PyJobTransforms.transform import transform
        tf = transform()
        addTeaArguments(tf.parser)
        self.assertEqual(tf.parseCmdLineArgs(['--cupsOfTea', '123']), None)

    @silent
    def test_EchoIntFail(self):
        from PyJobTransforms.transform import transform
        tf = transform()
        addTeaArguments(tf.parser)
        self.assertRaises(SystemExit, tf.parseCmdLineArgs, ['--cupsOfTea', 'HELLO'])
        
class trfFloatArgsUnitTests(unittest.TestCase):

    @silent
    def test_EchoFloat(self):
        from PyJobTransforms.transform import transform
        tf = transform()
        addTeaArguments(tf.parser)
        self.assertEqual(tf.parseCmdLineArgs(['--mugVolume', '1.23']), None)

    @silent
    def test_EchoFloatFail(self):
        from PyJobTransforms.transform import transform
        tf = transform()
        addTeaArguments(tf.parser)
        self.assertRaises(SystemExit, tf.parseCmdLineArgs, ['--mugVolume', 'LOL'])


class configureFromJSON(unittest.TestCase):
    def setUp(self):
        with open('argdict.json', 'w') as argdict:
            argdict.write('''{"conditionsTag": {"all": "CONDBR2-BLKPA-2015-05"},
                              "geometryVersion": {"all": "ATLAS-R2-2015-03-01-00"},
                              "preExec": {"athena": ["print 'Python says hi!'"]},
                              "skipEvents": {"first": 10},
                              "testFloat": 4.67,
                              "testInt": 5}''')
  
    def tearDown(self):
        for f in 'argdict.json', 'rewrite.json':
            try:
                os.unlink(f)
            except OSError:
                pass

    def test_configFromJSON(self):
        if 'ATN_PACKAGE' in os.environ:
            # While running in ATN Athena_tf.py is not yet in the PATH
            cmd = [path.join(os.environ['ATN_PACKAGE'], 'scripts', 'Athena_tf.py')]
        else:
            cmd = ['Athena_tf.py']

        cmd.extend(['--argJSON', 'argdict.json', '--dumpJSON', 'rewrite.json'])
        self.assertEqual(subprocess.call(cmd), 0)
        with open('rewrite.json') as rewritten_json:
            rewrite = json.load(rewritten_json)

        expectedDictionary = {u'argJSON': u'argdict.json',
                              u"conditionsTag": {u"all": u"CONDBR2-BLKPA-2015-05"},
                              u"geometryVersion": {u"all": u"ATLAS-R2-2015-03-01-00"},
                              u"preExec": {u"athena": [u"print 'Python says hi!'"]},
                              u"skipEvents": {u"first": 10},
                              u"testFloat": 4.67,
                              u"testInt": 5}

        self.assertDictEqual(rewrite, expectedDictionary)


if __name__ == '__main__':
    unittest.main()
