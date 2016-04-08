#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
##
# @file DozerUtils.py
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAmgmail.com)
# @brief collection of helper classes for CoolDozer

import unittest
import sys
sys.argv.append("-b")
try:
    from PyCool import cool, coral
    import ROOT
except ImportError:
    print "no COOL no ROOT"

import traceback

# basic os interface
import os

# regexp 
import re

# logging 
import logging

# CLI arguments parsing
import getopt

# RTTI 
from types import *

# XML DOM processing
import xml.dom.minidom as minidom

# to escape <,>,& and " characters 
# (quotation mark in case the second argument is true)
from cgi import escape as escapeCGI

# date and time juggling
#from time import strftime, localtime, strptime
import datetime
import time 

# import UUID
pyvers = sys.hexversion
noUUID = False
if ( pyvers >= 0x20501f0 ):
    try:
        from uuid import uuid4 as uuid    
    except ImportError:
        noUUID = True
else:
    def uuid():
        return os.popen("uuidgen").readline().strip()
        
## helper function to get uuid as default string 
# @return uuid in form of standard hex string 
def getUUID():
    return str( uuid() )

## escape special XML characters in CDATA or attribute value 
# @param msg a string 
# @brief do escape XML special characters (<,>,&,") 
def escape( msg ):
    return escapeCGI(msg, True)

##
# @class DozerSingleton 
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @brief Forcing singleton type for python class instance. 
class DozerSingleton( type ):

    ## c'tor
    # @param cls class instance
    # @param name class name
    # @param bases base classes
    # @param dic parameters dictionary
    def __init__(cls, name, bases, dic):
        super( DozerSingleton, cls ).__init__( name, bases, dic )
        cls.instance = None

    ## () operator
    # @param cls class instance
    # @param *args unnamed arguments
    # @param **kw keyword arguments
    def __call__( cls, *args, **kw ):
        if cls.instance is None:
            cls.instance = super( DozerSingleton, cls ).__call__( *args, **kw )
        return cls.instance

## helper function to check if file exists 
# @param fname path to file 
# @return True if path exists, False otherwise
def fileExists( fname ):
    path = os.path.abspath( fname )
    if ( os.path.exists( path ) ): return True
    return False

## helper function to get file type string from shell command 'file'
# @param fname path to file
# @return a string with file type description
def fileType( fname ):
    path = os.path.abspath( fname )
    return os.popen("file -b " + path).readline().strip()

## helper function to get mime type string form shell command 'file' 
# @param fname path to file
# @return a string with MIME type 
def getMime( fname ):
    path = os.path.abspath( fname )
    return os.popen("file -i " + path ).readline().strip().split(":")[1].strip()

## 
# @class DozerError 
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @brief  class for exception in CoolDozer execution 
#
# @htmlinclude cool_dozer_api.css
# <h3>Usage</h3>
#
# As any other normal Exception in python.
#
#<div><div class="dozerCode"><div class="dozerCodeLabel"> Example </div>  <div class="dozerCodeText">
#<b><pre>
#[lxplus205] ~/scratch2/CoolDozer/python % python 
#Python 2.5 (r25:51908, Oct 18 2007, 16:04:48) 
#[GCC 3.4.6 20060404 (Red Hat 3.4.6-8)] on linux2
#Type "help", "copyright", "credits" or "license" for more information.
#>>> import DozerUtils
#>>> raise DozerUtils.DozerError(1)
#Traceback (most recent call last):
#  File "<stdin>", line 1, in <module>
#DozerUtils.DozerError: '1'
#>>> raise DozerUtils.DozerError("intentional throw")
#Traceback (most recent call last):
#  File "<stdin>", line 1, in <module>
#DozerUtils.DozerError: 'intentional throw'
#</pre></b>
#</div></div></div> 
#
class DozerError( Exception ):
    
    ## c'tor
    # @brief c'tor
    # @param self "Me, myself and Irene"
    # @param val value thrown within excepton
    def __init__( self, val ):
        self.__value = val
        self.__name__ = "DozerError"

    ## 
    # @brief string representation of exception
    # @param self "Me, myself and Irene"
    def __str__( self ):
        return repr ( str(self.__value) )


##
# @class DozerLogger  
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @brief CoolDozer API customized logger 
#
# @htmlinclude cool_dozer_api.css
# <h3>Usage</h3>
#
# DozerLogger class gives the common interface to python logging module for CoolDozer (or user defined) classes.
# 
# <ul>
# <li> <h4>Constructing</h4>
# 
# <div><div class="dozerInfo"><div class="dozerInfoLabel">Notice</div><div class="dozerInfoText">
# DozerLogger is a singleton. Once constructed it is passed inside CoolDozer API between different
# instances and classes. 
# </div></div></div>
#
# <div><div class="dozerTrick"><div class="dozerTrickLabel"> Trick? </div> <div class="dozerTrickText"> 
# If you want to use CoolDozer logging facility in your aplication, you should create it as soon as possible, 
# e.g. inside your application constructor.   
# </div></div></div> 
#
# If you are using python::DozerApp::DozerApp as a base class for your CoolDozer application, you dont have to 
# construct DozerLogger by yourself. In any other cases you should call its constructor DozerLogger::__init__, e.g.:
# <div><div class="dozerCode"><div class="dozerCodeLabel"> Example </div> <div class="dozerCodeText"> 
#<b><pre>
#Python 2.5.4 (r254:67916, Jan 26 2009, 14:34:54) 
#[GCC 3.4.6 20060404 (Red Hat 3.4.6-10)] on linux2
#Type "help", "copyright", "credits" or "license" for more information.
#>>> from CoolDozer import DozerLogger
#>>> DozerLogger( "logfile.log", "loggerName", True )     # set last argument to False, if you don't need console handler 
#<DozerUtils.DozerLogger object at 0xf7f973ec>          
#>>> class A: pass                                        # dummy class A
#... 
#>>> a = A()                                              # a object
#>>> a.log = DozerLogger.getLogger( a )                   # let's use DozerLogger for this class
#>>> a.log
#<class 'DozerUtils.DozerLogger'>                         # so we have got it
#>>> a.log.info( "test info")                             # let's print some text to info level  
#A           : INFO     test info
#</pre></b>
#</div></div></div>
#
# <li> <h4>Logging</h4>
#
#<div><div class="dozerCode"><div class="dozerCodeLabel"> Example </div> <div class="dozerCodeText"> 
#<b><pre>
#>>> a.log.debug( " test debug ")                     # DEBUG level
#A           : DEBUG     test debug 
#>>> a.log.info( " test info ")                       # INFO level
#A           : INFO      test info 
#>>> a.log.warn( " test warn ")                       # WARNING level
#A           : WARNING   test warn 
#>>> a.log.error( " test error ")                     # ERROR level
#A           : ERROR     test error 
#>>> a.log.panic( " test panic ")                     # CRITICAL level
#A           : CRITICAL  test panic 
#>>> a.log.critical( " test panic once again ")       # and once again
#A           : CRITICAL  test panic once again 
#>>> a.log.epanic( " test exception ")                # this one should be used in exceptions only, will also add traceback (if any)
#A           : ERROR     test exception 
#None
#>>> try: raise AttributeError                        # so let's try, oh no! exception raised
#... except: a.log.epanic( "exception thrown")        # but we catch that one so relax
#... 
#A           : ERROR    exception thrown
#Traceback (most recent call last):
#  File "<stdin>", line 1, in <module>
#AttributeError
#</pre></b>
#</div></div></div>
#  
# <li> <h4>Tweaking</h4>
# <ul> 
#    
# <li> <h5>Setting logging level</h5> 
# You can set the loggin level using DozerLogger.setLevel function:
#
#<div><div class="dozerCode"><div class="dozerCodeLabel"> Example </div> <div class="dozerCodeText"> 
#<b><pre>
#>>> import logging
#>>> a.log.setLevel( logging.WARNING )                      # set the logging level to logging.WARNING
#>>> a.log.info("no output, as level set to WARNING")       # no printout, logging.INFO < logging.WARNING
#>>> a.log.warn("but this one will appear")                 # but higher or equal to logging.WARNING would be seen 
#A           : WARNING  but this one will appear
#</pre></b>
#</div></div></div>
#
# <li> <h5>Logging to console</h5>
#
# You can also toggle logging to console handler by executing DozerLogger.toConsole method, e.g.:
#
#<div><div class="dozerCode"><div class="dozerCodeLabel"> Example </div> <div class="dozerCodeText"> 
#<b><pre>
#>>> a.log.toConsole(False)
#>>> a.log.warn("no output to console")
#>>> a.log.toConsole(True)
#>>> a.log.warn("console logging available once again")
#A           : WARNING  console logging available once again
#</pre></b>
#</div></div></div>
# </ul>
# </ul>
class DozerLogger(object):

    ## metaclass type
    __metaclass__ = DozerSingleton

    ## reference to original logger instance
    __log = None

    ## name of the log file 
    __logFile = None 
    
    ## default name of the log file
    __defaultLogFile = "./CoolDozerLogger.log"

    ## flag to trigger stdout print outs
    __console = None

    ## c'tor
    # @brief c'tor
    # @param self "Me, myself and Irene"
    # @param fileName name of log file 
    # @param logName name of logger
    # @param toConsole flag to trigger stdout printout
    def __init__( self, fileName=None, logName="CoolDozer", toConsole=False  ):
                
        if ( fileName == None ): fileName = self.__defaultLogFile

        self.__logName = logName

        self.__logFile = os.path.abspath( fileName )
      
        logging.basicConfig( level=logging.DEBUG,
                             format='%(name)-20s : %(asctime)s %(levelname)-8s : %(message)s',
                             filename=fileName,
                             filemode='w' )                
        self.toConsole( toConsole )
        
        self.__log = logging.getLogger( str(self.__logName) )

    
    ## logging level setter
    # @param cls class reference
    # @param lvl logging level 
    @classmethod
    def setLevel( cls, lvl = logging.WARNING ):
        logging.getLogger().setLevel( lvl )

    ## 
    # @brief log file name getter
    # @param cls class reference
    # @return name of log file 
    @classmethod
    def getFileName( cls ):
        return cls.__logFile

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
    # @param toConsole bool flag
    @classmethod
    def toConsole( cls, toConsole=False):
        if ( toConsole ):
            if ( not isinstance(cls.__console, logging.StreamHandler)  ):
                cls.__console = logging.StreamHandler()
                cls.__console.setLevel(logging.DEBUG)
                formatter = logging.Formatter('%(name)-12s: %(levelname)-8s %(message)s')
                cls.__console.setFormatter(formatter)

            logging.getLogger('').addHandler(cls.__console)

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
# @class DozerVersion
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @brief CoolDozer API version holder 
#
# @htmlinclude cool_dozer_api.css
#
# This class holds revision number of CoolDozer API, name of author and his email address.  
# It also tries to print above informations in pretty way:
#
#<div><div class="dozerCode" style='background-color: $fbf;'><div class="dozerCodeLabel"> Example </div> <div class="dozerCodeText"> 
#<b><pre>
#Python 2.5.4 (r254:67916, Jan 26 2009, 14:34:54) 
#[GCC 3.4.6 20060404 (Red Hat 3.4.6-10)] on linux2
#Type "help", "copyright", "credits" or "license" for more information.
#>>> import DozerUtils
#>>> v = DozerUtils.DozerVersion()
#>>> v
#<DozerUtils.DozerVersion object at 0xf7eca08c>
#>>> v.project()
#'CoolDozer API'
#>>> v.author()
#'Krzysztof Daniel Ciba'
#>>> v.version()
#'\$Rev: 219420 \$'
#>>> v.mailto()
#"mailto://Krzysztof.Ciba@NOSPAMgmail.com?Subject='Hello Krzysztof'"
#>>> print v
#CoolDozer API version \$Rev: 219420 \$ by Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
#</pre></b>
#</div></div></div>
# 
class DozerVersion(object):

    # version number
    __apiVersion = "$Rev: 300946 $"
    # author name 
    __apiAuthor = "Krzysztof Daniel Ciba"
    # email address
    __apiMailTo = "Krzysztof.Ciba@NOSPAMgmail.com"
    # project name
    __apiName = "CoolDozer API"

    ## str() operator
    # @param self "Me, myself and Irene"
    def __str__( self ):
        return "%s version %s by %s (%s)" % ( self.__apiName, 
                                              self.__apiVersion,
                                              self.__apiAuthor, 
                                              self.__apiMailTo )

    ## version getter
    # @param self "Me, myself and Irene"
    def version( self ):
        return self.__apiVersion

    ## API name getter
    # @param self "Me, myself and Irene"
    def project( self ):
        return self.__apiName

    ## author name getter
    # @param self "Me, myself and Irene" 
    def author( self ):
        return self.__apiAuthor

    ## email address for author
    # @param self "Me, myself and Irene"
    def mailto( self ):
        return "mailto://" + self.__apiMailTo + "?Subject='Hello Krzysztof'"

##
# @class python::DozerUtils::test_DozerUtils
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @brief test case for DozerUtils helper classes
class test_DozerUtils( unittest.TestCase ):

    ## setup test suite 
    # @param self "Me, myself and Irene"
    def setUp( self ):
        self.__log = DozerLogger("DozerUtils_unittest.log", "test_DozerUtils", toConsole=False)
        
    ## testing DozerLogger
    # @param self "Me, myself and Irene"
    def test_01_DozerLogger( self ):
        try:
            self.__log = DozerLogger("DozerUtils_unittest.log", "test_DozerUtils", toConsole=False)
        except:
            pass
        self.assertEqual( isinstance(self.__log, DozerLogger), True )

        self.__log.getLogger(self).debug( "you can see DEBUG" )
        self.__log.getLogger(self).info( "and INFO too" )
        self.__log.getLogger(self).setLevel( logging.WARNING )
        self.__log.getLogger(self).debug( "but not this one" )
        self.__log.getLogger(self).info( "and this one too" )
        self.__log.getLogger(self).warn( "warn test message" )
        self.__log.getLogger(self).error( "error test message" )
        self.__log.getLogger(self).panic( "panic test message" )

        try:
            raise DozerError("intentional test for log.epanic(), don't panic")
        except:
            self.__log.getLogger(self).epanic( "epanic test message" )



    ## testing DozerError
    # @param self "Me, myself and Irene"
    def test_02_DozerError( self ):
        anErr = DozerError( "intentional test DozerError, don't panic!")
        self.assertEqual( isinstance(anErr, DozerError), True )
        try: 
            raise anErr
        except:
            traceback.print_exc()

    ## testing CoolDozerVersion
    # @param self "Me, myself and Irene"
    def test_03_DozerVersion( self ):
        cdv = DozerVersion()
        self.__log.info( cdv )


## test sute execution
if __name__ == "__main__":
    ##
    # @class unittest::TestCase
    # @brief standard python unittest class
    # @see http://docs.python.org/library/unittest.html#unittest.TestCase
    testLoader = unittest.TestLoader()
    suite = testLoader.loadTestsFromTestCase(test_DozerUtils)      
    unittest.TextTestRunner(verbosity=3).run(suite)
    

