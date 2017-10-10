#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##
# @file DCubeClient/python/DCubeValid.py
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @date Apr 21st, 2008
# @brief implementation of DCubeValid class


import xml.dom.minidom
import re
import sys
import os
import unittest

from DCubeUtils import DCubeLogger, DCubeObject, DCubeException, DCubeSingleton

##
# @class DCubeValid 
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @brief Holder for valid configurations and presettings.
#
# <h3>Usage</h3>
# 
# DCubeValid object holds legal values for:
# <ul> 
# <li> nightly names
# <li> installation names
# <li> CMTCONFIG values
# <li> Atlas project names
# <li> test frameworks names
# </ul>
#
# The above values are taken from the CoolDozer XML configuration file picked up during DozerValid object construction.
# If user does not pass the path to the CoolDozer configuration XML file, the default one will be chosen.
# 
class DCubeValid( object ):

    __default_dcubeValid_xmlfile = "DCubeValid.xml"

    __metaclass__ = DCubeSingleton

    __server = ""

    __testFrameworks = { }

    __nightlies = { }

    __installs = { }

    __cmtconfigs = { }

    __projects = { }
    

    ## c'tor
    # @param self "Me, myself and Irene"
    # @param xmlfile path to file with XMl configuration
    # @param log DCubeLogger instance 
    def __init__( self, xmlfile=None, log=None ):


        inInstallArea = None
        if ( not xmlfile ):
            inInstallArea = os.path.abspath( os.path.join( os.path.dirname( globals()["__file__"] ), "../share/DCubeValid.xml" ) )
            if ( not xmlfile and os.path.exists( inInstallArea) ): xmlfile = inInstallArea

        inDCubePath = None
        ## in current dir
        if ( not xmlfile ):
            path = os.path.abspath( globals()["__file__"] ).split("/")[0:-1]
            path.append( self.__default_dcubeValid_xmlfile )
            path = "/".join(path)
            inDCubePath = path
            if ( os.path.exists( inDCubePath ) ): xmlfile = path

        if ( xmlfile ):        
            self.__xmlfile = xmlfile
            self.__parse( xmlfile )
        else:
            print "DCubeValid.xml not found in InstallArea nor DCubeClient install directories, wrong installation?"
            sys.exit( -1 )

    ##
    # @param self "Me, myself and Irene"
    # @brief valid nightlies name getter
    def branches( self ):
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
                     self.__server = server
                 else:
                     self.panic( msg )
                     raise DCubeException( msg )


             # get known nightlies 
             nightlies = xmldoc.getElementsByTagName("branch")
            
             if ( len(nightlies) != 0 ):
                 for nightly in nightlies:
                     ndata = self.__getCData( nightly.childNodes )
                     if ( ndata != "" ):
                         nre = ndata
                         if ( nightly.hasAttribute("re") ):
                             nre = nightly.getAttribute("re")
                         if ( nre == ndata ): nre = "^" + nre + "$"
                         self.__nightlies[ndata] = re.compile(nre)                
           
 
             # get known install list
             install = xmldoc.getElementsByTagName("install")
             if ( len(install) != 0):
                 for ins in install:
                     idata = self.__getCData( ins.childNodes )
                     if ( idata != "" ):
                         self.__installs[idata] = idata
           
           
             # get known CMTCONFIGs list
             cmts = xmldoc.getElementsByTagName("cmt")
             if ( len(cmts) != 0):
                 for  cmt in cmts:
                     cdata = self.__getCData( cmt.childNodes )
                     if ( cdata != "" ):
                         self.__cmtconfigs[cdata] = cdata
           
             # projects 
             projects = xmldoc.getElementsByTagName("project")
             if ( len(projects) != 0):
                 for bundle in projects:
                     cdata = self.__getCData( bundle.childNodes )
                     if ( cdata != "" ):
           
                         self.__projects[cdata] = cdata

    ## list valid branch names
    # @param self "Me, myself and Irene"
    def listBranches( self ):
        out = "- valid branches values and thier regexp patterns:\n"
        i = 1
        for k  in sorted(  self.branches().keys() ):
            v = self.branches()[k]
            out += "  [%02d] %-14s regexp pattern=%s\n" % ( i, k, v.pattern )
            i += 1
        return out
                   
    ## list valid install names
    # @param self "Me, myself and Irene"
    def listInstalls( self ):
        out = "- valid installs values:\n"
        i = 1
        for k in sorted( self.installs() ):
            out += "  [%02d] %s\n" % ( i, k )
            i += 1
        return out
 
    ## list valid cmtconfig names
    # @param self "Me, myself and Irene"
    def listCmtconfigs( self ):
        out = "- valid cmtconfigs names:\n"
        i = 1
        for k in sorted( self.cmtconfigs() ):
            out += "  [%02d] %s\n" % ( i, k )
            i += 1
        return out
    
    ## list valid project names
    # @param self "Me, myself and Irene"
    def listProjects( self ):
        out = "- valid projects names:\n"
        i = 1
        for k in sorted( self.projects() ):
            out += "  [%02d] %s\n" % ( i, k )
            i += 1    
        return out

    ## str operator
    # @param self "Me, myself and Irene"
    def __str__( self ):
        out  = "DCube valid names based on DCube XML configuarionm file %s\n" % self.__xmlfile
        out += self.listBranches()
        out += self.listInstalls()
        out += self.listCmtconfigs()
        out += self.listProjects()
        return out.strip()

##
# @class test_DCubeValid
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @date May 22, 2008
# @brief test case for DCubeValid class
class test_DCubeValid( unittest.TestCase ):

    ## test case setup
    # @param self "Me, myself and Irene"
    def setUp( self ):
        self.__log = DCubeLogger("test_DCubeValid.log", "test_DCubeValid",toConsole=True)
        self.cv = DCubeValid()
    
    ##__init__
    # @param self "Me, myself and Irene"
    def test_01_constructor( self ):
        try:
            self.cv = DCubeValid()
        except:
            pass
        self.assertEqual( isinstance( self.cv, DCubeValid ), True)

    ## nightlies()
    # @param self "Me, myself and Irene"
    def test_02_nightlies( self ):
        nightlies = self.cv.branches()
        self.assertEqual( "dev" in nightlies, True )
        self.assertEqual( "devval" in nightlies, True )
        self.assertEqual( "bugfix" in nightlies, True )
        self.assertEqual( "*" in nightlies, True )
        self.assertEqual( "val" in nightlies, True )
        self.assertEqual( "numbered" in nightlies, True )
        self.assertEqual( "pcache" in nightlies, True )
        self.assertEqual( "NN.N.NX" in nightlies, True )
        print nightlies

    ## installs()
    # @param self "Me, myself and Irene"
    def test_03_installs( self ):
        installs = self.cv.installs()
        self.assertEqual( "build" in installs, True )
        self.assertEqual( "kit" in installs, True )
        self.assertEqual( "*" in installs, True )

    ## cmtconfigs()
    # @param self "Me, mylsef and Irene"
    def test_04_cmtconfigs( self ):
        cmtconfigs = self.cv.cmtconfigs()
        self.assertEqual( len(cmtconfigs.keys()), 17)
        
    ## projects()
    # @param self "Me, myslf and Irene"
    def test_05_projects( self ):
        projects = self.cv.projects()
        self.assertEqual( len( projects.keys()), 12 )


    ## str()
    # @param self "Me, myself and Irene"
    def test_06_str( self ):
        print self.cv
   
        
## test suite execution
if __name__ == "__main__":
    
    testLoader = unittest.TestLoader()
    suite = testLoader.loadTestsFromTestCase(test_DCubeValid)      
    unittest.TextTestRunner(verbosity=3).run(suite)
    

