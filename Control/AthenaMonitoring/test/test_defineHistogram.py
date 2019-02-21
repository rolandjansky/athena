#!/usr/bin/env python
#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
import unittest

from AthenaMonitoring.GenericMonitoringTool import defineHistogram

class Test( unittest.TestCase ):
   def test_1D( self ):
      s = defineHistogram('var', 'TH1F', 'EXPERT', 'title', 10, 0.0, 10.0)
      self.assertEqual(s, 'EXPERT, TH1F, var, title, 10, 0.000000, 10.000000')

   def test_1D_label( self ):
      s = defineHistogram('var', 'TH1F', 'EXPERT', 'title', 10, 0.0, 10.0, labels=['a','b'])
      self.assertEqual(s, 'EXPERT, TH1F, var, title, 10, 0.000000, 10.000000, a:b:')

   def test_1D_opt( self ):
      s = defineHistogram('var', 'TH1F', 'EXPERT', 'title', 10, 0.0, 10.0, opt='myopt')
      self.assertEqual(s, 'EXPERT, TH1F, var, title, 10, 0.000000, 10.000000, myopt')

   def test_2D( self ):
      s = defineHistogram('var1,var2', 'TH2F', 'EXPERT', 'title', 10, 0.0, 10.0, 20, 0.0, 20.0)
      self.assertEqual(s, 'EXPERT, TH2F, var1,var2, title, 10, 0.000000, 10.000000, 20, 0.000000, 20.000000')

   def test_3D( self ):
      s = defineHistogram('var1,var2,var3', 'TProfile2D', 'EXPERT', 'title', 10, 0.0, 10.0, 20, 0.0, 20.0, -1.0, 1.0)
      self.assertEqual(s, 'EXPERT, TProfile2D, var1,var2,var3, title, 10, 0.000000, 10.000000, 20, 0.000000, 20.000000, -1.000000, 1.000000')

   def test_enforcePath( self ):
      with self.assertRaises(AssertionError):
         defineHistogram('var', 'TH1F')

   def test_labelType( self ):
      with self.assertRaises(AssertionError):
         defineHistogram('var', 'TH1F', path='EXPERT', labels='l1:l2')

if __name__ == '__main__':
   unittest.main()
