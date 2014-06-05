# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

######################################################
# ConfiguredTileVolumeBuilder module
#
# it inherits from TileVolumeBuilder and performs 
# standard configuration
#
######################################################



# import the Extrapolator configurable
from TileTrackingGeometry.TileTrackingGeometryConf import Tile__TileVolumeBuilder

# define the class
class ConfiguredTileVolumeBuilder( Tile__TileVolumeBuilder ):
    # constructor
    def __init__(self,name = 'TileVolumeBuilder'):

        from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags

        # import the ToolSvc if necessary
        from AthenaCommon.AppMgr import ToolSvc
        if 'ToolSvc' not in dir():
           ToolSvc = ToolSvc()
       
        meotps = []
        #if TrkDetFlags.TileUseMaterialEffectsOnTrackProvider() :
          
          #from CaloMaterialEffectsOnTrackProvider.ConfCaloMaterialEffectsOnTrackProvider import ConfCaloMaterialEffectsOnTrackProvider as ConfMEOTP

          #MEOTP = ConfMEOTP('CaloMaterialEffectsOnTrackProvider')
          #ToolSvc += MEOTP
          #meotps += [ MEOTP ]
          
        # The volume helper for the Tile
        from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__TrackingVolumeHelper
        TileTrackingVolumeHelper = Trk__TrackingVolumeHelper(name='TrackingVolumeHelper')
        ToolSvc += TileTrackingVolumeHelper 
              
        Tile__TileVolumeBuilder.__init__(self,name,
                                         MaterialEffectsOnTrackProviders = meotps,
                                         UseCaloSurfBuilder = TrkDetFlags.TileUseCaloSurfBuilder(),
                                         TrackingVolumeHelper = TileTrackingVolumeHelper,                                         
                                         BarrelEnvelopeCover  = TrkDetFlags.TileBarrelEnvelopeCover(),
                                         BarrelMaterialBinsRz = TrkDetFlags.TileBarrelLayerMaterialBinsRz(),      
                                         BarrelMaterialBinsPhi = TrkDetFlags.TileBarrelLayerMaterialBinsPhi(),       
                                         GirderMaterialBinsRz = TrkDetFlags.TileGirderLayerMaterialBinsRz(),       
                                         GirderMaterialBinsPhi = TrkDetFlags.TileGirderLayerMaterialBinsPhi(),       
                                         FingerMaterialBinsRz = TrkDetFlags.TileFingerLayerMaterialBinsRz(),       
                                         FingerMaterialBinsPhi = TrkDetFlags.TileFingerLayerMaterialBinsPhi(),       
                                         ExtendedBarrelMaterialBinsRz = TrkDetFlags.TileExtendedBarrelLayerMaterialBinsRz(), 
                                         ExtendedBarrelMaterialBinsPhi = TrkDetFlags.TileExtendedBarrelLayerMaterialBinsPhi(), 
                                         GapMaterialBinsRz = TrkDetFlags.TileGapLayerMaterialBinsRz(),         
                                         GapMaterialBinsPhi = TrkDetFlags.TileGapLayerMaterialBinsPhi(),                                                   
                                         OutputLevel = TrkDetFlags.TileBuildingOutputLevel(),
                                         MagneticFieldMode = TrkDetFlags.MagneticFieldMode())
