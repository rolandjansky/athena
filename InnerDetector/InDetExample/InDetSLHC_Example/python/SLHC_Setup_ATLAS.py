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

class SLHC_Setup :
    # constructor requires the SLHC_Flags
    def __init__(self):

        import os, shutil
        from PyJobTransformsCore.envutil import *
        
        #--------------------------------------------------------------
        # XML reader
        #-------------------------------------------------------------

        ###### Setup ITk XML reader ######
        from InDetTrackingGeometryXML.InDetTrackingGeometryXMLConf import InDet__XMLReaderSvc
        xmlReader = InDet__XMLReaderSvc(name='InDetXMLReaderSvc')

        PixelLayout = "ATLAS"
        SCTLayout   = "ATLAS"
#        VeryForward = False
        dictionaryFileName = "InDetIdDictFiles/IdDictInnerDetector_SLHC-23-24-dev9.xml"
        createXML = True
        doPix = True
        doSCT = True
        
        PIXMODULEFILE = "PixelModules.xml"
        PIXSTAVEFILE  = str(PixelLayout) + "_PixelStave.xml"
        PIXBARRELFILE = str(PixelLayout) + "_PixelBarrel.xml"
        PIXENDCAPFILE = str(PixelLayout) + "_PixelEndcap.xml"

        SCTMODULEFILE = str(SCTLayout) + "_SCTModules.xml"
        SCTSTAVEFILE  = str(SCTLayout) + "_SCTStave.xml"
        SCTBARRELFILE = str(SCTLayout) + "_SCTBarrel.xml"
        SCTENDCAPFILE = str(SCTLayout) + "_SCTEndcap.xml"

##         if VeryForward:
##             PIXSTAVEFILE  = str(PixelLayout) + "_PixelStave_VF.xml"
##             PIXBARRELFILE = str(PixelLayout) + "_PixelBarrel_VF.xml"
##             PIXENDCAPFILE = str(PixelLayout) + "_PixelEndcap_VF.xml"

        ## ###### Setup dictionary file to use  ######
        ## dictDir = os.path.dirname(dictionaryFileName)
        ## if not os.path.exists(dictDir):
        ##     os.makedirs(dictDir)

        ###### Setup XML builders properties ######
        xmlReader.dictionaryFileName  = dictionaryFileName
        xmlReader.createXMLDictionary = createXML
        xmlReader.doPix = doPix
        xmlReader.doSCT = doSCT

        ###### Setup XML files for Material ######
        xmlReader.XML_Materials         = find_file_env("Materials.xml",'DATAPATH')

        ###### Setup XML filesfor pixels ######
        xmlReader.XML_PixelModules      = find_file_env(str(PIXMODULEFILE),'DATAPATH')
        xmlReader.XML_PixelStaves       = find_file_env(str(PIXSTAVEFILE),'DATAPATH')
        xmlReader.XML_PixelBarrelLayers = find_file_env(str(PIXBARRELFILE),'DATAPATH')
        xmlReader.XML_PixelEndcapLayers = find_file_env(str(PIXENDCAPFILE),'DATAPATH')
        ###### Setup XML files for SCT ######
        xmlReader.XML_SCTModules      = find_file_env(str(SCTMODULEFILE),'DATAPATH')
        xmlReader.XML_SCTStaves       = find_file_env(str(SCTSTAVEFILE),'DATAPATH')
        xmlReader.XML_SCTBarrelLayers = find_file_env(str(SCTBARRELFILE),'DATAPATH')
        xmlReader.XML_SCTEndcapLayers = find_file_env(str(SCTENDCAPFILE),'DATAPATH')


        from AthenaCommon.AppMgr import theApp
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        from AthenaCommon.AppMgr import ToolSvc as toolSvc

        svcMgr += xmlReader
        theApp.CreateSvc.insert(0,"InDet::XMLReaderSvc/InDetXMLReaderSvc")

        print svcMgr

        # Only use local text file and dictionary if SLHC_Version set
        if (SLHC_Flags.SLHC_Version() and not (SLHC_Flags.SLHC_Version() == 'None')) : 

            # get the environment variable datapath
            data_path = os.environ.get('DATAPATH')
            # database file name
            database_file = SLHC_Flags.LocalGeometryDbBaseName()+SLHC_Flags.SLHC_Version()+'.txt'
            # dictionary file 
            dict_file = SLHC_Flags.LocalDictionaryBaseName()+SLHC_Flags.SLHC_Version()+'.xml'
            # search the file in the path
            database_file_path = self.search_file(database_file,data_path)
            dict_file_path = self.search_file(dict_file,data_path)
            # screen output
            print '[ SLHC ] Local geometry database: ',database_file
            print '           - found in: ',database_file_path
            print '[ SLHC ] Local dictionary: ',dict_file
            print '           - found in: ',dict_file_path

            # the database 
            database_full_path_name = database_file_path+'/'+database_file

            # Pass text file name to GeometryDBSvc
            if not hasattr(svcMgr,'InDetGeometryDBSvc'):
                from GeometryDBSvc.GeometryDBSvcConf import GeometryDBSvc
                svcMgr+=GeometryDBSvc("InDetGeometryDBSvc")
            svcMgr.InDetGeometryDBSvc.TextFileName = database_full_path_name
            svcMgr.InDetGeometryDBSvc.Sections = ["Pixel","SCT","InDetServMat"]

            # Deprecated: this is for old SCT text files
            os.environ["LocalSlhcGeometryDatabase"]=database_full_path_name

##             # dictionary creation
##             from DetDescrCnvSvc.DetDescrCnvSvcConf import DetDescrCnvSvc
##             DetDescrCnvSvc = DetDescrCnvSvc()
##             DetDescrCnvSvc.IdDictFromRDB = False
##             DetDescrCnvSvc.InDetIDFileName = dict_file_path+'/'+dict_file

        else:
            print 'SLHC_Setup: Geometry coming fully from database'
            
##         # Alignments have to disabled for Pixels
##         pixelTool = svcMgr.GeoModelSvc.DetectorTools['PixelDetectorTool']
##         pixelTool.Alignable = False

        # GeoModelConfiguration 

        xmlFileDict={}
        xmlFileDict["Pixel"]={
            "PIXELGENERAL":"AlpinePixelGeneral",
#            "PIXELSIMPLESERVICE":"PixelSimpleService",
            "SILICONMODULES":"PixelModules",
            "SILICONREADOUT":"PixelModuleReadout",
            "STAVESUPPORT":"ATLASStaveSupport",
            "MATERIAL":"ATLASMaterial",
#            "PIXELROUTINGSERVICE":"AlpinePixelRoutingService",
            }

        for subDet in ["Pixel"]:
            for key in xmlFileDict[subDet].keys():
                fileName=xmlFileDict[subDet][key]+".xml"
                envName=subDet.upper()+"_"+key+"_GEO_XML"
                os.environ[envName]=fileName
                print "ENV ",envName," ",fileName


        # Service used to build module geometry
        from PixelModuleTool.PixelModuleToolConf import PixelModuleBuilder
        moduleGeomBuilder=PixelModuleBuilder(name="PixelModuleSvc")
        svcMgr+=moduleGeomBuilder

        # Service used to build module design
        from PixelModuleTool.PixelModuleToolConf import PixelDesignBuilder
        moduleDesignBuilder=PixelDesignBuilder(name="PixelDesignSvc")
        svcMgr+=moduleDesignBuilder
                    
        from PixelServicesTool.PixelServicesToolConf import PixelServicesTool
        serviceTool=PixelServicesTool(name="PixelServicesTool")
        serviceTool.ReadSvcFromDB = True
        serviceTool.SvcDynAutomated = False
        toolSvc+=serviceTool


        print "******************************************************************************************"
        
        print "PixelGeoModel - import GeoPixelBarrelLoITool"
        from PixelLayoutLoI.PixelLayoutLoIConf import GeoPixelBarrelLoITool
        geoBarrelTool=GeoPixelBarrelLoITool(name="GeoPixelBarrelLoITool")
        geoBarrelTool.PixelServicesTool = serviceTool
        toolSvc+=geoBarrelTool
        
        
        print "******************************************************************************************"
        
        print "PixelGeoModel - import GeoPixelEndcapLoITool"
        from PixelLayoutLoI.PixelLayoutLoIConf import GeoPixelEndcapLoITool
        geoEndcapTool=GeoPixelEndcapLoITool(name="GeoPixelEndcapLoITool")
        geoEndcapTool.PixelServicesTool = serviceTool
        toolSvc+=geoEndcapTool
        
        print "******************************************************************************************"
        
        print "PixelGeoModel - import GeoPixelEnvelopeLoITool"
        from PixelLayoutLoI.PixelLayoutLoIConf import GeoPixelEnvelopeLoITool
        geoEnvelopeTool=GeoPixelEnvelopeLoITool(name="GeoPixelEnvelopeLoITool")
        geoEnvelopeTool.GeoPixelBarrelTool=geoBarrelTool
        geoEnvelopeTool.GeoPixelEndcapTool=geoEndcapTool
        geoEnvelopeTool.PixelServicesTool = serviceTool
        toolSvc+=geoEnvelopeTool


        print "******************************************************************************************"

        pixelTool = svcMgr.GeoModelSvc.DetectorTools['PixelDetectorTool']
        pixelTool.Alignable = False
        pixelTool.FastBuildGeoModel = True
        pixelTool.ConfigGeoAlgTool = True
        pixelTool.ConfigGeoBase = "GeoPixelEnvelopeLoITool"
        

    def search_file(self,filename, search_path):
        """Given a search path, find file
           -- will return the first occurrence
        """
        file_found = 0
        paths = search_path.split(os.pathsep)
        for path in paths:
            if exists(join(path, filename)):
                file_found = 1
                break
        if file_found:
            return path
        else:
            return None
