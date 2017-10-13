#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
##
# @file DCubeClient/python/DCubeConfig.py
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @brief implementation of DCubeConfig and test_DCubeConfig classes

import os
import re
import DCubeUtils
from DCubeOptParser import DCubeOptParser
import unittest
import ROOT
import xml.parsers.expat
import xml.dom.minidom
from time import strftime, localtime, strptime


## 
# @class DCubeConfig
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @brief DCube configuration holder
class DCubeConfig( DCubeUtils.DCubeObject ):

    ## forcing to be a singleton
    __metaclass__ = DCubeUtils.DCubeSingleton

    ## configuration dictionary
    __config = { }


    ## flag to trigger transformation from old format 
    needTransform = False
    
    ## c'tor
    # @param self "Me, myself and Irene"
    # @param parsed arguments and options from DCubeOptParser 
    def __init__(self, parsed):

        super(DCubeConfig, self).__init__( self )
        self.info("constructing DCubeConfig...")
        
        self.opts, self.args = parsed
        
        if ( self.opts.config != "" ):
            self.xmldoc = self.__readXML() 
        

    ## branch name getter
    # @param self "Me, myself and Irene"
    def branch( self ):
        return self.opts.branch

    ## install name getter
    # @param self "Me, myslf and Irene"
    def install( self ):
        return self.opts.install

    ## cmtconfig name getter
    # @param self "Me, myself and Irene"
    def cmtconfig( self ):
        return self.opts.cmtconfig

    ## project name getter
    # @param self "Me, myself and Irene"
    def project( self ):
        return self.opts.project

    ## job id getter
    # @param self "Me, myself and Irene"
    def jobId( self ):
        return self.opts.jobId

    ## remove white spaces nodes from XML DOM tree
    # @param self "Me, myself and Irene"
    # @param node xml.dom.Node object
    def __removeWS( self, node ):

        toRemove = []
        for child in node.childNodes :
            
            if ( ( child.nodeType is xml.dom.Node.TEXT_NODE ) and 
                 not child.data.strip() ):
                toRemove.append( child )
            elif ( child.hasChildNodes() ):
                self.__removeWS( child )
        for node in toRemove:
            self.debug( "removing node %s" % str(node) )
            node.unlink()
                     

    ## save new config to file
    # @param self "Me, myself and Irene"
    def __save( self ):
        
        configURI = self.opts.config
        self.__backup( configURI )
        try:
          fileXML = open( configURI, "w+")
          self.xmldoc.normalize()
      
          xmlstring =  self.xmldoc.toprettyxml(" ", "\n").split("\n")
          for line in xmlstring:
              if line.strip("\n").strip() != "":
                  fileXML.write( line+"\n" )
          fileXML.close()
          self.info("DCubeConfig has been saved to file %s" % configURI)
        except IOError, value:
            msg = "creation of XML file %s failed - %s" % ( configURI, str(value) )
            self.epanic( msg )

        return True

    ## create backup for old config file
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
        
                 
    ## (re)genaration of DCube xml configuration file
    # @param self "Me, myself and Irene"
    def generate( self ):

        self.error( self.opts.config )
        configURI = os.path.abspath( self.opts.config )
        
        fromScratch = False

        msg = "configuration file %s" % configURI
        if ( os.path.exists( configURI ) and 
             os.path.isfile( configURI ) ):
            msg += " exists"
            if ( os.path.getsize( configURI ) ):
                msg += " and will be overwritten"
            else:
                msg += ", but is empty, will create configuration from scratch"
                fromScratch = True
        else:
            msg += " doesn't exist, will create it from scratch..."
            fromScratch = True

        self.info( msg )

        if ( fromScratch ):
            self.debug( "will create XML DOM document and store dcube config in file %s" % configURI )
            self.xmldoc = self.__createXMLDOC()
        else:
            self.debug( "will ready dcube config from file %s" % configURI )
            self.xmldoc = self.__readXML() 

        self.refFileHandle = None
        self.refFileURI = os.path.abspath( self.opts.reference )
        self.debug("will try to open refernce root file=%s" % self.refFileURI )
        try:
            self.refFileHandle = ROOT.TFile.Open( self.refFileURI, "READ" )
            self.debug("done")
        except:
            pass

        if ( not self.refFileHandle ):
            raise DCubeUtils.DCubeException("cannot open %s reference root file" % self.refFileURI )
        
        cliConfig = "/".join( [ self.opts.branch, self.opts.install,
                      self.opts.cmtconfig, self.opts.project,
                      self.opts.jobId ] )

        self.info("will try to match CLI config %s to XML config" % cliConfig)
        runConfigNode = self.getConfig( strict=True )
        self.info("done!")
        if ( not runConfigNode ):
            self.warn("non matching config found in XML, will create it from scratch")
            runConfigNode = self.__emptyConfig()
        else:
            self.warn("XML config has been found and will be overwritten" )

        commentFound = False
        for child in  runConfigNode.childNodes:
            if ( child.nodeType == child.COMMENT_NODE ):
                runConfigNode.replaceChild( self.__configComment(), child )
                commentFound = True
                break
            
        if ( not commentFound ):
            runConfigNode.insertBefore( self.__configComment(), runConfigNode.firstChild  )

        ## overwrite reference node
        refNode = runConfigNode.getElementsByTagName("reference")
        if ( len(refNode) == 0 ):
            refNode = self.xmldoc.createElement("reference")
            runConfigNode.appendChild( refNode )
        else:
            refNode = refNode[0]
        refNode.setAttribute("file", os.path.abspath( self.opts.reference ) )

        ## overwrite pvalue node
        pValueNode = runConfigNode.getElementsByTagName("plimit")
        if ( len(pValueNode) == 0 ):
            pValueNode = self.xmldoc.createElement("plimit")
            runConfigNode.appendChild( pValueNode )
        else:
            pValueNode = pValueNode[0]

        pValueNode.setAttribute("fail", str( self.opts.pfail ) )
        pValueNode.setAttribute("warn", str( self.opts.pwarn ) )
        
        ## overwrite top level tdirectory tag
        rootDirNodes = runConfigNode.getElementsByTagName("TDirectory")

        if ( len(rootDirNodes) != 0 ):
            for rootDirNode in rootDirNodes:
                if ( rootDirNode.parentNode == runConfigNode ): 
                    runConfigNode.removeChild( rootDirNode )
                
        rootDirNode = self.xmldoc.createElement("TDirectory")
        rootDirNode.setAttribute("name", "/")
        runConfigNode.appendChild( rootDirNode ) 

        self.info("will scan reference file for objects...")
        self.__scan( self.refFileHandle, rootDirNode )       
        self.info("done!")

        msg = "generation of dcube config file='%s' " % os.path.abspath( self.opts.config )
        if ( self.__save() ):
            self.info( msg +  "completed" )
            return True
        
        self.error(msg + "failed" % self.opts.config )
        return False


    ## match runtime config (from CLI) to offline XML config
    # @param self "Me, myself and Irene"
    # @param fromRun tuple from CLI
    # @param fromXML tuple from XML
    # @param strict flag to forve only str to str matching
    def __match( self, fromRun, fromXML, strict=True ):

        mode = "strict" if strict else "regexp" 
        self.info("maching config in %s mode" % mode)

        pattern = "[\S]+"
        whats = ["branch", "install", "cmtconfig", "project", "jobId" ]
        cPairs = dict( zip (whats, zip( fromRun, fromXML ) ) )
        cMatch = dict( zip (whats, [False for i in range(5)] ) )
       

        for k,v in cPairs.iteritems():
            cliValue, xmlValue = v
            msg = "what=%-10s fromCLI=%-23s fromXML=%-23s" % ( k , cliValue, xmlValue )
            if ( cliValue.strip() == xmlValue.strip() ): 
                msg += "match (str to str)" 
                self.debug( msg )
                cMatch[k] = True
            else:
                if ( not strict ):
                    if ( "*" in cliValue and "*" not in xmlValue ):
                        reCLI = re.compile( cliValue.replace("*", pattern, cliValue.count("*") ) )
                        if ( reCLI.match( xmlValue ) ):
                            msg += "match (reqexp to str)"
                            self.debug( msg ) 
                            cMatch[k] = True
                            continue
                    elif ("*" in xmlValue and "*" not in cliValue ):
                        reXML = re.compile( xmlValue.replace("*", pattern, xmlValue.count("*") ) )
                        if ( reXML.match( cliValue ) ):
                            msg += "match (str to regexp)"
                            self.debug( msg )
                            cMatch[k] = True
                    else:
                        msg += "not match"
                        self.debug( msg )
                else:
                    msg += "not match (strict mode on)"
                    self.debug( msg )
                    cMatch[k] = False
                    
       
        return all(cMatch.values())


    ## get config XML node based on runtime configuration
    # @param self "Me, myself and Irene"
    # @param strict match config strings w/o regexp
    def getConfig( self, strict=True ):
        
        runConfig = ( self.opts.branch,
                      self.opts.install,
                      self.opts.cmtconfig,
                      self.opts.project,
                      self.opts.jobId )
        
        self.info("will try to find CLI config='%s' in DCubeClient configuration file %s" % ( "/".join(runConfig), 
                                                                                               os.path.abspath( self.opts.config) ) )

       
        configs = self.__config.keys()
        configs.sort( reverse=True )
        for config in configs:
            if  self.__match( runConfig, config, strict ):
                self.info("runtime config='%s' match to XML config='%s'" % ( "/".join(runConfig), "/".join(config) ) )
                return self.__config[ config ]
        
        self.warn("<dcube> DOM XML node for CLI config %s not found" % "/".join( runConfig ) )
        return None

    
    def __oldConfig( self ):
        pass


    def __configComment( self ):
        comNode = "\n"
        comNode += "  " + "#"*51 + "\n"
        comNode += "  DCubeClient for:\n"
        comNode += "  [1] branch: %s\n" % self.opts.branch
        comNode += "  [2] install: %s\n" % self.opts.install
        comNode += "  [3] cmtconfig: %s\n" % self.opts.cmtconfig 
        comNode += "  [4] project: %s\n" % self.opts.project
        comNode += "  [5] jobId: %s\n" % self.opts.jobId
        comNode += "  [6] reference root file: %s\n" % os.path.abspath( self.opts.reference )
        comNode += "  [7] p-values limits FAIL = %f WARN = %f\n" % ( self.opts.pfail, self.opts.pwarn )
        comNode += "  " + "#"*51 + "\n"
        comNode = self.xmldoc.createComment( comNode )
        return comNode
     
    ## prepare empty config XML DOM element with attributes from CLI passed thru DCubeOptParser
    # @param self "Me, myself and Irene"
    def __emptyConfig( self ):

        self.info("will create empty <dcube> DOM XML node...")

        runConfigNode = self.xmldoc.createElement("dcube")
        runConfigNode.setAttribute( "branch", self.opts.branch )
        runConfigNode.setAttribute( "install", self.opts.install )
        runConfigNode.setAttribute( "cmtconfig", self.opts.cmtconfig )
        runConfigNode.setAttribute( "project", self.opts.project )
        runConfigNode.setAttribute( "jobId", self.opts.jobId )


        referenceNode = self.xmldoc.createElement("reference")
        referenceNode.setAttribute("file", os.path.abspath( self.opts.reference ) )

        referenceDescNode = self.xmldoc.createElement("ref_desc")
        refDescCData = self.xmldoc.createTextNode("PUT YOUR REFERENCE DESCRIPTION HERE")
   
        referenceDescNode.appendChild( refDescCData )

        testDescNode = self.xmldoc.createElement("test_desc")
        testDescCData = self.xmldoc.createTextNode("PUT YOUR TEST DESCRIPTION HERE")

        testDescNode.appendChild( testDescCData )

        pValueNode = self.xmldoc.createElement("plimit")
        pValueNode.setAttribute("fail", str( self.opts.pfail ) )
        pValueNode.setAttribute("warn", str( self.opts.pwarn ) )

        ## append all nodes
        runConfigNode.appendChild( referenceNode )
        runConfigNode.appendChild( referenceDescNode )
        runConfigNode.appendChild( testDescNode )

        runConfigNode.appendChild( pValueNode )
        
        rootDirNode = self.xmldoc.createElement("TDirectory")
        rootDirNode.setAttribute("name", "/")
        
        runConfigNode.appendChild( rootDirNode )
        
        self.xmldoc.getElementsByTagName("dcube_client_config")[0].appendChild( runConfigNode )

        self.info("creation of empty <dcube> node completed")
        return runConfigNode

    
    ## 
    # @param self "Me, myself and Irene"
    def __handleConfigNodes( self, parent ):

        
        runConfig = ( self.opts.branch,
                      self.opts.install,
                      self.opts.cmtconfig,
                      self.opts.project,
                      self.opts.jobId )
        
        configNodes = parent.getElementsByTagName("dcube")

        self.debug("found %d <dcube> tags" % len( configNodes) )
        if ( len(configNodes) ):
            for configNode in configNodes:

                nodeConfig = ( configNode.getAttribute("branch") if configNode.getAttribute("branch") != "" else "*",
                               configNode.getAttribute("install") if configNode.getAttribute("install") != "" else "*",
                               configNode.getAttribute("cmtconfig") if configNode.getAttribute("cmtconfig") != "" else "*",
                               configNode.getAttribute("project") if configNode.getAttribute("project") != "" else "*",
                               configNode.getAttribute("jobId") if configNode.getAttribute("jobId") != "" else "*" ) 
               
                self.info("found <dcube> tag for configuration %s/%s/%s/%s/%s" % nodeConfig )
                self.__config[nodeConfig] = configNode
        else:
            self.warn("DCube config file is empty? no <config> tags found!")
            



    ## generate and return empty XML DOM document 
    # @param self "Me, myself and Irene"
    def __createXMLDOC( self ):
        xmldoc = xml.dom.minidom.Document()
        
        commentNode = xmldoc.createComment( self.__mainXMLComment() )
        xmldoc.appendChild( commentNode )

        dcubeNode = xmldoc.createElement("dcube_client_config")      
        xmldoc.appendChild( dcubeNode )
   
        return xmldoc
    

    ## produce scratch config comment string
    # @param self "Me, myself and Irene"
    def __mainXMLComment( self ):
        
        out  = "\n DCube configuration XML file = %s\n" % self.opts.config
        out += " auto-generated using %s\n" %  DCubeUtils.DCubeVersion()
        out += " on %s\n" %  strftime("%a, %d %b %Y %H:%M:%S %Z" , localtime())
        out += " from Root reference file: %s\n" % str( self.opts.reference )
        out += " Statistics tests:\n"
        out += " %r\n" % repr( self.opts.tests )
        out += " Legend:\n"
        out += " * KS    - Kolmogorov-Smirnov test (default)\n"
        out += " * chi2  - chi2 test\n"
        out += " * bbb   - bin-by-bin comparision\n"
        out += " * meany - avg eff mean y [TProfiles only!]\n"
        out += " * all   - all above tests\n\n"
        out += " Tweaks:\n"
        out += " [1] remove or comment out (using HTML comments tags) histograms not relevant\n"
        out += "     to your analysis\n"
        out += " [2] provide a new content to \"ref_desc\" and \"mon_desc\" tags to something\n"
        out += "     more meaningful\n"
        out += " [3] and of course feel free to change attribute \"tests\" in each \"hist\" tag\n"
        out += " [4] limits of p-value warning and failure can be set by changing attributes\n"
        out += "     in <plimit> tag\n\n"
        out += " [5] if you want to make plots FOR EVERY histogram in your monitored file\n"
        out += "     change attribute \"inclusive\" in <mode> tag to \"true\"\n\n"
        return out



    ## read XML configuration from 
    # @param self "Me, myself and Irene"
    def __readXML( self ):

        configURI = os.path.abspath( self.opts.config )
        self.debug("will try to read and parse configuration file %s" % configURI )

        if ( os.path.exists( configURI ) and 
             os.path.isfile( configURI ) and 
             os.path.getsize( configURI ) ):
            try:
                xmldoc = xml.dom.minidom.parse( configURI )    
            except xml.parsers.expat.ExpatError, value:
                self.panic("configuration file parsing failed, %s" % str( value ) )
                return False
            except xml.dom.DOMException, value:
                self.panic("configuration file parsing failed, %s" % str( value ) )
                return False


            if ( xmldoc ):
                self.debug("will try to extract <config> tags...")
                self.__handleConfigNodes( xmldoc )
                self.debug("done!")
                return xmldoc
            

    ## scan reference root file and dump its contents to dom xml nodes  
    # @param self "Me, myself and Irene"
    # @param target ROOT.TDirectory to scan
    # @param configNode XML DOM Element 
    # @param level level of recursion to pretty print
    def __scan( self, target, configNode, level=1 ):
            
        nbsp = " " * level 
        target.cd()
        path = target.GetPath().split(":")[1]
        curdir = ROOT.gDirectory
        nextkey = ROOT.TIter( curdir.GetListOfKeys() ) 

        for ik in range( target.GetNkeys() ):
            
            key = nextkey.Next()

            obj = key.ReadObj()
            name = obj.GetName()
            cl  = obj.Class().GetName()
            isa = obj.IsA()
            fp = None

            fp = os.path.join( path, name)
    
            self.debug( nbsp + "--> found " + name + " of type " + cl ) 

            if ( isa.InheritsFrom("TH1") ):

                objDim = obj.GetDimension()
                self.debug( nbsp + " --> found object '%s' at location '%s' of name '%s'" % ( cl, path, name) )

                if ( objDim <= 2 ):
                    objNode = self.xmldoc.createElement( "hist%dD" % objDim  )
                    objNode.setAttribute( "name", name )
                    objNode.setAttribute( "type", cl )
                    objNode.setAttribute( "plotopts", "" )
                    if ( "TProfile" in cl ):
                        if ( "meany" in self.opts.tests ):
                            objNode.setAttribute( "tests", "meany" ) 
                    else:
                        tests = self.opts.tests[:]
                        if ( "meany" in tests ): tests.remove("meany")
                        objNode.setAttribute( "tests", ",".join(tests)  )
                    configNode.appendChild( objNode )
                else:
                    self.warn("dimension > 2 for histogram '%s', skipping" % fp)

            elif ( isa.InheritsFrom("TGraph") ):

                self.debug( nbsp + " --> found object '%s' at location '%s' of name '%s'" % ( cl, path, name) )

                objNode = self.xmldoc.createElement( "graph" )
                objNode.setAttribute( "name", name )
                objNode.setAttribute( "type", cl )
                objNode.setAttribute( "plotopts", "" )
                configNode.appendChild( objNode )
                
            #elif ( isa.InheritsFrom("TGraph2D") ):
            #    self.debug( nbsp + " --> found object %s at location %s of name %s" % ( cl, path, name) )

            #    objNode = self.xmldoc.createElement( cl )
            #    objNode.setAttribute( "name", name )
            #    configNode.appendChild( objNode )
               
            elif ( isa.InheritsFrom("TDirectory") ):
                self.debug( nbsp +" --> found TDirectory=" + fp )
                
                dirNode = self.xmldoc.createElement("TDirectory")
                dirNode.setAttribute( "name", name )
                
                configNode.appendChild( dirNode )
                
                self.__scan( obj, dirNode, level+1 )
           
            else:
                self.warn( nbsp +" --> unsupported object of type '%s' found at '%s'" % ( cl, fp ) )  
                pass
   
        return configNode       

            
    ## 
    # @param self "Me, myself and Irene"
    def __DOMXML( self ):
        return self.xmldoc
  
##
# @class test_DCubeConfig
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @brief test case for DCubeConfig class
class test_DCubeConfig( unittest.TestCase ):
    

    ## test suite setup
    # @param self "Me, myself and Irene"
    def setUp( self ):

        import sys
        def sysExit( value ):
            pass
        sys.exit = sysExit
        
        DCubeUtils.DCubeLogger("test_DCubeConfig.log", "test_DCubeConfig", True )
        self.log =  DCubeUtils.DCubeLogger.getLogger( self )

        self.info = self.log.info 
        
        self.args = [ __file__,
                      "-c",  "dcube_config.xml",
                      "-r" , "reference.root",
                      "-t", "bbb", "KS",
                      "--branch", "dev",
                      "--project", "AtlasProduction" ]
        self.parser = DCubeOptParser()
        self.parsed =  self.parser.parse( self.args )



        self.config = DCubeConfig( self.parsed )
        

    ## test constructor
    # @param self "Me, myself and Irene"
    def test_01_constructor( self ):
        try:
            self.config = DCubeConfig( self.parsed )
        except:
            pass
        self.assertEqual( isinstance( self.config, DCubeConfig), True )

    ## test generate()
    # @param self "Me, myself nad Irene"    
    def test_02_generate( self ):       
        self.config.generate()

    ## test get()
    # @param self "Me, myself and Irene"
    def test_03_get( self ):
        configNode = self.config.getConfig( strict = False )
        print configNode
        



if __name__ == "__main__":
        
    testLoader = unittest.TestLoader()
    suite = testLoader.loadTestsFromTestCase(test_DCubeConfig)      
    unittest.TextTestRunner(verbosity=3).run(suite)


