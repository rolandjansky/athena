# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# flake8: noqa  (legacy trigger)

""" Instantiates tools for LowPt tracking
"""

__author__ = "J. Masik"
__version__= "$Revision: 1.1 $"
__doc__    = "InDetTrigConfigRecLoadToolsLowPt"


from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.DetFlags import DetFlags
from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags

from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import EFIDTrackingCutsLowPt

from SiSpacePointsSeedTool_xk.SiSpacePointsSeedTool_xkConf import InDet__SiSpacePointsSeedMaker_LowMomentum 
from InDetTrigRecExample.InDetTrigConfigRecLoadTools import \
    InDetTrigPrdAssociationTool,InDetTrigPatternPropagator,\
    InDetTrigPatternUpdator,InDetTrigRotCreator,InDetTrigTRTDriftCircleCut,InDetTrigExtrapolator,\
    InDetTrigTrackSummaryTool,InDetTrigTrackFitterLowPt

InDetTrigSiSpacePointsSeedMakerLowPt =  \
    InDet__SiSpacePointsSeedMaker_LowMomentum( name = 'InDetTrigSiSpacePointsSeedMakerLowPt',
                                               pTmin = EFIDTrackingCutsLowPt.minPT(),
                                               pTmax = EFIDTrackingCutsLowPt.maxPT(),
                                               maxdImpact = EFIDTrackingCutsLowPt.maxPrimaryImpact(),
                                               maxZ = EFIDTrackingCutsLowPt.maxZImpact(),
                                               minZ = -EFIDTrackingCutsLowPt.maxZImpact(),
                                               useOverlapSpCollection   = False, #overlapCollections not used in EFID
                                               #useOverlapSpCollection   = True, #testing
                                               usePixel = EFIDTrackingCutsLowPt.usePixel(),
                                               useSCT   = EFIDTrackingCutsLowPt.useSCT(),
                                               SpacePointsSCTName = 'SCT_TrigSpacePoints',
                                               SpacePointsPixelName = 'PixelTrigSpacePoints',
                                               SpacePointsOverlapName = 'SPTrigOverlap',
                                               PRDtoTrackMap = "", # @TODO
                                               radMax = EFIDTrackingCutsLowPt.radMax(),
                                               mindRadius = 4.0
                                               )
ToolSvc += InDetTrigSiSpacePointsSeedMakerLowPt
      
from SiZvertexTool_xk.SiZvertexTool_xkConf import InDet__SiZvertexMaker_xk
InDetTrigZvertexMakerLowPt = InDet__SiZvertexMaker_xk( name = 'InDetTrigZvertexMakerLowPt',
                                                  Zmax = EFIDTrackingCutsLowPt.maxZImpact(),
                                                  Zmin = -EFIDTrackingCutsLowPt.maxZImpact(),
                                                  minRatio = 0.17, # not default
                                                  SeedMakerTool = InDetTrigSiSpacePointsSeedMakerLowPt
                                                  )
ToolSvc += InDetTrigZvertexMakerLowPt


# SCT and Pixel detector elements road builder
#
from SiDetElementsRoadTool_xk.SiDetElementsRoadTool_xkConf import InDet__SiDetElementsRoadMaker_xk

InDetTrigSiDetElementsRoadMakerLowPt = \
    InDet__SiDetElementsRoadMaker_xk(name = 'InDetTrigSiDetElementsRoadMakerLowPt',
                                     PropagatorTool = InDetTrigPatternPropagator,
                                     usePixel     = DetFlags.haveRIO.pixel_on(), 
                                     useSCT       = DetFlags.haveRIO.SCT_on(),
                                     RoadWidth    = EFIDTrackingCutsLowPt.RoadWidth()
                                     )
# Condition algorithm for InDet__SiDetElementsRoadMaker_xk
if DetFlags.haveRIO.SCT_on():
  from AthenaCommon.AlgSequence import AthSequencer
  condSeq = AthSequencer("AthCondSeq")
  if not hasattr(condSeq, "InDet__SiDetElementsRoadCondAlg_xk"):
    from SiDetElementsRoadTool_xk.SiDetElementsRoadTool_xkConf import InDet__SiDetElementsRoadCondAlg_xk
    condSeq += InDet__SiDetElementsRoadCondAlg_xk(name = "InDet__SiDetElementsRoadCondAlg_xk")

# Local combinatorial track finding using space point seed and detector element road
#
from InDetTrigRecExample.InDetTrigConfigRecLoadTools \
     import InDetTrigPixelConditionsSummaryTool, InDetTrigSCTConditionsSummaryTool
from SiCombinatorialTrackFinderTool_xk.SiCombinatorialTrackFinderTool_xkConf import InDet__SiCombinatorialTrackFinder_xk

InDetTrigSiComTrackFinderLowPt = \
    InDet__SiCombinatorialTrackFinder_xk(name = 'InDetTrigSiComTrackFinderLowPt',
                                         PropagatorTool	= InDetTrigPatternPropagator,
                                         UpdatorTool	= InDetTrigPatternUpdator,
                                         RIOonTrackTool   = InDetTrigRotCreator,
                                         usePixel         = DetFlags.haveRIO.pixel_on(),
                                         useSCT           = DetFlags.haveRIO.SCT_on(),   
                                         PixelClusterContainer = 'PixelTrigClusters',
                                         SCT_ClusterContainer = 'SCT_TrigClusters',
                                         PixelSummaryTool = InDetTrigPixelConditionsSummaryTool,
                                         SctSummaryTool = InDetTrigSCTConditionsSummaryTool
                                         )															
if DetFlags.haveRIO.pixel_on():
  # Condition algorithm for SiCombinatorialTrackFinder_xk
  from AthenaCommon.AlgSequence import AthSequencer
  condSeq = AthSequencer("AthCondSeq")
  if not hasattr(condSeq, "InDetSiDetElementBoundaryLinksPixelCondAlg"):
    from SiCombinatorialTrackFinderTool_xk.SiCombinatorialTrackFinderTool_xkConf import InDet__SiDetElementBoundaryLinksCondAlg_xk
    condSeq += InDet__SiDetElementBoundaryLinksCondAlg_xk(name = "InDetSiDetElementBoundaryLinksPixelCondAlg",
                                                          ReadKey = "PixelDetectorElementCollection",
                                                          WriteKey = "PixelDetElementBoundaryLinks_xk")
if DetFlags.haveRIO.SCT_on():
  # Condition algorithm for SiCombinatorialTrackFinder_xk
  from AthenaCommon.AlgSequence import AthSequencer
  condSeq = AthSequencer("AthCondSeq")
  if not hasattr(condSeq, "InDetSiDetElementBoundaryLinksSCTCondAlg"):
    from SiCombinatorialTrackFinderTool_xk.SiCombinatorialTrackFinderTool_xkConf import InDet__SiDetElementBoundaryLinksCondAlg_xk
    condSeq += InDet__SiDetElementBoundaryLinksCondAlg_xk(name = "InDetSiDetElementBoundaryLinksSCTCondAlg",
                                                          ReadKey = "SCT_DetectorElementCollection",
                                                          WriteKey = "SCT_DetElementBoundaryLinks_xk")


from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigSiDetElementsRoadMaker
from InDetTrigRecExample.InDetTrigSliceSettings import InDetTrigSliceSettings
from SiTrackMakerTool_xk.SiTrackMakerTool_xkConf import InDet__SiTrackMaker_xk
InDetTrigSiTrackMakerLowPt = InDet__SiTrackMaker_xk( name = 'InDetTrigSiTrackMakerLowPt',
                                                RoadTool       = InDetTrigSiDetElementsRoadMakerLowPt,
                                                CombinatorialTrackFinder = InDetTrigSiComTrackFinderLowPt,
                                                pTmin          = InDetTrigSliceSettings[('pTmin','lowPt')],
                                                nClustersMin   = EFIDTrackingCutsLowPt.minClusters(),
                                                nHolesMax      = EFIDTrackingCutsLowPt.nHolesMax(),
                                                nHolesGapMax   = EFIDTrackingCutsLowPt.nHolesGapMax(),
                                                SeedsFilterLevel = EFIDTrackingCutsLowPt.seedFilterLevel(),
                                                Xi2max         = EFIDTrackingCutsLowPt.Xi2max(),
                                                Xi2maxNoAdd    = EFIDTrackingCutsLowPt.Xi2maxNoAdd(),
                                                nWeightedClustersMin= EFIDTrackingCutsLowPt.nWeightedClustersMin(),
                                                CosmicTrack              = False,
                                                UseAssociationTool       = True
                                                )


ToolSvc += InDetTrigSiTrackMakerLowPt


if InDetTrigFlags.doAmbiSolving():

  import InDetRecExample.TrackingCommon as TrackingCommon
  from InDetAmbiTrackSelectionTool.InDetAmbiTrackSelectionToolConf import InDet__InDetAmbiTrackSelectionTool
  InDetTrigAmbiTrackSelectionToolLowPt = \
      InDet__InDetAmbiTrackSelectionTool(name               = 'InDetTrigAmbiTrackSelectionToolLowPt',
                                         DriftCircleCutTool = InDetTrigTRTDriftCircleCut,
                                         AssociationTool    = TrackingCommon.getInDetTrigPRDtoTrackMapToolGangedPixels(),
                                         minHits         = EFIDTrackingCutsLowPt.minClusters()-2,
                                         minNotShared    = EFIDTrackingCutsLowPt.minSiNotShared(),
                                         maxShared       = EFIDTrackingCutsLowPt.maxShared(),
                                         minTRTHits      = 0,  # used for Si only tracking !!!
                                         Cosmics         = False,  #there is a different instance
                                         UseParameterization = False
                                         )
   
   
  ToolSvc += InDetTrigAmbiTrackSelectionToolLowPt
  if (InDetTrigFlags.doPrintConfigurables()):
    print (InDetTrigAmbiTrackSelectionToolLowPt)


  from InDetTrackScoringTools.InDetTrackScoringToolsConf import InDet__InDetAmbiScoringTool
  from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTRTDriftCircleCut
  InDetTrigScoringToolLowPt = InDet__InDetAmbiScoringTool(name         = 'InDetTrigScoringToolLowPt',
                                                          Extrapolator = InDetTrigExtrapolator,
                                                          SummaryTool  = InDetTrigTrackSummaryTool,
                                                          useAmbigFcn  = True,   # this is NewTracking
                                                          useTRT_AmbigFcn= False,
                                                          minPt        = InDetTrigSliceSettings[('pTmin','lowPt')],
                                                          maxRPhiImp   = EFIDTrackingCutsLowPt.maxPrimaryImpact(),
                                                          maxZImp      = EFIDTrackingCutsLowPt.maxZImpact(),
                                                          maxEta       = EFIDTrackingCutsLowPt.maxEta(),
                                                          minSiClusters= EFIDTrackingCutsLowPt.minClusters(),
                                                          maxSiHoles   = EFIDTrackingCutsLowPt.maxHoles(),
                                                          maxDoubleHoles= EFIDTrackingCutsLowPt.maxDoubleHoles(),
                                                          usePixel      = EFIDTrackingCutsLowPt.usePixel(),
                                                          useSCT        = EFIDTrackingCutsLowPt.useSCT(),
                                                          minTRTonTrk  = 0,    # no TRT here
                                                          doEmCaloSeed = False,
                                                          DriftCircleCutTool = InDetTrigTRTDriftCircleCut,
                                                         )
  ToolSvc += InDetTrigScoringToolLowPt

  import InDetRecExample.TrackingCommon as TrackingCommon
  from TrkAmbiguityProcessor.TrkAmbiguityProcessorConf import Trk__SimpleAmbiguityProcessorTool
  InDetTrigAmbiguityProcessorLowPt = \
                                   Trk__SimpleAmbiguityProcessorTool(name = 'InDetTrigAmbiguityProcessorLowPt',
                                                                     #AssoTool    = InDetTrigPrdAssociationTool,
                                                                     Fitter      = InDetTrigTrackFitterLowPt,
                                                                     AssociationTool = TrackingCommon.getInDetTrigPRDtoTrackMapToolGangedPixels(),
                                                                     TrackSummaryTool   = InDetTrigTrackSummaryTool,
                                                                     ScoringTool = InDetTrigScoringToolLowPt,
                                                                     SelectionTool = InDetTrigAmbiTrackSelectionToolLowPt,
                                                                     SuppressHoleSearch = False,
                                                                     RefitPrds   = not InDetTrigFlags.refitROT(),
                                            )
  ToolSvc += InDetTrigAmbiguityProcessorLowPt
  if (InDetTrigFlags.doPrintConfigurables()):
    print (InDetTrigAmbiguityProcessorLowPt)
  

#indetambiscoringtool lowpt
#ambitrackselectiontool
#ambiguityprocessor
#SiDetElementsRoadMaker_xk/InDetSiRoadMakerLowPt
#InDet::SiTrackMaker_xk/InDetSiTrackMakerLowPt
#Trk::SimpleAmbiguityProcessorTool/InDetAmbiguityProcessorLowPt
#InDet::InDetAmbiScoringTool/InDetAmbiScoringToolLowPt
#InDet::SiSpacePointsSeedMaker_LowMomentum/InDetSpSeedsMakerLowPt
#Trk::GlobalChi2Fitter/InDetTrackFitterLowPt done

