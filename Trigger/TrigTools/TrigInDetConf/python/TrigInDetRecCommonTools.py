# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
#
# the configuration of tools of wider usage
#
"""

___author___ = "Jiri Masik"
___version___ = "$Id: TrigInDetRecCommonTools.py 713541 2015-12-09 10:38:32Z masik $"

from AthenaCommon.AppMgr import ToolSvc
from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags


from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackSummaryHelperTool

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


