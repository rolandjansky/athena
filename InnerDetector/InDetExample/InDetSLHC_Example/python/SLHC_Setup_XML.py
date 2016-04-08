# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" SLHC_Setup
    Python module to hold storegate keys of InDet objects.
"""

__author__ =   "A. Salzburger"
__version__=   "$Revision: 1.13 $"
__doc__    =   "SLHC_PathSetting"
__all__    = [ "SLHC_PathSetting" ]

import os
from os.path import exists, join
from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags

class SLHC_Setup_XMLReader :
    # constructor requires the SLHC_Flags

    def __init__(self,**kwargs):

        from InDetTrackingGeometryXML.XMLReaderJobProperties import XMLReaderFlags
        print dir(XMLReaderFlags)
        if XMLReaderFlags.Initialized(): return

        import os, shutil
        from PyJobTransformsCore.envutil import find_file_env

        print "*******************************************************************************************"
        print "*******************************************************************************************"
        print "*******************************************************************************************"

        for key in kwargs:
            print "another keyword arg: %s: %s" % (key, kwargs[key])
        
        #--------------------------------------------------------------
        # XML reader
        #-------------------------------------------------------------

        ###### Setup ITk XML reader ######
        from InDetTrackingGeometryXML.InDetTrackingGeometryXMLConf import InDet__XMLReaderSvc
        xmlReader = InDet__XMLReaderSvc(name='InDetXMLReaderSvc')

        dictionaryFileName = ""
        if kwargs.has_key("dictionaryFileName"): dictionaryFileName = kwargs["dictionaryFileName"]
        createXML = False
        if kwargs.has_key("createXML"): createXML = kwargs["createXML"]
        doPix = kwargs["doPix"]
        doSCT = kwargs["doSCT"]
        
        PIXMODULEFILE = "ITK_PixelModules.xml"
        PIXSTAVEFILE  = str(kwargs["PixelLayout"]) + "_PixelStave.xml"
        PIXBARRELFILE = str(kwargs["PixelLayout"]) + "_PixelBarrel.xml"
        PIXENDCAPFILE = str(kwargs["PixelLayout"]) + "_PixelEndcap.xml"
        if kwargs.has_key("PixelEndcapLayout"): PIXENDCAPFILE = str(kwargs["PixelEndcapLayout"]) + "_PixelEndcap.xml"
        
        SCTMODULEFILE = "ITK_SCTModules.xml"
        SCTSTAVEFILE  = str(kwargs["SCTLayout"]) + "_SCTStave.xml"
        SCTBARRELFILE = str(kwargs["SCTLayout"]) + "_SCTBarrel.xml"
        SCTENDCAPFILE = str(kwargs["SCTLayout"]) + "_SCTEndcap.xml"

        ## ###### Setup dictionary file to use  ######
        ## dictDir = os.path.dirname(dictionaryFileName)
        ## if not os.path.exists(dictDir):
        ##     os.makedirs(dictDir)

        ###### Setup XMLreader flags
        print "**** FLAGS **************************************************"
        XMLReaderFlags.setValuesFromSetup( PixelBarrelLayout = find_file_env(str(PIXBARRELFILE),'DATAPATH'),
                                           PixelEndcapLayout = find_file_env(str(PIXENDCAPFILE),'DATAPATH'),
                                           SCTBarrelLayout = find_file_env(str(SCTBARRELFILE),'DATAPATH'),
                                           SCTEndcapLayout = find_file_env(str(SCTENDCAPFILE),'DATAPATH'),
                                           doPix = kwargs["doPix"],
                                           doSCT = kwargs["doSCT"]
                                           )
        XMLReaderFlags.dump()
        print "******************************************************"

        ###### Setup XML builders properties ######
        xmlReader.dictionaryFileName  = kwargs["dictionaryFileName"]
        xmlReader.createXMLDictionary = False
        if kwargs.has_key("createXML"): xmlReader.createXMLDictionary = kwargs["createXML"]
        xmlReader.doPix = XMLReaderFlags.doPix()
        xmlReader.doSCT = XMLReaderFlags.doSCT()

        ###### Setup XML files for Material ######
        xmlReader.XML_Materials         = find_file_env("Materials.xml",'DATAPATH')

        ###### Setup XML filesfor pixels ######
        xmlReader.XML_PixelModules      = find_file_env(str(PIXMODULEFILE),'DATAPATH')
        xmlReader.XML_PixelStaves       = find_file_env(str(PIXSTAVEFILE),'DATAPATH')
        xmlReader.XML_PixelBarrelLayers = XMLReaderFlags.PixelBarrelLayout()
        if XMLReaderFlags.PixelEndcapLayout()!='UNDEFINED' :
            xmlReader.XML_PixelEndcapLayers = XMLReaderFlags.PixelEndcapLayout()
        ###### Setup XML files for SCT ######
        xmlReader.XML_SCTModules      = find_file_env(str(SCTMODULEFILE),'DATAPATH')
        xmlReader.XML_SCTStaves       = find_file_env(str(SCTSTAVEFILE),'DATAPATH')
        xmlReader.XML_SCTBarrelLayers = XMLReaderFlags.SCTBarrelLayout()
        xmlReader.XML_SCTEndcapLayers = XMLReaderFlags.SCTEndcapLayout()



        from AthenaCommon.AppMgr import theApp
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        svcMgr += xmlReader
        theApp.CreateSvc.insert(0,"InDet::XMLReaderSvc/InDetXMLReaderSvc")
#        print svcMgr
