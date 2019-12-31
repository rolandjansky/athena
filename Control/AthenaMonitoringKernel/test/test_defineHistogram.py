#!/usr/bin/env python
#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

import unittest
import json

from AthenaMonitoringKernel.GenericMonitoringTool import defineHistogram

class Test( unittest.TestCase ):
   def test_1D( self ):
      check = defineHistogram('var', 'TH1F', 'EXPERT', 'title', '', '', 10, 0.0, 10.0)
      true = '{"alias": "var", "allvars": ["var"], "convention": "", "opt": "", "path": "EXPERT", "title": "title", "type": "TH1F", "weight": "", "xarray": [], "xbins": 10, "xlabels": [], "xmax": 10.0, "xmin": 0.0, "xvar": "var", "yarray": [], "ybins": 0.0, "ylabels": [], "ymax": 0.0, "ymin": 0.0, "yvar": "", "zlabels": [], "zmax": 0.0, "zmin": 0.0, "zvar": ""}'
      self.assertEqual(json.loads(check), json.loads(true))

   def test_1D_opt( self ):
      check = defineHistogram('var', opt='myopt')
      true = '{"alias": "var", "allvars": ["var"], "convention": "", "opt": "myopt", "path": "", "title": "var", "type": "TH1F", "weight": "", "xarray": [], "xbins": 100, "xlabels": [], "xmax": 1, "xmin": 0, "xvar": "var", "yarray": [], "ybins": 0.0, "ylabels": [], "ymax": 0.0, "ymin": 0.0, "yvar": "", "zlabels": [], "zmax": 0.0, "zmin": 0.0, "zvar": ""}'
      self.assertEqual(json.loads(check), json.loads(true))

   def test_1D_weight( self ):
      check = defineHistogram('var', weight='myweight')
      true = '{"alias": "var", "allvars": ["var"], "convention": "", "opt": "", "path": "", "title": "var", "type": "TH1F", "weight": "myweight", "xarray": [], "xbins": 100, "xlabels": [], "xmax": 1, "xmin": 0, "xvar": "var", "yarray": [], "ybins": 0.0, "ylabels": [], "ymax": 0.0, "ymin": 0.0, "yvar": "", "zlabels": [], "zmax": 0.0, "zmin": 0.0, "zvar": ""}'
      self.assertEqual(json.loads(check), json.loads(true))

   def test_1D_array( self ):
      check = defineHistogram('var', xbins=[0, 1, 2, 4, 8])
      true = '{"alias": "var", "allvars": ["var"], "convention": "", "opt": "", "path": "", "title": "var", "type": "TH1F", "weight": "", "xarray": [0, 1, 2, 4, 8], "xbins": 4, "xlabels": [], "xmax": 1, "xmin": 0, "xvar": "var", "yarray": [], "ybins": 0.0, "ylabels": [], "ymax": 0.0, "ymin": 0.0, "yvar": "", "zlabels": [], "zmax": 0.0, "zmin": 0.0, "zvar": ""}'
      self.assertEqual(json.loads(check), json.loads(true))

   def test_1D_title( self ):
      check = defineHistogram('var', title='mytitle')
      true = '{"alias": "var", "allvars": ["var"], "convention": "", "opt": "", "path": "", "title": "mytitle", "type": "TH1F", "weight": "", "xarray": [], "xbins": 100, "xlabels": [], "xmax": 1, "xmin": 0, "xvar": "var", "yarray": [], "ybins": 0.0, "ylabels": [], "ymax": 0.0, "ymin": 0.0, "yvar": "", "zlabels": [], "zmax": 0.0, "zmin": 0.0, "zvar": ""}'
      self.assertEqual(json.loads(check), json.loads(true))

   def test_1D_labelsX( self ):
      check = defineHistogram('var', xlabels=["bin0", "bin1"])
      true = '{"alias": "var", "allvars": ["var"], "convention": "", "opt": "", "path": "", "title": "var", "type": "TH1F", "weight": "", "xarray": [], "xbins": 2, "xlabels": ["bin0", "bin1"], "xmax": 1, "xmin": 0, "xvar": "var", "yarray": [], "ybins": 0.0, "ylabels": [], "ymax": 0.0, "ymin": 0.0, "yvar": "", "zlabels": [], "zmax": 0.0, "zmin": 0.0, "zvar": ""}'
      self.assertEqual(json.loads(check), json.loads(true))

   def test_1D_labelsY( self ):
      check = defineHistogram('var', ylabels=["bin0", "bin1"])
      true = '{"alias": "var", "allvars": ["var"], "convention": "", "opt": "", "path": "", "title": "var", "type": "TH1F", "weight": "", "xarray": [], "xbins": 100, "xlabels": [], "xmax": 1, "xmin": 0, "xvar": "var", "yarray": [], "ybins": 2, "ylabels": ["bin0", "bin1"], "ymax": 0.0, "ymin": 0.0, "yvar": "", "zlabels": [], "zmax": 0.0, "zmin": 0.0, "zvar": ""}'
      self.assertEqual(json.loads(check), json.loads(true))

   def test_2D( self ):
      check = defineHistogram('varX,varY', type='TH2F', xbins=10, xmin=0.0, xmax=10.0, ybins=40, ymin=0.0, ymax=20.0)
      true = '{"alias": "varX_vs_varY", "allvars": ["varX", "varY"], "convention": "", "opt": "", "path": "", "title": "varX,varY", "type": "TH2F", "weight": "", "xarray": [], "xbins": 10, "xlabels": [], "xmax": 10.0, "xmin": 0.0, "xvar": "varX", "yarray": [], "ybins": 40, "ylabels": [], "ymax": 20.0, "ymin": 0.0, "yvar": "varY", "zlabels": [], "zmax": 0.0, "zmin": 0.0, "zvar": ""}'
      self.assertEqual(json.loads(check), json.loads(true))

   def test_2D_array( self ):
      check = defineHistogram('varX,varY', 'TH2F', xbins=[0,1,2], ybins=[1,2,3,7])
      true = '{"alias": "varX_vs_varY", "allvars": ["varX", "varY"], "convention": "", "opt": "", "path": "", "title": "varX,varY", "type": "TH2F", "weight": "", "xarray": [0, 1, 2], "xbins": 2, "xlabels": [], "xmax": 1, "xmin": 0, "xvar": "varX", "yarray": [1, 2, 3, 7], "ybins": 3, "ylabels": [], "ymax": 0.0, "ymin": 0.0, "yvar": "varY", "zlabels": [], "zmax": 0.0, "zmin": 0.0, "zvar": ""}'
      self.assertEqual(json.loads(check), json.loads(true))

   def test_2D_labelsXY( self ):
      check = defineHistogram('varX,varY', 'TH2F', xlabels=["bin0", "bin1"], ylabels=["bin0", "bin1", "bin2"])
      true = '{"alias": "varX_vs_varY", "allvars": ["varX", "varY"], "convention": "", "opt": "", "path": "", "title": "varX,varY", "type": "TH2F", "weight": "", "xarray": [], "xbins": 2, "xlabels": ["bin0", "bin1"], "xmax": 1, "xmin": 0, "xvar": "varX", "yarray": [], "ybins": 3, "ylabels": ["bin0", "bin1", "bin2"], "ymax": 0.0, "ymin": 0.0, "yvar": "varY", "zlabels": [], "zmax": 0.0, "zmin": 0.0, "zvar": ""}'
      self.assertEqual(json.loads(check), json.loads(true))

   def test_3D( self ):
      check = defineHistogram('varX,varY,varZ', 'TProfile2D',
         xbins=10, xmin=0.0, xmax=10.0, ybins=40, ymin=0.0, ymax=20.0, zmin=-1.0, zmax=1.0)
      true = '{"alias": "varX_vs_varY_vs_varZ", "allvars": ["varX", "varY", "varZ"], "convention": "", "opt": "", "path": "", "title": "varX,varY,varZ", "type": "TProfile2D", "weight": "", "xarray": [], "xbins": 10, "xlabels": [], "xmax": 10.0, "xmin": 0.0, "xvar": "varX", "yarray": [], "ybins": 40, "ylabels": [], "ymax": 20.0, "ymin": 0.0, "yvar": "varY", "zlabels": [], "zmax": 1.0, "zmin": -1.0, "zvar": "varZ"}'
      self.assertEqual(json.loads(check), json.loads(true))

   def test_efficiency( self ):
      check = defineHistogram('var,pass', type='TEfficiency')
      true = '{"alias": "var_vs_pass", "allvars": ["var", "pass"], "convention": "", "opt": "", "path": "", "title": "var,pass", "type": "TEfficiency", "weight": "", "xarray": [], "xbins": 100, "xlabels": [], "xmax": 1, "xmin": 0, "xvar": "var", "yarray": [], "ybins": 0.0, "ylabels": [], "ymax": 0.0, "ymin": 0.0, "yvar": "pass", "zlabels": [], "zmax": 0.0, "zmin": 0.0, "zvar": ""}'
      self.assertEqual(json.loads(check), json.loads(true))

   def test_offlineNamingConvention( self ):
      check = defineHistogram('var', path='EXPERT', convention='OFFLINE:lowStat')
      true = '{"alias": "var", "allvars": ["var"], "convention": "OFFLINE:lowStat", "opt": "", "path": "EXPERT", "title": "var", "type": "TH1F", "weight": "", "xarray": [], "xbins": 100, "xlabels": [], "xmax": 1, "xmin": 0, "xvar": "var", "yarray": [], "ybins": 0.0, "ylabels": [], "ymax": 0.0, "ymin": 0.0, "yvar": "", "zlabels": [], "zmax": 0.0, "zmin": 0.0, "zvar": ""}'
      self.assertEqual(json.loads(check), json.loads(true))

   def test_enforcePath( self ):
      from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
      athenaCommonFlags.isOnline = True
      with self.assertRaises(AssertionError):
         defineHistogram('var', 'TH1F')

   def test_enforceType( self ):
      from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
      athenaCommonFlags.isOnline = True
      check = defineHistogram('var,pass', 'TEfficiency')
      self.assertIs(check, '')

if __name__ == '__main__':
   unittest.main()
