#!/usr/bin/env python
#
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

import unittest
import json
import pickle
import sys

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaMonitoringKernel.GenericMonitoringTool import (GenericMonitoringTool,
                                                          GenericMonitoringArray)

# suppress error messages for these tests
from AthenaMonitoringKernel.GenericMonitoringTool import log
log.setLevel(50)

def histogramDictionary(histogramList):
    jsonList = [json.loads(histogram) for histogram in histogramList]
    return dict((h['alias'],h) for h in jsonList)

class TestGMT(unittest.TestCase):
    """Tests for GenericMonitoringTool"""

    def test_createGMT(self):
        gmt = GenericMonitoringTool()
        self.assertIsInstance(gmt, CompFactory.GenericMonitoringTool)

    def test_pickle(self):
        gmt = GenericMonitoringTool()
        pickle.dumps(gmt)

    def test_defineHistogram(self):
        gmt = GenericMonitoringTool()
        gmt.defineHistogram('x')
        self.assertTrue(len(gmt.Histograms)>0)

    def test_defineTree(self):
        gmt = GenericMonitoringTool()
        gmt.defineTree('t', 'x/F')
        d = histogramDictionary(gmt.Histograms)
        self.assertEqual(d['t']['type'], 'TTree')

    def test_duration(self):
        gmt = GenericMonitoringTool(defaultDuration='run')
        gmt.convention = 'OFFLINE'
        gmt.defineHistogram('x;x_run')
        gmt.defineHistogram('x;x_lb', duration='lb')
        d = histogramDictionary(gmt.Histograms)
        self.assertEqual(d['x_run']['convention'], 'OFFLINE:run')
        self.assertEqual(d['x_lb']['convention'], 'OFFLINE:lb')

    # Tests for GenericMonitoringArray
    def test_toolVec(self):
        gma = GenericMonitoringArray('TestGMA', [2])
        self.assertIsInstance(gma, GenericMonitoringArray)

    def test_getitem(self):
        gma = GenericMonitoringArray('TestGMA', [2])
        gmt = gma[0]
        self.assertIsInstance(gmt, CompFactory.GenericMonitoringTool)

    def test_toolMap(self):
        gma = GenericMonitoringArray('TestGMA', [['x', 'y', 'z']])
        self.assertEqual(gma.Postfixes, ['_x', '_y', '_z'])

    def test_postfixes(self):
        gmaVec = GenericMonitoringArray('TestGMAVec', [2,2])
        gmaMap = GenericMonitoringArray('TestGMAMap', [['a', 'b'],['c','d']])
        self.assertTrue(len(gmaVec.Tools)==4)
        self.assertTrue(len(gmaMap.Tools)==4)
        self.assertEqual(gmaVec.Postfixes, ['_0_0', '_0_1', '_1_0', '_1_1'])
        self.assertEqual(gmaMap.Postfixes, ['_a_c', '_a_d', '_b_c', '_b_d'])

    def test_broadcast(self):
        gma = GenericMonitoringArray('TestGMA', [2])
        gma.broadcast('HistPath', '/path/to/set/')
        self.assertEqual(gma[0].HistPath, '/path/to/set/')
        self.assertEqual(gma[1].HistPath, '/path/to/set/')

    def test_defineHistograms(self):
        gmaVec = GenericMonitoringArray('TestGMAVec', [2])
        gmaMap = GenericMonitoringArray('TestGMAMap', [['a', 'b']])
        gmaVec.defineHistogram('x')
        gmaMap.defineHistogram('x')
        dVec = histogramDictionary(list(gmaVec[0].Histograms)+list(gmaVec[1].Histograms))
        dMap = histogramDictionary(list(gmaMap[0].Histograms)+list(gmaMap[1].Histograms))
        self.assertIn('x_0', dVec)
        self.assertIn('x_1', dVec)
        self.assertIn('x_a', dMap)
        self.assertIn('x_b', dMap)
        self.assertEqual(dVec['x_0']['xvar'], 'x')
        self.assertEqual(dVec['x_1']['xvar'], 'x')
        self.assertEqual(dMap['x_a']['xvar'], 'x')
        self.assertEqual(dMap['x_b']['xvar'], 'x')

    def test_titleTemplate(self):
        gma = GenericMonitoringArray('TestGMA', [['a', 'b']])
        gma.defineHistogram('x', title='Plot of x for {}')
        d = histogramDictionary(list(gma[0].Histograms)+list(gma[1].Histograms))
        self.assertEqual(d['x_a']['title'], 'Plot of x for a')
        self.assertEqual(d['x_b']['title'], 'Plot of x for b')
        with self.assertRaises(IndexError):
            gma.defineHistogram('y', title='{1}')

    def test_pathTemplate(self):
        gma = GenericMonitoringArray('TestGMA', [['a','b']])
        gma.defineHistogram('x', path='/xxx/group_{}/yyy/')
        d = histogramDictionary(list(gma[0].Histograms)+list(gma[1].Histograms))
        self.assertEqual(d['x_a']['path'], '/xxx/group_a/yyy/')
        self.assertEqual(d['x_b']['path'], '/xxx/group_b/yyy/')
        with self.assertRaises(IndexError):
            gma.defineHistogram('y', path='{1}')

    def test_aliasTemplate(self):
        gma = GenericMonitoringArray('TestGMA', [['a','b']])
        gma.defineHistogram('x;x-alias-{}-formatted')
        d = histogramDictionary(list(gma[0].Histograms)+list(gma[1].Histograms))
        self.assertEqual(d['x-alias-a-formatted']['alias'], 'x-alias-a-formatted')
        self.assertEqual(d['x-alias-b-formatted']['alias'], 'x-alias-b-formatted')
        with self.assertRaises(IndexError):
            gma.defineHistogram('y;y-alias-{1}-formatted')

    def test_forwardsHistogramArguments(self):
        gma = GenericMonitoringArray('TestGMA', [2])
        gma.defineHistogram('x,y', type='TH2D', weight='z')
        d = histogramDictionary(list(gma[0].Histograms)+list(gma[1].Histograms))
        self.assertEqual(d['y_vs_x_0']['xvar'], 'x')
        self.assertEqual(d['y_vs_x_1']['xvar'], 'x')
        self.assertEqual(d['y_vs_x_0']['yvar'], 'y')
        self.assertEqual(d['y_vs_x_1']['yvar'], 'y')
        self.assertEqual(d['y_vs_x_0']['weight'], 'z')
        self.assertEqual(d['y_vs_x_1']['weight'], 'z')


class TestLegacy(TestGMT):
    """All the same tests as above but now with legacy Configurables"""

    @classmethod
    def setUpClass(cls):
        # force Run-2 config in CompFactory
        sys.modules['AthenaCommon.Include'] = 'dummy'

    @classmethod
    def tearTownClass(cls):
        sys.modules.pop('AthenaCommon.Include')

    def test_legacy(self):
        from AthenaConfiguration.ComponentFactory import isRun3Cfg
        self.assertFalse(isRun3Cfg())

    def test_configurableType(self):
        gmt = GenericMonitoringTool('gmt')
        self.assertFalse(hasattr(gmt,'__cpp_type__'))


class TestGaudiConfig2(unittest.TestCase):
    """Tests specific to GaudiConfig2"""

    @classmethod
    def setUpClass(cls):
        from AthenaConfiguration import AtlasSemantics  # noqa: F401 (ATLAS merge semantics)

    def test_merge(self):
        t1 = GenericMonitoringTool(Histograms = ['x','y'])
        t2 = GenericMonitoringTool(Histograms = ['z'])
        t2.merge(t1)
        self.assertEqual(list(t1.Histograms), ['x','y','z'])


if __name__ == '__main__':
   unittest.main()
