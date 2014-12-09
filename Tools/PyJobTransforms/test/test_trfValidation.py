#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @Package test_trfValidation.py
#  @brief Unittests for trfValidation.py
#  @author graeme.andrew.stewart@cern.ch
#  @version $Id: test_trfValidation.py 634639 2014-12-09 08:56:18Z graemes $

import unittest

import logging
import os

msg = logging.getLogger(__name__)

# Allowable to import * from the package for which we are the test suite
from PyJobTransforms.trfValidation import *

import PyJobTransforms.trfArgClasses as trfArgClasses

from PyJobTransforms.trfValidation import eventMatch, athenaLogFileReport

## Unittests for this module
class eventCheckTests(unittest.TestCase):
    
    def test_basicMatch(self):
        evmatch = eventMatch(executor=None)
        evmatch.configureCheck(override={'inEventDict': {'BS': 500},
                                         'outEventDict': {'ESD': 500},
                                         'skipEvents': None,
                                         'maxEvents': None,
                                         'evAccEff': 0.99})
        self.assertTrue(evmatch.decide())
        
    def test_matchWithSkipEvents(self):
        evmatch = eventMatch(executor=None)
        evmatch.configureCheck(override={'inEventDict': {'BS': 500},
                                         'outEventDict': {'ESD': 100},
                                         'skipEvents': 400,
                                         'maxEvents': None,
                                         'evAccEff': 0.99})
        self.assertTrue(evmatch.decide())

    def test_matchWithMaxEvents(self):
        evmatch = eventMatch(executor=None)
        evmatch.configureCheck(override={'inEventDict': {'BS': 500},
                                         'outEventDict': {'ESD': 100},
                                         'skipEvents': None,
                                         'maxEvents': 100,
                                         'evAccEff': 0.99})
        self.assertTrue(evmatch.decide())
 
    def test_matchWithSkipAndMaxEvents(self):
        evmatch = eventMatch(executor=None)
        evmatch.configureCheck(override={'inEventDict': {'BS': 500},
                                         'outEventDict': {'ESD': 100},
                                         'skipEvents': 100,
                                         'maxEvents': 100,
                                         'evAccEff': 0.99})
        self.assertTrue(evmatch.decide())
 
    def test_matchWithSkipAndMaxEventsOvershoot(self):
        evmatch = eventMatch(executor=None)
        evmatch.configureCheck(override={'inEventDict': {'BS': 500},
                                         'outEventDict': {'ESD': 100},
                                         'skipEvents': 400,
                                         'maxEvents': 200,
                                         'evAccEff': 0.99})
        self.assertTrue(evmatch.decide())
         
    def test_tooFewEvents(self):
        evmatch = eventMatch(executor=None)
        evmatch.configureCheck(override={'inEventDict': {'BS': 500},
                                         'outEventDict': {'ESD': 499},
                                         'skipEvents': None,
                                         'maxEvents': None,
                                         'evAccEff': 0.99})
        self.assertRaises(trfExceptions.TransformValidationException, evmatch.decide)
 
    def test_tooFewEventsWithSkipEvents(self):
        evmatch = eventMatch(executor=None)
        evmatch.configureCheck(override={'inEventDict': {'BS': 500},
                                         'outEventDict': {'ESD': 399},
                                         'skipEvents': 100,
                                         'maxEvents': None,
                                         'evAccEff': 0.99})
        self.assertRaises(trfExceptions.TransformValidationException, evmatch.decide)
 
    def test_tooFewEventsWithMaxEvents(self):
        evmatch = eventMatch(executor=None)
        evmatch.configureCheck(override={'inEventDict': {'BS': 500},
                                         'outEventDict': {'ESD': 199},
                                         'skipEvents': None,
                                         'maxEvents': 200,
                                         'evAccEff': 0.99})
        self.assertRaises(trfExceptions.TransformValidationException, evmatch.decide)
 
    def test_tooFewEventsWithSkipAndMaxEvents(self):
        evmatch = eventMatch(executor=None)
        evmatch.configureCheck(override={'inEventDict': {'BS': 500},
                                         'outEventDict': {'ESD': 199},
                                         'skipEvents': 100,
                                         'maxEvents': 200,
                                         'evAccEff': 0.99})
        self.assertRaises(trfExceptions.TransformValidationException, evmatch.decide)
 
    def test_minEff(self):
        evmatch = eventMatch(executor=None, eventCountConf={'Evgen': {'Hits': 'minEff'}})
        evmatch.configureCheck(override={'inEventDict': {'Evgen': 1000},
                                         'outEventDict': {'Hits': 991},
                                         'skipEvents': None,
                                         'maxEvents': None,
                                         'evAccEff': 0.99})
        self.assertTrue(evmatch.decide())
 
    def test_minEffTooFew(self):
        evmatch = eventMatch(executor=None, eventCountConf={'Evgen': {'Hits': 'minEff'}})
        evmatch.configureCheck(override={'inEventDict': {'Evgen': 1000},
                                         'outEventDict': {'Hits': 989},
                                         'skipEvents': None,
                                         'maxEvents': None,
                                         'evAccEff': 0.99})
        self.assertRaises(trfExceptions.TransformValidationException, evmatch.decide)
 
    def test_badConf(self):
        evmatch = eventMatch(executor=None, eventCountConf={'Evgen': {'Hits': "unknownAction"}})
        evmatch.configureCheck(override={'inEventDict': {'Evgen': 1000},
                                         'outEventDict': {'Hits': 989},
                                         'skipEvents': None,
                                         'maxEvents': None,
                                         'evAccEff': 0.99})
        self.assertRaises(trfExceptions.TransformValidationException, evmatch.decide)

    def test_badConfEfficency(self):
        evmatch = eventMatch(executor=None, eventCountConf={'Evgen': {'Hits': 10.0}})
        evmatch.configureCheck(override={'inEventDict': {'Evgen': 1000},
                                         'outEventDict': {'Hits': 989},
                                         'skipEvents': None,
                                         'maxEvents': None,
                                         'evAccEff': 0.99})
        self.assertRaises(trfExceptions.TransformValidationException, evmatch.decide)
 
    def test_minEffRounding(self):
        # Test the eff * inputEvents is rounded correctly with floor()
        evmatch = eventMatch(executor=None, eventCountConf={'Evgen': {'Hits': 'minEff'}})
        evmatch.configureCheck(override={'inEventDict': {'Evgen': 1},
                                         'outEventDict': {'Hits': 0},
                                         'skipEvents': None,
                                         'maxEvents': None,
                                         'evAccEff': 0.99})
        self.assertTrue(evmatch.decide())
         
    def test_globDataTypeMatch(self):
        evmatch = eventMatch(executor=None)
        evmatch.configureCheck(override={'inEventDict': {'BS': 100},
                                         'outEventDict': {'DESD_SGLMU': 10},
                                         'skipEvents': None,
                                         'maxEvents': None,
                                         'evAccEff': 0.99})
        self.assertTrue(evmatch.decide())

    def test_multiData(self):
        evmatch = eventMatch(executor=None)
        evmatch.configureCheck(override={'inEventDict': {'BS': 100},
                                         'outEventDict': {'ESD': 100, 'DESD_SGLMU': 10},
                                         'skipEvents': None,
                                         'maxEvents': None,
                                         'evAccEff': 0.99})
        self.assertTrue(evmatch.decide())

    def test_multiDataFail(self):
        evmatch = eventMatch(executor=None)
        evmatch.configureCheck(override={'inEventDict': {'BS': 100},
                                         'outEventDict': {'ESD': 99, 'DESD_SGLMU': 10},
                                         'skipEvents': None,
                                         'maxEvents': None,
                                         'evAccEff': 0.99})
        self.assertRaises(trfExceptions.TransformValidationException, evmatch.decide)
        
    def test_inputGlobWithFail(self):
        evmatch = eventMatch(executor=None)
        evmatch.configureCheck(override={'inEventDict': {'NTUP_ZEE': 100},
                                         'outEventDict': {'NTUP_ZEE_MRG': 98},
                                         'skipEvents': None,
                                         'maxEvents': None,
                                         'evAccEff': 0.99})
        self.assertRaises(trfExceptions.TransformValidationException, evmatch.decide)


## Unittests for this module
class athenaLogFileReportTests(unittest.TestCase):

    def setUp(self):
        testLogExcerpt = '''
16:00:59 Py:Generate          INFO            MetaData attributes: (None)
16:00:59      +-- AthSequencer/AthOutSeq
16:00:59      +-- AthSequencer/AthRegSeq
16:00:59 MetaData: physicsComment = Dijet truth jet slice JZ2W, with the AU2 CT10 tune
16:00:59 MetaData: generatorName = Pythia8
16:00:59 MetaData: generatorTune = AU2 CT10
16:00:59 MetaData: keywords = dijet, qcd
16:00:59 MetaData: specialConfig = NONE
16:00:59 MetaData: contactPhysicist = MC group
16:00:59 srcFilePrefix = /cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.0.3/AtlasSimulation/19.0.3/InstallArea/share, dirName =
16:00:59 srcFilePrefix = /cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.0.3/AtlasSimulation/19.0.3/InstallArea/share, dirName =        '''

        testErrorExcerpt = '''
09:36:22 Py:Athena            INFO including file "eflowRec/eflowRecESDList.py"
09:36:22 Py:Athena            ERROR too many floobles'''

        with open('file1', 'w') as f1:
            print >> f1, 'This is test file 1 w/o meaning'
        with open('file2', 'w') as f2:
            print >> f2, testLogExcerpt
        with open('file3', 'w') as f3:
            print >> f3, testLogExcerpt
            print >> f3, testErrorExcerpt

        self.myFileReport1 = athenaLogFileReport('file1')
        self.myFileReport2 = athenaLogFileReport('file2')
        self.myFileReport3 = athenaLogFileReport('file3')

    def tearDown(self):
        for f in 'file1', 'file2', 'file3':
            try:
                os.unlink(f)
            except OSError:
                pass

    def test_emptyScanLogMetaData(self):
        self.assertEqual(self.myFileReport1._metaData, {})

    def test_gatherScanLogMetaData(self):
        resultDict = {'physicsComment': 'Dijet truth jet slice JZ2W, with the AU2 CT10 tune',
                      'generatorName': 'Pythia8',
                      'generatorTune': 'AU2 CT10',
                      'keywords': 'dijet, qcd',
                      'specialConfig': 'NONE',
                      'contactPhysicist': 'MC group'
                      }
        self.assertEqual(self.myFileReport2._metaData, resultDict)
        
    def test_logscanError(self):
        self.assertEqual(self.myFileReport3.worstError(), {'level': 'ERROR', 'nLevel': logging.ERROR, 
                                                           'firstError': {'count': 1, 'firstLine': 15,
                                                                          'message': 'Py:Athena            ERROR too many floobles'},})

    ## TODO
    # Special tests for G4 errors and core dumps

## Unittests for this module
if __name__ == '__main__':
    unittest.main()
