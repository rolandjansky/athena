# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

######################################################
# CustomStagedInDetTrackingGeometry module
#
# it inherits from StagedTrackingGeometryBuilderConf and performs 
# standard configuration
#
######################################################

# import system commands
import os, shutil

# import the Extrapolator configurable
from InDetTrackingGeometry.InDetTrackingGeometryConf import InDet__StagedTrackingGeometryBuilder

from AthenaCommon.SystemOfUnits import *

from ISF_FatrasDetDescrAlpine.CustomInDetTrackingGeometryAlpineProperties import GeometryAlpineLayoutFlags
print "# XML Flags #############################################################"
print GeometryAlpineLayoutFlags
print "#####################################################################"

# define the class
class CustomStagedInDetTrackingGeometryBuilder( InDet__StagedTrackingGeometryBuilder ):
    # constructor
    def __init__(self,name           = 'InDetTrackingGeometryBuilder',
                 namePrefix          = '',
                 PixelLayout         = GeometryAlpineLayoutFlags.PixelLayout.get_Value(),
                 PixelEndcapLayout   = GeometryAlpineLayoutFlags.PixelEndcapLayout.get_Value(),
                 SCTLayout           = GeometryAlpineLayoutFlags.SCTLayout.get_Value(),
                 splitBarrelLayers   = GeometryAlpineLayoutFlags.splitBarrelLayers.get_Value(), 
                 isRingLayout        = GeometryAlpineLayoutFlags.isRingLayout.get_Value(), 
                 doPix               = GeometryAlpineLayoutFlags.doPix.get_Value(),
                 doSCT               = GeometryAlpineLayoutFlags.doSCT.get_Value(),
                 dictionaryFileName  = GeometryAlpineLayoutFlags.IdDictFilename.get_Value(),
                 createXML           = True):	  

        # get the ToolSvc
        from AthenaCommon.AppMgr import ToolSvc, ServiceMgr      
        from AthenaCommon.DetFlags import DetFlags
       
        # the Detector flags to be imported
        from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
        
        # beampipe        
        from InDetTrackingGeometry.InDetTrackingGeometryConf import InDet__BeamPipeBuilder
        BeamPipeBuilder = InDet__BeamPipeBuilder(name=namePrefix+'BeamPipeBuilder')
        BeamPipeBuilder.BeamPipeFromGeoModel    = False
        BeamPipeBuilder.BeamPipeEnvelope        = 0.
        BeamPipeBuilder.BeamPipeOffsetX         = 0.
        BeamPipeBuilder.BeamPipeOffsetY         = 0.
        BeamPipeBuilder.BeamPipeRadius 	        = 25.
        #BeamPipeBuilder.BeamPipeRadius          = TrkDetFlags.BeamPipeRadius()
        BeamPipeBuilder.BeamPipeThickness       = TrkDetFlags.BeamPipeThickness() 
        BeamPipeBuilder.BeamPipeMaterialBinsZ   = TrkDetFlags.BeamPipeLayerMaterialBinsZ()
        BeamPipeBuilder.OutputLevel             = TrkDetFlags.BeamPipeBuildingOutputLevel()
        print "# BeamPipeBuilder####################################################"
        print BeamPipeBuilder
        print "#####################################################################"
        ToolSvc += BeamPipeBuilder

        # the layer providers
        from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__LayerProvider
        BeamPipeProvider = Trk__LayerProvider(name=namePrefix+'BeamPipeProvider')
        BeamPipeProvider.LayerBuilder = BeamPipeBuilder
        ToolSvc += BeamPipeProvider

        # binning of the beam pipe
        BeamPipeBinning = 2

        # the layer builders
        layerProviders  = [ BeamPipeProvider ]
        binningsCenter  = [ BeamPipeBinning ]
        binningsEndcap  = [ BeamPipeBinning ]
        colors          = [ 2 ]
                        
        ###### Setup ITk XML reader ######
        from InDetTrackingGeometryXML.InDetTrackingGeometryXMLConf import InDet__XMLReaderSvc
        xmlReader = InDet__XMLReaderSvc(name='InDetXMLReaderSvc')

        ##### Setup XML files here #######
        from envutil import * ## cool stuff found in PyJobTransformsCore/python/

        PIXMODULEFILE = "ITK_PixelModules.xml"
        SCTMODULEFILE = "ITK_SCTModules.xml"
        if( "ATLAS" in PixelLayout ):
            PIXMODULEFILE = "ATLAS_PixelModules.xml"
        if( "ATLAS" in SCTLayout ):
            SCTMODULEFILE = "ATLAS_SCTModules.xml"

        PIXSTAVEFILE  = str(PixelLayout) + "_PixelStave.xml"
        PIXBARRELFILE = str(PixelLayout) + "_PixelBarrel.xml"
        PIXENDCAPFILE = str(PixelEndcapLayout) + "_PixelEndcap.xml"
 
        SCTSTAVEFILE  = str(SCTLayout) + "_SCTStave.xml"
        SCTBARRELFILE = str(SCTLayout) + "_SCTBarrel.xml"
        SCTENDCAPFILE = str(SCTLayout) + "_SCTEndcap.xml"

        ###### Setup dictionary file to use  ######
        dictDir = os.path.dirname(dictionaryFileName)
        if not os.path.exists(dictDir):
            os.makedirs(dictDir)

        ###### Setup XML builders properties ######
        xmlReader.dictionaryFileName  = dictionaryFileName
        xmlReader.createXMLDictionary = createXML
        xmlReader.doPix = doPix
        xmlReader.doSCT = doSCT
        
        ###### Flags required to properly build the tracking volumes
        isRingLayout       = False 
        splitBarrelLayers  = False
        InnerLayers        = []

        if( "ECRing" in PixelEndcapLayout ):
            isRingLayout  = True
            
        if( "Alpine" in PixelLayout ):
            isRingLayout  = True
            splitBarrelLayers = True          
            InnerLayers = [0]

        if( "ExtBrl" in PixelLayout ):
            splitBarrelLayers = True          
            InnerLayers = [0,1]

        if( "InclBrl" in PixelLayout ):
            splitBarrelLayers = True
            InnerLayers = [0,1]

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
        ServiceMgr += xmlReader
        theApp.CreateSvc += [ "InDet::XMLReaderSvc/InDetXMLReaderSvc" ]

        ###### Setup builders for modules ######
        from ISF_FatrasDetDescrAlpine.ISF_FatrasDetDescrAlpineConf import iFatras__PlanarModuleProvider
        moduleProvider = iFatras__PlanarModuleProvider(name='PlanarModuleProvider')
        ToolSvc += moduleProvider

        ###### Setup builders for staves ######
        from InDetTrackingGeometryXML.InDetTrackingGeometryXMLConf import InDet__StaveBuilderXML
        staveBuilder = InDet__StaveBuilderXML(name='StaveBuilderXML')
        staveBuilder.ModuleProvider = moduleProvider
        ToolSvc += staveBuilder

        ###### Setup builders for pixels ######
        from InDetTrackingGeometryXML.InDetTrackingGeometryXMLConf import InDet__BarrelBuilderXML
        barrelPixLayerBuilder = InDet__BarrelBuilderXML(name='PixelsBarrelBuilder')
        barrelPixLayerBuilder.StaveBuilder = staveBuilder
        barrelPixLayerBuilder.ModuleProvider = moduleProvider
        ToolSvc += barrelPixLayerBuilder

        from InDetTrackingGeometryXML.InDetTrackingGeometryXMLConf import InDet__EndcapBuilderXML
        endcapPixLayerBuilder = InDet__EndcapBuilderXML(name='PixelsEndcapBuilder')
        endcapPixLayerBuilder.ModuleProvider = moduleProvider
        ToolSvc += endcapPixLayerBuilder 

        ###### Setup XML builders for SCT ######
        barrelSCTLayerBuilder = InDet__BarrelBuilderXML(name='SCTBarrelBuilder')
        barrelSCTLayerBuilder.StaveBuilder = staveBuilder
        barrelSCTLayerBuilder.ModuleProvider = moduleProvider
        ToolSvc += barrelSCTLayerBuilder

        endcapSCTLayerBuilder = InDet__EndcapBuilderXML(name='SCTEndcapBuilder')
        endcapSCTLayerBuilder.ModuleProvider = moduleProvider
        ToolSvc += endcapSCTLayerBuilder 
                 
        print "# XML Layout ########################################################"
        print xmlReader
        if(isRingLayout):
            print '--> Building Ring Layout '                        
        if(splitBarrelLayers):
            print '--> Splitting LayerProvider for Inner Layers ', InnerLayers[0], ' to ', InnerLayers[-1]            
        print "#####################################################################"
        #print theApp

        # Pixel layer provider
        from InDetTrackingGeometryXML.InDetTrackingGeometryXMLConf import InDet__LayerProviderXML
        startLayer = 0
        if splitBarrelLayers:
            endLayer = InnerLayers[-1]
            # Pixel layer provider
            PixelFirstLayerProvider = InDet__LayerProviderXML(name=namePrefix+'PixelFirstLayerProvider')
            PixelFirstLayerProvider.Identification          = 'Pixel0'
            PixelFirstLayerProvider.OutputLevel             = 1
            PixelFirstLayerProvider.doPix = doPix
            PixelFirstLayerProvider.doSCT = False
            PixelFirstLayerProvider.startLayer = startLayer
            PixelFirstLayerProvider.endLayer = endLayer
            PixelFirstLayerProvider.ModuleProvider = moduleProvider
            PixelFirstLayerProvider.PixelBarrelBuilder = barrelPixLayerBuilder
            PixelFirstLayerProvider.PixelEndcapBuilder = endcapPixLayerBuilder            
            #####startLayer += 1
            startLayer = endLayer + 1
            ToolSvc += PixelFirstLayerProvider
            PixelFirstLayerBinning = 2
            layerProviders  += [ PixelFirstLayerProvider ]
            binningsCenter  += [ PixelFirstLayerBinning ]
            binningsEndcap  += [ PixelFirstLayerBinning ]
            colors          += [ 3 ]
            #print PixelFirstLayerProvider
        if doPix:
            # Pixel layer provider
            PixelLayerProvider = InDet__LayerProviderXML(name=namePrefix+'PixelLayerProvider')
            PixelLayerProvider.Identification          = 'Pixel'
            PixelLayerProvider.OutputLevel             = 1
            PixelLayerProvider.doPix = doPix
            PixelLayerProvider.doSCT = False
            PixelLayerProvider.startLayer = startLayer
            PixelLayerProvider.ModuleProvider = moduleProvider
            PixelLayerProvider.PixelBarrelBuilder = barrelPixLayerBuilder
            PixelLayerProvider.PixelEndcapBuilder = endcapPixLayerBuilder                        
            ToolSvc += PixelLayerProvider
            PixelLayerBinning = 2
            layerProviders  += [ PixelLayerProvider ]
            binningsCenter  += [ PixelLayerBinning ]
            binningsEndcap  += [ PixelLayerBinning ]
            colors          += [ 3 ]
            #print PixelLayerProvider

        if doSCT:
            # SCT layer provider
            SCTLayerProvider = InDet__LayerProviderXML(name=namePrefix+'SCTLayerProvider')
            SCTLayerProvider.Identification          = 'SCT'
            SCTLayerProvider.OutputLevel             = 1
            SCTLayerProvider.doPix = False
            SCTLayerProvider.doSCT = doSCT
            SCTLayerProvider.ModuleProvider = moduleProvider
            SCTLayerProvider.SCTBarrelBuilder = barrelSCTLayerBuilder
            SCTLayerProvider.SCTEndcapBuilder = endcapSCTLayerBuilder                                    
            ToolSvc += SCTLayerProvider
            SCTLayerBinning = 2
            layerProviders  += [ SCTLayerProvider ]
            binningsCenter  += [ SCTLayerBinning ]
            binningsEndcap  += [ SCTLayerBinning ]
            colors          += [ 4 ]
        
        # output level
        TrkDetFlags.InDetBuildingOutputLevel = 0

        # layer && tracking volume array creator

        # helpers for the InDetTrackingGeometry Builder : layer array creator
        from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__LayerArrayCreator
        InDetLayerArrayCreator = Trk__LayerArrayCreator(name = 'InDetLayerArrayCreator')
        InDetLayerArrayCreator.EmptyLayerMode           = 2 # deletes empty material layers from arrays
        InDetLayerArrayCreator.OutputLevel              = TrkDetFlags.InDetBuildingHelperOutputLevel()
        # add to ToolSvc
        ToolSvc += InDetLayerArrayCreator

        # helpers for the InDetTrackingGeometry Builder : volume array creator
        from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__TrackingVolumeArrayCreator
        InDetTrackingVolumeArrayCreator                       = Trk__TrackingVolumeArrayCreator(name = 'InDetTrackingVolumeArrayCreator')
        InDetTrackingVolumeArrayCreator.OutputLevel           = TrkDetFlags.InDetBuildingOutputLevel()
        # add to ToolSvc
        ToolSvc += InDetTrackingVolumeArrayCreator
        
        # helpers for the InDetTrackingGeometry Builder : tracking voluem helper for glueing
        from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__TrackingVolumeHelper
        InDetTrackingVolumeHelper                             = Trk__TrackingVolumeHelper(name ='InDetTrackingVolumeHelper')
        InDetTrackingVolumeHelper.OutputLevel                 = TrkDetFlags.InDetBuildingHelperOutputLevel()
        # the material bins 
        InDetTrackingVolumeHelper.BarrelLayerBinsZ            = TrkDetFlags.InDetPassiveLayerMaterialBinsRz()
        InDetTrackingVolumeHelper.BarrelLayerBinsPhi          = TrkDetFlags.InDetPassiveLayerMaterialBinsPhi() 
        InDetTrackingVolumeHelper.EndcapLayerBinsR            = TrkDetFlags.InDetPassiveLayerMaterialBinsRz()
        InDetTrackingVolumeHelper.EndcapLayerBinsPhi          = TrkDetFlags.InDetPassiveLayerMaterialBinsPhi()  
        # add to ToolSvc
        ToolSvc += InDetTrackingVolumeHelper
        
        # helpers for the InDetTrackingGeometry Builder : cylinder volume creator
        from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__CylinderVolumeCreator
        InDetCylinderVolumeCreator = Trk__CylinderVolumeCreator(name = 'InDetCylinderVolumeCreator')
        # give him the layer array creator
        InDetCylinderVolumeCreator.LayerArrayCreator          = InDetLayerArrayCreator
        InDetCylinderVolumeCreator.TrackingVolumeArrayCreator = InDetTrackingVolumeArrayCreator
        InDetCylinderVolumeCreator.TrackingVolumeHelper       = InDetTrackingVolumeHelper
        # specifiy the binning, passive layers, entry layers
        InDetCylinderVolumeCreator.PassiveLayerThickness      = TrkDetFlags.InDetPassiveLayerThickness()  
        InDetCylinderVolumeCreator.PassiveLayerBinsPhi        = TrkDetFlags.InDetPassiveLayerMaterialBinsPhi()  
        InDetCylinderVolumeCreator.PassiveLayerBinsRZ         = TrkDetFlags.InDetPassiveLayerMaterialBinsRz()  
        # output level                                       
        InDetCylinderVolumeCreator.OutputLevel                = TrkDetFlags.InDetBuildingHelperOutputLevel()        
        # add to ToolSvc
        ToolSvc += InDetCylinderVolumeCreator        
        
        # the envelope definition service
        from SubDetectorEnvelopes.SubDetectorEnvelopesConf import DetDescrDBEnvelopeSvc
        AtlasEnvelopeSvc = DetDescrDBEnvelopeSvc('AtlasEnvelopeSvcDefinitionSvc')
        # set the output level for the Envelope service
        AtlasEnvelopeSvc.OutputLevel      = TrkDetFlags.InDetBuildingHelperOutputLevel()  
        # add to SvcMgr
        ServiceMgr += AtlasEnvelopeSvc
        
        # the tracking geometry builder
        InDet__StagedTrackingGeometryBuilder.__init__(self,namePrefix+name,\
                                                      LayerBuilders                 = layerProviders,
                                                      LayerBinningTypeCenter        = binningsCenter,
                                                      LayerBinningTypeEndcap        = binningsEndcap,
                                                      ColorCodes                    = colors,
                                                      EnvelopeDefinitionSvc         = AtlasEnvelopeSvc,
                                                      EnvelopeCover                 = 0.5,
                                                      VolumeEnclosureDiscPositions  = [ 3250., 3450. ],
                                                      TrackingVolumeCreator         = InDetCylinderVolumeCreator,
                                                      LayerArrayCreator             = InDetLayerArrayCreator,
                                                      CheckForRingLayout            = isRingLayout,
                                                      BuildBoundaryLayers           = TrkDetFlags.InDetBuildMaterialBoundaries(),
                                                      ReplaceAllJointBoundaries     = TrkDetFlags.InDetBuildJointBoundaries(),
                                                      OutputLevel                   = TrkDetFlags.InDetBuildingOutputLevel(),
                                                      ExitVolumeName                = TrkDetFlags.InDetContainerName(),
                                                      MagneticFieldMode             = TrkDetFlags.MagneticFieldMode())

        ###print "# StagedBuilder #####################################################"
        ###print InDet__StagedTrackingGeometryBuilder
        ###print "#####################################################################"
        
