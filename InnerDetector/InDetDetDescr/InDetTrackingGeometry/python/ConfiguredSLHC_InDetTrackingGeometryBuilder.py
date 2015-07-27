# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

######################################################
# ConfiguredSLHC_InDetTrackingGeometry module
#
# it inherits from StagedTrackingGeometryBuilderConf and performs 
# standard configuration
#
######################################################

# import the Extrapolator configurable
from InDetTrackingGeometry.InDetTrackingGeometryConf import InDet__StagedTrackingGeometryBuilder

# define the class
class ConfiguredSLHC_InDetTrackingGeometryBuilder( InDet__StagedTrackingGeometryBuilder ):
    # constructor
    def __init__(self,name = 'InDetTrackingGeometryBuilder',
                      namePrefix = '',
                      setLayerAssociation = True):

        # get the ToolSvc
        from AthenaCommon.AppMgr import ToolSvc, ServiceMgr           
        from AthenaCommon.DetFlags import DetFlags
       
        # the Detector flags to be imported
        from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
        
        # beampipe        
        from InDetTrackingGeometry.InDetTrackingGeometryConf import InDet__BeamPipeBuilder
        BeamPipeBuilder = InDet__BeamPipeBuilder(name=namePrefix+'BeamPipeBuilder')
        BeamPipeBuilder.BeamPipeRadius 	        = TrkDetFlags.BeamPipeRadius()
        BeamPipeBuilder.BeamPipeThickness       = TrkDetFlags.BeamPipeThickness() 
        BeamPipeBuilder.BeamPipeMaterialBinsZ   = TrkDetFlags.BeamPipeLayerMaterialBinsZ()
        BeamPipeBuilder.OutputLevel             = TrkDetFlags.InDetBuildingOutputLevel()
        # the binning type of the layers   
        BeamPipeBinning = 2
        # where to build it from
        BeamPipeBuilder.BeamPipeFromGeoModel    = TrkDetFlags.BeamPipeFromGeoModel()
        BeamPipeBuilder.BeamPipeEnvelope        = TrkDetFlags.BeamPipeEnvelope()
        if not TrkDetFlags.BeamPipeFromGeoModel():
            BeamPipeBuilder.BeamPipeOffsetX	        = TrkDetFlags.BeamPipeOffsetX()
            BeamPipeBuilder.BeamPipeOffsetY	        = TrkDetFlags.BeamPipeOffsetY()
            BeamPipeBuilder.BeamPipeRadius 	        = TrkDetFlags.BeamPipeRadius()

        ToolSvc += BeamPipeBuilder

        # the layer providers
        from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__LayerProvider
        BeamPipeProvider = Trk__LayerProvider(name=namePrefix+'BeamPipeProvider')
        BeamPipeProvider.LayerBuilder = BeamPipeBuilder
        ToolSvc += BeamPipeProvider
                
        # the layer builders
        layerProviders   = [ BeamPipeProvider ]
        binningsCenter  = [ BeamPipeBinning ]
        binningsEndcap  = [ BeamPipeBinning ]
        colors          = [ 2 ]
                                
        # PIXEL building
        from InDetTrackingGeometry.InDetTrackingGeometryConf import InDet__SiLayerBuilder
        PixelLayerBuilder = InDet__SiLayerBuilder(name=namePrefix+'PixelLayerBuilder')
        PixelLayerBuilder.PixelCase 	       = True
        PixelLayerBuilder.Identification       = 'Pixel1'
        PixelLayerBuilder.SiDetManagerLocation = 'Pixel'
        # The pixel split mode
        PixelLayerBuilder.SplitMode            = TrkDetFlags.PixelSplitMode()
        # Pixel barrel specifications
        PixelLayerBuilder.BarrelLayerBinsZ     = TrkDetFlags.PixelBarrelLayerMaterialBinsZ()
        PixelLayerBuilder.BarrelLayerBinsPhi   = TrkDetFlags.PixelBarrelLayerMaterialBinsPhi()
        PixelLayerBuilder.EndcapLayerBinsR     = TrkDetFlags.PixelEndcapLayerMaterialBinsR()
        PixelLayerBuilder.EndcapLayerBinsPhi   = TrkDetFlags.PixelEndcapLayerMaterialBinsPhi()
        # set the layer association
        PixelLayerBuilder.SetLayerAssociation  = setLayerAssociation
        # output level
        PixelLayerBuilder.OutputLevel         = TrkDetFlags.PixelBuildingOutputLevel()
        # the binning type of the layers   
        PixelLayerBinning = 2
        # add it to tool service
        ToolSvc += PixelLayerBuilder
        # put them to the caches
        PixelLayerProvider = Trk__LayerProvider(name=namePrefix+'PixelLayerProvider')
        PixelLayerProvider.LayerBuilder = PixelLayerBuilder
        ToolSvc += PixelLayerProvider
        layerProviders  += [ PixelLayerProvider ]
        binningsCenter += [ PixelLayerBinning ]
        binningsEndcap += [ PixelLayerBinning ]
        colors         += [ 3 ]
        # check if the pixel split mode is one
        if TrkDetFlags.PixelSplitMode() is not 0:
            # set split mode to pixel layer builder 
            print "[SLHC_GeometryBuilder] Building Pixel layers in split mode"
            # change the identificaiton of the first one
            PixelLayerBuilder.Identification       = 'Pixel1'
            # and now configure the second one
            PixelLayerBuilder2 = InDet__SiLayerBuilder(name=namePrefix+'PixelLayerBuilder2')
            PixelLayerBuilder2.PixelCase 	        = True
            PixelLayerBuilder2.SplitMode            = TrkDetFlags.PixelSplitMode()
            PixelLayerBuilder2.Identification       = 'Pixel2'
            PixelLayerBuilder2.SiDetManagerLocation = 'Pixel'
            # Pixel barrel specifications
            PixelLayerBuilder2.BarrelLayerBinsZ     = TrkDetFlags.PixelBarrelLayerMaterialBinsZ()
            PixelLayerBuilder2.BarrelLayerBinsPhi   = TrkDetFlags.PixelBarrelLayerMaterialBinsPhi()
            PixelLayerBuilder2.EndcapLayerBinsR     = TrkDetFlags.PixelEndcapLayerMaterialBinsR()
            PixelLayerBuilder2.EndcapLayerBinsPhi   = TrkDetFlags.PixelEndcapLayerMaterialBinsPhi()
            # set the layer association
            PixelLayerBuilder2.SetLayerAssociation  = setLayerAssociation
            # output level
            PixelLayerBuilder2.OutputLevel         = TrkDetFlags.PixelBuildingOutputLevel()
            # add it to tool service
            ToolSvc += PixelLayerBuilder2

            # the layer providers
            PixelLayerProvider2 = Trk__LayerProvider(name=namePrefix+'PixelLayerProvider2')
            PixelLayerProvider2.LayerBuilder = PixelLayerBuilder2
            ToolSvc += PixelLayerProvider2
            # put them to the caches
            layerProviders  += [ PixelLayerProvider2 ]
            binningsCenter += [ PixelLayerBinning ]
            binningsEndcap += [ PixelLayerBinning ]
            colors         += [ 3 ]
             
        
        if DetFlags.SCT_on() :
            # SCT building
            SCT_LayerBuilder = InDet__SiLayerBuilder(name=namePrefix+'SCT_LayerBuilder')
            SCT_LayerBuilder.PixelCase                       = False
            SCT_LayerBuilder.Identification                  = 'SCT1'
            SCT_LayerBuilder.SiDetManagerLocation            = 'SCT'
            # general steering
            SCT_LayerBuilder.SplitMode                       = TrkDetFlags.SCT_SplitMode()
            # SCT barrel specifications
            SCT_LayerBuilder.BarrelLayerBinsZ                = TrkDetFlags.SCT_BarrelLayerMaterialBinsZ()
            SCT_LayerBuilder.BarrelLayerBinsPhi              = TrkDetFlags.SCT_BarrelLayerMaterialBinsPhi()
            # SCT endcap specifications                          
            SCT_LayerBuilder.EndcapLayerBinsR                = TrkDetFlags.SCT_EndcapLayerMaterialBinsR()
            SCT_LayerBuilder.EndcapLayerBinsPhi              = TrkDetFlags.SCT_EndcapLayerMaterialBinsPhi()
            # SCT_LayerBuilder.EndcapComplexRingBinning        = TrkDetFlags.SCT_EndcapLayerDynamicRings()
            # set the layer association                   
            SCT_LayerBuilder.SetLayerAssociation             = setLayerAssociation        
            # output level                                 
            SCT_LayerBuilder.OutputLevel                     = TrkDetFlags.SCT_BuildingOutputLevel()
            # the binning type of the layer     
            SCT_LayerBinning = 2
            # SCT -> ToolSvc                             
            ToolSvc += SCT_LayerBuilder                       
            # the layer providers
            SCT_LayerProvider = Trk__LayerProvider(name=namePrefix+'SCT_LayerProvider')
            SCT_LayerProvider.LayerBuilder = SCT_LayerBuilder
            ToolSvc += SCT_LayerProvider

            # put them to the caches
            layerProviders  += [ SCT_LayerProvider ]
            binningsCenter += [ SCT_LayerBinning ]
            binningsEndcap += [ SCT_LayerBinning ]
            colors         += [ 4 ]
                                                              
            # the split and the second SCT layer builder is needed if the SCT outer barrel
            # spans over the full width      
            if TrkDetFlags.SCT_SplitMode() is not 0 :
              print "[SLHC_GeometryBuilder] Building SCT layers in split mode"
              from InDetTrackingGeometry.InDetTrackingGeometryConf import InDet__SiLayerBuilder
              SCT_LayerBuilder2 = InDet__SCT_LayerBuilder(name=namePrefix+'SCT_LayerBuilder2')
            
              # switch of the pixel case
              SCT_LayerBuilder2.PixelCase                = False
              SCT_LayerBuilder2.SiDetManagerLocation     = 'SCT'
              # set the general parameters
              SCT_LayerBuilder2.OutputLevel              = 2
              SCT_LayerBuilder2.SplitMode                = TrkDetFlags.SCT_SplitMode()
              SCT_LayerBuilder2.Identification           = "SCT2"
              # SCT barrel specifications
              SCT_LayerBuilder2.BarrelLayerBinsZ         = TrkDetFlags.SCT_BarrelLayerMaterialBinsZ()
              SCT_LayerBuilder2.BarrelLayerBinsPhi       = TrkDetFlags.SCT_BarrelLayerMaterialBinsPhi()
              # SCT endcap specifications                     
              SCT_LayerBuilder2.EndcapLayerBinsR         = TrkDetFlags.SCT_EndcapLayerMaterialBinsR()
              SCT_LayerBuilder2.EndcapLayerBinsPhi       = TrkDetFlags.SCT_EndcapLayerMaterialBinsPhi()
              # set the layer association
              SCT_LayerBuilder2.SetLayerAssociation = setLayerAssociation        
              # output level
              SCT_LayerBuilder2.OutputLevel         = TrkDetFlags.SCT_BuildingOutputLevel()
              ToolSvc += SCT_LayerBuilder2
              # the layer providers
              SCT_LayerProvider2 = Trk__LayerProvider(name=namePrefix+'SCT_LayerProvider2')
              SCT_LayerProvider2.LayerBuilder = SCT_LayerBuilder2
              ToolSvc += SCT_LayerProvider2
              # put them to the caches
              layerProviders  += [ SCT_LayerProvider2 ] 
              binningsCenter += [ SCT_LayerBinning ]
              binningsEndcap += [ SCT_LayerBinning ]
              colors         += [ 5 ]

        
        # helpers for the InDetTrackingGeometry Builder : layer array creator
        from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__LayerArrayCreator
        InDetLayerArrayCreator = Trk__LayerArrayCreator(name = 'InDetLayerArrayCreator')
        InDetLayerArrayCreator.EmptyLayerMode           = 2 # deletes empty material layers from arrays
        InDetLayerArrayCreator.OutputLevel              = TrkDetFlags.InDetBuildingOutputLevel()
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
        InDetTrackingVolumeHelper.OutputLevel                 = TrkDetFlags.InDetBuildingOutputLevel()
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
        InDetCylinderVolumeCreator.OutputLevel                = TrkDetFlags.InDetBuildingOutputLevel()        
        # add to ToolSvc
        ToolSvc += InDetCylinderVolumeCreator        
        
        # the envelope definition service
        from SubDetectorEnvelopes.SubDetectorEnvelopesConf import DetDescrDBEnvelopeSvc
        AtlasEnvelopeSvc = DetDescrDBEnvelopeSvc('AtlasEnvelopeSvcDefinitionSvc')
        # set the output level for the Envelope service
        AtlasEnvelopeSvc.OutputLevel      = TrkDetFlags.InDetBuildingOutputLevel()  
        # add to SvcMgr
        ServiceMgr += AtlasEnvelopeSvc
        
        # the tracking geometry builder
        InDet__StagedTrackingGeometryBuilder.__init__(self,namePrefix+name,\
                                                      BeamPipeBuilder           = BeamPipeBuilder,\
                                                      LayerBuilders             = layerProviders,
                                                      LayerBinningTypeCenter    = binningsCenter,
                                                      LayerBinningTypeEndcap    = binningsEndcap,
                                                      ColorCodes                = colors,
                                                      EnvelopeDefinitionSvc     = AtlasEnvelopeSvc,
                                                      # VolumeEnclosureDiscPositions = [ 3000., 3450. ],
                                                      TrackingVolumeCreator     = InDetCylinderVolumeCreator,
                                                      LayerArrayCreator         = InDetLayerArrayCreator,
                                                      BuildBoundaryLayers       = True,
                                                      # ReplaceAllJointBoundaries = TrkDetFlags.InDetBuildMaterialBoundaries(),
                                                      OutputLevel               = TrkDetFlags.InDetBuildingOutputLevel(),
                                                      ExitVolumeName            = TrkDetFlags.InDetContainerName(),
                                                      MagneticFieldMode         = TrkDetFlags.MagneticFieldMode(),
                                                      isSLHC = True)
