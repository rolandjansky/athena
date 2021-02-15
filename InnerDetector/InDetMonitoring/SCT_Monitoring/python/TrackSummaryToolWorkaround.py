#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

'''@file TrackingConfigurationWorkaround.py
@author Peter Onyisi
@date 2019-12-17
@brief Centralize the DQ workarounds for setting up ID tracking tools in new-style config, until this is provided centrally
'''
from InDetConfig import InDetRecToolConfig

def TrackSummaryToolWorkaround(flags):
    from AthenaConfiguration.ComponentFactory import CompFactory
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()
    ############################## WORKAROUND (START) ##########################
    ############################## TO RUN TRACKSUMMARYTOOL #####################

    # Taken from InnerDetector/InDetDigitization/PixelDigitization/python/PixelDigitizationConfigNew.py

    InDetPixelConditionsSummaryTool = result.popToolsAndMerge(InDetRecToolConfig.PixelConditionsSummaryToolCfg(flags))

    InDetTestPixelLayerTool = result.popToolsAndMerge(InDetRecToolConfig.InDetTestPixelLayerToolCfg(flags,
                                                             PixelSummaryTool = InDetPixelConditionsSummaryTool,
                                                             CheckActiveAreas=True,
                                                             CheckDeadRegions=True,
                                                             CheckDisabledFEs=True))
    result.addPublicTool(InDetTestPixelLayerTool)

    InDetBoundaryCheckTool_acc = InDetRecToolConfig.InDetBoundaryCheckToolCfg(flags, PixelLayerTool = InDetTestPixelLayerTool)
    InDetHoleSearchTool_acc = InDetRecToolConfig.InDetTrackHoleSearchToolCfg(flags,
                                                                             BoundaryCheckTool= result.popToolsAndMerge(InDetBoundaryCheckTool_acc) )
    InDetHoleSearchTool = InDetHoleSearchTool_acc.getPrimary()
    result.merge(InDetHoleSearchTool_acc)

    InDetTrackSummaryHelperTool_acc = InDetRecToolConfig.InDetTrackSummaryHelperToolCfg(flags,HoleSearch=InDetHoleSearchTool)
    InDetTrackSummaryHelperTool=InDetTrackSummaryHelperTool_acc.getPrimary()
    result.merge(InDetTrackSummaryHelperTool_acc)

    # @TODO should there be a version InDetRecToolConfig ?
    InDetTrackSummaryTool = CompFactory.Trk.TrackSummaryTool(name = "InDetTrackSummaryTool",
                                                  InDetSummaryHelperTool = InDetTrackSummaryHelperTool,
                                                  doSharedHits           = False,
                                                  doHolesInDet           = True,
                                                  TRT_ElectronPidTool    = '',
                                                  PixelToTPIDTool        = '')
    result.setPrivateTools(InDetTrackSummaryTool)
    ############################## WORKAROUND (END) ############################

    # To run job only with ID
    if hasattr(flags, "Detector") and hasattr(flags.Detector, "GeometryMuon") and hasattr(flags.Detector, "GeometryID"):
        TrkEventCnvSuperTool = CompFactory.Trk.EventCnvSuperTool(name = "EventCnvSuperTool",
                                                                 DoMuons = flags.Detector.GeometryMuon,
                                                                 DoID = flags.Detector.GeometryID)
        result.addPublicTool(TrkEventCnvSuperTool)

    return result
