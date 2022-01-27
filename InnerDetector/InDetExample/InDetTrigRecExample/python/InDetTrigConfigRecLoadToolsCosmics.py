# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

""" InDetTrigConfigRecLoadToolsCosmics
    various tools for cosmicsN slice
"""

__author__ = "J. Masik"
__version__= "$Revision: 1.2 $"
__doc__    = "InDetTrigConfigRecLoadToolsCosmics"


from AthenaCommon.AppMgr import ToolSvc
from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
from AthenaCommon.Logging import logging 
log = logging.getLogger("InDetTrigConfigRecLoadToolsCosmics.py")

from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import EFIDTrackingCutsCosmics

from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigHoleSearchTool
from InDetTrackSummaryHelperTool.InDetTrackSummaryHelperToolConf import InDet__InDetTrackSummaryHelperTool
from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTRTStrawStatusSummaryTool

InDetTrigTrackSummaryHelperToolCosmics = \
    InDet__InDetTrackSummaryHelperTool(name = "InDetTrigSummaryHelperCosmics",
                                       HoleSearch   = InDetTrigHoleSearchTool,
                                       TRTStrawSummarySvc=InDetTrigTRTStrawStatusSummaryTool,
                                       DoSharedHits = False)
ToolSvc += InDetTrigTrackSummaryHelperToolCosmics

if (InDetTrigFlags.doPrintConfigurables()):
  print (     InDetTrigTrackSummaryHelperToolCosmics )

from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
InDetTrigTrackSummaryToolCosmics = \
    Trk__TrackSummaryTool(name = "InDetTrigTrackSummaryToolCosmics",
                          InDetSummaryHelperTool = InDetTrigTrackSummaryHelperToolCosmics,
                          doHolesInDet           = True)
ToolSvc += InDetTrigTrackSummaryToolCosmics
if (InDetTrigFlags.doPrintConfigurables()):
  print (     InDetTrigTrackSummaryToolCosmics)

from InDetTrackScoringTools.InDetTrackScoringToolsConf import InDet__InDetCosmicScoringTool
InDetTrigScoringToolCosmics_SiPattern = \
    InDet__InDetCosmicScoringTool(name = 'InDetTrigCosmicScoringTool_SiPattern',
                                  SummaryTool  = InDetTrigTrackSummaryToolCosmics)

InDetTrigScoringToolCosmics_SiPattern.nWeightedClustersMin = EFIDTrackingCutsCosmics.nWeightedClustersMin()
InDetTrigScoringToolCosmics_SiPattern.minTRTHits = 0
ToolSvc += InDetTrigScoringToolCosmics_SiPattern

if (InDetTrigFlags.doPrintConfigurables()):
  print (     InDetTrigScoringToolCosmics_SiPattern)


from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigPrdAssociationTool  # noqa: F401
from InDetAmbiTrackSelectionTool.InDetAmbiTrackSelectionToolConf import InDet__InDetAmbiTrackSelectionTool
from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTRTDriftCircleCut
import InDetRecExample.TrackingCommon as TrackingCommon

InDetTrigAmbiTrackSelectionToolCosmicsN = InDet__InDetAmbiTrackSelectionTool \
  (name = 'InDetTrigAmbiTrackSelectionToolCosmicsN',
   minHits         = 0,
   minNotShared    = 3,
   maxShared       = 0,
   minTRTHits      = 0, # used for Si only tracking !!!
   Cosmics=True,
   maxTracksPerSharedPRD = 10,
   UseParameterization = False,
   DriftCircleCutTool = InDetTrigTRTDriftCircleCut,
   AssociationTool    = TrackingCommon.getInDetTrigPRDtoTrackMapToolGangedPixels())

ToolSvc += InDetTrigAmbiTrackSelectionToolCosmicsN


from InDetTrigRecExample.InDetTrigConfigRecLoadTools import \
     InDetTrigPatternPropagator
from AthenaCommon.DetFlags import DetFlags

from SiDetElementsRoadTool_xk.SiDetElementsRoadTool_xkConf import InDet__SiDetElementsRoadMaker_xk
InDetTrigSiDetElementsRoadMakerCosmics = \
    InDet__SiDetElementsRoadMaker_xk( name = 'InDetTrigSiDetElementsRoadMakerCosmics',
                                      PropagatorTool = InDetTrigPatternPropagator,
                                      usePixel     = DetFlags.haveRIO.pixel_on(), 
                                      useSCT       = DetFlags.haveRIO.SCT_on(),
                                      RoadWidth          = 75.     #wider for cosmics
                                      )
# Condition algorithm for InDet__SiDetElementsRoadMaker_xk
if DetFlags.haveRIO.SCT_on():
  from AthenaCommon.AlgSequence import AthSequencer
  condSeq = AthSequencer("AthCondSeq")
  if not hasattr(condSeq, "InDet__SiDetElementsRoadCondAlg_xk"):
    from SiDetElementsRoadTool_xk.SiDetElementsRoadTool_xkConf import InDet__SiDetElementsRoadCondAlg_xk
    condSeq += InDet__SiDetElementsRoadCondAlg_xk(name = "InDet__SiDetElementsRoadCondAlg_xk")

#SP formation
from SiSpacePointTool.SiSpacePointToolConf import InDet__SiSpacePointMakerTool
InDetTrigSiSpacePointMakerToolCosmics = InDet__SiSpacePointMakerTool(name = 'InDetTrigSiSpacePointMakerToolCosmics',
                                                                     StripLengthTolerance = 0.05,
                                                                     UsePerpendicularProjection = True)
if (InDetTrigFlags.doPrintConfigurables()):
  print (InDetTrigSiSpacePointMakerToolCosmics)
ToolSvc += InDetTrigSiSpacePointMakerToolCosmics

