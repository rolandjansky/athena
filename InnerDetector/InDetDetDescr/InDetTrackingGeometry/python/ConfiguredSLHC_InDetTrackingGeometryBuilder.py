# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

######################################################
# ConfiguredSLHC_InDetTrackingGeometry module
#
# it inherits from RobustTrackingGeometryBuilderCinf and performs 
# standard configuration
#
######################################################

# import the include statement
from AthenaCommon.Include import Include, IncludeError, include

# import the ToolSvc
from AthenaCommon.AppMgr import ToolSvc

# import the Extrapolator configurable
from InDetTrackingGeometry.InDetTrackingGeometryConf import InDet__RobustTrackingGeometryBuilder

# define the class
class ConfiguredSLHC_InDetTrackingGeometryBuilder( InDet__RobustTrackingGeometryBuilder ):
    # constructor
    def __init__(self,name = 'InDetTrackingGeometryBuilder',
                      namePrefix = '',
                      setLayerAssociation = True):

        # get the ToolSvc
        from AthenaCommon.AppMgr import ToolSvc           
       
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

        print "[SLHC_GeometryBuilder] Building BeamPipe"
        from InDetTrackingGeometry.InDetTrackingGeometryConf import InDet__BeamPipeBuilder
        BeamPipeBuilder = InDet__BeamPipeBuilder(name='BeamPipeBuilder')
        BeamPipeBuilder.BeamPipeFromGeoModel    = TrkDetFlags.BeamPipeFromGeoModel()
        BeamPipeBuilder.BeamPipeEnvelope        = TrkDetFlags.BeamPipeEnvelope()
        if not TrkDetFlags.BeamPipeFromGeoModel():
            BeamPipeBuilder.BeamPipeOffsetX	        = TrkDetFlags.BeamPipeOffsetX()
            BeamPipeBuilder.BeamPipeOffsetY	        = TrkDetFlags.BeamPipeOffsetY()
            BeamPipeBuilder.BeamPipeRadius 	        = TrkDetFlags.BeamPipeRadius()
        BeamPipeBuilder.BeamPipeThickness       = TrkDetFlags.BeamPipeThickness() 
        BeamPipeBuilder.BeamPipeMaterialBinsZ   = TrkDetFlags.BeamPipeLayerMaterialBinsZ()
        ToolSvc += BeamPipeBuilder
        
        SLHC_LayerBuilders    = []
        SLHC_LayerBinningType = []
        SLHC_ColorCodes       = []

        print "[SLHC_GeometryBuilder] Building Pixels"        
        from InDetTrackingGeometry.InDetTrackingGeometryConf import InDet__SiLayerBuilder
        PixelLayerBuilder = InDet__SiLayerBuilder(name=namePrefix+'PixelLayerBuilder')

        PixelLayerBuilder.RobustOverlaps           = True

        # sset the pixel case
        PixelLayerBuilder.PixelCase                = True
        PixelLayerBuilder.SiDetManagerLocation     = 'Pixel'
        PixelLayerBuilder.Identification           = 'Pixel'
        PixelLayerBuilder.BarrelAdditionalLayerRadii = [ 110.0 , 347.0]
        # Pixel barrel specifications
        PixelLayerBuilder.BarrelLayerDynamicExtend = True
        PixelLayerBuilder.BarrelLayerExtend        = 0.
        PixelLayerBuilder.BarrelLayerBinsZ         = TrkDetFlags.PixelBarrelLayerMaterialBinsZ()
        PixelLayerBuilder.BarrelLayerBinsPhi       = TrkDetFlags.PixelBarrelLayerMaterialBinsPhi()
        # Pixel endcap specifications
        PixelLayerBuilder.EndcapLayerDynamicExtend = True
        PixelLayerBuilder.EndcapLayerInnerR        = 0.
        PixelLayerBuilder.EndcapLayerOuterR        = 0.
        PixelLayerBuilder.EndcapLayerBinsR         = TrkDetFlags.PixelEndcapLayerMaterialBinsR()
        PixelLayerBuilder.EndcapLayerBinsPhi       = TrkDetFlags.PixelEndcapLayerMaterialBinsPhi()
        # set the layer association
        PixelLayerBuilder.SetLayerAssociation = setLayerAssociation
        # output level
        PixelLayerBuilder.OutputLevel         = TrkDetFlags.PixelBuildingOutputLevel()
        # the binning type of the layers                
        PixelLayerBinning = 2
        # experimental
        PixelLayerBuilder.BarrelLayerComplexZBinning = [ 0, 0, 1, 1 ]
        ToolSvc += PixelLayerBuilder
        SLHC_LayerBuilders    += [ PixelLayerBuilder ]         
        SLHC_LayerBinningType += [ 2 ]
        SLHC_ColorCodes       += [ 3 ]

        print "[SLHC_GeometryBuilder] Building SCT1"
        
        SCT_LayerBuilder1 = InDet__SiLayerBuilder(name=namePrefix+'SCT_LayerBuilder1')

        SCT_LayerBuilder1.RobustOverlaps           = True

        # switch of the pixel case
        SCT_LayerBuilder1.PixelCase                = False
        SCT_LayerBuilder1.SiDetManagerLocation     = 'SCT'
        # general steering
        SCT_LayerBuilder1.OutputLevel              = 2
        SCT_LayerBuilder1.SplitMode                = splitMode
        SCT_LayerBuilder1.Identification           = "SCT1"
        SCT_LayerBuilder1.EndcapAdditionalLayerPositionsZ = [1278.0, -1278.0]
        SCT_LayerBuilder1.BarrelAdditionalLayerRadii = [363.5, 476.5, 589, 720, 947.5]
        # SCT barrel specifications
        SCT_LayerBuilder1.BarrelLayerDynamicExtend = True
        SCT_LayerBuilder1.BarrelLayerExtend        = 0.
        SCT_LayerBuilder1.BarrelLayerBinsZ         = TrkDetFlags.SCT_BarrelLayerMaterialBinsZ()
        SCT_LayerBuilder1.BarrelLayerBinsPhi       = TrkDetFlags.SCT_BarrelLayerMaterialBinsPhi()
        # SCT endcap specifications                     
        SCT_LayerBuilder1.EndcapLayerDynamicExtend = True
        SCT_LayerBuilder1.EndcapLayerInnerR        = 0.
        SCT_LayerBuilder1.EndcapLayerOuterR        = 0.  
        SCT_LayerBuilder1.EndcapLayerBinsR         = TrkDetFlags.SCT_EndcapLayerMaterialBinsR()
        SCT_LayerBuilder1.EndcapLayerBinsPhi       = TrkDetFlags.SCT_EndcapLayerMaterialBinsPhi()
        # set the layer association
        SCT_LayerBuilder1.SetLayerAssociation = setLayerAssociation        
        # output level
        SCT_LayerBuilder1.OutputLevel         = TrkDetFlags.SCT_BuildingOutputLevel()
        ToolSvc += SCT_LayerBuilder1
        SCT_LayerBuilder1.BarrelLayerComplexZBinning = [ 0, 0, 0, 0, 1, 0 ]
        SLHC_LayerBuilders    += [ SCT_LayerBuilder1 ]         
        SLHC_LayerBinningType += [ 2 ]
        SLHC_ColorCodes       += [ 4 ]  
         
        # the split and the second SCT layer builder is needed if the SCT outer barrel
        # spans over the full width      
        if splitMode :
          print "[SLHC_GeometryBuilder] Building SCT2"
          from InDetTrackingGeometry.InDetTrackingGeometryConf import InDet__SCT_LayerBuilder
          SCT_LayerBuilder2 = InDet__SCT_LayerBuilder(name=namePrefix+'SCT_LayerBuilder2')

          SCT_LayerBuilder2.RobustOverlaps           = True

          # switch of the pixel case
          SCT_LayerBuilder2.PixelCase                = False
          SCT_LayerBuilder2.SiDetManagerLocation     = 'SCT'
          # set the general parameters
          SCT_LayerBuilder2.OutputLevel              = 2
          SCT_LayerBuilder2.SplitMode                = True
          SCT_LayerBuilder2.Identification           = "SCT2"
          # SCT barrel specifications
          SCT_LayerBuilder2.BarrelLayerDynamicExtend = True
          SCT_LayerBuilder2.BarrelLayerExtend        = 0.
          SCT_LayerBuilder2.BarrelLayerBinsZ         = TrkDetFlags.SCT_BarrelLayerMaterialBinsZ()
          SCT_LayerBuilder2.BarrelLayerBinsPhi       = TrkDetFlags.SCT_BarrelLayerMaterialBinsPhi()
          # SCT endcap specifications                     
          SCT_LayerBuilder2.EndcapLayerDynamicExtend = True
          SCT_LayerBuilder2.EndcapLayerInnerR        = 0.
          SCT_LayerBuilder2.EndcapLayerOuterR        = 0.  
          SCT_LayerBuilder2.EndcapLayerBinsR         = TrkDetFlags.SCT_EndcapLayerMaterialBinsR()
          SCT_LayerBuilder2.EndcapLayerBinsPhi       = TrkDetFlags.SCT_EndcapLayerMaterialBinsPhi()
          # set the layer association
          SCT_LayerBuilder2.SetLayerAssociation = setLayerAssociation        
          # output level
          SCT_LayerBuilder2.OutputLevel         = TrkDetFlags.SCT_BuildingOutputLevel()
          ToolSvc += SCT_LayerBuilder2
          SLHC_LayerBuilders    += [ SCT_LayerBuilder2 ] 
          SLHC_LayerBinningType += [ 2 ]
          SLHC_ColorCodes       += [ 5 ]

        # build the geometry builder
        InDet__RobustTrackingGeometryBuilder.__init__(self,namePrefix+name,
                                                      BeamPipeBuilder = BeamPipeBuilder,
                                                      LayerBuilders = SLHC_LayerBuilders,
                                                      LayerBinningType = SLHC_LayerBinningType,
                                                      ColorCodes = SLHC_ColorCodes,
                                                      EndcapEntryLayers = [ 1, 1],
                                                      BarrelEntryLayer = [2,2],
                                                      VolumeEnclosure = False,
                                                      OuterEnevelope = 50.,
                                                      OutputLevel = 1,
                                                      isSLHC = True)
