#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##
# @file DozerValid.py
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @date Apr 21st, 2008
# @brief implementation of DozerValid class


import xml.dom.minidom
import re
import sys
import os
import unittest

from DozerUtils import DozerLogger, DozerError, DozerSingleton
import DozerObject

##
# @class DozerValid 
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @brief Holder for valid configurations and presettings.
#
# <h3>Usage</h3>
# 
# DozerValid object holds legal values for:
# <ul> 
# <li> nightly names
# <li> installation names
# <li> CMTCONFIG values
# <li> Atlas project names
# <li> test frameworks names
# <li> databases names
# <li> connection strings to CoolDozer databases
# </ul>
#
# The above values are taken from the CoolDozer XML configuration file picked up during DozerValid object construction.
# If user does not pass the path to the CoolDozer configuration XML file, the default one will be chosen.
# 
class DozerValid( DozerObject.DozerObject ):

    __default_coolValid_xmlfile = "CoolDozerConfig.xml"

    __metaclass__ = DozerSingleton

    __server = ""

    __authXML = ""

    __connects = { }

    __testFrameworks = { }

    __nightlies = { }

    __installs = { }

    __cmtconfigs = { }

    __projects = { }
    

    ## c'tor
    # @param self "Me, myself and Irene"
    # @param xmlfile path to file with XMl configuration
    # @param log DozerLogger instance 
    def __init__( self, xmlfile=None, log=None ):

        super( DozerValid, self ).__init__( log )
        self.__log = log

        if ( not xmlfile ):
            path = os.path.abspath(os.path.join(os.path.dirname(__file__), self.__default_coolValid_xmlfile))
            # path = os.path.abspath(__file__.replace(__file__,  self.__default_coolValid_xmlfile))
            if ( os.path.isfile(path) ):
                xmlfile = path

        if ( not xmlfile ):
            self.debug( "will search for CoolDozer XML configuration file in $XMLPATH...")
            XMLPATH = os.getenv("XMLPATH")
            if XMLPATH:
                XMLPATH = XMLPATH.split(":")
                for p in XMLPATH:
                    p += "/CoolDozer/" +  self.__default_coolValid_xmlfile
                    self.debug("checking path " + p )
                    if ( os.path.isfile(p) ):                  
                        xmlfile = p
                        break

        if ( xmlfile ):
            self.info( "constructing DozerValid using config file=%s" % xmlfile )
            self.__xmlfile = xmlfile
            self.__parse( xmlfile )
        else:
            self.error("CoolDozer XML Configuration file not found - exiting!")
            sys.exit(1)
    ##
    # @param self "Me, myself and Irene"
    # @brief valid connections strings getter
    def connects( self ):
        return self.__connects

    ##
    # @param self "Me, myself and Irene"
    # @brief valid nightlies name getter
    def nightlies( self ):
        return self.__nightlies

    ##
    # @param self "Me, myself and Irene"
    # @brief valid installs names getter
    def installs( self ):
        return self.__installs
    ##
    # @param self "Me, myself and Irene"
    # @brief valid CMTCONFIGs values getter
    def cmtconfigs( self ):
        return self.__cmtconfigs

    ##
    # @param self "Me, myself and Irene"
    # @brief valid project names getter
    def projects( self ):
        return self.__projects

    ##
    # @param self "Me, myself and Irene"
    # @brief server path getter
    def server( self ):
        return self.__server

    ##
    # @param self "Me, myself and Irene"
    # @brief valid test frameworks names getter
    def frameworks( self ):
        return self.__testFrameworks

    ##
    # @param self "Me, myself and Irene"
    # @brief valid DB names getter
    def databases( self ):
        return self.__testFrameworks

    ##
    # @param self "Me, myself and Irene"
    # @param nodeList children of XML node element
    # @brief CDATA section getter 
    def __getCData( self, nodeList ):
        cdata = ""
        for node in nodeList:
            if node.nodeType == node.TEXT_NODE:
                cdata += node.data
        return cdata

    ##
    # @param self "Me, myself and Irene"
    # @param where path to XML file 
    def __parse( self, where ):

             xmldoc = xml.dom.minidom.parse( where )
             rootNode = xmldoc.documentElement
                
             # get server location
             server = xmldoc.getElementsByTagName("server")
             if ( len(server) == 1 ):
                 server = self.__getCData( server[0].childNodes )
                 if ( server != "" ):
                     self.debug("setting server path to %s" % server )
                     self.__server = server
                 else:
                     msg = "error when parsing configuration file, <server> tag is empty" 
                     self.panic( msg )
                     raise DozerError( msg )

             # get authentication.xml file location
             #authXML = xmldoc.getElementsByTagName("auth")
             #if ( len(authXML) == 1 ):
             #    authXML = self.__getCData( authXML[0].childNodes )
             #    if ( authXML != "" ):
             #        self.debug( "authXML file set to %s" % authXML )
             #        self.__authXML = authXML
             #    else:
             #        msg = "error when parsing configuration file, <auth> tag is empty!"
             #        self.panic( msg )
             #        raise DozerError( msg )

             # get connects
             connects = xmldoc.getElementsByTagName("connect")
             self.debug("found %d <connect> tags" % len(connects) )
             if ( len(connects) != 0 ):
                 for connect in connects:
                     cdata = self.__getCData( connect.childNodes )
                     db = None
                     if ( connect.hasAttribute("db") ):
                         db = connect.getAttribute("db")
                     if ( db  and cdata  ):
                         self.debug("adding db='%s' to known connection strings" % (db ))
                         self.__connects[ db ] = cdata
                     else:
                         self.warn("found corrupted <connect> tag, attribute 'db' od CDATA is missing")
             else:
                 msg = "error when parsing configuration file, no <connect> tag found!"
                 self.panic( msg )
                 raise DozerError( msg )
                         

             # test frameworks 
             testFrameworks = xmldoc.getElementsByTagName("testFramework")
             self.debug("found %d <testFramework> tags" % len(testFrameworks) )
             if ( len( testFrameworks) != 0 ):
                 for testFramework in testFrameworks:
                     cdata = self.__getCData( testFramework.childNodes )
                     if ( cdata != "" ):
                         self.debug( "adding test framework %s to known test frameworks" % cdata)
                         self.__testFrameworks[cdata] = cdata
                     else:
                         self.warn( "empty <testFramework> tag found, skipping!")
             else:
                 msg = "error when parsing configuration file, no <testFramework> tags found!"
                 self.panic( msg )
                 raise DozerError( msg )

             # get known nightlies 
             nightlies = xmldoc.getElementsByTagName("nightly")
             self.debug("found %d <nightly> tags" % len(nightlies) )
             if ( len(nightlies) != 0 ):
                 for nightly in nightlies:
                     ndata = self.__getCData( nightly.childNodes )
                     if ( ndata != "" ):
                         nre = ndata
                         if ( nightly.hasAttribute("re") ):
                             nre = nightly.getAttribute("re")
                         if ( nre == ndata ): nre = "^" + nre + "$"
                         self.debug("adding nightly '%s' (re: %s) to known nightlies" % ( ndata, nre ) )
                         self.__nightlies[ndata] = re.compile(nre)
                            
                     else:
                         self.warn("empty <nightly> tag found, skipping!")
             else:
                 msg = "error when parsing configuration file, no <nightly> tags found!"
                 self.panic( msg )
                 raise DozerError( msg )
 
             # get known install list
             install = xmldoc.getElementsByTagName("install")
             self.debug("found %d <install> tags" % len(install) )
             if ( len(install) != 0):
                 for ins in install:
                     idata = self.__getCData( ins.childNodes )
                     if ( idata != "" ):
                         self.debug("adding installation %s to known installs" % idata )
                         self.__installs[idata] = idata
                     else:
                         self.warn("empty <install> tag found, skipping!")
             else:
                 msg = "error when parsing configuration file, no <install> tags found!"
                 self.panic(msg)
                 raise DozerError(msg)

             # get known CMTCONFIGs list
             cmts = xmldoc.getElementsByTagName("cmt")
             self.debug("found %d <cmt> tags" % len(cmts) )
             if ( len(cmts) != 0):
                 for  cmt in cmts:
                     cdata = self.__getCData( cmt.childNodes )
                     if ( cdata != "" ):
                         self.debug("adding CMTCONFIG '%s' to known cmts" % cdata )
                         self.__cmtconfigs[cdata] = cdata
                     else:
                         self.warn("empty <cmt> tag found, skipping!")
             else:
                 msg = "error when parsing configuration file, no <cmt> tags found!"
                 self.panic(msg)
                 raise DozerError(msg)

             # projects 
             projects = xmldoc.getElementsByTagName("project")
             self.debug("found %d <project> tags" % len(projects) )
             if ( len(projects) != 0):
                 for bundle in projects:
                     cdata = self.__getCData( bundle.childNodes )
                     if ( cdata != "" ):
                         self.debug("adding project '%s' to known projects" % cdata )
                         self.__projects[cdata] = cdata
                     else:
                         self.warn("empty <project> tag found, skipping!")
             else:
                 msg = "error when parsing configuration file, no <project> tags found!"
                 self.panic(msg)
                 raise DozerError(msg)
                    
    ## str operator
    # @param self "Me, myself and Irene"
    def __str__( self ):
        return "DozerValid based on CoolDozer XML configuarionm file %s" % self.__xmlfile
        

##
# @class test_DozerValid
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @date May 22, 2008
# @brief test case for DozerValid class
class test_DozerValid( unittest.TestCase ):

    ## test case setup
    # @param self "Me, myself and Irene"
    def setUp( self ):
        self.__log = DozerLogger("DozerValid_unittest.log", "test_DozerValid",toConsole=True)
        self.cv = DozerValid()
     
    ##__init__
    # @param self "Me, myself and Irene"
    def test_01_constructor( self ):
        try:
            self.cv = DozerValid()
        except:
            pass
        self.assertEqual( isinstance( self.cv, DozerValid ), True)

    ## connects()
    # @param self "Me, myself and Irene"
    def test_02_connects( self ):
        connects = self.cv.connects() 
        self.assertEqual( connects.keys(), [ u"TEST", u"PROD", u"PROD_READ" ] )
        
    ## nightlies()
    # @param self "Me, myself and Irene"
    def test_03_nightlies( self ):
        nightlies = self.cv.nightlies()
        self.assertEqual( "dev" in nightlies, True )
        self.assertEqual( "devval" in nightlies, True )
        self.assertEqual( "bugfix" in nightlies, True )
        self.assertEqual( "*" in nightlies, True )
        self.assertEqual( "val" in nightlies, True )
        self.assertEqual( "numbered" in nightlies, True )
        self.assertEqual( "pcache" in nightlies, True )
        self.assertEqual( "NN.N.NX" in nightlies, True )
        self.assertEqual( len(nightlies), 25)

    ## installs()
    # @param self "Me, myself and Irene"
    def test_04_installs( self ):
        installs = self.cv.installs()
        self.assertEqual( "build" in installs, True )
        self.assertEqual( "kit" in installs, True )
        self.assertEqual( "*" in installs, True )

    ## cmtconfigs()
    # @param self "Me, mylsef and Irene"
    def test_05_cmtconfigs( self ):
        cmtconfigs = self.cv.cmtconfigs()
        self.assertEqual( len(cmtconfigs.keys()), 17)
        
    ## projects()
    # @param self "Me, myslf and Irene"
    def test_06_projects( self ):
        projects = self.cv.projects()
        self.assertEqual( len( projects.keys()), 13 )
        
    ## frameworks()
    # @param self "Me, myself and Irene"
    def test_07_testFrameworks( self ):
        frameworks = self.cv.frameworks()
        self.assertEqual( "RTT" in frameworks, True )
        self.assertEqual( "ATN" in frameworks, True )
        self.assertEqual( "FCT" in frameworks, True )
   
    ## branches matching
    # @param self "Me, myself and Irene"
    def test_08_testBranches( self ):
        branches = self.cv.nightlies()
        testNames = [ "15.6.X", "15.6.6.Y", "15.6.X-VAL", "15.6.6.Y-VAL" ]
        for name in testNames:
            for branch,regexp in sorted(branches.iteritems(), reverse=True):
                if ( regexp.match( name ) ):
                    print name + " match to " + branch

## test suite execution
if __name__ == "__main__":
    testLoader = unittest.TestLoader()
    suite = testLoader.loadTestsFromTestCase(test_DozerValid)      
    unittest.TextTestRunner(verbosity=3).run(suite)
    

