#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @Package test_trfArgs.py
#  @brief Unittests for trfArgs.py
#  @author maddocks.harvey@gmail.com, graeme.andrew.stewart@cern.ch
#  @version $Id: test_trfArgs.py 676728 2015-06-19 15:31:09Z graemes $

import argparse
import json
import os
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


class configureFromJSON(unittest.TestCase):
    def setUp(self):
        with open('argdict.json', 'w') as argdict:
            argdict.write("""{"conditionsTag": {"all": "CONDBR2-BLKPA-2015-05"}, "inputBSFile": ["data15_13TeV.00267167.physics_Main.merge.RAW#dummy.input"], "outputESDFile": "data15_13TeV.00267167.physics_Main.recon.ESD.f594#data15_13TeV.00267167.physics_Main.recon.ESD.f594._lb0176._SFO-1._0002", "ignorePatterns": ["ToolSvc.InDetSCTRodDecoder.+ERROR.+Unknown.+offlineId.+for.+OnlineId"], "AMITag": "f594", "outputAODFile": "data15_13TeV.00267167.physics_Main.recon.AOD.f594#data15_13TeV.00267167.physics_Main.recon.AOD.f594._lb0176._SFO-1._0002", "ignoreErrors": true, "maxEvents": "2000", "autoConfiguration": ["everything"], "preExec": {"all": ["DQMonFlags.enableLumiAccess=False;DQMonFlags.doCTPMon=False;from MuonRecExample.MuonRecFlags import muonRecFlags;muonRecFlags.useLooseErrorTuning.set_Value_and_Lock(True);"]}, "skipEvents": "2000", "beamType": "collisions", "geometryVersion": {"all": "ATLAS-R2-2015-03-01-00"}, "postExec": {"r2e": ["topSequence.LArNoisyROAlg.Tool.BadChanPerFEB=30"]}, "outputHISTFile": "data15_13TeV.00267167.physics_Main.recon.HIST.f594#data15_13TeV.00267167.physics_Main.recon.HIST.f594._lb0176._SFO-1._0002"}""")
        with open('dummy.input', 'w') as dummy:
            pass
    
    def tearDown(self):
        for f in 'argdict.json', 'rewrite.json', 'dummy.input':
            try:
                os.unlink(f)
            except OSError:
                pass 

    def test_configFromJSON(self):
        cmd = ['Reco_tf.py', '--argJSON', 'argdict.json', '--dumpJSON', 'rewrite.json']
        self.assertEqual(subprocess.call(cmd), 0)
        with open('rewrite.json') as rewritten_json:
            rewrite = json.load(rewritten_json)
        self.assertEqual(rewrite, {u'conditionsTag': {u'all': u'CONDBR2-BLKPA-2015-05'}, u'inputBSFile': [u'dummy.input'], u'beamType': u'collisions', u'outputESDFile': [u'data15_13TeV.00267167.physics_Main.recon.ESD.f594._lb0176._SFO-1._0002'], u'outputHISTFile': [u'data15_13TeV.00267167.physics_Main.recon.HIST.f594._lb0176._SFO-1._0002'], u'skipEvents': {u'first': 2000}, u'ignoreErrors': True, u'autoConfiguration': [u'everything'], u'maxEvents': {u'first': 2000}, u'AMITag': u'f594', u'postExec': {u'r2e': [u'topSequence.LArNoisyROAlg.Tool.BadChanPerFEB=30']}, u'outputAODFile': [u'data15_13TeV.00267167.physics_Main.recon.AOD.f594._lb0176._SFO-1._0002'], u'preExec': {u'all': [u'DQMonFlags.enableLumiAccess=False;DQMonFlags.doCTPMon=False;from MuonRecExample.MuonRecFlags import muonRecFlags;muonRecFlags.useLooseErrorTuning.set_Value_and_Lock(True);']}, u'geometryVersion': {u'all': u'ATLAS-R2-2015-03-01-00'}, u'argJSON': u'argdict.json', u'ignorePatterns': [u'ToolSvc.InDetSCTRodDecoder.+ERROR.+Unknown.+offlineId.+for.+OnlineId']})

if __name__ == '__main__':
    unittest.main()
