#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
##
# @file DCubeClient/python/DCubeApp.py
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @brief implementation of DozerApp and test_DozerApp classes
import sys,os
from DCubeUtils import *
from DCubeOptParser import DCubeOptParser
from DCubeValid import DCubeValid
from DCubeConfig import DCubeConfig
from DCubeTester import DCubeTester
from DCubePHPWriter import DCubePHPWriter
import unittest
##
# @class DCubeApp 
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @brief DCube client application
class DCubeApp( DCubeObject ):

    ## parser options (optparse.Values)
    opts = None

    ## left over positional arguments as list
    args = None

    ## path to monitored root file 
    monitoredURI = None

    ## path to reference root file
    referenceURI = None

    ## monitored file handle
    monTFile = None

    ## reference file handle
    refTFile = None

    ## c'tor
    # @param self "Me, myself and Irene"
    def __init__( self ):

        parser = DCubeOptParser()
        self.parsed = self.opts, self.args = parser.parse( sys.argv )

        import ROOT
        global ROOT
        
        monitoredURI = None
        if ( len(self.args) == 2 ):
            self.monitoredURI = self.args[-1] 
                
        DCubeLogger( self.opts.log, logName="", toConsole=True )

        super( DCubeApp, self ).__init__( self )
      
        ## DCubeVersion instance
        theVersion = DCubeVersion()

        self.info( theVersion )
        self.info( theVersion.python() )
        self.info( theVersion.root() )

       
        ## DCubeValid instance
        self.valid = DCubeValid()
        
        for line in str(self.valid).split("\n"):    
            self.info( line )

        self.__dumpOptions()
        self.__dumpArgs()

        ##
        ## run dispatcher
        ##

        try:
            ## generation of config XML file
            if ( self.opts.generate and self.opts.reference ) :

                self.info("RUN: generation of DCube configuration")

                if ( DCubeConfig( self.parsed ).generate( ) ):
                    self.info("*** SUCCESS ***")
                    sys.exit(0)

            elif ( self.opts.config and self.monitoredURI ):            

                self.info("RUN: test suite execution")

                dcubeConfig = DCubeConfig( self.parsed )
                runConfig =  dcubeConfig.getConfig( strict = False )
                #if ( not runConfig ):
                #    runConfig = dcubeConfig.getConfig( strict = False )
                

                if ( self.monitoredURI ):
                    if ( self.exists( self.monitoredURI ) ):
                        self.__update( "monitored", self.monitoredURI )
                    else:
                        self.error("monitored root file %s not found" % self.monitoredURI )
                else:
                    raise DCubeException(  "wrong arguments, monitored file not specified!" )
                
                if ( runConfig ):
                    
                    if ( self.runConfig ( runConfig ) ):

                        if ( self.tester ):

                            if ( self.__producePHPs() ):
                                self.info("creation of PHP files completed")
                                self.info("*** SUCCESS ***")
                            else:
                                self.warn("some problems occured when producing PHP files")
                                self.info("*** WARNING ***")
                            sys.exit(0)
                        else:
                            self.error("no DCubeTester instance, no summary information found")
                            sys.exit(-1)
                else:
                    raise DCubeException( "bad configuration -  wrong configuration requested?" )
            else:
                self.error( "wrong set of CLI options, neither generation of config (-g) nor test suite excecution (-c CONFIG.XML MONITORED.root) specified" )
                sys.exit(-1)
                
        except DCubeException, value:
            self.epanic( value )
            self.epanic("*** FAILURE ***")
            sys.exit(-1)

    ## testing and plotting using \<dcube\> node  
    # @param self "Me, myself and Irene"
    # @param xmlConfig dcube node from configuration file
    def runConfig( self, xmlConfig ):
        self.info("will run test suite using xml configuration..." )

        ## output XML DOM document
        self.xmldoc = xml.dom.minidom.Document() 
        self.xmldoc.appendChild( xmlConfig )

        ## overwritting xml config to match run config
        self.debug("will overwrite xml config to match run config...")
        xmlConfig.setAttribute( "branch", self.opts.branch ) 
        xmlConfig.setAttribute( "install", self.opts.install )
        xmlConfig.setAttribute( "cmtconfig", self.opts.cmtconfig )
        xmlConfig.setAttribute( "project", self.opts.project )
        xmlConfig.setAttribute( "jobId", self.opts.jobId )
        self.debug("done!")

        config = "/".join( [ xmlConfig.getAttribute( "branch" ),
                             xmlConfig.getAttribute( "install" ),
                             xmlConfig.getAttribute( "cmtconfig" ),
                             xmlConfig.getAttribute( "project" ),
                             xmlConfig.getAttribute( "jobId" ) ] )
        self.debug( "the run config is %s" % config )

        ## get pvalue limits
        pvalues = xmlConfig.getElementsByTagName( "plimit" )
        if ( len(pvalues) == 0 ):
            self.warn("tag <plimit> not found, will use CLI pvalue limits FAIL=%f WARN=%f" % ( self.opts.pfail,
                                                                                               self.opts.pwarn ) )
        elif ( len(pvalues) > 1 ):
            self.warn("too many <plimit> tags, will use CLI pvalue limits FAIL=%f WARN=%f" % ( self.opts.pfail,
                                                                                               self.opts.pwarn ) )
        else:
            self.debug("tag <plimit> found, will user p-value limits from its attributes")
            try:
                pfail = float ( pvalues[0].getAttribute("fail") )
                self.__update( "pfail", pfail )
            except TypeError, value:
                raise DCubeException( "pvalue limit for FAIL is NAN - %s" % str(value) )

            try:
                pwarn = float( pvalues[0].getAttribute( "warn" ) )
                self.__update( "pwarn", pwarn )
            except TypeError, value:
                raise DCubeException( "pvalue limit for WARN is NAN - %s" % str(value) )

        referenceNode = None
        self.referenceURI = None
        ## reference root file    
        if ( self.opts.reference != None ):

            self.referenceURI = os.path.abspath( self.opts.reference )
            
            self.debug("will use reference root file from CLI = %s" % self.referenceURI )
            referenceNode = self.xmldoc.createElement( "reference" )
            referenceNode.setAttribute( "file", self.referenceURI )

            if ( xmlConfig.getElementsByTagName("reference") ):
                xmlConfig.replaceChild( referenceNode, xmlConfig.getElementsByTagName("reference")[0] )
            else:
                xmlConfig.appendChild( referenceNode )

        else:

            reference = xmlConfig.getElementsByTagName( "reference" )
            if ( len(reference) == 0 ): raise DCubeException("tag <reference> not found!")
            elif ( len(reference) > 1 ): raise DCubeException("too many (>1) <reference> tags")
            else:
                referenceNode = reference[0]
                self.referenceURI = os.path.abspath( referenceNode.getAttribute("file") )
                self.__update( "reference", self.referenceURI )
                    
            self.debug("will use reference root file from XML = %s" % self.referenceURI )

        # monitored root file
        monitoredNode = self.xmldoc.createElement( "monitored" )
        monitoredNode.setAttribute( "file", self.monitoredURI )

        xmlConfig.insertBefore( monitoredNode, referenceNode  )

        # date node
        dateNode = self.xmldoc.createElement( "date" )
        dateCDataNode = self.xmldoc.createTextNode( self.opts.isodate ) 

        dateNode.appendChild( dateCDataNode )

        xmlConfig.insertBefore( dateNode, monitoredNode )


        # update xml output file
        if ( self.opts.output == "" ):
            self.__update("output", self.opts.monitored + ".dcube.xml" )

        # get <tdirectory> tags and run tester
        tdirs = xmlConfig.getElementsByTagName( "TDirectory" )    
        if ( len(tdirs) == 0 ): raise DCubeException("no <TDirectory> tags found!")
        else:
            ## DCubeTester instance
            self.tester = DCubeTester( self.xmldoc, self.parsed )
            
            self.refTFile = None
            if ( self.exists( self.referenceURI ) ):
                try:
                    self.refTFile = ROOT.TFile.Open( self.referenceURI, "READ" )
                except Exception, value:
                    self.epanic( "cannot get handle for referece root file, all tests will be skipped, reason: %s" % str(value) )
            else:
                self.warn("reference root file not exists, all tests will be skipped!")

            self.monTFile = None
            if ( self.exists( self.monitoredURI ) ):
                try:
                    self.monTFile = ROOT.TFile.Open( self.monitoredURI , "READ")
                except Exception, value:
                    msg = "cannot get handle for monitored root file, reason: %s" % str(value) 
                    raise DCubeException( msg )
            else:
                raise DCubeException( "monitored root file %s not exists!" % self.monitoredURI )
                
            # run DCubeTester recursively
            self.test( tdirs[0], tdirs[0].getAttribute("name") ) 

            if ( self.refTFile ): self.refTFile.Close()
            if ( self.monTFile ): self.monTFile.Close()

            # print and append summary node 
            xmlConfig.appendChild (  self.tester.summary() )
                    
        # save output XML to file
        return self.__save( )
    
    
    ## get CDATA section
    # @param self "Me, myself and Irene"
    # @param nodelist list of XML DOM Node objects
    def __cdata( self, nodelist ):
        out = ""
        for node in nodelist:
            if node.nodeType == node.TEXT_NODE:
                out = out + node.data
        return out 
        
    ## main worker here
    # @param self "Me, myself and Irene"
    # @param node xml \<tdirectory\> node
    # @param path full path inside both root files
    def test( self, node, path ):

        self.debug("now testing objects in path = %s" % path )

        status = [ ]
        for child in node.childNodes:

            if ( child.nodeType == node.ELEMENT_NODE ):
                self.debug("child tagName %s" % child.tagName )
                tagName = child.tagName
                if ( tagName == "TDirectory" ):
                    status.append ( self.test( child, os.path.join( path , child.getAttribute("name")  ) ) )
                    
                    for test in [ "KS", "chi2", "bbb" ]:
                        for stat in [ "OK", "WARN", "FAIL" ]:
                            attr = test + stat
                                
                            parentCounter = node.getAttribute( attr )
                            if ( not parentCounter ): parentCounter = "0"
                            
                            nodeCounter = child.getAttribute( attr )
                            if ( not nodeCounter ): nodeCounter = "0"
                            
                            node.setAttribute( attr, "%d" % ( int(parentCounter) + int(nodeCounter) )  )            
                else:

                    if ( child.hasAttribute("name") ):
                        objName = child.getAttribute( "name" )
                        objTests = child.getAttribute( "tests" ).split(",")
                        self.debug( "found type=%s name=%s tests=%s in path %s" % ( tagName, objName, str(objTests), path ) )
                        
                        if ( path == "/" ):
                            objPath = objName
                        else:
                            objPath = os.path.join( path, objName )
                        
                        refObj = self.refTFile.Get( objPath )
                        monObj = self.monTFile.Get( objPath )
                    
                        status.append( self.tester.test( child, monObj, refObj ) )
                    else:
                        self.warn( "empty attribute 'name' found for tag <%s> in path %s, skipping!" % ( tagName, path ) )
                    
        statusAttr = "OK"    
        if ( ( "FAIL" in status ) or
             ( None in status ) ):
            statusAttr = "FAIL"    
        elif ( "WARN" in status ):
            statsuAttr = "WARN"

        self.debug("TDirectory %s status %s" % ( path, statusAttr ) )
        node.setAttribute( "status", statusAttr )

        return statusAttr 
        
    ## update parsed options from DCubeOptParser
    # @param self "Me, myself and Irene"
    # @param key option name
    # @param what new value
    def __update(  self, key, what ):
        self.debug("updating option %s to %s" % ( str(key), str(what) ) )
        self.opts._update_loose( { key : what } )
        
    ## dump left arguments to logger
    # @param self "Me, myself and Irene"
    def __dumpArgs( self ):
        if ( len(self.args) ):
            self.info("dumping not parsed arguments...")
            i = 1
            for item in self.args:
                self.info("[%02d] %s" % ( i, item ) )
                i += 1

    ## dump parsed options to logger
    # @param self "Me, myself and Irene"
    def __dumpOptions( self ):
        self.info("dumping parsed options...")
        i = 1
        for k, v in self.opts.__dict__.iteritems():
            if ( v == "" ): v = "not set"
            self.info("[%02d] %s %s" % ( i, k, v))
            i += 1
        
    ## save new output XML to file
    # @param self "Me, myself and Irene"
    def __save( self ):
        
        outputURI = os.path.abspath( self.opts.output )
        self.__backup( outputURI )
        try:
          fileXML = open( outputURI, "w+")
          self.xmldoc.normalize()
      
          xmlstring =  self.xmldoc.toprettyxml(" ", "\n").split("\n")
          for line in xmlstring:
              if line.strip("\n").strip() != "":
                  fileXML.write( line+"\n" )
          fileXML.close()
          self.info("DCube output has been saved to file %s" % outputURI )
        except IOError, value:
            msg = "creation of XML output file %s failed - %s" % ( outputURI, str(value) )
            self.epanic( msg )
            return False

        return True

    ## create backup for old XML output file file
    # @param self "Me, myself and Irene"
    # @param file path to file
    def __backup( self, file ):

        fileURI = os.path.abspath(file)
        self.debug( "will create backup file for %s" % file)

        if ( os.path.exists( fileURI ) and os.path.isfile(fileURI) ):
            self.warn("file %s exists and will be overwritten!" % fileURI )
            backupURI = fileURI + ".bak"
            self.debug("will create backup file %s" % backupURI )
            if ( os.path.isfile( backupURI ) ):
                self.warn("backup file %s exists and will be overwritten!" % backupURI )
            try:
                os.rename( fileURI, backupURI )
                self.info("backup file %s from file %s has been created" % ( backupURI, fileURI ) )
            except OSError, value:
                msg = "creation of backup file %s failed - %s" % ( backupURI, str(value) )
                self.epanic( msg )
                    
    ## check existence of file 
    # @param self "Me, myself and Irene"
    # @param filename a file name to check
    def exists( self, filename ):
        if ( "://" not in filename ):
            return os.path.exists( os.path.abspath( str(filename) ) )
        self.warn("can't check exsitence of file at URI %s" % filename )
        return True

    ## produce PHP files
    # @param self "Me, myself and Irene"
    # @return boolean status 
    def __producePHPs( self ):
        self.info("will produce PHP files...")

        if ( not self.opts.server ):
            self.error("path to DCubeServer not set, PHP files won't be produced")
            return False
        
        if ( None not in ( self.opts.log, self.opts.config, self.opts.output ) ):
            phpWriter = DCubePHPWriter( self.parsed )

            phpURI = os.path.abspath( self.opts.output + ".php" )
            logURI = os.path.abspath( os.path.join( os.path.dirname(self.opts.output), "dcubelog.php" ) )
            
            self.debug( "out php URI %s" % phpURI )
            self.debug( "log php URI %s" % logURI )
            
            phpOK = False
            try:
                filePHP = open( phpURI, "w+" )
                filePHP.writelines( phpWriter.dcubePHP() )
                filePHP.close()
                self.debug("out php file has been created")
                phpOK = True
            except Exception, value:
                self.error("can't create output PHP file %s, reason - %s" % ( phpURI, str(value) ) )

            logOK = False
            try:
                filePHP = open( logURI, "w+" )
                filePHP.writelines( phpWriter.dcubeLOG() )
                filePHP.close()
                self.debug("log php file has been created")
                logOK = True
            except Exception, value:
                self.error("can't create log PHP file %s, reason - %s" % ( logURI, str(value) ) )

            return ( phpOK and logOK )


    ## main comment in output XML file 
    # @param self "Me, myself and Irene"
    # @return a comment string
    def __mainXMLComment( self ):
        pass

    
## 
# @class test_DCubeApp 
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @brief test case for DCubeApp
# @todo cover more!
class test_DCubeApp( unittest.TestCase ):

    ## setup test suite
    # @param self "Me, myself and Irene"
    def setUp( self ):
        self.sysArgv = [ "--branch", "dev",
                         "--install", "build",
                         "--cmtconfig", "i686-slc4-gcc34-opt",
                         "--project", "AtlasProduction",
                         "--jobId", "job_1",
                         "--server", "/afs/cern.ch/user/c/cibak/scratch1/dcube/dcube.php",
                         "monitored.root" ]
        
        self.sysExitHolder = sys.exit
        def exitCode( value ):
            print "sys.exit called with value %s" % str(value)    
        sys.exit = exitCode

    ## test contructor
    # @param self "Me, myself and Irene"
    def test_01_ctor( self ):
        sys.argv = self.sysArgv
        theApp = DCubeApp( )
        del theApp

    ## test public API
    # @param self "Me, myself and Irene"
    def test_02_api( self ):
        sys.argv = self.sysArgv
        theApp = DCubeApp( )
        del theApp

    
## test suite execution
if __name__ == "__main__":
    
    # sometimes I believe python interpreter ignores all my comments
    testLoader = unittest.TestLoader()
    suite = testLoader.loadTestsFromTestCase(test_DCubeApp)      
    unittest.TextTestRunner(verbosity=3).run(suite)
    
