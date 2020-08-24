# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

######################################################
# ConfiguredCaloTrackingGeometry module
#
# it inherits from CaloTrackingGeometryBuilder and performs 
# standard configuration
#
######################################################

# import the Extrapolator configurable
from CaloTrackingGeometry.CaloTrackingGeometryConf import Calo__CaloTrackingGeometryBuilderCond

# define the class
class ConfiguredCaloTrackingGeometryBuilderCond( Calo__CaloTrackingGeometryBuilderCond ):
    # constructor
    def __init__(self,name = 'CaloTrackingGeometryBuilderCond'):
        
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
        
        Calo__CaloTrackingGeometryBuilderCond.__init__(self,name,\
                                                   LArVolumeBuilder = ConfLArVolumeBuilder,\
                                                   TileVolumeBuilder = ConfTileVolumeBuilder,\
                                                   TrackingVolumeHelper = CaloTrackingVolumeHelper,\
                                                   GapLayerEnvelope    = TrkDetFlags.CaloEnvelopeCover(),\
                                                   EntryVolumeName = TrkDetFlags.CaloEntryVolumeName(),\
                                                   ExitVolumeName  = TrkDetFlags.CaloContainerName(),
                                                   MagneticFieldMode = TrkDetFlags.MagneticFieldMode())
