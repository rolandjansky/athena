# Blocking the include for after first inclusion
include.block ('InDetRecExample/ConfiguredTRTStandalone.py')

# ------------------------------------------------------------------------------------
#
# ----------- TRT Standelone Track Finding
#
# ------------------------------------------------------------------------------------

class ConfiguredTRTStandalone:

  def __init__(self, extension = "",InputCollections = None, NewTrackingCuts = None,
               BarrelSegments = None,
               TrackCollectionKeys=[], TrackCollectionTruthKeys=[], PRDtoTrackMap='' ):

    from InDetRecExample.InDetJobProperties import InDetFlags
    from InDetRecExample.InDetKeys          import InDetKeys
    from AthenaCommon.DetFlags              import DetFlags

    import InDetRecExample.TrackingCommon   as TrackingCommon
    #
    # get ToolSvc and topSequence
    #
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()

    # --- Always use PRD association tool (even if only 1 collection) to remove TRT
    #     segments with significant overlaping hits 
    usePrdAssociationTool = True
    #usePrdAssociationTool = True if len(InputCollections) > 0 else False

    #
    # --- get list of already associated hits (always do this, even if no other tracking ran before)
    #
    prd_to_track_map = PRDtoTrackMap
    if usePrdAssociationTool and extension != "_TRT" :
      prefix='InDetTRTonly_'
      InDetTRTonly_PRD_Association = TrackingCommon.getInDetTrackPRD_Association(namePrefix = prefix,
                                                                                 nameSuffix = extension,
                                                                                 TracksName = list(InputCollections))

      prd_to_track_map = prefix+'PRDtoTrackMap'+extension
      topSequence += InDetTRTonly_PRD_Association
      if (InDetFlags.doPrintConfigurables()):
        printfunc (InDetTRTonly_PRD_Association)

    #
    # Cut values and output key for the TRT segments standalone TRT track finder
    #
    if extension == "_TRT":
      # TRT track segments
      pTmin                      = NewTrackingCuts.minPT()
      self.__TRTStandaloneTracks = InDetKeys.TRTTracks()
    else:
      # TRT standalone
      # pTmin                      = NewTrackingCuts.minSecondaryPt()       
      pTmin                      = NewTrackingCuts.minTRTonlyPt() # new cut parameter to make it flexible...
      self.__TRTStandaloneTracks = InDetKeys.TRTTracks_NewT()

    #
    # --- set up special Scoring Tool for standalone TRT tracks
    #
    from InDetTrackScoringTools.InDetTrackScoringToolsConf import InDet__InDetTrtTrackScoringTool
    InDetTRT_StandaloneScoringTool = InDet__InDetTrtTrackScoringTool(name                = 'InDetTRT_StandaloneScoringTool'+extension,
                                                                     SummaryTool         = TrackingCommon.getInDetTrackSummaryTool(),
                                                                     DriftCircleCutTool  = InDetTRTDriftCircleCut,
                                                                     useAmbigFcn         = True,
                                                                     useSigmaChi2        = False,
                                                                     PtMin               = pTmin,
                                                                     minTRTonTrk         = NewTrackingCuts.minTRTonly(),
                                                                     maxEta              = 2.1,
                                                                     UseParameterization = NewTrackingCuts.useTRTonlyParamCuts(),
                                                                     OldTransitionLogic  = NewTrackingCuts.useTRTonlyOldLogic(),
                                                                     minTRTPrecisionFraction = NewTrackingCuts.minSecondaryTRTPrecFrac())
    # InDetTRT_StandaloneScoringTool.OutputLevel = VERBOSE 
    ToolSvc += InDetTRT_StandaloneScoringTool
    if (InDetFlags.doPrintConfigurables()):
      printfunc (InDetTRT_StandaloneScoringTool)


    #
    # set up TRT_SegmentToTrackTool
    #
    from AthenaCommon import CfgGetter
    from TRT_SegmentToTrackTool.TRT_SegmentToTrackToolConf import InDet__TRT_SegmentToTrackTool
    asso_tool = TrackingCommon.getInDetPRDtoTrackMapToolGangedPixels() if usePrdAssociationTool else None
    InDetTRT_SegmentToTrackTool = InDet__TRT_SegmentToTrackTool(name = 'InDetTRT_SegmentToTrackTool'+extension,
                                                                RefitterTool          = CfgGetter.getPublicTool('InDetTrackFitterTRT'),
                                                                AssociationTool       = asso_tool,
                                                                TrackSummaryTool      = TrackingCommon.getInDetTrackSummaryTool(),
                                                                ScoringTool           = InDetTRT_StandaloneScoringTool,
                                                                Extrapolator          = TrackingCommon.getInDetExtrapolator(),
                                                                FinalRefit            = True,
                                                                MaxSharedHitsFraction = NewTrackingCuts.maxTRTonlyShared(),
                                                                SuppressHoleSearch    = True)

    ToolSvc += InDetTRT_SegmentToTrackTool
    if (InDetFlags.doPrintConfigurables()):
      printfunc (InDetTRT_SegmentToTrackTool)

    if not InDetFlags.doCosmics():
      #
      # --- TRT standalone tracks algorithm
      #

      from TRT_StandaloneTrackFinder.TRT_StandaloneTrackFinderConf import InDet__TRT_StandaloneTrackFinder
      InDetTRT_StandaloneTrackFinder = InDet__TRT_StandaloneTrackFinder(name                  = 'InDetTRT_StandaloneTrackFinder'+extension,
                                                                        MinNumDriftCircles    = NewTrackingCuts.minTRTonly(),
                                                                        MinPt                 = NewTrackingCuts.minTRTonlyPt(),
                                                                        InputSegmentsLocation = BarrelSegments,
                                                                        MaterialEffects       = 0,
                                                                        PRDtoTrackMap         = prd_to_track_map,
                                                                        OldTransitionLogic    = NewTrackingCuts.useTRTonlyOldLogic(),
                                                                        OutputTracksLocation  = self.__TRTStandaloneTracks,
                                                                        TRT_SegToTrackTool    = InDetTRT_SegmentToTrackTool
                                                                        )
      #InDetTRT_StandaloneTrackFinder.OutputLevel = VERBOSE
      topSequence += InDetTRT_StandaloneTrackFinder
      if InDetFlags.doPrintConfigurables():
        printfunc (InDetTRT_StandaloneTrackFinder)

      # --- Delete TRT segments for the subdetector pattern only (back-tracking has already run by this point)
      from InDetRecExample.ConfiguredInDetSGDeletion import InDetSGDeletionAlg
      InDetSGDeletionAlg(container = "Trk::SegmentCollection#", key = BarrelSegments)
    
    else: 
      #
      # --- cosmics segment to track conversion for Barrel
      #
      from AthenaCommon import CfgGetter
      from TRT_SegmentsToTrack.TRT_SegmentsToTrackConf import InDet__TRT_SegmentsToTrack
      InDetTrkSegmenttoTrk = InDet__TRT_SegmentsToTrack(name                      = "InDetTRT_SegmentsToTrack_Barrel"+extension,
                                                        InputSegmentsCollection   = BarrelSegments,
                                                        OutputTrackCollection     = self.__TRTStandaloneTracks,
                                                        TrackFitter               = CfgGetter.getPublicTool('InDetTrackFitter'),
                                                        SummaryTool               = TrackingCommon.getInDetTrackSummaryToolTRTTracks(),
                                                        AssociationTool           = TrackingCommon.getInDetPRDtoTrackMapToolGangedPixels() if prd_to_track_map !='' else None,
                                                        InputAssociationMapName   = prd_to_track_map,
                                                        MinNHit                   = NewTrackingCuts.minTRTonly(),
                                                        OutlierRemoval            = True,
                                                        MaterialEffects           = False)
      #InDetTrkSegmenttoTrk.OutputLevel = VERBOSE
      topSequence += InDetTrkSegmenttoTrk
      if InDetFlags.doPrintConfigurables():
        printfunc (InDetTrkSegmenttoTrk)


    #
    #
    # ------------ Track truth.
    #
    if ( extension == "_TRT" or not InDetFlags.doSGDeletion() ):
      if InDetFlags.doTruth():
        #
        # set up the truth info for this container
        #
        include ("InDetRecExample/ConfiguredInDetTrackTruth.py")
        InDetTracksTruth = ConfiguredInDetTrackTruth(self.__TRTStandaloneTracks,
                                                     self.__TRTStandaloneTracks+"DetailedTruth",
                                                     self.__TRTStandaloneTracks+"TruthCollection")
        #
        # add final output for statistics
        #
        TrackCollectionKeys      += [ InDetTracksTruth.Tracks() ]
        TrackCollectionTruthKeys += [ InDetTracksTruth.TracksTruth() ]
      else:
        TrackCollectionKeys      += [ self.__TRTStandaloneTracks ]
        
  def TRTStandaloneTracks (self):
    return self.__TRTStandaloneTracks
