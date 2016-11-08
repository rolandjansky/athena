# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
#
# the configuration of tools for processing of FTK tracks
#
"""

___author___ = "Jiri Masik"
___version___ = "$Id: TrigInDetRecToolsFTK.py 783022 2016-11-08 15:40:49Z masik $"


from AthenaCommon.AppMgr import ToolSvc
from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags

from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import FTKTrackingCuts

from InDetAmbiTrackSelectionTool.InDetAmbiTrackSelectionToolConf import InDet__InDetAmbiTrackSelectionTool
InDetTrigAmbiTrackSelectionToolFTK = \
    InDet__InDetAmbiTrackSelectionTool(name               = 'InDetTrigAmbiTrackSelectionToolFTK',
                                       #AssociationTool    = None,
                                       #DriftCircleCutTool = None,
                                       minHits         = FTKTrackingCuts.minClusters(),
                                       minNotShared    = FTKTrackingCuts.minSiNotShared(),
                                       maxShared       = FTKTrackingCuts.maxShared(),
                                       minTRTHits      = 0,  # used for Si only tracking !!!
                                       Cosmics         = False,  #there is a different instance
                                       UseParameterization = False,
                                       )
    
   
ToolSvc += InDetTrigAmbiTrackSelectionToolFTK

if (InDetTrigFlags.doPrintConfigurables()):
  print InDetTrigAmbiTrackSelectionToolFTK

