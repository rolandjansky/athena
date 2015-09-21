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
      job += CfgMgr.THistWrite('MyTHistWriteAlg')
      job += CfgMgr.THistRead ('MyTHistReadAlg')

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
      self.failUnless(hasattr(topSequence, 'MyTHistWriteAlg'),
                      "topSequence is missing 'MyTHistWriteAlg' !")
      self.failUnless(hasattr(topSequence, 'MyTHistReadAlg' ),
                      "topSequence is missing 'MyTHistReadAlg' !")

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
      assert(hasattr(topSequence, 'MyTHistWriteAlg'),
             'topSequence is missing MyTHistWriteAlg !')
      assert(hasattr(topSequence, 'MyTHistReadAlg' ),
             'topSequence is missing MyTHistReadAlg !')
      
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
      job += CfgMgr.THistWrite('MyTHistWriteAlg')
      job += CfgMgr.THistRead ('MyTHistReadAlg')

      from AthenaCommon.AppMgr import ServiceMgr as svcMgr
      svcMgr += CfgMgr.THistSvc( 'MyTHistSvc' )
      svcMgr.MyTHistSvc.Output = %(THistSvcOutput1)s

      ## now store the configuration as a pickle
      from AthenaCommon.ConfigurationShelve import saveToPickle, saveToAscii
      saveToPickle('%(OutputPickleFileName)s', cfgName='wholeCfg')

      ## remove one alg
      del job.MyTHistWriteAlg

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
      assert(     hasattr(job, 'MyTHistReadAlg'),
              'topSequence is missing MyTHistReadAlg !')

      assert( not hasattr(job, 'MyTHistWriteAlg'),
              'topSequence has MyTHistWriteAlg (and it should NOT) !')
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
      assert(hasattr(job, 'MyTHistReadAlg'),
             'topSequence is missing MyTHistReadAlg !')

      assert(hasattr(job, 'MyTHistWriteAlg'),
             'topSequence is missing MyTHistReadAlg !')
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

## actual test run
if __name__ == '__main__':
   loader = unittest.TestLoader()
   testSuite = loader.loadTestsFromModule( sys.modules[ __name__ ] )

   runner = unittest.TextTestRunner( verbosity = 2 )
   result = not runner.run( testSuite ).wasSuccessful()

 # don't want to depend on gaudimodule
   assert( not 'gaudimodule' in sys.modules )

   sys.exit( result )
