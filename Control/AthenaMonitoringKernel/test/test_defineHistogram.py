#!/usr/bin/env python
#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

import unittest
import json

from AthenaMonitoringKernel.GenericMonitoringTool import defineHistogram, defineTree

class Test( unittest.TestCase ):
   def test_1D( self ):
      check = defineHistogram('var', 'TH1F', 'EXPERT', 'title', '', 10, 0.0, 10.0)
      true = '{"alias": "var", "allvars": ["var"], "convention": "", "merge": "", "path": "EXPERT", "title": "title", "treeDef": "", "type": "TH1F", "weight": "", "cutMask": "", "xarray": [], "xbins": 10, "xlabels": [], "xmax": 10.0, "xmin": 0.0, "xvar": "var", "yarray": [], "ybins": 0.0, "ylabels": [], "ymax": 0.0, "ymin": 0.0, "yvar": "", "zbins": 0.0, "zlabels": [], "zmax": 0.0, "zmin": 0.0, "zvar": "", "Sumw2": false, "kLBNHistoryDepth": 0, "kAddBinsDynamically": false, "kRebinAxes": false, "kCanRebin": false, "kVec": false, "kVecUO": false, "kCumulative": false}'
      self.assertEqual(json.loads(check), json.loads(true))

   def test_1D_weight( self ):
      check = defineHistogram('var', weight='myweight')
      true = '{"alias": "var", "allvars": ["var"], "convention": "", "merge": "", "path": "", "title": "var", "treeDef": "", "type": "TH1F", "weight": "myweight", "cutMask": "", "xarray": [], "xbins": 100, "xlabels": [], "xmax": 1, "xmin": 0, "xvar": "var", "yarray": [], "ybins": 0.0, "ylabels": [], "ymax": 0.0, "ymin": 0.0, "yvar": "", "zbins": 0.0, "zlabels": [], "zmax": 0.0, "zmin": 0.0, "zvar": "", "Sumw2": false, "kLBNHistoryDepth": 0, "kAddBinsDynamically": false, "kRebinAxes": false, "kCanRebin": false, "kVec": false, "kVecUO": false, "kCumulative": false}'
      self.assertEqual(json.loads(check), json.loads(true))

   def test_1D_cutmask( self ):
      check = defineHistogram('var', cutmask='mycutmask')
      true = '{"alias": "var", "allvars": ["var"], "convention": "", "merge": "", "path": "", "title": "var", "treeDef": "", "type": "TH1F", "weight": "", "cutMask": "mycutmask", "xarray": [], "xbins": 100, "xlabels": [], "xmax": 1, "xmin": 0, "xvar": "var", "yarray": [], "ybins": 0.0, "ylabels": [], "ymax": 0.0, "ymin": 0.0, "yvar": "", "zbins": 0.0, "zlabels": [], "zmax": 0.0, "zmin": 0.0, "zvar": "", "Sumw2": false, "kLBNHistoryDepth": 0, "kAddBinsDynamically": false, "kRebinAxes": false, "kCanRebin": false, "kVec": false, "kVecUO": false, "kCumulative": false}'
      self.assertEqual(json.loads(check), json.loads(true))

   def test_1D_array( self ):
      check = defineHistogram('var', xbins=[0, 1, 2, 4, 8])
      true = '{"alias": "var", "allvars": ["var"], "convention": "", "merge": "", "path": "", "title": "var", "treeDef": "", "type": "TH1F", "weight": "", "cutMask": "", "xarray": [0, 1, 2, 4, 8], "xbins": 4, "xlabels": [], "xmax": 1, "xmin": 0, "xvar": "var", "yarray": [], "ybins": 0.0, "ylabels": [], "ymax": 0.0, "ymin": 0.0, "yvar": "", "zbins": 0.0, "zlabels": [], "zmax": 0.0, "zmin": 0.0, "zvar": "", "Sumw2": false, "kLBNHistoryDepth": 0, "kAddBinsDynamically": false, "kRebinAxes": false, "kCanRebin": false, "kVec": false, "kVecUO": false, "kCumulative": false}'
      self.assertEqual(json.loads(check), json.loads(true))

   def test_1D_title( self ):
      check = defineHistogram('var', title='mytitle')
      true = '{"alias": "var", "allvars": ["var"], "convention": "", "merge": "", "path": "", "title": "mytitle", "treeDef": "", "type": "TH1F", "weight": "", "cutMask": "", "xarray": [], "xbins": 100, "xlabels": [], "xmax": 1, "xmin": 0, "xvar": "var", "yarray": [], "ybins": 0.0, "ylabels": [], "ymax": 0.0, "ymin": 0.0, "yvar": "", "zbins": 0.0, "zlabels": [], "zmax": 0.0, "zmin": 0.0, "zvar": "", "Sumw2": false, "kLBNHistoryDepth": 0, "kAddBinsDynamically": false, "kRebinAxes": false, "kCanRebin": false, "kVec": false, "kVecUO": false, "kCumulative": false}'
      self.assertEqual(json.loads(check), json.loads(true))

   def test_1D_labelsX( self ):
      check = defineHistogram('var', xlabels=["bin0", "bin1"])
      true = '{"alias": "var", "allvars": ["var"], "convention": "", "merge": "", "path": "", "title": "var", "treeDef": "", "type": "TH1F", "weight": "", "cutMask": "", "xarray": [], "xbins": 2, "xlabels": ["bin0", "bin1"], "xmax": 1, "xmin": 0, "xvar": "var", "yarray": [], "ybins": 0.0, "ylabels": [], "ymax": 0.0, "ymin": 0.0, "yvar": "", "zbins": 0.0, "zlabels": [], "zmax": 0.0, "zmin": 0.0, "zvar": "", "Sumw2": false, "kLBNHistoryDepth": 0, "kAddBinsDynamically": false, "kRebinAxes": false, "kCanRebin": false, "kVec": false, "kVecUO": false, "kCumulative": false}'
      self.assertEqual(json.loads(check), json.loads(true))

   def test_1D_labelsY( self ):
      check = defineHistogram('var', ylabels=["bin0", "bin1"])
      true = '{"alias": "var", "allvars": ["var"], "convention": "", "merge": "", "path": "", "title": "var", "treeDef": "", "type": "TH1F", "weight": "", "cutMask": "", "xarray": [], "xbins": 100, "xlabels": [], "xmax": 1, "xmin": 0, "xvar": "var", "yarray": [], "ybins": 2, "ylabels": ["bin0", "bin1"], "ymax": 0.0, "ymin": 0.0, "yvar": "", "zbins": 0.0, "zlabels": [], "zmax": 0.0, "zmin": 0.0, "zvar": "", "Sumw2": false, "kLBNHistoryDepth": 0, "kAddBinsDynamically": false, "kRebinAxes": false, "kCanRebin": false, "kVec": false, "kVecUO": false, "kCumulative": false}'
      self.assertEqual(json.loads(check), json.loads(true))

   def test_2D( self ):
      check = defineHistogram('varX,varY', type='TH2F', xbins=10, xmin=0.0, xmax=10.0, ybins=40, ymin=0.0, ymax=20.0)
      true = '{"alias": "varY_vs_varX", "allvars": ["varX", "varY"], "convention": "", "merge": "", "path": "", "title": "varX,varY", "treeDef": "", "type": "TH2F", "weight": "", "cutMask": "", "xarray": [], "xbins": 10, "xlabels": [], "xmax": 10.0, "xmin": 0.0, "xvar": "varX", "yarray": [], "ybins": 40, "ylabels": [], "ymax": 20.0, "ymin": 0.0, "yvar": "varY", "zbins": 0.0, "zlabels": [], "zmax": 0.0, "zmin": 0.0, "zvar": "", "Sumw2": false, "kLBNHistoryDepth": 0, "kAddBinsDynamically": false, "kRebinAxes": false, "kCanRebin": false, "kVec": false, "kVecUO": false, "kCumulative": false}'
      self.assertEqual(json.loads(check), json.loads(true))

   def test_2D_space( self ):
      check = defineHistogram(' varX , varY ', type='TH2F', xbins=10, xmin=0.0, xmax=10.0, ybins=40, ymin=0.0, ymax=20.0)
      true = '{"alias": "varY_vs_varX", "allvars": ["varX", "varY"], "convention": "", "merge": "", "path": "", "title": " varX , varY ", "treeDef": "", "type": "TH2F", "weight": "", "cutMask": "", "xarray": [], "xbins": 10, "xlabels": [], "xmax": 10.0, "xmin": 0.0, "xvar": "varX", "yarray": [], "ybins": 40, "ylabels": [], "ymax": 20.0, "ymin": 0.0, "yvar": "varY", "zbins": 0, "zlabels": [], "zmax": 0.0, "zmin": 0.0, "zvar": "", "Sumw2": false, "kLBNHistoryDepth": 0, "kAddBinsDynamically": false, "kRebinAxes": false, "kCanRebin": false, "kVec": false, "kVecUO": false, "kCumulative": false}'
      self.assertEqual(json.loads(check), json.loads(true))

   def test_2D_array( self ):
      check = defineHistogram('varX,varY', 'TH2F', xbins=[0,1,2], ybins=[1,2,3,7])
      true = '{"alias": "varY_vs_varX", "allvars": ["varX", "varY"], "convention": "", "merge": "", "path": "", "title": "varX,varY", "treeDef": "", "type": "TH2F", "weight": "", "cutMask": "", "xarray": [0, 1, 2], "xbins": 2, "xlabels": [], "xmax": 1, "xmin": 0, "xvar": "varX", "yarray": [1, 2, 3, 7], "ybins": 3, "ylabels": [], "ymax": 0.0, "ymin": 0.0, "yvar": "varY", "zbins": 0.0, "zlabels": [], "zmax": 0.0, "zmin": 0.0, "zvar": "", "Sumw2": false, "kLBNHistoryDepth": 0, "kAddBinsDynamically": false, "kRebinAxes": false, "kCanRebin": false, "kVec": false, "kVecUO": false, "kCumulative": false}'
      self.assertEqual(json.loads(check), json.loads(true))

   def test_2D_labelsXY( self ):
      check = defineHistogram('varX,varY', 'TH2F', xlabels=["bin0", "bin1"], ylabels=["bin0", "bin1", "bin2"])
      true = '{"alias": "varY_vs_varX", "allvars": ["varX", "varY"], "convention": "", "merge": "", "path": "", "title": "varX,varY", "treeDef": "", "type": "TH2F", "weight": "", "cutMask": "", "xarray": [], "xbins": 2, "xlabels": ["bin0", "bin1"], "xmax": 1, "xmin": 0, "xvar": "varX", "yarray": [], "ybins": 3, "ylabels": ["bin0", "bin1", "bin2"], "ymax": 0.0, "ymin": 0.0, "yvar": "varY", "zbins": 0.0, "zlabels": [], "zmax": 0.0, "zmin": 0.0, "zvar": "", "Sumw2": false, "kLBNHistoryDepth": 0, "kAddBinsDynamically": false, "kRebinAxes": false, "kCanRebin": false, "kVec": false, "kVecUO": false, "kCumulative": false}'
      self.assertEqual(json.loads(check), json.loads(true))

   def test_3D( self ):
      check = defineHistogram('varX,varY,varZ', 'TProfile2D',
         xbins=10, xmin=0.0, xmax=10.0, ybins=40, ymin=0.0, ymax=20.0, zmin=-1.0, zmax=1.0)
      true = '{"alias": "varZ_vs_varY_vs_varX", "allvars": ["varX", "varY", "varZ"], "convention": "", "merge": "", "path": "", "title": "varX,varY,varZ", "treeDef": "", "type": "TProfile2D", "weight": "", "cutMask": "", "xarray": [], "xbins": 10, "xlabels": [], "xmax": 10.0, "xmin": 0.0, "xvar": "varX", "yarray": [], "ybins": 40, "ylabels": [], "ymax": 20.0, "ymin": 0.0, "yvar": "varY", "zbins": 0.0, "zlabels": [], "zmax": 1.0, "zmin": -1.0, "zvar": "varZ", "Sumw2": false, "kLBNHistoryDepth": 0, "kAddBinsDynamically": false, "kRebinAxes": false, "kCanRebin": false, "kVec": false, "kVecUO": false, "kCumulative": false}'
      self.assertEqual(json.loads(check), json.loads(true))

   def test_efficiency( self ):
      check = defineHistogram('var,pass', type='TEfficiency')
      true = '{"alias": "pass_vs_var", "allvars": ["var", "pass"], "convention": "", "merge": "", "path": "", "title": "var,pass", "treeDef": "", "type": "TEfficiency", "weight": "", "cutMask": "", "xarray": [], "xbins": 100, "xlabels": [], "xmax": 1, "xmin": 0, "xvar": "var", "yarray": [], "ybins": 0.0, "ylabels": [], "ymax": 0.0, "ymin": 0.0, "yvar": "pass", "zbins": 0.0, "zlabels": [], "zmax": 0.0, "zmin": 0.0, "zvar": "", "Sumw2": false, "kLBNHistoryDepth": 0, "kAddBinsDynamically": false, "kRebinAxes": false, "kCanRebin": false, "kVec": false, "kVecUO": false, "kCumulative": false}'
      self.assertEqual(json.loads(check), json.loads(true))

   def test_tree( self ):
      check = defineTree('var,pass', treedef='var/F:pass/I')
      true = '{"alias": "pass_vs_var", "allvars": ["var", "pass"], "convention": "", "merge": "", "path": "", "title": "var,pass", "treeDef": "var/F:pass/I", "type": "TTree", "weight": "", "cutMask": "", "xarray": [], "xbins": 100, "xlabels": [], "xmax": 1, "xmin": 0, "xvar": "var", "yarray": [], "ybins": 0.0, "ylabels": [], "ymax": 0.0, "ymin": 0.0, "yvar": "pass", "zbins": 0, "zlabels": [], "zmax": 0.0, "zmin": 0.0, "zvar": "", "Sumw2": false, "kLBNHistoryDepth": 0, "kAddBinsDynamically": false, "kRebinAxes": false, "kCanRebin": false, "kVec": false, "kVecUO": false, "kCumulative": false}'
      self.assertEqual(json.loads(check), json.loads(true))

   def test_offlineNamingConvention( self ):
      check = defineHistogram('var', path='EXPERT', convention='OFFLINE:lowStat')
      true = '{"alias": "var", "allvars": ["var"], "convention": "OFFLINE:lowStat", "merge": "", "path": "EXPERT", "title": "var", "treeDef": "", "type": "TH1F", "weight": "", "cutMask": "", "xarray": [], "xbins": 100, "xlabels": [], "xmax": 1, "xmin": 0, "xvar": "var", "yarray": [], "ybins": 0.0, "ylabels": [], "ymax": 0.0, "ymin": 0.0, "yvar": "", "zbins": 0.0, "zlabels": [], "zmax": 0.0, "zmin": 0.0, "zvar": "", "Sumw2": false, "kLBNHistoryDepth": 0, "kAddBinsDynamically": false, "kRebinAxes": false, "kCanRebin": false, "kVec": false, "kVecUO": false, "kCumulative": false}'
      self.assertEqual(json.loads(check), json.loads(true))

   def test_merge( self ):
      check = defineHistogram('var', 'TH1F', 'EXPERT', 'title', '', 10, 0.0, 10.0, merge='weightedAverage')
      true = '{"alias": "var", "allvars": ["var"], "convention": "", "merge": "weightedAverage", "path": "EXPERT", "title": "title", "treeDef": "", "type": "TH1F", "weight": "", "cutMask": "", "xarray": [], "xbins": 10, "xlabels": [], "xmax": 10.0, "xmin": 0.0, "xvar": "var", "yarray": [], "ybins": 0.0, "ylabels": [], "ymax": 0.0, "ymin": 0.0, "yvar": "", "zbins": 0, "zlabels": [], "zmax": 0.0, "zmin": 0.0, "zvar": "", "Sumw2": false, "kLBNHistoryDepth": 0, "kAddBinsDynamically": false, "kRebinAxes": false, "kCanRebin": false, "kVec": false, "kVecUO": false, "kCumulative": false}'
      self.assertEqual(json.loads(check), json.loads(true))

   def test_badAlias( self ):
      check = defineHistogram('var;alias;more')
      self.assertFalse(check)

   def test_invalidAlias( self ):
      from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
      athenaCommonFlags.isOnline = True
      check = defineHistogram('var;myhist(', path='EXPERT')
      self.assertIs(check, '')
      check = defineHistogram('var;myhist', path='EXPERT')
      self.assertNotEqual(check, '')

      athenaCommonFlags.isOnline = False
      check = defineHistogram('var;my/hist')
      self.assertIs(check, '')
      check = defineHistogram('var;myhist(')
      self.assertNotEqual(check, '')

   def test_enforcePath( self ):
      from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
      athenaCommonFlags.isOnline = True
      with self.assertRaises(AssertionError):
         defineHistogram('var', 'TH1F')
      athenaCommonFlags.isOnline = False

   def test_enforceType( self ):
      from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
      athenaCommonFlags.isOnline = True
      check = defineHistogram('var,pass', 'TEfficiency')
      self.assertIs(check, '')
      check = defineTree('var,pass', treedef='var/F:pass/I')
      self.assertIs(check, '')
      athenaCommonFlags.isOnline = False

   def test_enforceMergeTypesTest( self ):
      with self.assertRaises(AssertionError):
         defineHistogram('var,pass', type='TEfficiency', merge='weightedAverage')
      with self.assertRaises(AssertionError):
         defineHistogram('var,pass', type='TTree', merge='weightedAverage')
     

if __name__ == '__main__':
   unittest.main()
