#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
## 
# @file DCubeClient/python/DCubeUtils.py
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @brief Utility classes for DCubeApp
#

import xml.dom.minidom
import logging
import os
import sys
from types import *
import unittest

##
# @class Singleton 
# @author Krzysztof Daniel Ciba (Kerzysztof.Ciba@NOSPAMgmail.com)
# @brief Forcing singleton type for python class instance. 
class DCubeSingleton( type ):

    ## c'tor
    # @param cls class instance
    # @param name class name
    # @param bases base classes
    # @param dic parameters dictionary
    def __init__(cls, name, bases, dic):
        super( DCubeSingleton, cls ).__init__( name, bases, dic )
        cls.instance = None

    ## call operator
    # @param cls class instance
    # @param *args unnamed arguments
    # @param **kw keyword arguments
    def __call__( cls, *args, **kw ):
        if cls.instance is None:
            cls.instance = super( DCubeSingleton, cls ).__call__( *args, **kw )
        return cls.instance

##
# @class DCubeLogger 
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAgmail.com)
# @brief DCube customized logger
class DCubeLogger( object ):

    ## metaclass type
    __metaclass__ = DCubeSingleton

    ## reference to original logger instance
    __log = None

    ## name of the log file 
    __logFile = None 
    
    ## default name of the log file
    __defaultLogFile = "./dcube.log"

    ## flag to trigger stdout print outs
    __console = None

    ## logging levels
    __levels = { "CRITICAL": logging.CRITICAL,
                 "ERROR": logging.ERROR, 
                 "WARNING" : logging.WARNING, 
                 "INFO" : logging.INFO,
                 "DEBUG" : logging.DEBUG }

    ## c'tor
    # @brief c'tor
    # @param self "Me, myself and Irene"
    # @param fileName name of log file 
    # @param logName name of logger
    # @param toConsole flag to trigger stdout printout
    def __init__( self, fileName=None, logName="DCube", toConsole=True ):
                
        if ( fileName == None ): fileName = self.__defaultLogFile

        self.__logFile = os.path.abspath( fileName )
      
        logging.basicConfig( level=logging.DEBUG,
                             format='%(name)-12s %(asctime)s %(levelname)-8s: %(message)s',
                             filename=fileName,
                             filemode='w' )
          
        self.toConsole( toConsole )
        
        self.__log = logging.getLogger( str(logName) )

        ## save stdout and stderr handle
        #self.__stdout = sys.stdout
        #self.__stderr = sys.stderr

         
    ## 
    # @brief set logging level
    # @param cls class reference
    # @param level string with level name 
    @classmethod
    def setLevel(cls, level="DEBUG"):
        if ( level in cls.__levels.keys() ):
            cls.__log.setLevel( cls.__levels[level] ) 


    ## 
    # @brief log file name getter
    # @param cls class reference
    # @return name of log file 
    def getFileName( self ):
        return self.__logFile

    ##
    # @brief logger getter
    # @param cls class reference
    # @param caller who call me?
    @classmethod
    def getLogger( cls, caller ):
        logName = caller.__class__.__name__ 
        cls.__log = logging.getLogger( logName )
        return cls

    ## add/remove logging to the stdout and stderr
    # @param cls class reference
    # @param toConsole bool flag [default True]
    @classmethod
    def toConsole( cls, toConsole=True):
        if ( toConsole ):

            if ( not isinstance(cls.__console, logging.StreamHandler)  ):
                cls.__console = logging.StreamHandler()
                cls.__console.setLevel(logging.DEBUG)
                formatter = logging.Formatter('%(name)-12s: %(levelname)-8s %(message)s')
                cls.__console.setFormatter(formatter)

            logging.getLogger('').addHandler( cls.__console )

        else:
            if ( isinstance(cls.__console, logging.StreamHandler)  ):
                logging.getLogger('').removeHandler( cls.__console )
            
    ## 
    # @brief debug level logger 
    # @param cls class reference
    # @param msg message to log
    @classmethod
    def debug( cls, msg ):
        cls.__log.debug( msg )

    ## 
    # @brief info level logger 
    # @param cls class reference
    # @param msg message to log
    @classmethod
    def info( cls, msg ):
        cls.__log.info( msg )

    ## 
    # @brief warning level logger 
    # @param cls class reference
    # @param msg message to log
    @classmethod
    def warn( cls, msg ):
        cls.__log.warning( msg )

    ## 
    # @brief error level logger 
    # @param cls class reference
    # @param msg message to log
    @classmethod
    def error( cls, msg ):
        cls.__log.error( msg )

    ## 
    # @brief critical level logger 
    # @param cls class reference
    # @param msg message to log
    @classmethod
    def panic( cls, msg ):
        cls.__log.critical( msg )

    ## 
    # @brief critical level logger 
    # @param cls class reference
    # @param msg message to log
    @classmethod
    def critical( cls, msg ):
        cls.__log.critical( msg )

    ## 
    # @brief exception level logger 
    # @param cls class reference
    # @param msg message to log
    @classmethod
    def exception( cls, msg ):
        cls.__log.exception( msg )

    ## 
    # @brief exception level logger 
    # @param cls class reference
    # @param msg message to log
    @classmethod
    def epanic( cls, msg ):
        cls.__log.exception( msg )


##
# @class DCubeObject
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @brief base class for all DCube classes
class DCubeObject( DCubeLogger ):
    
    ## c'tor
    # @param self "Me, myself and Irene"
    # @param caller reference to inherited object 
    def __init__( self, caller ):
        ## setting up logger
        self.__log = DCubeLogger.getLogger( caller )

    ## debug level logger
    # @param self "Me, myself and Irene"
    # @param msg logging string
    def debug( self, msg ):
        if ( self.__log ):
            self.__log.getLogger(self).debug( str(msg) )
        else:
            print msg

    ## info level logger
    # @param self "Me, myself and Irene"
    # @param msg logging string
    def info( self, msg  ):
        if ( self.__log ):
            self.__log.getLogger(self).info( str(msg) )
        else:
            print msg
        
    ## warning level logger
    # @param self "Me, myself and Irene"
    # @param msg logging string
    def warn( self, msg ):
        if ( self.__log ):
            self.__log.getLogger(self).warn( str(msg) )
        else:
            print msg
    
    ## error level logger
    # @param self "Me, myself and Irene"
    # @param msg logging string
    def error( self, msg ):
        if ( self.__log ):
            self.__log.getLogger(self).error( str(msg) )
        else:
            print msg

    ## critical level logger
    # @param self "Me, myself and Irene"
    # @param msg logging string
    def panic( self, msg ):
        if ( self.__log ):
            self.__log.getLogger(self).critical( str(msg) )
        else:
            print msg

    ## exception level logger
    # @param self "Me, myself and Irene"
    # @param msg logging string
    def epanic( self, msg ):
        if ( self.__log ):
            self.__log.getLogger(self).epanic( str(msg) )
        else:
            print msg


##
# @class DCubeVersion
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @brief DCube version holder
class DCubeVersion( object ):

    __project = "DCube"
    __version = "4.6692016 ($Rev: 217525 $ $Date$)"
    __author = "Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)"
          
    ## str operator
    # @param self "Me, myself and Irene"
    def __str__( self ):
        return "%s %s by %s" % ( self.__project, self.__version, self.__author )

    ## DCube version getter
    # @param self "Me, myself and Irene"
    def version( self ):
        return str(self)

    ## python version getter 
    # @param self "Me, myself and Irene"
    def python( self ):
        return "using Python %s" % sys.version.replace("\n", "").strip("\n") 

    ## root version getter
    # @param self "Me, myself and Irene"
    def root( self ):
        out = "using ROOT "
        try:
            if ( "ROOT" not in dir() ):
                import ROOT
            out += ROOT.gROOT.GetVersion().strip("\n") 
        except:
            out += "UNKNOWN"
        return out

## 
# @class DCubeException
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAgmail.com)
# @brief class for DCube triggered execution errors 
class DCubeException( Exception ):
    
    ## c'tor
    # @param self "Me, myself and Irene"
    # @param value whatever, who cares, this is python, dumb!  
    def __init__( self, value ):
        self.value = value
   
    ## exception printing
    # @param self "Me, myself and Irene"
    def __str__( self ):
        return repr( self.value )


##
# @class test_DCubeUtils
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @brief test case for DCubeUtils module classes
class test_DCubeUtils( unittest.TestCase ):

    ## test case setup
    # @param self "Me, myself and Irene"
    def setUp( self ):

        self.logger = DCubeLogger( "test_DCubeUtils.log", logName="test_DCubeUtils", toConsole=True )
       
        self.exception = DCubeException("test exception, don't panic")
        self.version = DCubeVersion() 
        self.base = DCubeObject( self )
        self.log = DCubeLogger.getLogger( self )
        pass

    ## c'tors
    # @param self "Me, myself and Irene"
    def test_01_constructors( self ):

        try:
            self.exception = DCubeException("test exception, don't panic!")
            self.version = DCubeVersion() 
            self.base = DCubeObject( self )
            self.log = DCubeLogger.getLogger( self )
        except:
            pass
 
        self.assertEqual( isinstance( self.exception, DCubeException), True )
        self.assertEqual( isinstance( self.version, DCubeVersion), True )
        self.assertEqual( isinstance( self.base, DCubeObject), True )

    ## DCubeLogger interface
    # @param self "me, myself and Irene"
    def test_02_logger( self ):
        self.log.debug("debug level text")
        self.log.info("info level text")
        self.log.warn("warning level text")
        self.log.error("error level text")
        self.log.panic("fatal level text")
        try:
            raise NameError, "intentional NameError, don't panic!"
        except NameError, value:
            self.log.epanic("exception level text")

        self.log.toConsole(False)
        self.log.info("this won't be printed to console")
        self.log.toConsole(True)
        self.log.info("this will be printed to console")

        self.logger.setLevel("INFO")
        self.log.debug("this should not be printed")
        
        self.logger.setLevel("DEBUG")
        self.log.debug("while this should be printed")

    ## DCubeException interface
    # @param self "Me, myslf and Irene"
    def test_03_exception( self ):
        try:
            raise self.exception
        except DCubeException, value:
            self.log.epanic(value)
        
    ## DCubeObject interface
    # @param self "Me, myself and Irene"
    def test_04_DCubeObject( self ):
        self.base.debug("debug level text")
        self.base.info("info level text")
        self.base.warn("warn level text")
        self.base.error("error level text")
        self.base.panic("panic level text")
        try:
            raise NameError, "intentional NameError, don't panic!"
        except NameError, value:
            self.base.epanic("exception level text")
            
    ## DCubeVersion interface
    # @param self "Me, myself and Irene"
    def test_05_DCubeVersion( self ):
        self.log.info( self.version.version() )
        self.log.info( self.version.root() )
        self.log.info( self.version.python() )
        self.log.info( self.version )


## test case execution
if __name__ == "__main__":

    testLoader = unittest.TestLoader()
    suite = testLoader.loadTestsFromTestCase(test_DCubeUtils)      
    unittest.TextTestRunner(verbosity=3).run(suite)
    
