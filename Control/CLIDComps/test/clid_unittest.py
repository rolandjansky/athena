#!/usr/bin/env python
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

## @file clid_unittest.py
# @brief Unit tests for clidGenerator
"""Unit tests for clidGenerator."""


import os, sys, unittest, subprocess
from CLIDComps.clidGenerator import clidGenerator


## CLIDSvc test fixture
class TestFixture( unittest.TestCase ):
   def setUp( self ):
      pass

   def tearDown( self ):
      self.cgen.cleardb()


## test existence and validity of site specific information 
class CLIDTestCase( TestFixture ):
   def test1ClidDB( self ):
      """Testing clidGenerator with Gaudi_clid.db"""
      self.cgen =  clidGenerator("Gaudi_clid.db")
      self.assertEqual( 1, self.cgen.genClidFromName("DataObject") )
      self.assertEqual( "DataObject", self.cgen.getNameFromClid(1) )

   def test2GenClids( self ):
      """Testing clidGenerator with no clid.db"""
      # get rid of expected error message
      sys.stdout = open("dummyIgnore.txt", 'w')
      self.cgen = clidGenerator("NOTTHERE.db")
      sys.stdout.close()
      sys.stdout = sys.__stdout__
      os.remove("dummyIgnore.txt");
      
      self.assertEqual( 245732527, self.cgen.genClidFromName("DataObject") )
      self.assertEqual( 205083834, self.cgen.genClidFromName("Data<Foo23_45___Bar_, dsfl__>") )

## test the 'clid' command line script
class CLIDScriptTest( unittest.TestCase ):
   def testScript( self ):
      self.assertEqual( 245732527, int(subprocess.check_output(['clid','-s','DataObject'])))
      self.assertEqual( subprocess.check_output(['clid','-sc','DataObject']),
                        subprocess.check_output(['clid','-s','CondCont<DataObject>']) )


## run tests if in standalone mode
if __name__ == '__main__':
   unittest.main()

