# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

######################################################
# ConfiguredStagedTrackingGeometry module
#
# it inherits from RobustTrackingGeometryBuilderConf and performs 
# standard configuration
#
######################################################

# import the Extrapolator configurable
from InDetTrackingGeometry.InDetTrackingGeometryConf import InDet__StagedTrackingGeometryBuilderCond

# define the class
class ConfiguredStagedTrackingGeometryBuilderCond( InDet__StagedTrackingGeometryBuilderCond ):
    # constructor
    def __init__(self,name = 'InDetTrackingGeometryBuilderCond',
                      namePrefix = '',
                      setLayerAssociation = True,
                      buildTrtStrawLayers = False):

        # get the ToolSvc
        from AthenaCommon.AppMgr import ToolSvc, ServiceMgr
        from AthenaCommon.DetFlags import DetFlags
       
        # the Detector flags to be imported
        from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
        
        # beampipe
        from InDetTrackingGeometry.InDetTrackingGeometryConf import InDet__BeamPipeBuilderCond
        BeamPipeBuilder = InDet__BeamPipeBuilderCond(name=namePrefix+'BeamPipeBuilderCond')
        BeamPipeBuilder.BeamPipeRadius 	        = TrkDetFlags.BeamPipeRadius()
        BeamPipeBuilder.BeamPipeThickness       = TrkDetFlags.BeamPipeThickness() 
        BeamPipeBuilder.BeamPipeMaterialBinsZ   = TrkDetFlags.BeamPipeLayerMaterialBinsZ()
        BeamPipeBuilder.OutputLevel             = TrkDetFlags.BeamPipeBuildingOutputLevel()
        ToolSvc += BeamPipeBuilder

        # the layer providers
        from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__LayerProviderCond
        BeamPipeProvider = Trk__LayerProviderCond(name=namePrefix+'BeamPipeProviderCond')
        BeamPipeProvider.LayerBuilder = BeamPipeBuilder
        ToolSvc += BeamPipeProvider

        # binning of the beam pipe
        BeamPipeBinning = 2
                
        # the layer builders
        layerProviders  = [ BeamPipeProvider ]
        binningsCenter  = [ BeamPipeBinning ]
        binningsEndcap  = [ BeamPipeBinning ]
        colors          = [ 2 ]
                                
        # PIXEL building
        if DetFlags.pixel_on() :
            from InDetTrackingGeometry.InDetTrackingGeometryConf import InDet__SiLayerBuilderCond
            PixelLayerBuilder = InDet__SiLayerBuilderCond(name=namePrefix+'PixelLayerBuilderCond')
            PixelLayerBuilder.PixelCase 	       = True
            PixelLayerBuilder.Identification       = 'Pixel'
            PixelLayerBuilder.SiDetManagerLocation = 'Pixel'
            # Pixel barrel specifications
            PixelLayerBuilder.BarrelLayerBinsZ     = TrkDetFlags.PixelBarrelLayerMaterialBinsZ()
            PixelLayerBuilder.BarrelLayerBinsPhi   = TrkDetFlags.PixelBarrelLayerMaterialBinsPhi()
            PixelLayerBuilder.EndcapLayerBinsR     = TrkDetFlags.PixelEndcapLayerMaterialBinsR()
            PixelLayerBuilder.EndcapLayerBinsPhi   = TrkDetFlags.PixelEndcapLayerMaterialBinsPhi()
            PixelLayerBuilder.EndcapRingLayout     = TrkDetFlags.PixelRingLayout()
            # set the layer association
            PixelLayerBuilder.SetLayerAssociation  = setLayerAssociation
            # output level
            PixelLayerBuilder.OutputLevel         = TrkDetFlags.PixelBuildingOutputLevel()
            # the binning type of the layers   
            PixelLayerBinning = 2
            # add it to tool service
            ToolSvc += PixelLayerBuilder
            
            # the layer providers
            PixelLayerProvider = Trk__LayerProviderCond(name=namePrefix+'PixelLayerProviderCond')
            PixelLayerProvider.LayerBuilder = PixelLayerBuilder
            ToolSvc += PixelLayerProvider
            layerProviders  += [ PixelLayerProvider ]
            binningsCenter  += [ PixelLayerBinning ]
            binningsEndcap  += [ PixelLayerBinning ]
            colors          += [ 3 ]
        
        # SCT building
        if DetFlags.SCT_on() :
            SCT_LayerBuilder = InDet__SiLayerBuilderCond(name=namePrefix+'SCT_LayerBuilderCond')
            SCT_LayerBuilder.PixelCase                       = False
            SCT_LayerBuilder.Identification                  = 'SCT'
            SCT_LayerBuilder.SiDetManagerLocation            = 'SCT'
            # additionall layers - handle with care !
            SCT_LayerBuilder.EndcapAdditionalLayerPositionsZ = [ -2850 , 2850 ] 
            SCT_LayerBuilder.EndcapAdditionalLayerType       = [  0 , 0 ] 
            # SCT barrel specifications
            SCT_LayerBuilder.BarrelLayerBinsZ                = TrkDetFlags.SCT_BarrelLayerMaterialBinsZ()
            SCT_LayerBuilder.BarrelLayerBinsPhi              = TrkDetFlags.SCT_BarrelLayerMaterialBinsPhi()
            # SCT endcap specifications                          
            SCT_LayerBuilder.EndcapLayerBinsR                = TrkDetFlags.SCT_EndcapLayerMaterialBinsR()
            SCT_LayerBuilder.EndcapLayerBinsPhi              = TrkDetFlags.SCT_EndcapLayerMaterialBinsPhi()
            SCT_LayerBuilder.EndcapComplexRingBinning        = TrkDetFlags.SCT_EndcapLayerDynamicRings()
            # set the layer association                   
            SCT_LayerBuilder.SetLayerAssociation             = setLayerAssociation        
            # output level                                 
            SCT_LayerBuilder.OutputLevel                     = TrkDetFlags.SCT_BuildingOutputLevel()
            # the binning type of the layer     
            SCT_LayerBinning = 2
            # SCT -> ToolSvc                             
            ToolSvc += SCT_LayerBuilder     
            
            # the layer providers
            SCT_LayerProvider = Trk__LayerProviderCond(name=namePrefix+'SCT_LayerProviderCond')
            SCT_LayerProvider.LayerBuilder = SCT_LayerBuilder
            ToolSvc += SCT_LayerProvider
                              
            # put them to the caches
            layerProviders  += [ SCT_LayerProvider ]
            binningsCenter  += [ SCT_LayerBinning ]
            binningsEndcap  += [ SCT_LayerBinning ]
            colors          += [ 4 ]

        # TRT building
        if DetFlags.TRT_on() :
            from InDetTrackingGeometry.InDetTrackingGeometryConf import InDet__TRT_LayerBuilderCond
            TRT_LayerBuilder = InDet__TRT_LayerBuilderCond(name=namePrefix+'TRT_LayerBuilderCond')
            # TRT barrel specifications
            TRT_LayerBuilder.ModelBarrelLayers  = TrkDetFlags.TRT_BarrelModelLayers()
            TRT_LayerBuilder.BarrelLayerBinsZ   = TrkDetFlags.TRT_BarrelLayerMaterialBinsZ()
            TRT_LayerBuilder.BarrelLayerBinsPhi = TrkDetFlags.TRT_BarrelLayerMaterialBinsPhi()
            # SCT endcap specifications                     
            TRT_LayerBuilder.ModelEndcapLayers  = TrkDetFlags.TRT_EndcapModelLayers()
            TRT_LayerBuilder.EndcapLayerBinsR   = TrkDetFlags.TRT_EndcapLayerMaterialBinsR()
            TRT_LayerBuilder.EndcapLayerBinsPhi = TrkDetFlags.TRT_EndcapLayerMaterialBinsPhi()                
            # set the binning from bi-aequidistant to arbitrary for complex TRT volumes
            TRT_LayerBinning = 1        
            if buildTrtStrawLayers or TrkDetFlags.TRT_BuildStrawLayers() :
               TRT_LayerBinning = 2
               TRT_LayerBuilder.ModelLayersOnly = False
            # output level
            TRT_LayerBuilder.OutputLevel        = TrkDetFlags.TRT_BuildingOutputLevel()
            # TRT -> ToolSvc
            ToolSvc += TRT_LayerBuilder
            
            
            # the layer providers
            TRT_LayerProvider = Trk__LayerProviderCond(name=namePrefix+'TRT_LayerProviderCond')
            TRT_LayerProvider.LayerBuilder = TRT_LayerBuilder
            ToolSvc += TRT_LayerProvider
                              
            # put them to the caches
            layerProviders  += [ TRT_LayerProvider ]
            binningsCenter  += [ TRT_LayerBinning ]
            binningsEndcap  += [ TRT_LayerBinning ]
            colors          += [ 5 ]
        
        
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
        InDet__StagedTrackingGeometryBuilderCond.__init__(self,namePrefix+name,\
                                                      LayerBuilders                 = layerProviders,
                                                      LayerBinningTypeCenter        = binningsCenter,
                                                      LayerBinningTypeEndcap        = binningsEndcap,
                                                      ColorCodes                    = colors,
                                                      EnvelopeDefinitionSvc         = AtlasEnvelopeSvc,
                                                      VolumeEnclosureDiscPositions  = [ 3000., 3450. ],
                                                      TrackingVolumeCreator         = InDetCylinderVolumeCreator,
                                                      LayerArrayCreator             = InDetLayerArrayCreator,
                                                      CheckForRingLayout            = True,
                                                      BuildBoundaryLayers           = TrkDetFlags.InDetBuildMaterialBoundaries(),
                                                      ReplaceAllJointBoundaries     = TrkDetFlags.InDetBuildJointBoundaries(),
                                                      OutputLevel                   = TrkDetFlags.InDetBuildingOutputLevel(),
                                                      ExitVolumeName                = TrkDetFlags.InDetContainerName(),
                                                      MagneticFieldMode             = TrkDetFlags.MagneticFieldMode())


