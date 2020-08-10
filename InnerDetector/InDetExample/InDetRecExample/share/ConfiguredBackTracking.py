# Blocking the include for after first inclusion
include.block ('InDetRecExample/ConfiguredBackTracking.py')

# ------------------------------------------------------------
#
# ----------- 2nd iteration, outside in tracking
#
# ------------------------------------------------------------

class ConfiguredBackTracking:

   def __init__(self, InputCollections = None, NewTrackingCuts = None, TrackCollectionKeys=[] , TrackCollectionTruthKeys=[]):

      from InDetRecExample.InDetJobProperties import InDetFlags
      from InDetRecExample.InDetKeys          import InDetKeys

      import InDetRecExample.TrackingCommon   as TrackingCommon
      #
      # get ToolSvc and topSequence
      #
      from AthenaCommon.AppMgr                import ToolSvc
      from AthenaCommon.AppMgr                import ServiceMgr
      from AthenaCommon.AlgSequence           import AlgSequence
      topSequence = AlgSequence()

      #
      # --- decide if use the association tool
      #
      asso_tool = None
      if len(InputCollections) > 0:
         usePrdAssociationTool = True
         prefix = 'InDetSegment'
         suffix = ''
         asso_tool = TrackingCommon.getConstPRD_AssociationTool(namePrefix = prefix,nameSuffix = suffix)
      else:
         prefix = ''
         suffix = ''
         usePrdAssociationTool = False

      # --- the PRD association tool is filled by the Segment making
      #     no need to run again

      # ------------------------------------------------------------
      #
      # ---------- TRT Seeded Tracking
      #
      # ------------------------------------------------------------

      if InDetFlags.doTRTSeededTrackFinder():
         #
         # --- decide which TRT seed space point finder to use
         #
         if InDetFlags.loadTRTSeededSPFinder():
            #
            # --- defaul space point finder
            #
            from TRT_SeededSpacePointFinderTool.TRT_SeededSpacePointFinderToolConf import InDet__TRT_SeededSpacePointFinder_ATL
            InDetTRT_SeededSpacePointFinder = \
                  InDet__TRT_SeededSpacePointFinder_ATL(name                   = 'InDetTRT_SeededSpFinder'  ,
                                                        SpacePointsSCTName     = InDetKeys.SCT_SpacePoints(),
                                                        SpacePointsOverlapName = InDetKeys.OverlapSpacePoints(),
                                                        PRDtoTrackMap          = prefix+'PRDtoTrackMap'+suffix \
                                                                                    if usePrdAssociationTool else "",
                                                        NeighborSearch         = True,
                                                        LoadFull               = False,
                                                        DoCosmics              = InDetFlags.doCosmics(),
                                                        pTmin                  = NewTrackingCuts.minSecondaryPt())
            # InDetTRT_SeededSpacePointFinder.OutputLevel = VERBOSE

         elif InDetFlags.loadSimpleTRTSeededSPFinder():
            #
            # --- alternative version using the region selector
            #
            from RegionSelector.RegSelSvcDefault import RegSelSvcDefault
            InDetRegSelSvc             = RegSelSvcDefault()
            InDetRegSelSvc.enablePixel = DetFlags.pixel_on()
            InDetRegSelSvc.enableSCT   = DetFlags.SCT_on()

            ServiceMgr += InDetRegSelSvc
            if (InDetFlags.doPrintConfigurables()):
               printfunc (InDetRegSelSvc)

            from TRT_SeededSpacePointFinderTool.TRT_SeededSpacePointFinderToolConf import InDet__SimpleTRT_SeededSpacePointFinder_ATL
            InDetTRT_SeededSpacePointFinder = InDet__SimpleTRT_SeededSpacePointFinder_ATL(name                   = 'InDetTRT_SeededSpFinder'  ,
                                                                                          SpacePointsSCTName     = InDetKeys.SCT_SpacePoints(),
                                                                                          SpacePointsOverlapName = InDetKeys.OverlapSpacePoints(),
                                                                                          PerigeeCut             = 1000.,
                                                                                          DirectionPhiCut        = .3,
                                                                                          DirectionEtaCut        = 1.,
                                                                                          MaxHoles               = 2,
                                                                                          AssociationTool        = asso_tool,
                                                                                          RestrictROI            = True)
            # InDetTRT_SeededSpacePointFinder.OutputLevel = VERBOSE
            if not usePrdAssociationTool:
               InDetTRT_SeededSpacePointFinder.AssociationTool = None

         # add either into the Tool Service
         ToolSvc += InDetTRT_SeededSpacePointFinder
         if (InDetFlags.doPrintConfigurables()):
            printfunc (InDetTRT_SeededSpacePointFinder)
         #
         # Silicon det elements road maker tool
         #
         from SiDetElementsRoadTool_xk.SiDetElementsRoadTool_xkConf import InDet__SiDetElementsRoadMaker_xk
         InDetTRT_SeededSiRoadMaker = InDet__SiDetElementsRoadMaker_xk(name               = 'InDetTRT_SeededSiRoad'  ,
                                                                       PropagatorTool     = InDetPatternPropagator   ,
                                                                       usePixel           = NewTrackingCuts.usePixel(),
                                                                       PixManagerLocation = InDetKeys.PixelManager() ,
                                                                       useSCT             = NewTrackingCuts.useSCT(),
                                                                       SCTManagerLocation = InDetKeys.SCT_Manager()  ,
                                                                       RoadWidth          = 35.,
                                                                       MaxStep            = 20.)                       # NOT DEFAULT ?
         # InDetTRT_SeededSiRoadMaker.OutputLevel = VERBOSE
         if InDetFlags.doCosmics():
            InDetTRT_SeededSiRoadMaker.RoadWidth = 50
         # Condition algorithm for InDet__SiDetElementsRoadMaker_xk
         if NewTrackingCuts.useSCT():
            from AthenaCommon.AlgSequence import AthSequencer
            condSeq = AthSequencer("AthCondSeq")
            if not hasattr(condSeq, "InDet__SiDetElementsRoadCondAlg_xk"):
               from SiDetElementsRoadTool_xk.SiDetElementsRoadTool_xkConf import InDet__SiDetElementsRoadCondAlg_xk
               condSeq += InDet__SiDetElementsRoadCondAlg_xk(name = "InDet__SiDetElementsRoadCondAlg_xk")

         ToolSvc += InDetTRT_SeededSiRoadMaker
         if (InDetFlags.doPrintConfigurables()):
            printfunc (InDetTRT_SeededSiRoadMaker)
         #
         # --- TRT seeded back tracking tool
         #
         from TRT_SeededTrackFinderTool.TRT_SeededTrackFinderToolConf import InDet__TRT_SeededTrackFinder_ATL
         InDetTRT_SeededTrackTool =  InDet__TRT_SeededTrackFinder_ATL(name                     = 'InDetTRT_SeededTrackMaker',
                                                                      PropagatorTool           = InDetPatternPropagator,
                                                                      UpdatorTool              = InDetPatternUpdator,
                                                                      RoadTool                 = InDetTRT_SeededSiRoadMaker,
                                                                      SeedTool                 = InDetTRT_SeededSpacePointFinder,
                                                                      CombinatorialTrackFinder = InDetSiComTrackFinder,
                                                                      pTmin                    = NewTrackingCuts.minSecondaryPt(),
                                                                      nHolesMax                = NewTrackingCuts.SecondarynHolesMax(),
                                                                      # ME bugfix: nHolesGapMax             = 2*NewTrackingCuts.SecondarynHolesGapMax(),
                                                                      nHolesGapMax             = NewTrackingCuts.SecondarynHolesGapMax(),
                                                                      Xi2max                   = NewTrackingCuts.SecondaryXi2max(),
                                                                      Xi2maxNoAdd              = NewTrackingCuts.SecondaryXi2maxNoAdd(),
                                                                      SearchInCaloROI          = False,
                                                                      InputClusterContainerName= InDetKeys.CaloClusterROIContainer(),
                                                                      ConsistentSeeds          = True,
                                                                      # BremCorrection           = True,
                                                                      BremCorrection           = False)
         if InDetFlags.doCosmics():
            InDetTRT_SeededTrackTool.nWClustersMin = 0

         ToolSvc   += InDetTRT_SeededTrackTool
         if (InDetFlags.doPrintConfigurables()):
            printfunc (InDetTRT_SeededTrackTool)

         #
         # --- Output key for the finder
         #
         self.__TRTSeededTracks = InDetKeys.TRTSeededTracks()
         #
         # TRT seeded back tracking algorithm
         

         from AthenaCommon import CfgGetter
         from TRT_SeededTrackFinder.TRT_SeededTrackFinderConf import InDet__TRT_SeededTrackFinder
         import InDetRecExample.TrackingCommon as TrackingCommon
         InDetTRT_SeededTrackFinder = InDet__TRT_SeededTrackFinder(name                  = 'InDetTRT_SeededTrackFinder',
                                                                   RefitterTool          = CfgGetter.getPublicTool('InDetTrackFitter'),
                                                                   TrackTool             = InDetTRT_SeededTrackTool,
                                                                   PRDtoTrackMap         = prefix+'PRDtoTrackMap'+suffix \
                                                                                               if usePrdAssociationTool else "",
                                                                   TrackSummaryTool      = TrackingCommon.getInDetTrackSummaryToolNoHoleSearch(),
                                                                   TrackExtensionTool    = InDetTRTExtensionTool,
                                                                   MinTRTonSegment       = NewTrackingCuts.minSecondaryTRTonTrk(),
                                                                   MinTRTonly            = NewTrackingCuts.minTRTonly(),
                                                                   TrtExtension          = True,
                                                                   SiExtensionCuts       = NewTrackingCuts.SiExtensionCuts(),
                                                                   minPt                 = NewTrackingCuts.minSecondaryPt(),
                                                                   maxRPhiImp            = NewTrackingCuts.maxSecondaryImpact(),
                                                                   maxZImp               = NewTrackingCuts.maxZImpact(),
                                                                   maxEta                = NewTrackingCuts.maxEta(),
                                                                   Extrapolator          = TrackingCommon.getInDetExtrapolator(),
                                                                   RejectShortExtension  = NewTrackingCuts.rejectShortExtensions(),
                                                                   FinalRefit            = False,
                                                                   FinalStatistics       = False,
                                                                   OutputSegments        = False,
                                                                   InputSegmentsLocation = InDetKeys.TRT_Segments(),
                                                                   OutputTracksLocation  = self.__TRTSeededTracks,
                                                                   CaloClusterEt         = NewTrackingCuts.minRoIClusterEt())
         
         if (NewTrackingCuts.RoISeededBackTracking()):
            from RegionSelector.RegSelToolConfig import makeRegSelTool_SCT
            InDetTRT_SeededTrackFinder.RegSelTool = makeRegSelTool_SCT()
            InDetTRT_SeededTrackFinder.CaloSeededRoI = True
         # InDetTRT_SeededTrackFinder.OutputLevel = VERBOSE
         topSequence += InDetTRT_SeededTrackFinder
         if (InDetFlags.doPrintConfigurables()):
            printfunc (InDetTRT_SeededTrackFinder)

         #
         # ------------ Track truth.
         #
         if not InDetFlags.doSGDeletion():
            if InDetFlags.doTruth():
               #
               # set up the truth info for this container
               #
               include ("InDetRecExample/ConfiguredInDetTrackTruth.py")
               InDetTracksTruth = ConfiguredInDetTrackTruth(self.__TRTSeededTracks,
                                                            self.__TRTSeededTracks+"DetailedTruth",
                                                            self.__TRTSeededTracks+"TruthCollection")
               #
               # add final output for statistics
               #
               TrackCollectionKeys      += [ InDetTracksTruth.Tracks() ]
               TrackCollectionTruthKeys += [ InDetTracksTruth.TracksTruth() ]
            else:
               TrackCollectionKeys      += [ self.__TRTSeededTracks ]

         # --- output track collection
         self.__BackTrackingTracks = self.__TRTSeededTracks

      # ------------------------------------------------------------
      #
      # --- Resolve back tracking tracks ?
      #
      # ------------------------------------------------------------

      if InDetFlags.doResolveBackTracks():
         #
         # --- set up special Scoring Tool for TRT seeded tracks
         #
         if InDetFlags.doCosmics():
            InDetTRT_SeededScoringTool = TrackingCommon.getInDetCosmicScoringTool_TRT(NewTrackingCuts)
            InDetTRT_SeededSummaryTool = TrackingCommon.getInDetTrackSummaryToolSharedHits()
         else:
            InDetTRT_SeededScoringTool = TrackingCommon.getInDetTRT_SeededScoringTool(NewTrackingCuts)
            InDetTRT_SeededSummaryTool = TrackingCommon.getInDetTrackSummaryTool()

         #
         # --- Load selection tool
         #
         from InDetAmbiTrackSelectionTool.InDetAmbiTrackSelectionToolConf import InDet__InDetAmbiTrackSelectionTool
         InDetTRT_SeededAmbiTrackSelectionTool = InDet__InDetAmbiTrackSelectionTool(name                = 'InDetTRT_SeededAmbiTrackSelectionTool',
                                                                                    DriftCircleCutTool  =  InDetTRTDriftCircleCut,
                                                                                    AssociationTool     = TrackingCommon.getInDetPRDtoTrackMapToolGangedPixels(),
                                                                                    minScoreShareTracks = -1., # off !
                                                                                    minHits             = NewTrackingCuts.minSecondaryClusters(),
                                                                                    minNotShared        = NewTrackingCuts.minSecondarySiNotShared(),
                                                                                    maxShared           = NewTrackingCuts.maxSecondaryShared(),
                                                                                    minTRTHits          = NewTrackingCuts.minSecondaryTRTonTrk(),
                                                                                    UseParameterization = NewTrackingCuts.useParameterizedTRTCuts(),
                                                                                    Cosmics             = InDetFlags.doCosmics(),
                                                                                    doPixelSplitting    = InDetFlags.doPixelClusterSplitting())

         # InDetTRT_SeededAmbiTrackSelectionTool.OutputLevel = DEBUG
         ToolSvc += InDetTRT_SeededAmbiTrackSelectionTool
         if (InDetFlags.doPrintConfigurables()):
            printfunc (InDetTRT_SeededAmbiTrackSelectionTool)

         #
         # --- load Ambiguity Processor
         #
         from TrkAmbiguityProcessor.TrkAmbiguityProcessorConf import Trk__SimpleAmbiguityProcessorTool
         InDetTRT_SeededAmbiguityProcessor = Trk__SimpleAmbiguityProcessorTool(name               = 'InDetTRT_SeededAmbiguityProcessor',
                                                                               Fitter             = CfgGetter.getPublicTool('InDetTrackFitter'),
                                                                               AssociationTool    = TrackingCommon.getInDetPRDtoTrackMapToolGangedPixels(),
                                                                               TrackSummaryTool   = InDetTRT_SeededSummaryTool,
                                                                               SelectionTool      = InDetTRT_SeededAmbiTrackSelectionTool,
                                                                               RefitPrds          = not InDetFlags.refitROT(),
                                                                               SuppressTrackFit   = False,
                                                                               SuppressHoleSearch = False,
                                                                               ScoringTool        = InDetTRT_SeededScoringTool)
         # InDetTRT_SeededAmbiguityProcessor.OutputLevel = DEBUG
         if InDetFlags.materialInteractions():
            InDetTRT_SeededAmbiguityProcessor.MatEffects = InDetFlags.materialInteractionsType()
         else:
            InDetTRT_SeededAmbiguityProcessor.MatEffects = 0
         #
         ToolSvc += InDetTRT_SeededAmbiguityProcessor
         if (InDetFlags.doPrintConfigurables()):
            printfunc (InDetTRT_SeededAmbiguityProcessor)


         #
         # --- load the algorithm
         #

         from TrkAmbiguitySolver.TrkAmbiguitySolverConf import Trk__TrkAmbiguityScore
         InDetAmbiguityScore = Trk__TrkAmbiguityScore(name               = 'InDetTRT_SeededAmbiguityScore',
                                                      TrackInput         = [ self.__TRTSeededTracks ],
                                                      TrackOutput        = 'ScoredMap'+'InDetTRT_SeededAmbiguityScore')
         topSequence += InDetAmbiguityScore


         self.__ResolvedTRTSeededTracks = InDetKeys.ResolvedTRTSeededTracks()
         #
         from TrkAmbiguitySolver.TrkAmbiguitySolverConf import Trk__TrkAmbiguitySolver
         InDetTRT_SeededAmbiguitySolver = Trk__TrkAmbiguitySolver(name               = 'InDetTRT_SeededAmbiguitySolver',
                                                                  TrackInput         = 'ScoredMap'+'InDetTRT_SeededAmbiguityScore',
                                                                  TrackOutput        = self.__ResolvedTRTSeededTracks,
                                                                  AmbiguityProcessor = InDetTRT_SeededAmbiguityProcessor)
         topSequence += InDetTRT_SeededAmbiguitySolver
         if (InDetFlags.doPrintConfigurables()):
            printfunc (InDetTRT_SeededAmbiguitySolver)

         # --- Delete (non-resloved) TRT seeded tracks
         from InDetRecExample.ConfiguredInDetSGDeletion import InDetSGDeletionAlg
         InDetSGDeletionAlg(key = InDetKeys.TRTSeededTracks())

         #
         # ------------ Track truth.
         #
         if not InDetFlags.doSGDeletion():
            if InDetFlags.doTruth():
               #
               # set up the truth info for this container
               #
               include ("InDetRecExample/ConfiguredInDetTrackTruth.py")
               InDetTracksTruth = ConfiguredInDetTrackTruth(self.__ResolvedTRTSeededTracks,
                                                            self.__ResolvedTRTSeededTracks+"DetailedTruth",
                                                            self.__ResolvedTRTSeededTracks+"TruthCollection")
               #
               # add final output for statistics
               #
               TrackCollectionKeys      += [ InDetTracksTruth.Tracks() ]
               TrackCollectionTruthKeys += [ InDetTracksTruth.TracksTruth() ]
            else:
               TrackCollectionKeys      += [ self.__ResolvedTRTSeededTracks ]

         # --- output track collection
         self.__BackTrackingTracks = self.__ResolvedTRTSeededTracks

   def BackTrackingTracks ( self ):
      return self.__BackTrackingTracks
