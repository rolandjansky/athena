# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

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
from InDetTrigRecExample.InDetTrigConditionsAccess import TRT_ConditionsSetup

InDetTrigTrackSummaryHelperToolCosmics = \
    InDet__InDetTrackSummaryHelperTool(name = "InDetTrigSummaryHelperCosmics",
                                       HoleSearch   = InDetTrigHoleSearchTool,
                                       TRTStrawSummarySvc=TRT_ConditionsSetup.instanceName('InDetTRTStrawStatusSummarySvc'),
                                       DoSharedHits = False)
ToolSvc += InDetTrigTrackSummaryHelperToolCosmics

if (InDetTrigFlags.doPrintConfigurables()):
  print      InDetTrigTrackSummaryHelperToolCosmics                                                    

from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
InDetTrigTrackSummaryToolCosmics = \
    Trk__TrackSummaryTool(name = "InDetTrigTrackSummaryToolCosmics",
                          InDetSummaryHelperTool = InDetTrigTrackSummaryHelperToolCosmics,
                          InDetHoleSearchTool    = InDetTrigHoleSearchTool)
ToolSvc += InDetTrigTrackSummaryToolCosmics
if (InDetTrigFlags.doPrintConfigurables()):
  print      InDetTrigTrackSummaryToolCosmics

from InDetTrackScoringTools.InDetTrackScoringToolsConf import InDet__InDetCosmicScoringTool
InDetTrigScoringToolCosmics_SiPattern = \
    InDet__InDetCosmicScoringTool(name = 'InDetTrigCosmicScoringTool_SiPattern',
                                  SummaryTool  = InDetTrigTrackSummaryToolCosmics)

InDetTrigScoringToolCosmics_SiPattern.nWeightedClustersMin = EFIDTrackingCutsCosmics.nWeightedClustersMin()
InDetTrigScoringToolCosmics_SiPattern.minTRTHits = 0
ToolSvc += InDetTrigScoringToolCosmics_SiPattern

if (InDetTrigFlags.doPrintConfigurables()):
  print      InDetTrigScoringToolCosmics_SiPattern


from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigPrdAssociationTool
from InDetAmbiTrackSelectionTool.InDetAmbiTrackSelectionToolConf import InDet__InDetAmbiTrackSelectionTool
from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTRTDriftCircleCut

InDetTrigAmbiTrackSelectionToolCosmicsN = InDet__InDetAmbiTrackSelectionTool \
  (name = 'InDetTrigAmbiTrackSelectionToolCosmicsN',
   AssociationTool = InDetTrigPrdAssociationTool,
   minHits         = 0,
   minNotShared    = 3,
   maxShared       = 0,
   minTRTHits      = 0, # used for Si only tracking !!!
   Cosmics=True,
   maxTracksPerSharedPRD = 10,
   UseParameterization = False,
   DriftCircleCutTool = InDetTrigTRTDriftCircleCut)

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
ToolSvc += InDetTrigSiDetElementsRoadMakerCosmics

#SP formation
from SiSpacePointTool.SiSpacePointToolConf import InDet__SiSpacePointMakerTool
InDetTrigSiSpacePointMakerToolCosmics = InDet__SiSpacePointMakerTool(name = 'InDetTrigSiSpacePointMakerToolCosmics',
                                                                     StripLengthTolerance = 0.05,
                                                                     UsePerpendicularProjection = True)
if (InDetTrigFlags.doPrintConfigurables()):
  print InDetTrigSiSpacePointMakerToolCosmics
ToolSvc += InDetTrigSiSpacePointMakerToolCosmics

from SiTrigSpacePointFormation.SiTrigSpacePointFormationConf import InDet__SCT_TrigSpacePointTool
SCT_TrigSpacePointToolCosmics = InDet__SCT_TrigSpacePointTool(name='InDetTrigSCTSpacePointToolCosmics',
                                                              ProcessOverlaps = InDetTrigFlags.doOverlapSP(),
                                                              SiSpacePointMakerToolName="InDet::SiSpacePointMakerTool/InDetTrigSiSpacePointMakerToolCosmics",
                                                              OverlapLimitOpposite=5.,
                                                              OverrideBeamSpot=True,
                                                              VertexY=99999999,
                                                              VertexX=0,
                                                              VertexZ=0,
                                                              )

if (InDetTrigFlags.doPrintConfigurables()):
  print SCT_TrigSpacePointToolCosmics
ToolSvc +=  SCT_TrigSpacePointToolCosmics
