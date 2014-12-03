# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

######################################################
# ConfiguredCaloTrackingGeometry module
#
# it inherits from CaloTrackingGeometryBuilder and performs 
# standard configuration
#
######################################################

# import the include statement
from AthenaCommon.Include import Include, IncludeError, include

# import the Extrapolator configurable
from CaloTrackingGeometry.CaloTrackingGeometryConf import Calo__CaloTrackingGeometryBuilder

# define the class
class ConfiguredCaloTrackingGeometryBuilder( Calo__CaloTrackingGeometryBuilder ):
    # constructor
    def __init__(self,name = 'CaloTrackingGeometryBuilder'):
        
        from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
                
        from LArTrackingGeometry.ConfiguredLArVolumeBuilder import ConfiguredLArVolumeBuilder

        # import the ToolSvc
        from AthenaCommon.AppMgr import ToolSvc
        if 'ToolSvc' not in dir() :
            ToolSvc = ToolSvc()

        ConfLArVolumeBuilder = ConfiguredLArVolumeBuilder()
        ToolSvc += ConfLArVolumeBuilder

        from TileTrackingGeometry.ConfiguredTileVolumeBuilder import ConfiguredTileVolumeBuilder

        ConfTileVolumeBuilder = ConfiguredTileVolumeBuilder()
        ToolSvc += ConfTileVolumeBuilder

        # The volume helper
        from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__TrackingVolumeHelper
        CaloTrackingVolumeHelper = Trk__TrackingVolumeHelper(name='TrackingVolumeHelper')
        ToolSvc += CaloTrackingVolumeHelper         
        
        Calo__CaloTrackingGeometryBuilder.__init__(self,name,\
                                                   LArVolumeBuilder = ConfLArVolumeBuilder,\
                                                   TileVolumeBuilder = ConfTileVolumeBuilder,\
                                                   TrackingVolumeHelper = CaloTrackingVolumeHelper,\
                                                   GapLayerEnvelope    = TrkDetFlags.CaloEnvelopeCover(),\
                                                   EntryVolumeName = TrkDetFlags.CaloEntryVolumeName(),\
                                                   ExitVolumeName  = TrkDetFlags.CaloContainerName(),
                                                   MagneticFieldMode = TrkDetFlags.MagneticFieldMode())
