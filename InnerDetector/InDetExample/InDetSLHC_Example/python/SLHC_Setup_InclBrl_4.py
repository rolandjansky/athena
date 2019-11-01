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

from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags
auto_isGMX = (SLHC_Flags.doGMX()) or (CommonGeometryFlags.StripGeoType() == "GMX") 

class SLHC_Setup_XMLReader :
    # constructor requires the SLHC_Flags
    def __init__(self):

        # XMLReader setup
        from SLHC_Setup_XML import SLHC_Setup_XMLReader
        if (SLHC_Flags.LayoutOption == "Light" ):
            SLHC_Setup_XMLReader(PixelLayout = "InclBrl4Ref_Light",
                                 PixelEndcapLayout = "ECRing4Ref_Light",
                                 PixelModules = "ITK_PixelModules",
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

        elif (SLHC_Flags.LayoutOption == "InclinedDuals" or SLHC_Flags.LayoutOption == "InclinedDuals_wFlex" or
              SLHC_Flags.LayoutOption == "InclinedDuals_fudgeSvc30pct" or SLHC_Flags.LayoutOption == "InclinedDuals_fudgeSvcInner30pct" or SLHC_Flags.LayoutOption == "InclinedDuals_fudgeSvcOuter30pct" or
              SLHC_Flags.LayoutOption == "InclinedDuals_fudgeStSup30pct" or SLHC_Flags.LayoutOption == "InclinedDuals_fudgeStSupInner30pct" or SLHC_Flags.LayoutOption == "InclinedDuals_fudgeStSupOuter30pct"):
            SLHC_Setup_XMLReader(PixelLayout = "InclBrl4Ref_InclinedDuals",
                                 PixelEndcapLayout = "ECRing4Ref_InclinedDuals",
                                 PixelModules = "ITK_PixelModules",
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
                                 PixelModules = "ITK_PixelModules",
                                 SCTLayout = "FourLayersNoStub_23-25-dev0",
                                 dictionaryFileName = "InDetIdDictFiles/IdDictInnerDetector_SLHC_InclBrl_4.xml",
                                 createXML = True,
                                 doPix=True,
                                 doSCT=True,
                                 isGMX=auto_isGMX,
                                 XML_SLHC_Version = "SLHC_InclinedAlternative"
                                 )          
        else:
            print "WARNING! No recognised value of SLHC_Flags.LayoutOption found. (SLHC_Flags.LayoutOption =",SLHC_Flags.LayoutOption,"). Using generic names "
            SLHC_Setup_XMLReader(PixelLayout = "InclBrl4Ref",
                                 PixelEndcapLayout = "ECRing4Ref",
                                 PixelModules = "ITK_PixelModules",
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
        if (SLHC_Flags.LayoutOption == "Light" ):
            xmlFileDict["Pixel"]={
                "PIXELGENERAL":"InclBrl4_PixelGeneral_Light",
                "PIXELSIMPLESERVICE":"InclBrl_PixelSimpleService",
                "SILICONMODULES":"ITK_Step3_PixelModules",
                "SILICONREADOUT":"PixelModuleReadout",
                "STAVESUPPORT":"InclBrl4_SlimStaveSupport_Light",
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

        elif (SLHC_Flags.LayoutOption == "InclinedDuals" or SLHC_Flags.LayoutOption == "InclinedDuals_wFlex" or
              SLHC_Flags.LayoutOption == "InclinedDuals_fudgeSvc30pct" or SLHC_Flags.LayoutOption == "InclinedDuals_fudgeSvcInner30pct" or SLHC_Flags.LayoutOption == "InclinedDuals_fudgeSvcOuter30pct" or 
              SLHC_Flags.LayoutOption == "InclinedDuals_fudgeStSup30pct" or SLHC_Flags.LayoutOption == "InclinedDuals_fudgeStSupInner30pct" or SLHC_Flags.LayoutOption == "InclinedDuals_fudgeStSupOuter30pct"):
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
            if(SLHC_Flags.LayoutOption == "InclinedDuals_fudgeSvc30pct"):
                xmlFileDict["Pixel"]["PIXELROUTINGSERVICE"]="InclBrl4_InclinedDuals_PixelRoutingService_fudgeSvc30pct"
                xmlFileDict["Pixel"]["STAVESUPPORT"]="InclBrl4_SlimStaveSupport_InclinedDuals_fudgeCurly30pct"
            if(SLHC_Flags.LayoutOption == "InclinedDuals_fudgeSvcInner30pct"):
                xmlFileDict["Pixel"]["PIXELROUTINGSERVICE"]="InclBrl4_InclinedDuals_PixelRoutingService_fudgeSvcInner30pct"
                xmlFileDict["Pixel"]["STAVESUPPORT"]="InclBrl4_SlimStaveSupport_InclinedDuals_fudgeCurly30pct"
            if(SLHC_Flags.LayoutOption == "InclinedDuals_fudgeSvcOuter30pct"):
                xmlFileDict["Pixel"]["PIXELROUTINGSERVICE"]="InclBrl4_InclinedDuals_PixelRoutingService_fudgeSvcOuter30pct"
            if(SLHC_Flags.LayoutOption == "InclinedDuals_fudgeStSup30pct"):
                xmlFileDict["Pixel"]["STAVESUPPORT"]="InclBrl4_SlimStaveSupport_InclinedDuals_fudgeStSup30pct"
            if(SLHC_Flags.LayoutOption == "InclinedDuals_fudgeStSupInner30pct"):
                xmlFileDict["Pixel"]["STAVESUPPORT"]="InclBrl4_SlimStaveSupport_InclinedDuals_fudgeStSupInner30pct"
            if(SLHC_Flags.LayoutOption == "InclinedDuals_fudgeStSupOuter30pct"):
                xmlFileDict["Pixel"]["STAVESUPPORT"]="InclBrl4_SlimStaveSupport_InclinedDuals_fudgeStSupOuter30pct"

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
