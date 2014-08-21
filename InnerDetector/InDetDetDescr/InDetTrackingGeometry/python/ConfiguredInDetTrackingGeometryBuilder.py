# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

######################################################
# ConfiguredInDetTrackingGeometry module
#
# it inherits from RobustTrackingGeometryBuilderConf and performs 
# standard configuration
#
######################################################

# import the Extrapolator configurable
from InDetTrackingGeometry.InDetTrackingGeometryConf import InDet__RobustTrackingGeometryBuilder

# define the class
class ConfiguredInDetTrackingGeometryBuilder( InDet__RobustTrackingGeometryBuilder ):
    # constructor
    def __init__(self,name = 'InDetTrackingGeometryBuilder',
                      namePrefix = '',
                      setLayerAssociation = True,
                      buildTrtStrawLayers = False):

        # get the ToolSvc
        from AthenaCommon.AppMgr import ToolSvc, ServiceMgr           
       
        # the Detector flags to be imported
        from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
        
        # beampipe        
        from InDetTrackingGeometry.InDetTrackingGeometryConf import InDet__BeamPipeBuilder
        BeamPipeBuilder = InDet__BeamPipeBuilder(name=namePrefix+'BeamPipeBuilder')
        BeamPipeBuilder.BeamPipeRadius 	        = TrkDetFlags.BeamPipeRadius()
        BeamPipeBuilder.BeamPipeThickness       = TrkDetFlags.BeamPipeThickness() 
        BeamPipeBuilder.BeamPipeMaterialBinsZ   = TrkDetFlags.BeamPipeLayerMaterialBinsZ()
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
        PixelLayerBuilder.EndcapAdditionalLayerPositionsZ = [ -1900. , 1900. ]
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
        SCT_LayerBuilder.PixelCase                  = False
        SCT_LayerBuilder.Identification             = 'SCT'
        SCT_LayerBuilder.SiDetManagerLocation       = 'SCT'
        # additionall radii - handle with care !
        SCT_LayerBuilder.BarrelAdditionalLayerRadii = [ 200., 225. ] # This contains the PST
        # SCT barrel specifications
        SCT_LayerBuilder.BarrelLayerBinsZ   = TrkDetFlags.SCT_BarrelLayerMaterialBinsZ()
        SCT_LayerBuilder.BarrelLayerBinsPhi = TrkDetFlags.SCT_BarrelLayerMaterialBinsPhi()
        # SCT endcap specifications                     
        # SCT_LayerBuilder.EndcapLayerInnerR        = TrkDetFlags.SCT_EndcapLayerInnerR()
        # SCT_LayerBuilder.EndcapLayerOuterR        = TrkDetFlags.SCT_EndcapLayerOuterR()
        SCT_LayerBuilder.EndcapLayerBinsR         = TrkDetFlags.SCT_EndcapLayerMaterialBinsR()
        SCT_LayerBuilder.EndcapLayerBinsPhi       = TrkDetFlags.SCT_EndcapLayerMaterialBinsPhi()
        SCT_LayerBuilder.EndcapComplexRingBinning = TrkDetFlags.SCT_EndcapLayerDynamicRings()
        # set the layer association
        SCT_LayerBuilder.SetLayerAssociation = setLayerAssociation        
        # output level
        SCT_LayerBuilder.OutputLevel        = TrkDetFlags.SCT_BuildingOutputLevel()
        # the binning type of the layer     
        SCT_LayerBinning = 2
        # SCT -> ToolSvc                             
        ToolSvc += SCT_LayerBuilder                       
        # put them to the caches
        layerbuilders += [ SCT_LayerBuilder ]
        binnings      += [ SCT_LayerBinning ]
        colors        += [ 4 ]
                                                          
        from InDetTrackingGeometry.InDetTrackingGeometryConf import InDet__TRT_LayerBuilder
        TRT_LayerBuilder = InDet__TRT_LayerBuilder(name=namePrefix+'TRT_LayerBuilder')
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
        # put them to the caches
        layerbuilders += [ TRT_LayerBuilder ]
        binnings      += [ TRT_LayerBinning ]
        colors        += [ 5 ]
        
        
        # layer && tracking volume array creator
        from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__LayerArrayCreator
        InDetLayerArrayCreator = Trk__LayerArrayCreator(name = 'InDetLayerArrayCreator')
        InDetLayerArrayCreator.OutputLevel           = TrkDetFlags.InDetBuildingOutputLevel()
        # add to ToolSvc
        ToolSvc += InDetLayerArrayCreator

        from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__TrackingVolumeArrayCreator
        InDetTrackingVolumeArrayCreator = Trk__TrackingVolumeArrayCreator(name = 'InDetTrackingVolumeArrayCreator')
        InDetTrackingVolumeArrayCreator.OutputLevel           = TrkDetFlags.InDetBuildingOutputLevel()
        # add to ToolSvc
        ToolSvc += InDetTrackingVolumeArrayCreator
        
        # helpers for the InDetTrackingGeometry Builder
        from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__CylinderVolumeCreator
        InDetCylinderVolumeCreator = Trk__CylinderVolumeCreator(name = 'InDetCylinderVolumeCreator')
        # give him the layer array creator
        InDetCylinderVolumeCreator.LayerArrayCreator          = InDetLayerArrayCreator
        InDetCylinderVolumeCreator.TrackingVolumeArrayCreator = InDetTrackingVolumeArrayCreator
        # specifiy the binning, passive layers, entry layers
        InDetCylinderVolumeCreator.PassiveLayerThickness = TrkDetFlags.InDetPassiveLayerThickness()  
        InDetCylinderVolumeCreator.PassiveLayerBinsPhi   = TrkDetFlags.InDetPassiveLayerMaterialBinsPhi()  
        InDetCylinderVolumeCreator.PassiveLayerBinsRZ    = TrkDetFlags.InDetPassiveLayerMaterialBinsRz()  
        InDetCylinderVolumeCreator.EntryLayerThickness   = TrkDetFlags.InDetEntryLayerThickness()          
        InDetCylinderVolumeCreator.EntryLayerBinsPhi     = TrkDetFlags.InDetEntryLayerMaterialBinsPhi()  
        InDetCylinderVolumeCreator.EntryLayerBinsRZ      = TrkDetFlags.InDetEntryLayerMaterialBinsRz()  
        # specify the envelope cover
        InDetCylinderVolumeCreator.EnvelopeCoverInnerR   = TrkDetFlags.InDetEnvelopeCoverInnerR()  
        InDetCylinderVolumeCreator.EnvelopeCoverOuterR   = TrkDetFlags.InDetEnvelopeCoverOuterR()  
        InDetCylinderVolumeCreator.EnvelopeCoverMinZ     = TrkDetFlags.InDetEnvelopeCoverMinZ()  
        InDetCylinderVolumeCreator.EnvelopeCoverMaxZ     = TrkDetFlags.InDetEnvelopeCoverMaxZ()  
        # output level
        InDetCylinderVolumeCreator.OutputLevel           = TrkDetFlags.InDetBuildingOutputLevel()        
        # add to ToolSvc
        ToolSvc += InDetCylinderVolumeCreator        
        
        # the envelope definition service
        from SubDetectorEnvelopes.SubDetectorEnvelopesConf import DetDescrDBEnvelopeSvc
        AtlasEnvelopeSvc = DetDescrDBEnvelopeSvc('AtlasEnvelopeSvcDefinitionSvc')
        AtlasEnvelopeSvc.EnableFallback = True
        #AtlasEnvelopeSvc.FallbackInDetVol =  [ 0, 0, 0, 0 ]
        AtlasEnvelopeSvc.FallbackInDetR   =  [ 0., 1148.]
        AtlasEnvelopeSvc.FallbackInDetZ   =  [ 3475., 3475. ]
        # set the output level for the Envelope service
        AtlasEnvelopeSvc.OutputLevel      = TrkDetFlags.InDetBuildingOutputLevel()  
        # add to SvcMgr
        ServiceMgr += AtlasEnvelopeSvc
        
        # the tracking geometry builder
        InDet__RobustTrackingGeometryBuilder.__init__(self,namePrefix+name,\
                                                      BeamPipeBuilder = BeamPipeBuilder,\
                                                      LayerBuilders    = layerbuilders,
                                                      LayerBinningType = binnings,
                                                      ColorCodes       = colors,
                                                      BarrelEntryLayers = [ 2, 2, 2 ],
                                                      EndcapEntryLayers = [ 1, 0, 1 ],
                                                      EnvelopeDefinitionSvc = AtlasEnvelopeSvc,
                                                      AdditionalEndcapCylinder = [ 225. ],
                                                      EndcapEnclosureEntryLayers = 1, 
                                                      VolumeEnclosureDiscPositions = [ 2800., 3450. ],
                                                      TrackingVolumeCreator = InDetCylinderVolumeCreator,
                                                      LayerArrayCreator = InDetLayerArrayCreator,
                                                      InnerEnevelope = TrkDetFlags.InDetEnvelopeCoverInnerR(),
                                                      OuterEnevelope = TrkDetFlags.InDetEnvelopeCoverOuterR(),
                                                      OutputLevel = TrkDetFlags.InDetBuildingOutputLevel(),
                                                      ExitVolumeName = TrkDetFlags.InDetContainerName(),
                                                      MagneticFieldMode = TrkDetFlags.MagneticFieldMode())


