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
               TrackCollectionKeys=[], TrackCollectionTruthKeys=[] ):

    from InDetRecExample.InDetJobProperties import InDetFlags
    from InDetRecExample.InDetKeys          import InDetKeys
    from AthenaCommon.DetFlags              import DetFlags
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
    if usePrdAssociationTool:
      from InDetTrackPRD_Association.InDetTrackPRD_AssociationConf import InDet__InDetTrackPRD_Association
      InDetTRTonly_PRD_Association = InDet__InDetTrackPRD_Association(name            = 'InDetTRTonly_PRD_Association'+extension,
                                                                      AssociationTool = InDetPrdAssociationTool,
                                                                      TracksName      = list(InputCollections)) 
      topSequence += InDetTRTonly_PRD_Association
      if (InDetFlags.doPrintConfigurables()):
        print InDetTRTonly_PRD_Association

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
                                                                     SummaryTool         = InDetTrackSummaryTool,
                                                                     DriftCircleCutTool  = InDetTRTDriftCircleCut,
                                                                     useAmbigFcn         = True,
                                                                     useSigmaChi2        = False,
                                                                     PtMin               = pTmin,
                                                                     minTRTonTrk         = NewTrackingCuts.minTRTonly(),
                                                                     maxEta              = 2.1,
                                                                     UseParameterization = NewTrackingCuts.useTRTonlyParamCuts(),
                                                                     OldTransitionLogic  = NewTrackingCuts.useTRTonlyOldLogic())
    # InDetTRT_StandaloneScoringTool.OutputLevel = VERBOSE 
    ToolSvc += InDetTRT_StandaloneScoringTool
    if (InDetFlags.doPrintConfigurables()):
      print InDetTRT_StandaloneScoringTool


    #
    # set up TRT_SegmentToTrackTool
    #
    from TRT_SegmentToTrackTool.TRT_SegmentToTrackToolConf import InDet__TRT_SegmentToTrackTool
    InDetTRT_SegmentToTrackTool = InDet__TRT_SegmentToTrackTool(name = 'InDetTRT_SegmentToTrackTool'+extension,
                                                                    RefitterTool          = InDetTrackFitterTRT,
                                                                    UseAssociationTool    = usePrdAssociationTool,
                                                                    AssociationTool       = InDetPrdAssociationTool,
                                                                    ScoringTool           = InDetTRT_StandaloneScoringTool,
                                                                    Extrapolator          = InDetExtrapolator,
                                                                    FinalRefit            = True,
                                                                    MaxSharedHitsFraction = NewTrackingCuts.maxTRTonlyShared(),
                                                                    SuppressHoleSearch    = True
                                                                    )
    ToolSvc += InDetTRT_SegmentToTrackTool
    if (InDetFlags.doPrintConfigurables()):
      print InDetTRT_SegmentToTrackTool

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
                                                                        ResetPRD              = True if extension == "_TRT" else False,
                                                                        OldTransitionLogic    = NewTrackingCuts.useTRTonlyOldLogic(),
                                                                        OutputTracksLocation  = self.__TRTStandaloneTracks,
                                                                        TRT_SegToTrackTool    = InDetTRT_SegmentToTrackTool
                                                                        )
      #InDetTRT_StandaloneTrackFinder.OutputLevel = VERBOSE
      topSequence += InDetTRT_StandaloneTrackFinder
      if InDetFlags.doPrintConfigurables():
        print InDetTRT_StandaloneTrackFinder

      # --- Delete TRT segments for the subdetector pattern only (back-tracking has already run by this point)
      from InDetRecExample.ConfiguredInDetSGDeletion import InDetSGDeletionAlg
      InDetSGDeletionAlg(container = "Trk::SegmentCollection#", key = BarrelSegments)
    
    else: 
      #
      # --- cosmics segment to track conversion for Barrel
      #

      from TRT_SegmentsToTrack.TRT_SegmentsToTrackConf import InDet__TRT_SegmentsToTrack
      InDetTrkSegmenttoTrk = InDet__TRT_SegmentsToTrack(name                      = "InDetTRT_SegmentsToTrack_Barrel"+extension,
                                                        InputSegmentsCollection   = BarrelSegments,
                                                        OutputTrackCollection     = self.__TRTStandaloneTracks,
                                                        TrackFitter               = InDetTrackFitter,
                                                        MinNHit                   = NewTrackingCuts.minTRTonly(),
                                                        CombineTracks             = False,
                                                        OutputCombiCollection     = "",
                                                        InputSCTCollection        = "",
                                                        OutlierRemoval            = True,
                                                        MaterialEffects           = False)
      #InDetTrkSegmenttoTrk.OutputLevel = VERBOSE
      topSequence += InDetTrkSegmenttoTrk
      if InDetFlags.doPrintConfigurables():
        print InDetTrkSegmenttoTrk


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
