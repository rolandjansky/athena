# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
#
# the configuration of tools for postprocessing
#
"""

___author___ = "Jiri Masik"
___version___ = "$Id: TrigInDetPostTools.py 724055 2016-02-15 17:43:24Z masik $"

from AthenaCommon.AppMgr import ToolSvc
from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags


from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackSummaryHelperTool

# from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
# InDetTrigFastTrackSummaryTool = Trk__TrackSummaryTool(name = "InDetTrigFastTrackSummaryTool",
#                                                       InDetSummaryHelperTool = InDetTrigTrackSummaryHelperTool,
#                                                       InDetHoleSearchTool    = None,
#                                                       doSharedHits           = False,
#                                                       TRT_ElectronPidTool    = None
#                                                       )
# ToolSvc += InDetTrigFastTrackSummaryTool
# if (InDetTrigFlags.doPrintConfigurables()):
#     print      InDetTrigFastTrackSummaryTool


# from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigHoleSearchTool
# InDetTrigTrackSummaryToolWithHoleSearch = Trk__TrackSummaryTool(name = "InDetTrigTrackSummaryToolWithHoleSearch",
#                                                                 InDetSummaryHelperTool = InDetTrigTrackSummaryHelperTool,
#                                                                 InDetHoleSearchTool    = InDetTrigHoleSearchTool,
#                                                                 doSharedHits           = False,
#                                                                 TRT_ElectronPidTool    = None
#                                                       )
# ToolSvc += InDetTrigTrackSummaryToolWithHoleSearch
# if (InDetTrigFlags.doPrintConfigurables()):
#     print      InDetTrigTrackSummaryToolWithHoleSearch


from InDetTrigRecExample.InDetTrigConfigRecLoadTools import \
    InDetTrigTrackSummaryHelperTool, InDetTrigTrackSummaryTool, InDetTrigTrackSummaryToolSharedHits, \
    InDetTrigHoleSearchTool,InDetTrigExtrapolator

from TrigInDetConf.TrigInDetRecCommonTools import InDetTrigFastTrackSummaryTool, InDetTrigTrackSummaryToolSharedHitsWithTRTPid


from TrkParticleCreator.TrkParticleCreatorConf import Trk__TrackParticleCreatorTool
InDetTrigParticleCreatorTool = \
    Trk__TrackParticleCreatorTool( name = "InDetTrigParticleCreatorTool",
                                   Extrapolator = InDetTrigExtrapolator,
                                   TrackSummaryTool = InDetTrigTrackSummaryToolSharedHits,
                                   KeepParameters = False,
                                   ForceTrackSummaryUpdate = False,
                                   )

ToolSvc += InDetTrigParticleCreatorTool
if (InDetTrigFlags.doPrintConfigurables()):
    print InDetTrigParticleCreatorTool

InDetTrigParticleCreatorToolWithSummary = \
    Trk__TrackParticleCreatorTool( name = "InDetTrigParticleCreatorToolWithSummary",
                                   Extrapolator = InDetTrigExtrapolator,
                                   TrackSummaryTool = InDetTrigTrackSummaryToolSharedHits,
                                   KeepParameters = True,
                                   ForceTrackSummaryUpdate = True,
                                   )

ToolSvc += InDetTrigParticleCreatorToolWithSummary
if (InDetTrigFlags.doPrintConfigurables()):
    print InDetTrigParticleCreatorToolWithSummary

InDetTrigParticleCreatorToolWithSummaryTRTPid = \
    Trk__TrackParticleCreatorTool( name = "InDetTrigParticleCreatorToolWithSummaryTRTPid",
                                   Extrapolator = InDetTrigExtrapolator,
                                   TrackSummaryTool = InDetTrigTrackSummaryToolSharedHitsWithTRTPid,
                                   KeepParameters = True,
                                   ForceTrackSummaryUpdate = True,
                                   )

ToolSvc += InDetTrigParticleCreatorToolWithSummaryTRTPid
if (InDetTrigFlags.doPrintConfigurables()):
    print InDetTrigParticleCreatorToolWithSummaryTRTPid


InDetTrigParticleCreatorToolParams = \
    Trk__TrackParticleCreatorTool( name = "InDetTrigParticleCreatorToolParams",
                                   Extrapolator = InDetTrigExtrapolator,
                                   TrackSummaryTool = InDetTrigTrackSummaryToolSharedHits,
                                   KeepParameters = True,
                                   ForceTrackSummaryUpdate = False,
                                   )

ToolSvc += InDetTrigParticleCreatorToolParams
if (InDetTrigFlags.doPrintConfigurables()):
    print InDetTrigParticleCreatorToolParams

InDetTrigParticleCreatorToolFTF = \
    Trk__TrackParticleCreatorTool( name = "InDetTrigParticleCreatorToolFTF",
                                   Extrapolator = InDetTrigExtrapolator,
                                   TrackSummaryTool = InDetTrigFastTrackSummaryTool,
                                   KeepParameters = True,
                                   ForceTrackSummaryUpdate = False,
                                   )

ToolSvc += InDetTrigParticleCreatorToolFTF
if (InDetTrigFlags.doPrintConfigurables()):
    print InDetTrigParticleCreatorToolFTF

