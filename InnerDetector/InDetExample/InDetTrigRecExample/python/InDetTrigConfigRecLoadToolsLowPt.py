# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

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
                                               UseAssociationTool     = True,
                                               AssociationTool        = InDetTrigPrdAssociationTool,
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
ToolSvc += InDetTrigSiDetElementsRoadMakerLowPt


# Local combinatorial track finding using space point seed and detector element road
#
from InDetTrigRecExample.InDetTrigConfigRecLoadTools \
     import InDetTrigPixelConditionsSummarySvc, InDetTrigSCTConditionsSummarySvc
from SiCombinatorialTrackFinderTool_xk.SiCombinatorialTrackFinderTool_xkConf import InDet__SiCombinatorialTrackFinder_xk

InDetTrigSiComTrackFinderLowPt = \
    InDet__SiCombinatorialTrackFinder_xk(name = 'InDetTrigSiComTrackFinderLowPt',
                                         PropagatorTool	= InDetTrigPatternPropagator,
                                         UpdatorTool	= InDetTrigPatternUpdator,
                                         RIOonTrackTool   = InDetTrigRotCreator,
                                         AssosiationTool  = InDetTrigPrdAssociationTool,
                                         usePixel         = DetFlags.haveRIO.pixel_on(),
                                         useSCT           = DetFlags.haveRIO.SCT_on(),   
                                         PixelClusterContainer = 'PixelTrigClusters',
                                         SCT_ClusterContainer = 'SCT_TrigClusters',
                                         PixelSummarySvc = InDetTrigPixelConditionsSummarySvc,
                                         SctSummarySvc = InDetTrigSCTConditionsSummarySvc
                                         )															
ToolSvc += InDetTrigSiComTrackFinderLowPt

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

  from InDetAmbiTrackSelectionTool.InDetAmbiTrackSelectionToolConf import InDet__InDetAmbiTrackSelectionTool
  InDetTrigAmbiTrackSelectionToolLowPt = \
      InDet__InDetAmbiTrackSelectionTool(name               = 'InDetTrigAmbiTrackSelectionToolLowPt',
                                         AssociationTool    = InDetTrigPrdAssociationTool,
                                         DriftCircleCutTool = InDetTrigTRTDriftCircleCut,
                                         minHits         = EFIDTrackingCutsLowPt.minClusters()-2,
                                         minNotShared    = EFIDTrackingCutsLowPt.minSiNotShared(),
                                         maxShared       = EFIDTrackingCutsLowPt.maxShared(),
                                         minTRTHits      = 0,  # used for Si only tracking !!!
                                         Cosmics         = False,  #there is a different instance
                                         UseParameterization = False
                                         )
   
   
  ToolSvc += InDetTrigAmbiTrackSelectionToolLowPt
  if (InDetTrigFlags.doPrintConfigurables()):
    print InDetTrigAmbiTrackSelectionToolLowPt


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

  from TrkAmbiguityProcessor.TrkAmbiguityProcessorConf import Trk__SimpleAmbiguityProcessorTool
  InDetTrigAmbiguityProcessorLowPt = \
                                   Trk__SimpleAmbiguityProcessorTool(name = 'InDetTrigAmbiguityProcessorLowPt',
                                                                     #AssoTool    = InDetTrigPrdAssociationTool,
                                                                     Fitter      = InDetTrigTrackFitterLowPt,
                                                                     ScoringTool = InDetTrigScoringToolLowPt,
                                                                     SelectionTool = InDetTrigAmbiTrackSelectionToolLowPt,
                                                                     SuppressHoleSearch = False,
                                                                     RefitPrds   = not InDetTrigFlags.refitROT(),
                                            )
  ToolSvc += InDetTrigAmbiguityProcessorLowPt
  if (InDetTrigFlags.doPrintConfigurables()):
    print InDetTrigAmbiguityProcessorLowPt
  

#indetambiscoringtool lowpt
#ambitrackselectiontool
#ambiguityprocessor
#SiDetElementsRoadMaker_xk/InDetSiRoadMakerLowPt
#InDet::SiTrackMaker_xk/InDetSiTrackMakerLowPt
#Trk::SimpleAmbiguityProcessorTool/InDetAmbiguityProcessorLowPt
#InDet::InDetAmbiScoringTool/InDetAmbiScoringToolLowPt
#InDet::SiSpacePointsSeedMaker_LowMomentum/InDetSpSeedsMakerLowPt
#Trk::GlobalChi2Fitter/InDetTrackFitterLowPt done

