#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# Author: Paolo Calafiura, Wim Lavrijsen

"""Unit tests for verifying setting of global JobProperties setting."""

import unittest, sys



### data ---------------------------------------------------------------------
__version__ = '1.0.0'
__author__  = 'Wim Lavrijsen (WLavrijsen@lbl.gov)'

__all__ = [ 'JobPropertiesTestCase' ]


class JobPropertiesTestCase( unittest.TestCase ):

   def test1BasicCreation( self ):
      """Test proper behavior of property creation"""
      from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
      from AthenaCommon.JobProperties import jobproperties

      class MyProperty(JobProperty) :
          statusOn     = False     # flag is unactivated by default. 
          allowedTypes = ['bool']  # type is a bool
          StoredValue  = False   # default value is False

      class MyProperties(JobPropertyContainer):
         pass
      #create container and add property
      jobproperties.add_Container(MyProperties)
      jobproperties.MyProperties.add_JobProperty(MyProperty)

   def test2BasicAccess( self ):
      """Test proper behavior of property access"""
      from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
      from AthenaCommon.JobProperties import jobproperties
      self.assertFalse( jobproperties.MyProperties.MyProperty() )      
      jobproperties.MyProperties.MyProperty=True 
      self.assertTrue( jobproperties.MyProperties.MyProperty() )      

   def test3AccessUnset( self ):
      """Test proper behavior when accessing JobProperty with no StoredValue"""
      from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
      from AthenaCommon.JobProperties import jobproperties
      class MyUnsetProperty(JobProperty) :
          statusOn     = False     # flag is unactivated by default. 
          allowedTypes = ['bool']  # type is a bool
          #StoredValue  = False   leave unset
      jobproperties.MyProperties.add_JobProperty(MyUnsetProperty)
      self.assertEqual( jobproperties.MyProperties.MyUnsetProperty(), None )      



## actual test run
if __name__ == '__main__':
   loader = unittest.TestLoader()
   testSuite = loader.loadTestsFromModule( sys.modules[ __name__ ] )

   runner = unittest.TextTestRunner( verbosity = 2 )
   result = not runner.run( testSuite ).wasSuccessful()

   sys.exit( result )
