#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @Package test_trfArgs.py
#  @brief Unittests for trfArgs.py
#  @author maddocks.harvey@gmail.com, graeme.andrew.stewart@cern.ch
#  @version $Id: test_trfArgs.py 534178 2013-01-21 19:04:08Z graemes $

import unittest

import logging
msg = logging.getLogger(__name__)

# Allowable to import * from the package for which we are the test suite
from PyJobTransforms.trfArgs import *
from PyJobTransforms.trfArgClasses import trfArgParser

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
        args = ['--verbose', '--outputFile', 'someFile', '--runNumber', '1234', 
                '--unknownoption', 'what']
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
        self.assertEquals(myArgDict['preExec']._value, properArgDict)
        
    def test_Pickle(self):
        myParser = trfArgParser(description='test parser for pickled arguments, %s' % __name__)
        addStandardTrfArgs(myParser)
        addAthenaArguments(myParser)
        import pickle
        pickleArgs = {'cupsOfTea' : '3', 'mugVolume' : '314.56', 'teaType' : 'earl grey', 'drinkers': 'graeme,bjorn,mark'}
        pickle.dump(pickleArgs, open("testorama", "wb"))
        from PyJobTransforms.transform import transform
        tf = transform()
        addTeaArguments(tf.parser)
        self.assertEquals(tf.parseCmdLineArgs(['--argdict', 'testorama']), None)
        try:
            os.unlink('testorama')
        except OSError:
            pass


class trfIntArgsUnitTests(unittest.TestCase):

    @silent
    def test_EchoInt(self):
        from PyJobTransforms.transform import transform
        tf = transform()
        addTeaArguments(tf.parser)
        self.assertEquals(tf.parseCmdLineArgs(['--cupsOfTea', '123']), None)

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
        self.assertEquals(tf.parseCmdLineArgs(['--mugVolume', '1.23']), None)

    @silent
    def test_EchoFloatFail(self):
        from PyJobTransforms.transform import transform
        tf = transform()
        addTeaArguments(tf.parser)
        self.assertRaises(SystemExit, tf.parseCmdLineArgs, ['--mugVolume', 'LOL'])


if __name__ == '__main__':
    unittest.main()
