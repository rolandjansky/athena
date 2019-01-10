#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# Author: Wim Lavrijsen (LBNL, WLavrijsen@lbl.gov)

"""Unit tests for verifying setting of Gaudi/Athena configurables."""

import unittest, sys

import AthenaCommon.Configurable as Configurable
import AthenaCommon.ConfigurableDb as ConfigurableDb

### data ---------------------------------------------------------------------
__version__ = '1.0.0'
__author__  = 'Wim Lavrijsen (WLavrijsen@lbl.gov)'

__all__ = [ 'BasicConfigurableTestCase',
            'BasicConfigurableDbTestCase',
            'KeywordHandlingTestCase' ]


### basic behavior of Configurable & friends =================================
class BasicConfigurableTestCase( unittest.TestCase ):
   """Verify basic behavior of Configurable and derived to be as expected"""

   def test1MethodChecking( self ):
      """Test that needed methods are properly checked"""

      try:
         class MyIncompleteConfigurable( Configurable.Configurable ):
            pass
      except NotImplementedError:
         pass
      else:
         self.fail( "NotImplementedError exception not raised" )


class BasicConfigurableDbTestCase( unittest.TestCase ):
   """Verify basic behavior of the Configurable DB to be as expected"""

   def test1LoadAutoConfigurable( self ):
      """Test autoloading of a configurable (w/ getConfigurable)"""

    # retrieve a configurable
      conf = ConfigurableDb.getConfigurable( 'StoreGateSvc' )

    # now make sure it is what we think it is
      from StoreGate.StoreGateConf import StoreGateSvc
      self.assertEqual( conf, StoreGateSvc )

   def test2LoadAutoConfigurable( self ):
      """Test autoloading of a configurable (w/ CfgMgr)"""

      from AthenaCommon import CfgMgr
      
    # retrieve a configurable
      conf = CfgMgr.StoreGateSvc

    # now make sure it is what we think it is
      from StoreGate.StoreGateConf import StoreGateSvc
      self.assertEqual( conf, StoreGateSvc )


### basic behavior of Configurable & friends =================================
class KeywordHandlingTestCase( unittest.TestCase ):
   """Regression test on keyword handling"""

   def test1ConstructorKeywords( self ):
      """Test proper handling of constructor keywords"""

      from AthenaCommon.Constants import ERROR, FATAL
      from GaudiSvc.GaudiSvcConf import THistSvc

      name = 'MyHistSvc'

      th1 = THistSvc( name = name, OutputLevel = FATAL )
      self.assertEqual( th1.name(), name )
      self.assertEqual( th1.OutputLevel, FATAL )

    # used to fail w/ AttributeError due to incorrect handling of KeyWords
      th2 = THistSvc( name = name, OutputLevel = ERROR )
      self.assertEqual( th2.name(), name )
      self.assertEqual( th2.OutputLevel, ERROR )

### pickling configuration capabilities =================================
class ConfigurablePersistencyTestCase( unittest.TestCase ):
   """Regression test on configurables persistency"""

   def test1PicklePersistency( self ):
      """Test r/w a job from/to a pickle file"""

      import atexit
      pickleName = "test1PicklePersistency.pickle"
      asciiFileName = pickleName.replace('.pickle','.ascii')
      thistSvcOutput = [ "rec DATAFILE='test1PicklePersistency.root' TYP='ROOT' OPT='RECREATE'" ]

      def _cleanup_files():
         import os
         ## MacOSX silently appends the .db suffix to shelve files. I haven't found a
         ## mechanism for disabling this or for determining the actual filename, 
         ## which is why I've added the MacOSX-specific hack.
         from sys import platform
         if platform != 'darwin' :
             os.unlink( pickleName )
         else:
            os.unlink( pickleName+'.db' )            
         return
         os.unlink( asciiFileName )
         os.unlink( asciiFileName+'.ref' )
         pass
      atexit.register( _cleanup_files )
      
      job = """
      from AthenaCommon import CfgMgr
      from AthenaCommon.AppMgr import theApp

      ## testing algorithm sequence
      from AthenaCommon.AlgSequence import AlgSequence
      job = AlgSequence()
      job += CfgMgr.AthenaOutputStream('MyOutputStream')
      job += CfgMgr.AthenaConditionStream ('MyConditionStream')

      ## service manager
      from AthenaCommon.AppMgr import ServiceMgr as svcMgr
      svcMgr += CfgMgr.THistSvc( 'MyTHistSvc' )
      svcMgr.MyTHistSvc.Output = %(THistSvcOutput)s

      ## job properties
      from AthenaCommon.AthenaCommonFlags import jobproperties as jp
      jp.AthenaCommonFlags.EvtMax = 42
      theApp.EvtMax = jp.AthenaCommonFlags.EvtMax()
      
      ## now store the configuration as a pickle
      from AthenaCommon.ConfigurationShelve import saveToPickle, saveToAscii
      saveToPickle('%(OutputPickleFileName)s')
      saveToAscii ('%(AsciiFileName)s.ref')
      """ % {
         'THistSvcOutput'       : thistSvcOutput,
         'OutputPickleFileName' : pickleName,
         'AsciiFileName'        : asciiFileName,
         }
      
      import commands, os, tempfile
      sc,app = commands.getstatusoutput('which python')
      self.failUnless(sc==0, "Could not find 'python' exe !")

      jobO = tempfile.NamedTemporaryFile(suffix='.py')
      jobO.writelines([l.strip()+os.linesep for l in job.splitlines()])
      jobO.flush()

      ## run the job
      cmd = ' '.join([app, jobO.name])
      sc,out = commands.getstatusoutput(cmd)
      self.failUnless(sc==0, "Problem running py-script:\n%s" % out)
      jobO.close()
      del jobO
      ## check needed files have been produced
      ## MacOSX silently appends the .db suffix to shelve files. I haven't found a
      ## mechanism for disabling this or for determining the actual filename, 
      ## which is why I've added the MacOSX-specific hack.
      from sys import platform
      if platform != 'darwin' :
          self.failUnless(os.path.exists(pickleName))
      else:
          self.failUnless(os.path.exists(pickleName+'.db'))
      self.failUnless(os.path.exists(asciiFileName+'.ref'))
      
      ## load what we have stored...
      from AthenaCommon.ConfigurationShelve import loadFromPickle
      loadFromPickle(pickleName)

      from AthenaCommon.AlgSequence import AlgSequence
      topSequence = AlgSequence()
      self.failUnless(hasattr(topSequence, 'MyOutputStream'),
                      "topSequence is missing 'MyOutputStream' !")
      self.failUnless(hasattr(topSequence, 'MyConditionStream' ),
                      "topSequence is missing 'MyConditionStream' !")

      from AthenaCommon.AppMgr import ServiceMgr as svcMgr
      self.failUnless(hasattr(svcMgr, 'MyTHistSvc'),
                      "svcMgr is missing 'MyTHistSvc' !")
      self.failUnless(svcMgr.MyTHistSvc.Output == thistSvcOutput)

      ## create a job loading the pickle
      job = """
      from AthenaCommon.ConfigurationShelve import loadFromPickle, saveToAscii
      loadFromPickle('%(PickleFileName)s')

      ## test algorithm sequence
      from AthenaCommon.AlgSequence import AlgSequence
      topSequence = AlgSequence()
      assert(hasattr(topSequence, 'MyOutputStream'),
             'topSequence is missing MyOutputStream !')
      assert(hasattr(topSequence, 'MyConditionStream' ),
             'topSequence is missing MyConditionStream !')
      
      ## test service manager
      from AthenaCommon.AppMgr import ServiceMgr as svcMgr
      assert( hasattr(svcMgr, 'MyTHistSvc'), 'svcMgr is missing MyTHistSvc !' )
      assert( svcMgr.MyTHistSvc.Output == %(THistSvcOutput)s )

      ## test job properties
      from AthenaCommon.JobProperties import jobproperties as jp
      assert(jp.AthenaCommonFlags.EvtMax == 42)

      saveToAscii('%(AsciiFileName)s')
      """ % {
         'THistSvcOutput' : thistSvcOutput,
         'PickleFileName' : pickleName,
         'AsciiFileName'  : asciiFileName,
         }
      jobO = tempfile.NamedTemporaryFile(suffix='.py')
      jobO.writelines([l.strip()+os.linesep for l in job.splitlines()])
      jobO.flush()

      ## run job from pickle
      cmd = ' '.join([app, jobO.name])
      sc,out = commands.getstatusoutput(cmd)
      self.failUnless(sc==0, "Problem running py-script:\n%s" % out)
      jobO.close()
      del jobO

      ## check needed files have been produced
      self.failUnless(os.path.exists(asciiFileName))

      ## test hysteresis...
      cmd = 'diff -u %s.ref %s' % (asciiFileName, asciiFileName)
      sc,out = commands.getstatusoutput(cmd)
      self.failUnless(sc==0, "ASCII files are NOT identical:\n%s"%out)
      return

   def test2ConfigurableShelveConfigs( self ):
      """Test config names of shelves"""

      import atexit
      pickleName = "test2ConfigurableShelveConfigs.pickle"
      asciiFileName = pickleName.replace('.pickle','.ascii')
      thistSvcOutput1 = [ "rec DATAFILE='test2ConfigurableShelveConfigs.root' TYP='ROOT' OPT='RECREATE'" ]
      thistSvcOutput2 = [ "rec DATAFILE='new_test2ConfigurableShelveConfigs.root' TYP='ROOT' OPT='RECREATE'" ]

      def _cleanup_files():
         import os
         ## MacOSX silently appends the .db suffix to shelve files. I haven't found a
         ## mechanism for disabling this or for determining the actual filename, 
         ## which is why I've added the MacOSX-specific hack.
         from sys import platform
         if platform != 'darwin' :
             os.unlink( pickleName )
         else:
            os.unlink( pickleName+'.db' )            
         return
      atexit.register( _cleanup_files )
      
      job = """
      from AthenaCommon import CfgMgr
      from AthenaCommon.AppMgr import theApp
      from AthenaCommon.AlgSequence import AlgSequence
      job = AlgSequence()
      job += CfgMgr.AthenaOutputStream('MyOutputStream')
      job += CfgMgr.AthenaConditionStream ('MyConditionStream')

      from AthenaCommon.AppMgr import ServiceMgr as svcMgr
      svcMgr += CfgMgr.THistSvc( 'MyTHistSvc' )
      svcMgr.MyTHistSvc.Output = %(THistSvcOutput1)s

      ## now store the configuration as a pickle
      from AthenaCommon.ConfigurationShelve import saveToPickle, saveToAscii
      saveToPickle('%(OutputPickleFileName)s', cfgName='wholeCfg')

      ## remove one alg
      del job.MyOutputStream

      ## change MyTHistSvc output
      svcMgr.MyTHistSvc.Output = %(THistSvcOutput2)s

      ## save changes to another configuration (in the same pickle file)
      saveToPickle('%(OutputPickleFileName)s', cfgName='smallCfg')
      """ % {
         'THistSvcOutput1'      : thistSvcOutput1,
         'THistSvcOutput2'      : thistSvcOutput2,
         'OutputPickleFileName' : pickleName,
         }
      
      import commands, os, tempfile
      sc,app = commands.getstatusoutput('which python')
      self.failUnless(sc==0, "Could not find 'python' exe !")

      jobO = tempfile.NamedTemporaryFile(suffix='.py')
      jobO.writelines([l.strip()+os.linesep for l in job.splitlines()])
      jobO.flush()

      ## run the job
      cmd = ' '.join([app, jobO.name])
      sc,out = commands.getstatusoutput(cmd)
      self.failUnless(sc==0, "Problem running py-script:\n%s" % out)
      jobO.close()
      del jobO

      ## check needed files have been produced
      ## MacOSX silently appends the .db suffix to shelve files. I haven't found a
      ## mechanism for disabling this or for determining the actual filename, 
      ## which is why I've added the MacOSX-specific hack.
      from sys import platform
      if platform != 'darwin' :
          self.failUnless(os.path.exists(pickleName))
      else:
          self.failUnless(os.path.exists(pickleName+'.db'))
      
      ## create a job loading the pickle (smallCfg)
      job = """
      from AthenaCommon.ConfigurationShelve import loadFromPickle, saveToAscii
      loadFromPickle('%(PickleFileName)s', cfgName='smallCfg')

      from AthenaCommon.AppMgr import ServiceMgr as svcMgr
      assert( hasattr(svcMgr, 'MyTHistSvc'), 'svcMgr is missing MyTHistSvc !' )
      assert( svcMgr.MyTHistSvc.Output == %(THistSvcOutput)s )

      from AthenaCommon.AlgSequence import AlgSequence
      job = AlgSequence()
      assert(     hasattr(job, 'MyConditionStream'),
              'topSequence is missing MyConditionStream !')

      assert( not hasattr(job, 'MyOutputStream'),
              'topSequence has MyOutputStream (and it should NOT) !')
      """ % {
         'THistSvcOutput' : thistSvcOutput2,
         'PickleFileName' : pickleName,
         }
      jobO = tempfile.NamedTemporaryFile(suffix='.py')
      jobO.writelines([l.strip()+os.linesep for l in job.splitlines()])
      jobO.flush()

      ## run job from pickle
      cmd = ' '.join([app, jobO.name])
      sc,out = commands.getstatusoutput(cmd)
      self.failUnless(sc==0, "Problem running py-script:\n%s" % out)
      jobO.close()
      del jobO

      ## create a job loading the pickle (wholeCfg)
      job = """
      from AthenaCommon.ConfigurationShelve import loadFromPickle, saveToAscii
      loadFromPickle('%(PickleFileName)s', cfgName='wholeCfg')

      from AthenaCommon.AppMgr import ServiceMgr as svcMgr
      assert( hasattr(svcMgr, 'MyTHistSvc'), 'svcMgr is missing MyTHistSvc !' )
      assert( svcMgr.MyTHistSvc.Output == %(THistSvcOutput)s )

      from AthenaCommon.AlgSequence import AlgSequence
      job = AlgSequence()
      assert(hasattr(job, 'MyOutputStream'),
             'topSequence is missing MyOutputStream !')

      assert(hasattr(job, 'MyConditionStream'),
             'topSequence is missing MyConditionStream !')
      """ % {
         'THistSvcOutput' : thistSvcOutput1,
         'PickleFileName' : pickleName,
         }
      jobO = tempfile.NamedTemporaryFile(suffix='.py')
      jobO.writelines([l.strip()+os.linesep for l in job.splitlines()])
      jobO.flush()

      ## run job from pickle
      cmd = ' '.join([app, jobO.name])
      sc,out = commands.getstatusoutput(cmd)
      self.failUnless(sc==0, "Problem running py-script:\n%s" % out)
      jobO.close()
      del jobO

      return

### Equality comparisons of Configurable & friends =================================
from GaudiKernel.GaudiHandles import PrivateToolHandle, PrivateToolHandleArray

class DummyAlg( Configurable.ConfigurableAlgorithm ):
   __slots__ = { 
      'DummyIntProp' : 0, # int
      'DummyBoolProp' : True, # bool
      'DummyFloatProp' : 3.141, # float
      'DummyStringProp' : 'Mellon', # string
      'DummyToolHProp' : PrivateToolHandle('DummyToolA/DummyTool1'), # GaudiHandle
      'DummyToolHArrayProp' : PrivateToolHandleArray([]), # GaudiHandleArray
      }
   def __init__(self, name = Configurable.Configurable.DefaultName, **kwargs):
      super(DummyAlg, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
   def getDlls( self ):
      return 'AthenaCommon'
   def getType( self ):
      return 'DummyAlg'
   pass # class DummyAlg

class DummyToolA( Configurable.ConfigurableAlgTool ):
   __slots__ = { 
      'DummyIntProp' : 1, # int
      'DummyBoolProp' : False, # bool
      'DummyFloatProp' : 1.414, # float
      'DummyStringProp' : 'Kawan', # string
      'DummyToolHProp' : PrivateToolHandle('DummyToolB/DummyTool1'), # GaudiHandle
      }
   def __init__(self, name = Configurable.Configurable.DefaultName, **kwargs):
      super(DummyToolA, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
   def getDlls( self ):
      return 'AthenaCommon'
   def getType( self ):
      return 'DummyToolA'
   pass # class DummyToolA

class DummyToolB( Configurable.ConfigurableAlgTool ):
   __slots__ = { 
      'DummyIntProp' : 2, # int
      'DummyBoolProp' : True, # bool
      'DummyFloatProp' : 2.681, # float
      'DummyStringProp' : 'Rakan', # string
      }
   def __init__(self, name = Configurable.Configurable.DefaultName, **kwargs):
      super(DummyToolB, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
   def getDlls( self ):
      return 'AthenaCommon'
   def getType( self ):
      return 'DummyToolB'
   pass # class DummyToolB

class ConfigurableEqualityTestCase( unittest.TestCase ):
   """Verify behavior of Configurable equality comparisons"""

   # In the pre-Run3 behaviour, the same instance was always returned.
   def setUp(self):
      Configurable.Configurable.configurableRun3Behavior = True
      pass

   def tearDown(self):
      Configurable.Configurable.configurableRun3Behavior = False
      pass

   def test1EqualityIsReflexive( self ):
      """Test that x == x"""
      myDummyAlg = DummyAlg("MyDummyAlg")
      self.assertEqual( myDummyAlg, myDummyAlg )

   def test2EqualityIsSymmetric( self ):
      """Test that x == y and y == x"""
      myDummyAlg1 = DummyAlg("MyDummyAlg")
      myDummyAlg2 = DummyAlg("MyDummyAlg")

      # First and second instances should not be identical
      self.assertFalse( myDummyAlg1 is myDummyAlg2 )
      # However, they should be equal
      self.assertEqual( myDummyAlg1, myDummyAlg2 )
      self.assertEqual( myDummyAlg2, myDummyAlg1 )

   def test3InequalityWithoutChildren( self ):
      """Test that configurables with different properties
         compare non-equal without recursion into children"""
      myDummyTool1 = DummyToolB("MyDummyToolB")
      myDummyTool2 = DummyToolB("MyDummyToolB",DummyIntProp=-1)
      self.assertTrue( myDummyTool1 != myDummyTool2 )
      self.assertTrue( myDummyTool2 != myDummyTool1 )

   def test4InequalityWithChildren( self ):
      """Test that configurables with different properties
         compare non-equal with recursion into children"""
      myDummyAlg1 = DummyAlg("MyDummyAlg")
      myDummyAlg2 = DummyAlg("MyDummyAlg")
      myDummyAlg2.DummyToolHProp = DummyToolB("MyDummyToolB")
      self.assertTrue( myDummyAlg1 != myDummyAlg2 )
      self.assertTrue( myDummyAlg2 != myDummyAlg1 )

## actual test run
if __name__ == '__main__':
   loader = unittest.TestLoader()
   testSuite = loader.loadTestsFromModule( sys.modules[ __name__ ] )

   runner = unittest.TextTestRunner( verbosity = 2 )
   result = not runner.run( testSuite ).wasSuccessful()

 # don't want to depend on gaudimodule
   assert( not 'gaudimodule' in sys.modules )

   sys.exit( result )
