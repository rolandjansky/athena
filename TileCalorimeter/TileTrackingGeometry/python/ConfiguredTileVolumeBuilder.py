# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

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
    def __init__(self,name = 'TileVolumeBuilder',nameSuffix=''):

        from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags

        # import the ToolSvc if necessary
        from AthenaCommon.AppMgr import ToolSvc
        if 'ToolSvc' not in dir():
           ToolSvc = ToolSvc()
       
        # The volume helper for the Tile
        from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__TrackingVolumeHelper
        TileTrackingVolumeHelper = Trk__TrackingVolumeHelper(name='TrackingVolumeHelper')
        ToolSvc += TileTrackingVolumeHelper 
        
        Tile__TileVolumeBuilder.__init__(self,name+nameSuffix,
                                         UseCaloSurfBuilder = TrkDetFlags.TileUseCaloSurfBuilder(),
                                         TrackingVolumeHelper = TileTrackingVolumeHelper,                                         
                                         BarrelEnvelopeCover  = TrkDetFlags.TileBarrelEnvelopeCover(),
                                         OutputLevel = TrkDetFlags.TileBuildingOutputLevel(),
                                         MagneticFieldMode = TrkDetFlags.MagneticFieldMode())
