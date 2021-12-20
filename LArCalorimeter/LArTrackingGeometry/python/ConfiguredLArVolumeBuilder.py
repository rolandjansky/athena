# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

######################################################
# ConfiguredLArVolumeBuilder module
#
# it inherits from LArVolumeBuilder and performs 
# standard configuration
#
######################################################

# import the Extrapolator configurable
from LArTrackingGeometry.LArTrackingGeometryConf import LAr__LArVolumeBuilder

# define the class
class ConfiguredLArVolumeBuilder( LAr__LArVolumeBuilder ):
    # constructor
    def __init__(self,name = 'LArVolumeBuilder', nameSuffix=''):
        
        from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags

        # import the ToolSvc if necessary
        from AthenaCommon.AppMgr import ToolSvc
        if 'ToolSvc' not in dir():
          ToolSvc = ToolSvc()
        
        # The volume helper
        from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__TrackingVolumeHelper
        LArTrackingVolumeHelper = Trk__TrackingVolumeHelper(name='TrackingVolumeHelper')
        ToolSvc += LArTrackingVolumeHelper 
        
        LAr__LArVolumeBuilder.__init__(self,
                                       name+nameSuffix,
                                       UseCaloSurfBuilder = TrkDetFlags.LArUseCaloSurfBuilder(),
                                       TrackingVolumeHelper = LArTrackingVolumeHelper,
                                       BarrelEnvelopeCover  = TrkDetFlags.LArBarrelEnvelopeCover(),
                                       EndcapEnvelopeCover  = TrkDetFlags.LArEndcapEnvelopeCover(),
                                       OutputLevel = TrkDetFlags.LArBuildingOutputLevel(),
                                       MagneticFieldMode = TrkDetFlags.MagneticFieldMode())
