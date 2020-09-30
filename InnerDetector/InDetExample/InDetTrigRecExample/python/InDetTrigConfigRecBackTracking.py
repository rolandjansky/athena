# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# ----------- Draft version of TRT Segment finding
#
# common things
from __future__ import print_function

from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.Include import include

from AthenaCommon.Logging import logging 
log = logging.getLogger("InDetTrigConfigBackTracking.py")


from InDetTrigTrackPRD_Association.InDetTrigTrackPRD_AssociationConf import InDet__InDetTrigTrackPRD_Association
class InDetTrigTrackPRD_Association_EF( InDet__InDetTrigTrackPRD_Association ):
   __slots__ = []
   def __init__(self, name="InDetTrigTrackPRD_Association_Photon_EF", type="photon"):
      super( InDet__InDetTrigTrackPRD_Association, self ).__init__( name )
      
      import os
      
      if name.rfind('TRTStandalone') != -1:
         self.TracksName = []
      else:
         self.TracksName = ['AmbigSolv','ExtProcTracks','TRTSeededAmbigSolv']
      import InDetRecExample.TrackingCommon as TrackingCommon
      self.AssociationTool = TrackingCommon.getInDetTrigPRDtoTrackMapToolGangedPixels() # @TODO correct tool ?
      self.AssociationMapName  = "InDetTrigPRDtoTrackMap_Photon_EF"

from TRT_TrigTrackSegmentsFinder.TRT_TrigTrackSegmentsFinderConf import InDet__TRT_TrigTrackSegmentsFinder
class TRT_TrigTrackSegmentsFinder_EF( InDet__TRT_TrigTrackSegmentsFinder ):
   __slots__ = []
   def __init__(self, name="InDetTrigTRT_TrackSegmentsFinder_Photon_EF", type="photon", seqType="InsideOut"):
      super( InDet__TRT_TrigTrackSegmentsFinder, self ).__init__( name )

      from RegionSelector.RegSelToolConfig import makeRegSelTool_TRT
      self.RegSelTool = makeRegSelTool_TRT()

      from AthenaCommon.AppMgr import ToolSvc
      from InDetTrigRecExample.InDetTrigSliceSettings import InDetTrigSliceSettings
      from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
      from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigPrdAssociationTool, InDetTrigPatternPropagator, InDetTrigPatternUpdator, InDetTrigTRTDriftCircleCut
      from InDetTrigRecExample.InDetTrigConfigRecLoadToolsBack import InDetTrigTRTDetElementsRoadMaker
      from AthenaCommon.SystemOfUnits import GeV
      if seqType == "TRTOnly":
         from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import EFIDTrackingCutsTRT
         InDetTrigCutValues = EFIDTrackingCutsTRT
         suffixTRT = "_TRTOnly"
      else:
         from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import EFIDTrackingCuts
         InDetTrigCutValues = EFIDTrackingCuts
         suffixTRT = ""
         

      # Track extension to TRT tool
      # if new tracking is OFF then xk extension type (no DAF) has to be used!!

      from TRT_TrackExtensionTool_xk.TRT_TrackExtensionTool_xkConf import InDet__TRT_TrackExtensionTool_xk
      InDetTrigTRTExtensionTool =  InDet__TRT_TrackExtensionTool_xk(name           = 'InDetTrigTRTExtensionTool'+suffixTRT,
                                                                    MagneticFieldMode     = 'MapSolenoid',      # default
                                                                    TRT_ClustersContainer = 'TRT_TrigDriftCircles', # default
                                                                    TrtManagerLocation    = 'TRT',              # default
                                                                    PropagatorTool        = InDetTrigPatternPropagator,
                                                                    UpdatorTool           = InDetTrigPatternUpdator,
                                                                    DriftCircleCutTool    = InDetTrigTRTDriftCircleCut,
                                                                    UseDriftRadius        = not InDetTrigFlags.noTRTTiming(),
                                                                    RoadTool              = InDetTrigTRTDetElementsRoadMaker,
								    MinNumberDriftCircles = InDetTrigCutValues.minTRTonTrk(),
                                                                    ScaleHitUncertainty   = 2.,
                                                                    RoadWidth             = 20.,
                                                                    UseParameterization   = InDetTrigCutValues.useParameterizedTRTCuts())
      
      ToolSvc += InDetTrigTRTExtensionTool
      if (InDetTrigFlags.doPrintConfigurables()):
        print (     InDetTrigTRTExtensionTool)

      if seqType == "TRTOnly":
        # segment finding
        MinNumberDCs   = InDetTrigCutValues.minTRTonly()
      else:
        # back tracking has softer cuts
        MinNumberDCs   = InDetTrigCutValues.minSecondaryTRTonTrk()
        
      # TRT seed maker
      from TRT_TrackSegmentsTool_xk.TRT_TrackSegmentsTool_xkConf import InDet__TRT_TrackSegmentsMaker_ATLxk
      InDetTrigTRT_TrackSegmentsMaker = InDet__TRT_TrackSegmentsMaker_ATLxk(name                    = 'InDetTrigTRT_TrackSegmentsMaker_'+type+suffixTRT,
                                                                            TrtManagerLocation      = 'TRT' ,
                                                                            TRT_ClustersContainer   = 'TRT_TrigDriftCircles',
                                                                            MagneticFieldMode       = 'MapSolenoid',
                                                                            #OutputLevel            = 1,
                                                                            PropagatorTool          = InDetTrigPatternPropagator,
                                                                            PRDtoTrackMap           = "",
                                                                            TrackExtensionTool      = InDetTrigTRTExtensionTool,
                                                                            MinNumberDriftCircles   = MinNumberDCs,
									    RemoveNoiseDriftCircles = InDetTrigFlags.removeTRTNoise())
      
      InDetTrigTRT_TrackSegmentsMaker.pTmin = InDetTrigSliceSettings[('pTmin',type)]

      if type == 'photon':
         InDetTrigTRT_TrackSegmentsMaker.PRDtoTrackMap = 'InDetTrigPRDtoTrackMap_Photon_EF'
      elif type == 'cosmicsN':
         from TRT_TrackSegmentsTool_xk.TRT_TrackSegmentsTool_xkConf import InDet__TRT_TrackSegmentsMaker_BarrelCosmics

         #TODO COSMICS - replace old CTB TrackSegmentsMaker
         
         InDetTrigTRT_TrackSegmentsMaker_CTB = \
                                             InDet__TRT_TrackSegmentsMaker_BarrelCosmics(name= 'InDetTrigTRT_TrackSegmentsMaker_CTB',
                                                                               TrtManagerLocation      = 'TRT' ,
                                                                               TRT_ClustersContainer   = 'TRT_TrigDriftCircles',
                                                                               #MagneticField  = InDetTrigFlags.solenoidOn(),
                                                                               #TRTResolution  = 99.0,
                                                                               #DriftCircleCut = 4.0,
                                                                               #TRTHitsOnTrack = 20,
                                                                               #UseDriftTime   = False,
                                                                               )
         ToolSvc += InDetTrigTRT_TrackSegmentsMaker_CTB
         if (InDetTrigFlags.doPrintConfigurables()):
            print (InDetTrigTRT_TrackSegmentsMaker_CTB)


      ToolSvc += InDetTrigTRT_TrackSegmentsMaker
      if (InDetTrigFlags.doPrintConfigurables()):
        print (      InDetTrigTRT_TrackSegmentsMaker)
 	 
      # TRT track reconstruction
      self.SegmentsMakerTool = InDetTrigTRT_TrackSegmentsMaker
      # if type is 'cosmicsN':
      #    self.SegmentsMakerTool = InDetTrigTRT_TrackSegmentsMaker_CTB

      self.EtaHalfWidth = InDetTrigSliceSettings[('etaHalfWidth',type)]
      self.PhiHalfWidth = InDetTrigSliceSettings[('phiHalfWidth',type)]
      self.doFullScan =   InDetTrigSliceSettings[('doFullScan',type)]

      #monitoring
      from TRT_TrigTrackSegmentsFinder.TRT_TrigTrackSegmentsFinderMonitoring import TRT_TrigTrackSegmentsFinderValidationMonitor
      from TRT_TrigTrackSegmentsFinder.TRT_TrigTrackSegmentsFinderMonitoring import TRT_TrigTrackSegmentsFinderOnlineMonitor
      from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
      segtime = TrigTimeHistToolConfig("SegTime")
      segtime.TimerHistLimits = [0,100]
      if self.doFullScan:
         segtime.TimerHistLimits = [0,1000]
         
      self.AthenaMonTools = [TRT_TrigTrackSegmentsFinderValidationMonitor(),
                             TRT_TrigTrackSegmentsFinderOnlineMonitor(),
                             segtime]
      
# TRT seeded back tracking algorithm
#
from TRT_TrigSeededTrackFinder.TRT_TrigSeededTrackFinderConf import InDet__TRT_TrigSeededTrackFinder
class TRT_TrigSeededTrackFinder_EF( InDet__TRT_TrigSeededTrackFinder ):
   __slots__ = []
   def __init__(self, name="InDetTrigTRT_SeededTrackFinder_Photon_EF", type="photon"):
      super( InDet__TRT_TrigSeededTrackFinder, self ).__init__( name )
      
      from RegionSelector.RegSelToolConfig import makeRegSelTool_TRT
      self.RegSelTool = makeRegSelTool_TRT()

      from AthenaCommon.AppMgr import ToolSvc         
      from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import EFIDTrackingCuts
      InDetTrigCutValues = EFIDTrackingCuts
      from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
      from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigPrdAssociationTool, InDetTrigPatternPropagator, InDetTrigPatternUpdator, InDetTrigRotCreator, InDetTrigExtrapolator, InDetTrigTrackFitter, InDetTrigSCTConditionsSummaryTool


      if InDetTrigFlags.loadTRTSeededSPFinder():
         # configure tools used
         # TRT seeded space points seed maker
         #
         from TRT_SeededSpacePointFinderTool.TRT_SeededSpacePointFinderToolConf import InDet__TRT_SeededSpacePointFinder_ATL
         InDetTrigTRT_SeededSpacePointFinder =  InDet__TRT_SeededSpacePointFinder_ATL(name                   = 'InDetTrigTRT_SeededSpFinder_'+type  ,
                                                                                      SpacePointsSCTName     = "SCT_TrigSpacePoints" ,
                                                                                      #SpacePointsOverlapName = None ,
                                                                                      PRDtoTrackMap          = "",
                                                                                      NeighborSearch         = True ,
                                                                                      LoadFull               = False,
                                                                                      #DoCosmics
                                                                                      )
         if type == 'photon':
           InDetTrigTRT_SeededSpacePointFinder.PRDtoTrackMap = 'InDetTrigPRDtoTrackMap_Photon_EF'

      elif InDetTrigFlags.loadSimpleTRTSeededSPFinder():
         # from RegionSelector.RegSelSvcDefault import RegSelSvcDefault
         # InDetTrigRegSelSvc = RegSelSvcDefault()
         # InDetTrigRegSelSvc.enablePixel = DetFlags.pixel_on()
         # InDetTrigRegSelSvc.enableSCT   = DetFlags.SCT_on()
         # ServiceMgr += InDetTrigRegSelSvc
         # if (InDetTrigFlags.doPrintConfigurables()):
         #    print (              InDetTrigRegSelSvc)

         from TRT_SeededSpacePointFinderTool.TRT_SeededSpacePointFinderToolConf import InDet__SimpleTRT_SeededSpacePointFinder_ATL
         InDetTrigTRT_SeededSpacePointFinder =  InDet__SimpleTRT_SeededSpacePointFinder_ATL(name                   = 'InDetTrigTRT_SeededSpFinder_'+type  ,
                                                                                            SpacePointsSCTName     = "SCT_TrigSpacePoints",
                                                                                            #SpacePointsOverlapName = None,
                                                                                            PerigeeCut             = 1000.,
                                                                                            DirectionPhiCut        = .3,
                                                                                            DirectionEtaCut        = 1.,
                                                                                            MaxHoles               = 2,
                                                                                            PRDtoTrackMap          = "",
                                                                                            RestrictROI            = True)
         if type == 'photon':
           InDetTrigTRT_SeededSpacePointFinder.PRDtoTrackMap        = 'InDetTrigPRDtoTrackMap_Photon_EF' # @TODO correct ?

      ToolSvc += InDetTrigTRT_SeededSpacePointFinder
      if (InDetTrigFlags.doPrintConfigurables()):
        print (           InDetTrigTRT_SeededSpacePointFinder)

      # Silicon det elements road maker tool
      #
      from SiDetElementsRoadTool_xk.SiDetElementsRoadTool_xkConf import InDet__SiDetElementsRoadMaker_xk
      InDetTrigTRT_SeededSiRoadMaker = InDet__SiDetElementsRoadMaker_xk(name               = 'InDetTrigTRT_SeededSiRoad'  ,
                                                                        PropagatorTool     = InDetTrigPatternPropagator   ,
                                                                        PixManagerLocation = "Pixel",
                                                                        SCTManagerLocation = "SCT"  ,
                                                                        RoadWidth          = 35.    ,
                                                                        MaxStep            = 20.)
      ToolSvc += InDetTrigTRT_SeededSiRoadMaker
      if (InDetTrigFlags.doPrintConfigurables()):
        print (     InDetTrigTRT_SeededSiRoadMaker)

      # Local combinatorial track finding using space point seed and detector element road
      from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigSiComTrackFinder
      # uncomment the following only if modification to the previously defined InDetTrigSiComTrackFinder are needed
      # and also change the name of the instance 
      #
      
      # if (DetFlags.haveRIO.SCT_on()):
      #    InDetTrigSiComTrackFinder.SctSummaryTool = InDetTrigSCTConditionsSummaryTool
      # else:
      #    InDetTrigSiComTrackFinder.SctSummaryTool = None

      # ToolSvc += InDetTrigSiComTrackFinder
      # if (InDetTrigFlags.doPrintConfigurables()):
      #   print      (InDetTrigSiComTrackFinder)

      # TRT seeded back tracking tool
      #
      from InDetTrigRecExample.InDetTrigSliceSettings import InDetTrigSliceSettings
           
      from TRT_SeededTrackFinderTool.TRT_SeededTrackFinderToolConf import InDet__TRT_SeededTrackFinder_ATL
      InDetTrigTRT_SeededTrackTool =  InDet__TRT_SeededTrackFinder_ATL(name                     = 'InDetTrigTRT_SeededTrackTool_'+type,
                                                                       PropagatorTool           = InDetTrigPatternPropagator,
                                                                       UpdatorTool              = InDetTrigPatternUpdator,
                                                                       RoadTool                 = InDetTrigTRT_SeededSiRoadMaker,
                                                                       SeedTool                 = InDetTrigTRT_SeededSpacePointFinder,
                                                                       CombinatorialTrackFinder = InDetTrigSiComTrackFinder,
                                                                       nHolesMax                = InDetTrigCutValues.SecondarynHolesMax(),
	                                                               nHolesGapMax             = 2*InDetTrigCutValues.SecondarynHolesGapMax(),
	                                                               Xi2max                   = InDetTrigCutValues.SecondaryXi2max(),
	                                                               Xi2maxNoAdd              = InDetTrigCutValues.SecondaryXi2maxNoAdd(),
						                       ConsistentSeeds          = True,
                                                                       BremCorrection           = False )
      
      InDetTrigTRT_SeededTrackTool.pTmin = InDetTrigSliceSettings[('pTmin',type)]

      ToolSvc   += InDetTrigTRT_SeededTrackTool
      if (InDetTrigFlags.doPrintConfigurables()):
        print (     InDetTrigTRT_SeededTrackTool)

      self.RefitterTool          = InDetTrigTrackFitter
      self.TrackTool             = InDetTrigTRT_SeededTrackTool
      self.TrackExtensionTool    = ToolSvc.InDetTrigTRTExtensionTool
      self.TrtExtension          = True
      self.FinalRefit            = False
      self.OutputSegments        = False

      self.EtaHalfWidth = InDetTrigSliceSettings[('etaHalfWidth',type)]
      self.PhiHalfWidth = InDetTrigSliceSettings[('phiHalfWidth',type)]
      self.doFullScan =   InDetTrigSliceSettings[('doFullScan',type)]

      #monitoring
      from TRT_TrigSeededTrackFinder.TRT_TrigSeededTrackFinderMonitoring import TRT_TrigSeededTrackFinderValidationMonitor
      from TRT_TrigSeededTrackFinder.TRT_TrigSeededTrackFinderMonitoring import TRT_TrigSeededTrackFinderOnlineMonitor
      from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
      seedtime = TrigTimeHistToolConfig("SeedTime")
      seedtime.TimerHistLimits = [0,80]
      self.AthenaMonTools = [TRT_TrigSeededTrackFinderValidationMonitor(),
                             TRT_TrigSeededTrackFinderOnlineMonitor(),
                             seedtime]


# ---------- Ambiguity solving
#
from InDetTrigAmbiguitySolver.InDetTrigAmbiguitySolverConf import InDet__InDetTrigAmbiguitySolver
class TRTSeededTrigAmbiguitySolver_EF( InDet__InDetTrigAmbiguitySolver ):
   __slots__ = []
   def __init__(self, name="TRTSeededTrigAmbiguitySolver_Electron_EF", type="electron"):
      super(InDet__InDetTrigAmbiguitySolver , self ).__init__( name )

      from AthenaCommon.AppMgr import ToolSvc
      from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigPrdAssociationTool, InDetTrigTrackFitter, InDetTrigExtrapolator, InDetTrigTrackSummaryTool,InDetTrigTRTDriftCircleCut  
      from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
      from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import EFIDTrackingCuts
      InDetTrigCutValues = EFIDTrackingCuts

      # load InnerDetector TrackSelectionTool
      #
      from InDetAmbiTrackSelectionTool.InDetAmbiTrackSelectionToolConf import InDet__InDetAmbiTrackSelectionTool
      TRTSeededInDetTrigAmbiTrackSelectionTool = InDet__InDetAmbiTrackSelectionTool(name               = 'TRTSeededInDetTrigAmbiTrackSelectionTool',
                                                                                    DriftCircleCutTool  =  InDetTrigTRTDriftCircleCut,
                                                                                    AssociationTool     = TrackingCommon.getInDetTrigPRDtoTrackMapToolGangedPixels(),
                                                                                    minScoreShareTracks = -1., # off !
                                                                                    minHits             = InDetTrigCutValues.minSecondaryClusters(),
                                                                                    minNotShared        = InDetTrigCutValues.minSecondarySiNotShared(),
                                                                                    maxShared           = InDetTrigCutValues.maxSecondaryShared(),
                                                                                    minTRTHits          = InDetTrigCutValues.minSecondaryTRTonTrk(),
                                                                                    UseParameterization = InDetTrigCutValues.useParameterizedTRTCuts()   )
      
      ToolSvc += TRTSeededInDetTrigAmbiTrackSelectionTool
      if (InDetTrigFlags.doPrintConfigurables()):
        print (      TRTSeededInDetTrigAmbiTrackSelectionTool)

      from InDetTrigRecExample.InDetTrigSliceSettings import InDetTrigSliceSettings

      from InDetTrackScoringTools.InDetTrackScoringToolsConf import InDet__InDetAmbiScoringTool
      TRTSeededInDetTrigScoringTool = InDet__InDetAmbiScoringTool(name         = 'TRTSeededInDetTrigScoringTool_'+type,
                                                                  Extrapolator       = InDetTrigExtrapolator,
                                                                  DriftCircleCutTool = InDetTrigTRTDriftCircleCut,   
                                                                  SummaryTool        = InDetTrigTrackSummaryTool,
                                                                  useTRT_AmbigFcn    = True,
                                                                  useAmbigFcn        = False,
                                                                  maxRPhiImp         = InDetTrigCutValues.maxSecondaryImpact(),
                                                                  maxZImp            = InDetTrigCutValues.maxZImpact(),
                                                                  maxEta             = InDetTrigCutValues.maxEta(),
                                                                  minSiClusters      = InDetTrigCutValues.minSecondaryClusters(),
                                                                  maxSiHoles         = InDetTrigCutValues.maxSecondaryHoles(),
                                                                  maxPixelHoles      = InDetTrigCutValues.maxSecondaryPixelHoles(),
                                                                  maxSCTHoles        = InDetTrigCutValues.maxSecondarySCTHoles(),
                                                                  maxDoubleHoles     = InDetTrigCutValues.maxSecondaryDoubleHoles(),
                                                                  usePixel           = InDetTrigCutValues.usePixel(),
                                                                  useSCT             = InDetTrigCutValues.useSCT(),
                                                                  minTRTonTrk        = InDetTrigCutValues.minSecondaryTRTonTrk(),
                                                                  doEmCaloSeed = False,
                                                                  #useSigmaChi2       = False
                                                                  )
      
      TRTSeededInDetTrigScoringTool.minPt = InDetTrigSliceSettings[('pTmin',type)]
         
      #
      ToolSvc += TRTSeededInDetTrigScoringTool
      if (InDetTrigFlags.doPrintConfigurables()):
        print (         TRTSeededInDetTrigScoringTool)

      # load Ambiguity Processor
      #
      from TrkAmbiguityProcessor.TrkAmbiguityProcessorConf import Trk__SimpleAmbiguityProcessorTool
      TRTSeededInDetTrigAmbiguityProcessor = Trk__SimpleAmbiguityProcessorTool(name = 'TRTSeededInDetTrigAmbiguityProcessor',
                                                                               Fitter             = InDetTrigTrackFitter,
                                                                               AssociationTool    = TrackingCommon.getInDetTrigPRDtoTrackMapToolGangedPixels(),
                                                                               TrackSummaryTool   = InDetTrigTrackSummaryTool,
                                                                               SelectionTool      = TRTSeededInDetTrigAmbiTrackSelectionTool,
                                                                               RefitPrds          = not InDetTrigFlags.refitROT(),
                                                                               SuppressTrackFit   = False,
                                                                               SuppressHoleSearch = False,
                                                                               ScoringTool        = TRTSeededInDetTrigScoringTool)
                          
      if InDetTrigFlags.materialInteractions():
         TRTSeededInDetTrigAmbiguityProcessor.MatEffects = InDetTrigFlags.materialInteractionsType()
      else:
         TRTSeededInDetTrigAmbiguityProcessor.MatEffects = 0
         
      ToolSvc += TRTSeededInDetTrigAmbiguityProcessor
      if (InDetTrigFlags.doPrintConfigurables()):
        print (     TRTSeededInDetTrigAmbiguityProcessor)

      self.AmbiguityProcessor = TRTSeededInDetTrigAmbiguityProcessor
      self.InputTracksLabel = 'TRTSeededTracks'
      self.OutputTracksLabel = 'TRTSeededAmbigSolv'

      #monitoring
      from InDetTrigAmbiguitySolver.InDetTrigAmbiguitySolverMonitoring import InDetTrigAmbiguitySolverValidationMonitor
      from InDetTrigAmbiguitySolver.InDetTrigAmbiguitySolverMonitoring import InDetTrigAmbiguitySolverOnlineMonitor
      from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
      ambtime = TrigTimeHistToolConfig("AmbTime")
      ambtime.TimerHistLimits = [0,200]
      self.AthenaMonTools = [InDetTrigAmbiguitySolverValidationMonitor(),
                             InDetTrigAmbiguitySolverOnlineMonitor(),
                             ambtime]

# TRT seeded back tracking algorithm
#
from TRT_TrigStandaloneTrackFinder.TRT_TrigStandaloneTrackFinderConf import InDet__TRT_TrigStandaloneTrackFinder
class TRT_TrigStandaloneTrackFinder_EF( InDet__TRT_TrigStandaloneTrackFinder ):
   __slots__ = []
   def __init__(self, name="InDetTrigTRT_StandaloneTrackFinder_Photon_EF", type="photon", seqType="InsideOut"):
      super( InDet__TRT_TrigStandaloneTrackFinder, self ).__init__( name )
      
      from AthenaCommon.AppMgr import ToolSvc         
      from InDetTrigRecExample.InDetTrigSliceSettings import InDetTrigSliceSettings
      from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
      from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackSummaryTool, InDetTrigTrackFitterTRT, InDetTrigExtrapolator, InDetTrigPrdAssociationTool, InDetTrigTRTDriftCircleCut


      from AthenaCommon.SystemOfUnits import GeV
      if seqType == "TRTOnly":
         from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import EFIDTrackingCutsTRT
         InDetTrigCutValues = EFIDTrackingCutsTRT
         suffixTRT="_TRTOnly"
         prd_to_track_map=""  # no external PRD to track association, the PRD to track associations 
                              # are only computed from TRT standalone tracks
      else:
         from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import EFIDTrackingCuts
         InDetTrigCutValues = EFIDTrackingCuts
         suffixTRT=""
         prd_to_track_map="" # no external PRD to track association, the PRD to track associations 
                             # are only computed from TRT standalone tracks

      #
      # set up special Scoring Tool for standalone TRT tracks
      #
      from InDetTrackScoringTools.InDetTrackScoringToolsConf import InDet__InDetTrtTrackScoringTool
      InDetTrigTRT_StandaloneScoringTool = InDet__InDetTrtTrackScoringTool(name  = 'InDetTrigTRT_StandaloneScoringTool_'+type+suffixTRT,
                                                                           SummaryTool         = InDetTrigTrackSummaryTool,
                                                                           DriftCircleCutTool  = InDetTrigTRTDriftCircleCut,
                                                                           useAmbigFcn         = True,
                                                                           useSigmaChi2        = False,
                                                                           PtMin               = InDetTrigSliceSettings[('pTmin',type)],
                                                                           minTRTonTrk         = InDetTrigCutValues.minTRTonly(),
                                                                           maxEta              = 2.1)

      ToolSvc += InDetTrigTRT_StandaloneScoringTool
      if (InDetTrigFlags.doPrintConfigurables()):
         print (     InDetTrigTRT_StandaloneScoringTool)

      # import InDetRecExample.TrackingCommon   as TrackingCommon
      # asso_tool = TrackingCommon.getInDetTrigPRDtoTrackMapToolGangedPixels()
      prefix = 'InDet'
      suffix = ''
      #
      # set up TRT_SegmentToTrackTool
      #
      from TRT_SegmentToTrackTool.TRT_SegmentToTrackToolConf import InDet__TRT_SegmentToTrackTool
      InDetTrigTRT_SegmentToTrackTool = InDet__TRT_SegmentToTrackTool(name = 'InDetTrigTRT_SegmentToTrackTool_'+type+suffixTRT,
                                                                      RefitterTool          = InDetTrigTrackFitterTRT,
                                                                      Extrapolator          = InDetTrigExtrapolator,
                                                                      PRDtoTrackMap         = prefix+'PRDtoTrackMap'+suffix \
                                                                         if seqType != "InsideOutAndTRTOnly" else "",
                                                                      TrackSummaryTool      = InDetTrigTrackSummaryTool,
                                                                      ScoringTool           = InDetTrigTRT_StandaloneScoringTool,
                                                                      FinalRefit            = True,
                                                                      SuppressHoleSearch    = True,
                                                                      MaxSharedHitsFraction = InDetTrigCutValues.maxTRTonlyShared()
                                                                      )

      ToolSvc += InDetTrigTRT_SegmentToTrackTool
      if (InDetTrigFlags.doPrintConfigurables()):
         print (     InDetTrigTRT_SegmentToTrackTool)
      
      self.TRT_SegToTrackTool    = InDetTrigTRT_SegmentToTrackTool
      self.MinNumDriftCircles    = InDetTrigCutValues.minTRTonly()
      self.PRDtoTrackMap         = prd_to_track_map
      self.MaterialEffects       = 0
      
      #monitoring
      from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
      standtime = TrigTimeHistToolConfig("StandTime")
      standtime.TimerHistLimits = [0,100]
      if InDetTrigSliceSettings[('doFullScan',type)]:
         standtime.TimerHistLimits = [0,1000]

      from TRT_TrigStandaloneTrackFinder.TRT_TrigStandaloneTrackFinderMonitoring import TRT_TrigStandaloneTrackFinderValidationMonitor
      from TRT_TrigStandaloneTrackFinder.TRT_TrigStandaloneTrackFinderMonitoring import TRT_TrigStandaloneTrackFinderOnlineMonitor
      self.AthenaMonTools = [TRT_TrigStandaloneTrackFinderValidationMonitor(),
                             TRT_TrigStandaloneTrackFinderOnlineMonitor(),
                             standtime]

from InDetTrigTrackCollectionMerger.InDetTrigTrackCollectionMergerConf import Trk__TrigTrackCollectionMerger
class TrkTrigTrackCollectionMerger_EF( Trk__TrigTrackCollectionMerger ):
   __slots__ = []
   def __init__(self, name="TrigTrackCollectionMerger_Photon_EF", type="photon"):
      super( Trk__TrigTrackCollectionMerger, self ).__init__( name )

      from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigPrdAssociationTool,InDetTrigTrackSummaryToolSharedHits

      self.AssoTool    = InDetTrigPrdAssociationTool
      self.SummaryTool = InDetTrigTrackSummaryToolSharedHits
