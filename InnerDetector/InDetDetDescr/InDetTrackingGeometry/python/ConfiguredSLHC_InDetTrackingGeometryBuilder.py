# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

######################################################
# ConfiguredSLHC_InDetTrackingGeometry module
#
# it inherits from RobustTrackingGeometryBuilderConf and performs 
# standard configuration
#
######################################################

# import the Extrapolator configurable
from InDetTrackingGeometry.InDetTrackingGeometryConf import InDet__RobustTrackingGeometryBuilder

# define the class
class ConfiguredSLHC_InDetTrackingGeometryBuilder( InDet__RobustTrackingGeometryBuilder ):
    # constructor
    def __init__(self,name = 'InDetTrackingGeometryBuilder',
                      namePrefix = '',
                      setLayerAssociation = True):

        # get the ToolSvc
        from AthenaCommon.AppMgr import ToolSvc, ServiceMgr           
       
        # the Detector flags to be imported
        from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
        
        from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags
        SLHC_Version = SLHC_Flags.SLHC_Version.get_Value()
        splitMode = False
        # get the subversions out
        if SLHC_Flags.SLHC_Version() != '' :
           print '[SLHC Version] Chosen version : ', SLHC_Version
           SubVersions = SLHC_Version.split('-')
           SLHC_PixVersion = int(SubVersions[1])
           SLHC_SctVersion = int(SubVersions[2])
           splitMode = True
           if SLHC_SctVersion is not 2 :
             splitMode = False
             print '[ SLHC_TrackingGeometry ] SCT: Not running in split-mode - projective layout !'

        # beampipe        
        from InDetTrackingGeometry.InDetTrackingGeometryConf import InDet__BeamPipeBuilder
        BeamPipeBuilder = InDet__BeamPipeBuilder(name=namePrefix+'BeamPipeBuilder')
        BeamPipeBuilder.BeamPipeRadius 	        = TrkDetFlags.BeamPipeRadius()
        BeamPipeBuilder.BeamPipeThickness       = TrkDetFlags.BeamPipeThickness() 
        BeamPipeBuilder.BeamPipeMaterialBinsZ   = TrkDetFlags.BeamPipeLayerMaterialBinsZ()
        BeamPipeBuilder.OutputLevel             = TrkDetFlags.InDetBuildingOutputLevel()

        BeamPipeBuilder.BeamPipeFromGeoModel    = TrkDetFlags.BeamPipeFromGeoModel()
        BeamPipeBuilder.BeamPipeEnvelope        = TrkDetFlags.BeamPipeEnvelope()
        if not TrkDetFlags.BeamPipeFromGeoModel():
            BeamPipeBuilder.BeamPipeOffsetX	        = TrkDetFlags.BeamPipeOffsetX()
            BeamPipeBuilder.BeamPipeOffsetY	        = TrkDetFlags.BeamPipeOffsetY()
            BeamPipeBuilder.BeamPipeRadius 	        = TrkDetFlags.BeamPipeRadius()

        ToolSvc += BeamPipeBuilder
                
        # the layer builders
        layerbuilders = []
        binnings      = []
        colors        = []
                        
        # @TODO put DetFlags isOn here, but make sure that this is really when the detector is not built        
        
        # PIXEL building
        from InDetTrackingGeometry.InDetTrackingGeometryConf import InDet__SiLayerBuilder
        PixelLayerBuilder = InDet__SiLayerBuilder(name=namePrefix+'PixelLayerBuilder')
        PixelLayerBuilder.PixelCase 	       = True
        PixelLayerBuilder.Identification       = 'Pixel'
        PixelLayerBuilder.SiDetManagerLocation = 'Pixel'
        # additionall layers - handle with care !
        # PixelLayerBuilder.BarrelAdditionalLayerRadii      = [ 130 ]   # The PST
        # PixelLayerBuilder.BarrelAdditionalLayerType       = [ 0 ]     # -- will shift volume boundary to PST
        # PixelLayerBuilder.EndcapAdditionalLayerPositionsZ = [ -1900. , 1900. ] # DBM
        # PixelLayerBuilder.EndcapAdditionalLayerType       = [  1 , 1 ] # DBM
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
        layerbuilders += [ PixelLayerBuilder ]
        binnings      += [ PixelLayerBinning ]
        colors        += [ 3 ]
        
        # SCT building
        SCT_LayerBuilder = InDet__SiLayerBuilder(name=namePrefix+'SCT_LayerBuilder')
        SCT_LayerBuilder.PixelCase                       = False
        SCT_LayerBuilder.Identification                  = 'SCT1'
        SCT_LayerBuilder.SiDetManagerLocation            = 'SCT'
        # general steering
        SCT_LayerBuilder.SplitMode                = splitMode
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
        # put them to the caches
        layerbuilders += [ SCT_LayerBuilder ]
        binnings      += [ SCT_LayerBinning ]
        colors        += [ 4 ]
                                                          
        # the split and the second SCT layer builder is needed if the SCT outer barrel
        # spans over the full width      
        if splitMode :
          print "[SLHC_GeometryBuilder] Building SCT2"
          from InDetTrackingGeometry.InDetTrackingGeometryConf import InDet__SCT_LayerBuilder
          SCT_LayerBuilder2 = InDet__SCT_LayerBuilder(name=namePrefix+'SCT_LayerBuilder2')

          # switch of the pixel case
          SCT_LayerBuilder2.PixelCase                = False
          SCT_LayerBuilder2.SiDetManagerLocation     = 'SCT'
          # set the general parameters
          SCT_LayerBuilder2.OutputLevel              = 2
          SCT_LayerBuilder2.SplitMode                = True
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
          layerbuilders += [ SCT_LayerBuilder2 ] 
          binnings      += [ SCT_LayerBinning ]
          colors        += [ 5 ]

        
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
        InDet__RobustTrackingGeometryBuilder.__init__(self,namePrefix+name,\
                                                      BeamPipeBuilder   = BeamPipeBuilder,\
                                                      LayerBuilders     = layerbuilders,
                                                      LayerBinningType  = binnings,
                                                      ColorCodes        = colors,
                                                      EndcapEntryLayers = [ 1, 1 ],
                                                      BarrelEntryLayers = [ 2, 2 ],
                                                      EnvelopeDefinitionSvc = AtlasEnvelopeSvc,
                                                      # VolumeEnclosureDiscPositions = [ 3000., 3450. ],
                                                      TrackingVolumeCreator = InDetCylinderVolumeCreator,
                                                      LayerArrayCreator = InDetLayerArrayCreator,
                                                      BuildBoundaryLayers       = True,
                                                      # ReplaceAllJointBoundaries = TrkDetFlags.InDetBuildMaterialBoundaries(),
                                                      OutputLevel               = TrkDetFlags.InDetBuildingOutputLevel(),
                                                      ExitVolumeName            = TrkDetFlags.InDetContainerName(),
                                                      MagneticFieldMode         = TrkDetFlags.MagneticFieldMode(),
                                                      isSLHC = True)
