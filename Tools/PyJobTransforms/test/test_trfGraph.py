#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @Package test_trfGraph.py
#  @brief Unittests for trfGraph.py
#  @author graeme.andrew.stewart@cern.ch
#  @version $Id: test_trfGraph.py 542755 2013-03-27 15:11:16Z graemes $

import unittest

import logging
msg = logging.getLogger(__name__)

# Allowable to import * from the package for which we are the test suite
from PyJobTransforms.trfGraph import *

import PyJobTransforms.trfExceptions as trfExceptions

## Unit tests
class graphNodeTests(unittest.TestCase):
    
    def setUp(self):
        self.gn1 = graphNode('testNode1', ['ESD'], ['AOD'])
        self.gn2 = graphNode('testNode2', ['ESD', 'ESD1'], ['AOD', 'HIST_AOD'])
        self.gn3 = graphNode('testNode3', [('HIST_AOD', 'HIST_ESD')], ['HIST'])
        self.gn4 = graphNode('testNode4', ['BS', 'RDO'], ['ESD'])
    
    def test_graphNodeBasicNodeProperties(self):
        self.assertEqual(self.gn1.name, 'testNode1')
        self.assertEqual(self.gn1.inData, set(['ESD']))
        self.assertEqual(self.gn1.outData, set(['AOD']))
        self.assertEqual(self.gn1.connections, {'in':{}, 'out':{}})
        self.assertEqual(self.gn2.name, 'testNode2')
        self.assertEqual(self.gn2.inData, set(['ESD', 'ESD1']))
        self.assertEqual(self.gn2.outData, set(['AOD', 'HIST_AOD']))
        self.assertEqual(self.gn2.outputDataTypes, set(['HIST_AOD', 'AOD']))
        self.assertEqual(self.gn3.inputDataTypes, set(['HIST_AOD', 'HIST_ESD']))
        
    def test_graphNodeConnections(self):
        pass
    
    def test_badDataFlow(self):
        self.assertRaises(trfExceptions.TransformSetupException, executorGraph, executorSet=set(), inputData = ['RAW'], outputData = ['RAW'])
        

if __name__ == '__main__':
    unittest.main()
