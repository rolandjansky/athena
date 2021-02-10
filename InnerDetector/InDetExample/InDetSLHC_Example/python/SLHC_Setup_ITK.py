# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

""" SLHC_Setup
    Python module to hold storegate keys of InDet objects.
"""

import os
from os.path import exists, join
from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags
from AthenaCommon.DetFlags import DetFlags

from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags
auto_isGMX = (SLHC_Flags.doGMX()) or (CommonGeometryFlags.StripGeoType() == "GMX") 

dictVersion = "ITkHGTD"

auto_generateDictionary = (SLHC_Flags.UseLocalGeometry()) or (SLHC_Flags.ForceDictionaryGeneration())

from AthenaCommon.GlobalFlags import jobproperties
DetDescrVersion = jobproperties.Global.DetDescrVersion()
if DetDescrVersion.startswith('ATLAS-P2-ITK-23-00-01'):
    print "ERROR - This geometry tag (ATLAS-P2-ITK-23-00-01) has been replaced - please use ATLAS-P2-ITK-23-00-02 instead!"
    print "ERROR - The new geometry tag replaces the old one for reasons of dictionary backwards compatibility"
    print "ERROR - ATLAS-P2-ITK-23-00-02 can be used to process HITS created with ATLAS-P2-ITK-23-00-01_VALIDATION"
    exit(1)
    

class SLHC_Setup_XMLReader :
    # constructor requires the SLHC_Flags
    def __init__(self):
        from SLHC_Setup_XML import SLHC_Setup_XMLReader

        SLHC_Setup_XMLReader(dictionaryFileName = "InDetIdDictFiles/IdDictInnerDetector_ITK_LOCAL.xml",
                            createXML = auto_generateDictionary,
                            doPix=True,
                            doSCT=True,
                            doHGTD=True,
                            isGMX=auto_isGMX,
                            XML_SLHC_Version = dictVersion
                                 )

    
class SLHC_Setup :
    # constructor requires the SLHC_Flags
    def __init__(self):

        from InDetTrackingGeometryXML.XMLReaderJobProperties import XMLReaderFlags
        bReadXMLfromDB = XMLReaderFlags.readXMLfromDB()

        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        from AthenaCommon.AppMgr import ToolSvc as toolSvc

        # GeoModelConfiguration 

        xmlFileDict={}

                    
        xmlFileDict["Pixel"]={
                "PIXELGENERAL":"PixelGeneral",
                "PIXELSIMPLESERVICE":"PixelSimpleService",
                "SILICONMODULES":"ITK_PixelModules",
                "SILICONREADOUT":"PixelModuleReadout",
                "STAVESUPPORT":"SlimStaveSupport",
                "PIXELDISCSUPPORT":"DiskSupport",
                "MATERIAL":"Material",
                "PIXELROUTINGSERVICE":"PixelRoutingService",
                "BCMPRIME":"BCMPrime"
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

        
        print "PixelGeoModel - import GeoPixelLayerInclRefTool"
        from BarrelInclinedRef.BarrelInclinedRefConf import GeoPixelLayerInclRefTool
        geoLayerAlpineTool=GeoPixelLayerInclRefTool(name="AlpinePixelLayerTool")
        toolSvc+=geoLayerAlpineTool
        print "PixelGeoModel - import GeoPixelLayerPlanarRefTool"
        from BarrelInclinedRef.BarrelInclinedRefConf import GeoPixelLayerPlanarRefTool
        geoLayerPlanarTool=GeoPixelLayerPlanarRefTool(name="PlanarPixelLayerTool")
        toolSvc+=geoLayerPlanarTool
  
        print "PixelGeoModel - import GeoPixelBarrelInclRefTool"
        from BarrelInclinedRef.BarrelInclinedRefConf import GeoPixelBarrelInclRefTool
        geoBarrelTool=GeoPixelBarrelInclRefTool(name="GeoPixelBarrelInclRefTool")
        geoBarrelTool.PlanarPixelLayerTool = geoLayerPlanarTool
        geoBarrelTool.AlpinePixelLayerTool = geoLayerAlpineTool
        geoBarrelTool.PixelServicesTool = serviceTool
        toolSvc+=geoBarrelTool
        
        print "******************************************************************************************"
        print "PixelGeoModel - import GeoBCMPrimeTool"

        from BCMPrime.BCMPrimeConf import GeoBCMPrimeTool
        geoBCMPrimeTool = GeoBCMPrimeTool(name="GeoBCMPrimeTool")
        if DetFlags.BCM_on():
            geoBCMPrimeTool.ModulesOn = 15
        else:
            geoBCMPrimeTool.ModulesOn = 0
        toolSvc += geoBCMPrimeTool
        
        print "******************************************************************************************"
        print "PixelGeoModel - import GeoPixelLayerECRingRefTool"
        from EndcapRingRef.EndcapRingRefConf import GeoPixelLayerECRingRefTool
        geoECLayerTool=GeoPixelLayerECRingRefTool(name="GeoPixelLayerECRingRefTool")
        geoECLayerTool.GeoBCMPrimeTool = geoBCMPrimeTool
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
        if DetFlags.pixel_on():
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
