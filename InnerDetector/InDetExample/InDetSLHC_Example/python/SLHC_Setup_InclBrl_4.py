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

from AtlasGeoModel.InDetGMJobProperties import GeometryFlags
auto_isGMX = (SLHC_Flags.doGMX()) or (GeometryFlags.StripGeoType() == "GMX") 

class SLHC_Setup_XMLReader :
    # constructor requires the SLHC_Flags
    def __init__(self):

        # XMLReader setup
        from SLHC_Setup_XML import SLHC_Setup_XMLReader
        if (SLHC_Flags.LayoutOption == "LightBrlv2" ):
            SLHC_Setup_XMLReader(PixelLayout = "InclBrl4Ref_Light",
                                 PixelEndcapLayout = "ECRing4Ref_Light",
                                 SCTLayout = "FourLayersNoStub_23-25-dev0",
                                 dictionaryFileName = "InDetIdDictFiles/IdDictInnerDetector_SLHC_InclBrl_4.xml",
                                 createXML = True,
                                 doPix=True,
                                 doSCT=True,
                                 isGMX=auto_isGMX,
                                 )

        elif (SLHC_Flags.LayoutOption == "LightBrl" ):
            SLHC_Setup_XMLReader(PixelLayout = "InclBrl4Ref_LightBrl",
                                 PixelEndcapLayout = "ECRing4Ref",
                                 SCTLayout = "FourLayersNoStub_23-25-dev0",
                                 dictionaryFileName = "InDetIdDictFiles/IdDictInnerDetector_SLHC_InclBrl_4.xml",
                                 createXML = True,
                                 doPix=True,
                                 doSCT=True,
                                 isGMX=auto_isGMX,
                                 )

        elif (SLHC_Flags.LayoutOption == "RobustBrl" ):
            SLHC_Setup_XMLReader(PixelLayout = "InclBrl4Ref_Robust",
                                 PixelEndcapLayout = "ECRing4Ref_Robust",
                                 SCTLayout = "FourLayersNoStub_23-25-dev0",
                                 dictionaryFileName = "InDetIdDictFiles/IdDictInnerDetector_SLHC_InclBrl_4.xml",
                                 createXML = True,
                                 doPix=True,
                                 doSCT=True,
                                 isGMX=auto_isGMX,
                                 )

        elif (SLHC_Flags.LayoutOption == "InclinedQuads" ):
            SLHC_Setup_XMLReader(PixelLayout = "InclBrl4Ref_InclinedQuads",
                                 PixelEndcapLayout = "ECRing4Ref_InclinedQuads",
                                 SCTLayout = "FourLayersNoStub_23-25-dev0",
                                 dictionaryFileName = "InDetIdDictFiles/IdDictInnerDetector_SLHC_InclBrl_4.xml",
                                 createXML = True,
                                 doPix=True,
                                 doSCT=True,
                                 isGMX=auto_isGMX,
                                 addBCL=False, #True # If you want to set this to True, you must also change InnerDetector/InDetDetDescr/GmxLayouts/cmt/requirements to point to the Strips layout containing the BCL, otherwise Reco will segfault - Ben
                                 )

        elif (SLHC_Flags.LayoutOption == "InclinedDuals" or SLHC_Flags.LayoutOption == "InclinedDuals_wFlex"):
            SLHC_Setup_XMLReader(PixelLayout = "InclBrl4Ref_InclinedDuals",
                                 PixelEndcapLayout = "ECRing4Ref_InclinedDuals",
                                 SCTLayout = "FourLayersNoStub_23-25-dev0",
                                 dictionaryFileName = "InDetIdDictFiles/IdDictInnerDetector_SLHC_InclBrl_4.xml",
                                 createXML = True,
                                 doPix=True,
                                 doSCT=True,
                                 isGMX=auto_isGMX,
                                 addBCL=False, #True # If you want to set this to True, you must also change InnerDetector/InDetDetDescr/GmxLayouts/cmt/requirements to point to the Strips layout containing the BCL, otherwise Reco will segfault - Ben
                                 )

        elif (SLHC_Flags.LayoutOption == "InclinedAlternative"):
            SLHC_Setup_XMLReader(PixelLayout = "InclBrl4Ref_InclinedAlternative",
                                 PixelEndcapLayout = "ECRing4Ref_InclinedAlternative",
                                 SCTLayout = "FourLayersNoStub_23-25-dev0",
                                 dictionaryFileName = "InDetIdDictFiles/IdDictInnerDetector_SLHC_InclBrl_4.xml",
                                 createXML = True,
                                 doPix=True,
                                 doSCT=True,
                                 isGMX=auto_isGMX,
                                 )          
        else:
            print "WARNING! No recognised value of SLHC_Flags.LayoutOption found. (SLHC_Flags.LayoutOption =",SLHC_Flags.LayoutOption,") In this case the Step 1.7 Inclined layout will be used."
            SLHC_Setup_XMLReader(PixelLayout = "InclBrl4Ref",
                                 PixelEndcapLayout = "ECRing4Ref",
                                 SCTLayout = "FourLayersNoStub_23-25-dev0",
                                 dictionaryFileName = "InDetIdDictFiles/IdDictInnerDetector_SLHC_InclBrl_4.xml",
                                 createXML = True,
                                 doPix=True,
                                 doSCT=True,
                                 isGMX=auto_isGMX,
                                 )

    
class SLHC_Setup :
    # constructor requires the SLHC_Flags
    def __init__(self):

        from InDetTrackingGeometryXML.XMLReaderJobProperties import XMLReaderFlags
        bReadXMLfromDB = XMLReaderFlags.readXMLfromDB()

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

        else:
            print 'SLHC_Setup: Geometry coming fully from database'
            

        # GeoModelConfiguration 

        xmlFileDict={}
        if (SLHC_Flags.LayoutOption == "LightBrlv2" ):
            xmlFileDict["Pixel"]={
                "PIXELGENERAL":"InclBrl4_PixelGeneral",
                "PIXELSIMPLESERVICE":"InclBrl_PixelSimpleService",
                "SILICONMODULES":"ITK_PixelModules",
                "SILICONREADOUT":"PixelModuleReadout",
                "STAVESUPPORT":"InclBrl4_Light_StaveSupport",
                "PIXELDISCSUPPORT":"InclBrl4_Light_DiskSupport",
                "MATERIAL":"InclBrl_Material",
                "PIXELROUTINGSERVICE":"InclBrl4_Light_PixelRoutingService",
                }
        elif (SLHC_Flags.LayoutOption == "LightBrl" ):
            xmlFileDict["Pixel"]={
                "PIXELGENERAL":"InclBrl4_PixelGeneral",
                "PIXELSIMPLESERVICE":"InclBrl_PixelSimpleService",
                "SILICONMODULES":"ITK_PixelModules",
                "SILICONREADOUT":"PixelModuleReadout",
                "STAVESUPPORT":"InclBrl4_LightBrl_StaveSupport",
                "PIXELDISCSUPPORT":"InclBrl4_DiskSupport",
                "MATERIAL":"InclBrl_Material",
                "PIXELROUTINGSERVICE":"InclBrl4_PixelRoutingService",
                }
        elif (SLHC_Flags.LayoutOption == "RobustBrl" ):
            xmlFileDict["Pixel"]={
                "PIXELGENERAL":"InclBrl4_PixelGeneral",
                "PIXELSIMPLESERVICE":"InclBrl_PixelSimpleService",
                "SILICONMODULES":"ITK_PixelModules",
                "SILICONREADOUT":"PixelModuleReadout",
                "STAVESUPPORT":"InclBrl4_Robust_StaveSupport",
                "PIXELDISCSUPPORT":"InclBrl4_Robust_DiskSupport",
                "MATERIAL":"InclBrl_Material",
                "PIXELROUTINGSERVICE":"InclBrl4_Robust_PixelRoutingService",
                }
        elif (SLHC_Flags.LayoutOption == "InclinedQuads" ):
            xmlFileDict["Pixel"]={
                "PIXELGENERAL":"InclBrl4_PixelGeneral_InclinedQuads",
                "PIXELSIMPLESERVICE":"InclBrl_PixelSimpleService",
                "SILICONMODULES":"ITK_PixelModules",
                "SILICONREADOUT":"PixelModuleReadout",
                "STAVESUPPORT":"InclBrl4_SlimStaveSupport_InclinedQuads",
                "PIXELDISCSUPPORT":"InclBrl4_InclinedQuads_DiskSupport",
                "MATERIAL":"InclBrl_Material",
                "PIXELROUTINGSERVICE":"InclBrl4_InclinedQuads_PixelRoutingService",
                }

        elif (SLHC_Flags.LayoutOption == "InclinedDuals" or SLHC_Flags.LayoutOption == "InclinedDuals_wFlex"):
            xmlFileDict["Pixel"]={
                "PIXELGENERAL":"InclBrl4_PixelGeneral_InclinedDuals",
                "PIXELSIMPLESERVICE":"InclBrl_PixelSimpleService",
                "SILICONMODULES":"ITK_PixelModules",
                "SILICONREADOUT":"PixelModuleReadout",
                "STAVESUPPORT":"InclBrl4_SlimStaveSupport_InclinedDuals",
                "PIXELDISCSUPPORT":"InclBrl4_InclinedDuals_DiskSupport",
                "MATERIAL":"InclBrl_Material",
                "PIXELROUTINGSERVICE":"InclBrl4_InclinedDuals_PixelRoutingService",
                }
            if(SLHC_Flags.LayoutOption == "InclinedDuals_wFlex"):
                xmlFileDict["Pixel"]["PIXELROUTINGSERVICE"]="InclBrl4_InclinedDuals_PixelRoutingService_wFlex"

        elif (SLHC_Flags.LayoutOption == "InclinedAlternative" ):
            xmlFileDict["Pixel"]={
                "PIXELGENERAL":"InclBrl4_PixelGeneral_InclinedAlternative",
                "PIXELSIMPLESERVICE":"InclBrl_PixelSimpleService",
                "SILICONMODULES":"ITK_PixelModules",
                "SILICONREADOUT":"PixelModuleReadout",
                "STAVESUPPORT":"InclBrl4_SlimStaveSupport_InclinedAlternative",
                "PIXELDISCSUPPORT":"InclBrl4_InclinedAlternative_DiskSupport",
                "MATERIAL":"InclBrl_Material",
                "PIXELROUTINGSERVICE":"InclBrl4_InclinedAlternative_PixelRoutingService",
                }
        else:
            xmlFileDict["Pixel"]={
                "PIXELGENERAL":"InclBrl4_PixelGeneral",
                "PIXELSIMPLESERVICE":"InclBrl_PixelSimpleService",
                "SILICONMODULES":"ITK_PixelModules",
                "SILICONREADOUT":"PixelModuleReadout",
                "STAVESUPPORT":"InclBrl4_StaveSupport",
                "PIXELDISCSUPPORT":"InclBrl4_DiskSupport",
                "MATERIAL":"InclBrl_Material",
                "PIXELROUTINGSERVICE":"InclBrl4_PixelRoutingService",
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
        serviceTool.BarrelModuleMaterial = True
        toolSvc+=serviceTool

        #print "******************************************************************************************"
        #print "PixelGeoModel - import PixelLayerValidationTool"
        #from BarrelInclinedRef.BarrelInclinedRefConf import PixelLayerValidationTool
        #validTool=PixelLayerValidationTool(name="LayerValidationTool")
        #validTool.PixelServicesTool = serviceTool
        #toolSvc+=validTool
        
        if SLHC_Flags.LayoutOption == "InclinedAlternative":
          print "******************************************************************************************"
          print "PixelGeoModel - import GeoPixelLayerInclRefTool"
          from BarrelInclinedRef.BarrelInclinedRefConf import GeoPixelLayerInclRefTool
          geoLayerOuterTool=GeoPixelLayerInclRefTool(name="OuterPixelLayerTool")
          toolSvc+=geoLayerOuterTool
          from BarrelInclinedRef.BarrelInclinedRefConf import GeoPixelLayerPlanarRefTool
          geoLayerInnerTool=GeoPixelLayerPlanarRefTool(name="InnerPixelLayerTool")
          toolSvc+=geoLayerInnerTool
  
          print "PixelGeoModel - import GeoPixelBarrelInclRefTool"
          from BarrelInclinedRef.BarrelInclinedRefConf import GeoPixelBarrelInclRefTool
          geoBarrelTool=GeoPixelBarrelInclRefTool(name="GeoPixelBarrelInclRefTool")
          geoBarrelTool.InnerPixelLayerTool = geoLayerInnerTool
          geoBarrelTool.OuterPixelLayerTool = geoLayerOuterTool
          geoBarrelTool.MaxInnerLayerMax = 2
          geoBarrelTool.PixelServicesTool = serviceTool
          toolSvc+=geoBarrelTool
        
        else :
          print "******************************************************************************************"
          print "PixelGeoModel - import GeoPixelLayerInclRefTool"
          from BarrelInclinedRef.BarrelInclinedRefConf import GeoPixelLayerInclRefTool
          geoLayerInnerTool=GeoPixelLayerInclRefTool(name="InnerPixelLayerTool")
          #geoLayerInnerTool.LayerValidationTool = validTool
          toolSvc+=geoLayerInnerTool
          
          print "PixelGeoModel - import GeoPixelBarrelInclRefTool"
          from BarrelInclinedRef.BarrelInclinedRefConf import GeoPixelBarrelInclRefTool
          geoBarrelTool=GeoPixelBarrelInclRefTool(name="GeoPixelBarrelInclRefTool")
          geoBarrelTool.InnerPixelLayerTool = geoLayerInnerTool
          geoBarrelTool.MaxInnerLayerMax = 5
          geoBarrelTool.PixelServicesTool = serviceTool
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
        pixelTool.ReadXMLFromDB = bReadXMLfromDB
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
