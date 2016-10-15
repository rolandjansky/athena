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
    def __init__(self):

        # XMLReader setup
        from SLHC_Setup_XML import SLHC_Setup_XMLReader
        SLHC_Setup_XMLReader(PixelLayout = "BrlInclRef",
                             PixelEndcapLayout = "ECRingRef",
                             SCTLayout = "LoI",
                             dictionaryFileName = "InDetIdDictFiles/IdDictInnerDetector_SLHC-24-24-dev4.xml",
                             createXML = False,
                             doPix=True,
                             doSCT=False,
                             )

class SLHC_Setup :
    # constructor requires the SLHC_Flags
    def __init__(self):

        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        from AthenaCommon.AppMgr import ToolSvc as toolSvc

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
            
##        # Alignments have to disabled for Pixels
##        pixelTool = svcMgr.GeoModelSvc.DetectorTools['PixelDetectorTool']
##        pixelTool.Alignable = False

        # GeoModelConfiguration 

        xmlFileDict={}
        xmlFileDict["Pixel"]={
            "PIXELGENERAL":"BrlInclPixelGeneral",
            "PIXELSIMPLESERVICE":"BrlInclPixelSimpleService",
            "SILICONMODULES":"SiliconModules",
            "SILICONREADOUT":"PixelModuleReadout",
            "STAVESUPPORT":"BrlInclStaveSupport",
            "ENDCAP":"ECRingRef_PixelEndcap",
            "MATERIAL":"BrlInclMaterial",
            "PIXELROUTINGSERVICE":"BrlInclPixelRoutingService",
            }

        for subDet in ["Pixel"]:
            for key in xmlFileDict[subDet].keys():
                fileName=xmlFileDict[subDet][key]+".xml"
                envName=subDet.upper()+"_"+key+"_GEO_XML"
                os.environ[envName]=fileName
                print "ENV ",envName," ",fileName


        print "******************************************************************************************"
        print "PixelGeoModel - import module and design tools"
        # Service used to build module geometry
        from PixelModuleTool.PixelModuleToolConf import PixelModuleBuilder
        moduleGeomBuilder=PixelModuleBuilder(name="PixelModuleSvc")
        svcMgr+=moduleGeomBuilder

        # Service used to build module design
        from PixelModuleTool.PixelModuleToolConf import PixelDesignBuilder
        moduleDesignBuilder=PixelDesignBuilder(name="PixelDesignSvc")
        svcMgr+=moduleDesignBuilder
                    
        print "******************************************************************************************"
        print "PixelGeoModel - import PixelServiceTool"
        from PixelServicesTool.PixelServicesToolConf import PixelServicesTool
        serviceTool=PixelServicesTool(name="PixelServicesTool")
        serviceTool.ReadSvcFromDB = True
        serviceTool.SvcDynAutomated = False
        toolSvc+=serviceTool

        print "******************************************************************************************"
        print "PixelGeoModel - import GeoPixelLayerInclRefTool"
        from BarrelInclinedRef.BarrelInclinedRefConf import GeoPixelLayerInclRefTool
        geoLayerInnerTool=GeoPixelLayerInclRefTool(name="InnerPixelLayerTool")
        toolSvc+=geoLayerInnerTool
        from BarrelInclinedRef.BarrelInclinedRefConf import GeoPixelLayerPlanarRefTool
        geoLayerOuterTool=GeoPixelLayerPlanarRefTool(name="OuterPixelLayerTool")
        toolSvc+=geoLayerOuterTool

        print "PixelGeoModel - import GeoPixelBarrelInclRefTool"
        from BarrelInclinedRef.BarrelInclinedRefConf import GeoPixelBarrelInclRefTool
        geoBarrelTool=GeoPixelBarrelInclRefTool(name="GeoPixelBarrelInclRefTool")
        geoBarrelTool.PixelServicesTool = serviceTool
        geoBarrelTool.InnerPixelLayerTool = geoLayerInnerTool
        geoBarrelTool.OuterPixelLayerTool = geoLayerOuterTool
        toolSvc+=geoBarrelTool

        print "******************************************************************************************"
        print "PixelGeoModel - import GeoPixelLayerECRingRefTool"
        from EndcapRingRef.EndcapRingRefConf import GeoPixelLayerECRingRefTool
        geoECLayerTool=GeoPixelLayerECRingRefTool(name="GeoPixelLayerECRingRefTool")
        toolSvc+=geoECLayerTool
        
        print "PixelGeoModel - import GeoPixelEndcapECRingRefTool"
        from EndcapRingRef.EndcapRingRefConf import GeoPixelEndcapECRingRefTool
        geoEndcapTool=GeoPixelEndcapECRingRefTool(name="GeoPixelEndcapECRingRefTool")
        geoEndcapTool.GeoPixelEndcapLayerTool = geoECLayerTool
        geoEndcapTool.PixelServicesTool = serviceTool
        toolSvc+=geoEndcapTool

        print "******************************************************************************************"
        print "PixelGeoModel - import GeoPixelEnvelopeInclRefTool"
        from BarrelInclinedRef.BarrelInclinedRefConf import GeoPixelEnvelopeInclRefTool
        geoEnvelopeTool=GeoPixelEnvelopeInclRefTool(name="GeoPixelEnvelopeInclRefTool")
        geoEnvelopeTool.GeoPixelBarrelTool = geoBarrelTool
        geoEnvelopeTool.GeoPixelEndcapTool = geoEndcapTool
        geoEnvelopeTool.PixelServicesTool = serviceTool
        toolSvc+=geoEnvelopeTool

        print "******************************************************************************************"

        pixelTool = svcMgr.GeoModelSvc.DetectorTools['PixelDetectorTool']
        pixelTool.Alignable = False
        pixelTool.FastBuildGeoModel = True
        pixelTool.ConfigGeoAlgTool = True
        pixelTool.ConfigGeoBase = "GeoPixelEnvelopeInclRefTool"


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
