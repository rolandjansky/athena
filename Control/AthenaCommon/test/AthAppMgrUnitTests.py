#!/usr/bin/env python

# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
# Author: Sebastien Binet (binet@cern.ch)

"""Unit tests for verifying basic features of AthAppMgr."""

import unittest

from AthenaCommon.AppMgr import AthAppMgr

### data ---------------------------------------------------------------------
__version__ = '$Revision: 1.1 $'
__author__  = 'Sebastien Binet (binet@cern.ch)'

__all__ = [
    'BasicAthAppMgrTestCase',
    ]

import sys
import os

### helper class to sanitize output-------------------------------------------
from tempfile import NamedTemporaryFile
class ShutUp(object):
    """
    A little helper class to keep GAUDI app mgr silent...
    """
    def __init__(self):
        self.save  = file( '/dev/null', 'w' )
        self.quiet = NamedTemporaryFile( suffix = ".msg.log" )
        os.dup2( sys.stdout.fileno(), self.save.fileno() )
        return
    
    def mute(self):
        os.dup2( self.quiet.fileno(), sys.stdout.fileno() )
        return
    
    def unMute(self):
        os.dup2( self.save.fileno(), sys.stdout.fileno() )
        return

class Cout:
    instance = ShutUp()
    
### basic behaviour of AthAppMgr -------------------------------------------
class BasicAthAppMgrTestCase( unittest.TestCase ):
    """Verify basic behaviour of AthAppMgr to be as expected"""

    def test1NamedSingleton( self ):
        """Test that instances w/ same name are the same"""

        app1 = AthAppMgr( "MyApp" )
        app2 = AthAppMgr( "MyApp" )
        self.assertTrue( app1 == app2, "instances are not equal !" )
        self.assertTrue( app1 is app2, "instances are not identical !" )

    def test2CppAppMgr( self ):
        """Test communication b/w Py-proxy and C++ app mgr"""
        app = AthAppMgr( "MyApp" )
        createSvc = app.CreateSvc[:]
        app.CreateSvc += [ "StoreGateSvc/abcde" ]
        self.assertTrue( app.CreateSvc == createSvc + [ "StoreGateSvc/abcde" ] )

        Cout.instance.mute()
        ## instantiates the C++ application mgr (un-configured)
        cppApp = app.getHandle()
        Cout.instance.unMute()

        self.assertTrue( app._cppApp is not None )

        self.assertTrue( cppApp.CreateSvc == [] )

        app.CreateSvc += [ "StoreGateSvc/svcA" ]
        self.assertTrue( cppApp.CreateSvc == [ "StoreGateSvc/svcA" ] )

        ## configure the application (not just the app mgr)
        app.setup()
        self.assertTrue( cppApp.CreateSvc == app.CreateSvc )

        # really want to test that ?
        self.assertTrue( cppApp.CreateSvc[0] == "ToolSvc/ToolSvc" )

        app.CreateSvc += [ "StoreGateSvc/svcB" ]
        self.assertTrue( cppApp.CreateSvc == app.CreateSvc )

        app.CreateSvc = [ "StoreGateSvc/svcC" ]
        self.assertTrue( app.CreateSvc    == [ "StoreGateSvc/svcC" ] )
        self.assertTrue( cppApp.CreateSvc == [ "StoreGateSvc/svcC" ] )
        
        ## test all properties are synchronized b/w py-proxy & C++
        for p in AthAppMgr.__slots__:
            self.assertTrue( getattr(cppApp,p) == getattr(app,p),
                             "Property [%s] DIFFERS !!" % str(p) )

        ## test that multiple calls to setup() are stable
        Cout.instance.mute()
        app.setup()
        Cout.instance.unMute()
        createSvc = app.CreateSvc[:]
        for i in range(10):
            Cout.instance.mute()
            app.setup()
            Cout.instance.unMute()
            self.assertTrue( createSvc == app.CreateSvc[:] )
            pass
        return

    def test3ToolSvc( self ):
        """Test ToolSvc __iadd__"""
        from AthenaCommon.AppMgr import ToolSvc
        from AthenaCommon.Configurable import ConfigurableAlgTool, ConfigurableService

        t1 = ConfigurableAlgTool('tool1')
        t2 = ConfigurableAlgTool('tool2')
        s1 = ConfigurableService('svc1')

        ## Check silent adding of duplicate
        ToolSvc += t1
        self.assertEqual( ToolSvc.getChildren(), [t1] )
        ToolSvc += t1
        self.assertEqual( ToolSvc.getChildren(), [t1] )
        ToolSvc += [t1,t2]
        self.assertEqual( ToolSvc.getChildren(), [t1,t2] )

        ## Check type checking
        with self.assertRaises(TypeError):
            ToolSvc += s1

        with self.assertRaises(TypeError):
            ToolSvc += [s1]

        return


## actual test run
if __name__ == '__main__':
   loader = unittest.TestLoader()
   testSuite = loader.loadTestsFromModule( sys.modules[ __name__ ] )

   runner = unittest.TextTestRunner( verbosity = 2 )
   result = not runner.run( testSuite ).wasSuccessful()

   ## make Gaudi AppMgr silent...
   Cout.instance.mute()
   ## and exit...
   sys.exit( result )
