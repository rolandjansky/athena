#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
##
# @file DCubeClient/python/dcubeConvert_old.py
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# 
import os, sys
from time import strftime, localtime, strptime
import xml.dom.minidom
import ROOT

##
# @class DCubeConvert
# @author Krzysztof Daniel Ciba (Krzysztof.Ciba@NOSPAMgmail.com)
# @brief converter for XML DCubeClient configuration files
class DCubeConvert( object ):

    oldURI = None
    newURI = None
    referenceURI = None

    ## c'tor
    # @param self "Me, myself nad Irene"
    # @param old old configuration file
    # @param new new configuration file
    def __init__( self, old, ref=None, new=None):

        print "dcubeConvert arguments:"
        whatOLD = old
        whatREF = ref
        whatNEW = new
        if ( not whatREF ):
            whatREF = "not specified, will be taken from old configuration file"
        if ( not whatNEW ):
            whatNEW = "not specified, will be set to old configuration file"

        print "[01] old dcube configuration file: %s" % str(whatOLD)
        print "[02]          reference root file: %s" % str(whatREF)
        print "[03] new dcube configuration file: %s" % str(whatNEW)

        if ( not new ):
            new = old
            
        print "will convert old DCubeClient configuration file '%s' and save new configuration to file '%s'" % ( old, new )
        
        self.oldURI = os.path.abspath(old)
        self.newURI = os.path.abspath(new)

        if ( not os.path.isfile( self.oldURI) ):
            sys.exit("cannot find old DCubeClient config in path %s - exiting!" % self.oldURI )

        if ( self.oldURI == self.newURI ):
            print "DCubeClient configuration file %s exists and will be overwritten" % self.oldURI
        
        if ( ref ):
            self.referenceURI = os.path.abspath( ref )
            print "will use '%s' as reference root file" % self.referenceURI
            if ( not os.path.exists( self.referenceURI ) ):
                sys.exit("reference file '%s' does not exist!" % self.referenceURI )
        sys.exit( self.convert() )

    ## get CDATA from node list
    # @param self "Me, myself and Irene"
    # @param nodeList list of XML DOM Nodes
    def __cdata( self, nodeList ):
        out = ""
        for node in nodeList:
            if node.nodeType == node.TEXT_NODE:
                out += node.data
        return out
        
        pass

    ## parse old configuration file
    # @param self "Me, myself and Irene"
    def __parseOLD( self ):

        print "parsing old configuration file %s" % self.oldURI
        
        self.xmlOLD =  xml.dom.minidom.parse( self.oldURI )

        if ( self.xmlOLD.documentElement.tagName != "cfg" ):
            sys.exit("%s isn't an old DCubeClient configuration file, root node name is different form <cfg>" % os.path.basename(self.oldURI ))
        else:
            
            if ( not self.referenceURI ):
                self.referenceURI = self.__cdata( self.xmlOLD.getElementsByTagName("reference")[0].childNodes )

            print "OLD: reference URI=%s" % self.referenceURI

            if ( not os.path.exists(self.referenceURI ) ):
                sys.exit( "reference root file not exists!")
                
            
            self.refDesc = self.__cdata( self.xmlOLD.getElementsByTagName("ref_desc")[0].childNodes )

            print "OLD: reference description=%s" % self.refDesc

            self.monDesc = self.__cdata( self.xmlOLD.getElementsByTagName("mon_desc")[0].childNodes )

            print "OLD: monitored description=%s" % self.monDesc
            
            self.plimit = self.xmlOLD.getElementsByTagName("plimit")[0]

            print "OLD: plimit attributes fail=%s warn=%s" % ( self.plimit.getAttribute("fail"), self.plimit.getAttribute("warn"))

            self.hists = self.xmlOLD.getElementsByTagName("hist")

            i = 1
            for hist in self.hists:
                print "OLD: [%02d] hist path=%s tests=%s" % ( i, hist.getAttribute("path"), hist.getAttribute("tests") )
                i += 1
                
    ## create new XML DOM Document instance 
    # @param self "Me, myself and Irene"
    def __createNEW( self ):
        self.xmlNEW = xml.dom.minidom.Document()
        self.dcubeClientConfig = self.xmlNEW.createElement("dcube_client_config")
        self.xmlNEW.appendChild( self.dcubeClientConfig )
        self.dcubeClientConfig.appendChild( self.__mainComment() )

        self.dcube = self.xmlNEW.createElement("dcube")
        self.dcube.setAttribute("branch", "*")
        self.dcube.setAttribute("install", "*")
        self.dcube.setAttribute("cmtconfig", "*")
        self.dcube.setAttribute("project", "*")
        self.dcube.setAttribute("jobId", "*")

        reference = self.xmlNEW.createElement( "reference" )
        reference.setAttribute("path", self.referenceURI )

        self.dcube.appendChild( reference )
        
        refDesc = self.xmlNEW.createElement("ref_desc")
        refDescCDATA = self.xmlNEW.createTextNode( self.refDesc )
        refDesc.appendChild( refDescCDATA )

        self.dcube.appendChild( refDesc )

        testDesc = self.xmlNEW.createElement( "test_desc" )
        testDescCDATA = self.xmlNEW.createTextNode( self.monDesc )
        testDesc.appendChild( testDescCDATA )

        self.dcube.appendChild( testDesc )

        self.dcube.appendChild( self.plimit )

        rootTDir = self.xmlNEW.createElement("TDirectory")
        rootTDir.setAttribute("name", "/")

        self.dcube.appendChild( rootTDir )

        
        self.refROOT = ROOT.TFile().Open( self.referenceURI, "READ" )

        for hist in self.hists:
            xmlpath =  hist.getAttribute("path") 
            if ( xmlpath.count("/") and xmlpath[0] == "/" ):
                xmlpath = xmlpath[1:]
            obj = self.refROOT.Get( xmlpath )
            
            if ( obj ):
                type = obj.ClassName()
                path = hist.getAttribute("path").split("/")[1:]
                tests = hist.getAttribute("tests")
                self.__attach( path, rootTDir, tests, type )
                print "NEW: obj type=%s path=%s tests=%s has been added" % ( type, xmlpath, tests) 
            else:
                print "NEW: cannot find object at %s" % xmlpath
                

        self.refROOT.Close()
        self.dcubeClientConfig.appendChild( self.dcube )

        
    ## create XML DOM Comment node
    # @param self "Me, myself and Irene"
    def __mainComment( self ):
        com  = "\n  DCubeClient configuration file '%s'" % os.path.basename(self.newURI)
        com += "\n  converted from old configuration file '%s'" % os.path.basename(self.oldURI)
        com += "\n  using dcubeConvert.py script"
        com += "\n  on %s " % strftime("%a, %d %b %Y %H:%M:%S %Z" , localtime())
        return self.xmlNEW.createComment( com )
        
    ## create backup file
    # @param self "Me, myself and Irene"
    # @param path path to file
    def __backup( self, path ):

        backup = path + ".bak"
        if ( os.path.exists( backup ) ):
            print "BCK: backup file exists and will be overwitten!"
        try:
            os.rename( path, backup )
        except:
            print "BCK: creation of backup file failed"


    ## save new DCubeClient configuration to file
    # @param self "Me, myself and Irene"
    def __save( self ):

        if ( os.path.exists(self.newURI ) ): self.__backup( self.newURI )
        try:
            xmlNEW = open( self.newURI, "w+" )
            xmlNEW.writelines( self.xmlNEW.toprettyxml() )
            xmlNEW.close()
            print "NEW: new config file '%s' has been created" % self.newURI
        except:
            sys.exit("error when saving new configuration file!")
        

    ## perform conversion
    # @param self "Me, myself and Irene"
    def convert( self ):
        self.__parseOLD( )
        self.__createNEW()
        self.__save()
        return 0

    ## attach child node to parent
    # @param self "Me, myself and Irene"
    # @param what list of path chunks
    # @param where path inside reference root file
    def __attach( self, what, where, tests, type ):

        histName = what[-1]
        parent = what[0]
        
        if ( len(what) == 1 ):
            nodeName = ""
            if ( "TH1" in type ): nodeName = "hist1D"
            if ( "TH2" in type ): nodeName = "hist2D"
            if ( "TProfile" == type ): nodeName == "hist1D"
            if ( "TProfile2D" == type ): nodeName = "hist2D"
            if ( "TGraph" in type and "2" not in type ): nodeName = "graph"
            
            if ( nodeName != ""  ):
                histNode = self.xmlNEW.createElement( nodeName )
                histNode.setAttribute("tests", tests)
                histNode.setAttribute("name", histName)
                histNode.setAttribute( "type", type)
                
                print "NEW: attaching %s '%s' with tests '%s' at TDirectory '%s'" % ( type, histName, tests, where.getAttribute("name"))
                where.appendChild( histNode )
                return
            else:
                print "unsupported object name='%s' type='%s' found, skipping!" % ( histName, nodeName)
        else:
            subDirs = where.getElementsByTagName("TDirectory")
            subDirsDict = { }

            for subDir in subDirs:
                subDirsDict[subDir.getAttribute( "name")] =  subDir 
    
            if ( parent not in subDirsDict ):
                print "NEW: creating TDirectory name '%s'" % parent
                tdir = self.xmlNEW.createElement( "TDirectory" )
                tdir.setAttribute("name", parent )
                where.appendChild( tdir )
                print "NEW: calling recursive: what '%s' " % str(what[1:])
                self.__attach( what[1:], tdir, tests, type )
            else:
                print "NEW: calling recursive: what '%s' " % str(what[1:])
                self.__attach( what[1:], subDirsDict[parent], tests, type )
        

        pass

## script execution
if __name__ == "__main__":

    usage = "Usage: $sh> dcubeConvert.py OLDCONFIG.XML [REFERENCE.root] [NEWCONFIG.XML]"

    old = None
    ref = None
    new = None

    if ( len(sys.argv) == 1 ):
        print usage
        sys.exit(0)
    if ( len(sys.argv) == 2 ):
        old = sys.argv[1]
        DCubeConvert( old, ref, new )
    if ( len(sys.argv) == 3 ):
        old = sys.argv[1]
        what = sys.argv[2]
        if ( what.endswith(".root") ): ref = what
        elif ( what.endswith(".xml") ): new = what
        DCubeConvert( old, ref, new )
    elif ( len(sys.argv) == 4 ):
        old = sys.argv[1]
        what1 = sys.argv[2]
        what2  = sys.argv[3]
        if ( what1.endswith(".root") ): ref = what1
        if ( what1.endswith(".xml") ): new = what1
        if ( not ref and what2.endswith(".root") ): ref = what2
        if ( not new and what2.endswith(".xml") ): new = what2
        DCubeConvert(old, ref, new)
    else:
        print "ERROR: wrong arguments!"
        print usage
        sys.exit(1)
    
        
    

    

            
