# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
#
# the configuration of tools of wider usage
#
"""

___author___ = "Jiri Masik"
___version___ = "$Id: TrigInDetRecCommonTools.py 590759 2014-04-02 03:25:17Z masik $"

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
