# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
#
# the configuration of tools of wider usage
#
"""

___author___ = "Jiri Masik"
___version___ = "$Id: TrigInDetRecCommonTools.py 724055 2016-02-15 17:43:24Z masik $"

from AthenaCommon.AppMgr import ToolSvc
from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags


from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackSummaryHelperTool,\
    InDetTrigTrackSummaryHelperToolSharedHits,InDetTrigTRT_ElectronPidTool


from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
InDetTrigFastTrackSummaryTool = Trk__TrackSummaryTool(name = "InDetTrigFastTrackSummaryTool",
                                                      InDetSummaryHelperTool = InDetTrigTrackSummaryHelperTool,
                                                      InDetHoleSearchTool    = None,
                                                      doSharedHits           = False,
                                                      TRT_ElectronPidTool    = None
                                                      )
ToolSvc += InDetTrigFastTrackSummaryTool
if (InDetTrigFlags.doPrintConfigurables()):
    print      InDetTrigFastTrackSummaryTool


from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigHoleSearchTool
InDetTrigTrackSummaryToolWithHoleSearch = Trk__TrackSummaryTool(name = "InDetTrigTrackSummaryToolWithHoleSearch",
                                                                InDetSummaryHelperTool = InDetTrigTrackSummaryHelperTool,
                                                                InDetHoleSearchTool    = InDetTrigHoleSearchTool,
                                                                doSharedHits           = False,
                                                                TRT_ElectronPidTool    = None
                                                      )
ToolSvc += InDetTrigTrackSummaryToolWithHoleSearch
if (InDetTrigFlags.doPrintConfigurables()):
    print      InDetTrigTrackSummaryToolWithHoleSearch


InDetTrigTrackSummaryToolSharedHitsWithTRTPid = \
    Trk__TrackSummaryTool(name = "InDetTrigTrackSummaryToolSharedHitsWithTRT",
                          InDetSummaryHelperTool = InDetTrigTrackSummaryHelperToolSharedHits,
                          doSharedHits           = InDetTrigFlags.doSharedHits(),
                          InDetHoleSearchTool    = InDetTrigHoleSearchTool,
                          TRT_ElectronPidTool    = InDetTrigTRT_ElectronPidTool)

ToolSvc += InDetTrigTrackSummaryToolSharedHitsWithTRTPid
if (InDetTrigFlags.doPrintConfigurables()):
    print      InDetTrigTrackSummaryToolSharedHitsWithTRTPid

