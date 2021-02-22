#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

'''@file TrackingConfigurationWorkaround.py
@author Peter Onyisi
@date 2019-12-17
@brief Centralize the DQ workarounds for setting up ID tracking tools in new-style config, until this is provided centrally
'''
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from InDetConfig import TrackingCommonConfig

def TrackSummaryToolWorkaround(flags):
    result = ComponentAccumulator()
    result.setPrivateTools( result.popToolsAndMerge(TrackingCommonConfig.InDetTrackSummaryToolCfg(flags) ) )

    # To run job only with ID
    if hasattr(flags, "Detector") and hasattr(flags.Detector, "GeometryMuon") and hasattr(flags.Detector, "GeometryID"):
        TrkEventCnvSuperTool = CompFactory.Trk.EventCnvSuperTool(name = "EventCnvSuperTool",
                                                                 DoMuons = flags.Detector.GeometryMuon,
                                                                 DoID = flags.Detector.GeometryID)
        result.addPublicTool(TrkEventCnvSuperTool)

    return result
