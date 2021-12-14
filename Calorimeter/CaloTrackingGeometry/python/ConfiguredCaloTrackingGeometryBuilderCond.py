# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

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
class ConfiguredCaloTrackingGeometryBuilderCond( Calo__CaloTrackingGeometryBuilderCond, ):
    # constructor
    def __init__(self,name = 'CaloTrackingGeometryBuilder', nameSuffix='Cond'):
        
        from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
                
        from LArTrackingGeometry.ConfiguredLArVolumeBuilder import ConfiguredLArVolumeBuilder

        # import the ToolSvc
        from AthenaCommon.AppMgr import ToolSvc
        if 'ToolSvc' not in dir() :
            ToolSvc = ToolSvc()

        the_name='LArVolumeBuilder'+nameSuffix
        if not hasattr(ToolSvc,the_name) :
           ConfLArVolumeBuilder = ConfiguredLArVolumeBuilder(name='LArVolumeBuilder', nameSuffix=nameSuffix)
           ToolSvc += ConfLArVolumeBuilder
        else :
           ConfLArVolumeBuilder=getattr(ToolSvc,the_name)

        from TileTrackingGeometry.ConfiguredTileVolumeBuilder import ConfiguredTileVolumeBuilder

        the_name='TileVolumeBuilder'+nameSuffix
        if not hasattr(ToolSvc,the_name) :
            ConfTileVolumeBuilder = ConfiguredTileVolumeBuilder(name='TileVolumeBuilder',nameSuffix=nameSuffix)
            ToolSvc += ConfTileVolumeBuilder
        else :
            ConfTileVolumeBuilder=getattr(ToolSvc,the_name)


        # The volume helper
        the_name='TrackingVolumeHelper'
        if not hasattr(ToolSvc,the_name) :
           from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__TrackingVolumeHelper
           CaloTrackingVolumeHelper = Trk__TrackingVolumeHelper(the_name)
           ToolSvc += CaloTrackingVolumeHelper
        else :
            CaloTrackingVolumeHelper=getattr(ToolSvc,the_name)
        
        Calo__CaloTrackingGeometryBuilderCond.__init__(self,name+nameSuffix,\
                                                   LArVolumeBuilder = ConfLArVolumeBuilder,\
                                                   TileVolumeBuilder = ConfTileVolumeBuilder,\
                                                   TrackingVolumeHelper = CaloTrackingVolumeHelper,\
                                                   GapLayerEnvelope    = TrkDetFlags.CaloEnvelopeCover(),\
                                                   EntryVolumeName = TrkDetFlags.CaloEntryVolumeName(),\
                                                   ExitVolumeName  = TrkDetFlags.CaloContainerName(),
                                                   MagneticFieldMode = TrkDetFlags.MagneticFieldMode())
