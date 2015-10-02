# Blocking the include for after first inclusion
include.block ('InDetRecExample/ConfiguredTRTSegmentFinding.py')

# ------------------------------------------------------------------------------------
#
# ----------- TRT Segment finding
#
# -------------------------------------------------------------------------------------

class ConfiguredTRTSegmentFinding:

  def __init__(self, extension = "",InputCollections = None, NewTrackingCuts = None, BarrelSegments = None, EndcapSegments = None, doPhase = False):

    from InDetRecExample.InDetJobProperties import InDetFlags
    from InDetRecExample.InDetKeys          import InDetKeys
    from AthenaCommon.DetFlags              import DetFlags
    #
    # get ToolSvc and topSequence
    #
    from AthenaCommon.AppMgr                import ToolSvc
    from AthenaCommon.AlgSequence           import AlgSequence
    topSequence = AlgSequence()

    #
    # --- decide if use the association tool
    #
    if len(InputCollections) > 0:
      usePrdAssociationTool = True
    else:
      usePrdAssociationTool = False
    #
    # --- get list of already associated hits (always do this, even if no other tracking ran before)
    #
    if usePrdAssociationTool:
      from InDetTrackPRD_Association.InDetTrackPRD_AssociationConf import InDet__InDetTrackPRD_Association
      InDetSegmentPRD_Association = InDet__InDetTrackPRD_Association(name            = 'InDetSegmentPRD_Association'+extension,
                                                                     AssociationTool = InDetPrdAssociationTool,
                                                                     TracksName      = list(InputCollections)) 
      topSequence += InDetSegmentPRD_Association
      if (InDetFlags.doPrintConfigurables()):
        print InDetSegmentPRD_Association

    # ---------------------------------------------------------------
    #
    # --- now the main steering of the TRT segments finding
    #
    # ---------------------------------------------------------------

    if InDetFlags.doCosmics():
      #
      # --- cosmics barrel segments (use TRT track segements even for NewT) 
      #
      
      if doPhase:
       from TRT_TrackSegmentsTool_xk.TRT_TrackSegmentsTool_xkConf import InDet__TRT_TrackSegmentsMaker_BarrelCosmics
       InDetTRT_TrackSegmentsMakerPhase = InDet__TRT_TrackSegmentsMaker_BarrelCosmics(name = 'InDetTRTSegmentsMakerPhase'+extension,
                                                                                      TrtManagerLocation      = InDetKeys.TRT_Manager(),
                                                                                      TRT_ClustersContainer   = InDetKeys.TRT_DriftCirclesUncalibrated(),
                                                                                      AssosiationTool         = InDetPrdAssociationTool,
                                                                                      UseAssosiationTool      = usePrdAssociationTool,
                                                                                      IsMagneticFieldOn       = InDetFlags.solenoidOn())
       ToolSvc += InDetTRT_TrackSegmentsMakerPhase
       #InDetTRT_TrackSegmentsMakerPhase.OutputLevel = VERBOSE 
       if (InDetFlags.doPrintConfigurables()):
         print InDetTRT_TrackSegmentsMakerPhase

      else:
       from TRT_TrackSegmentsTool_xk.TRT_TrackSegmentsTool_xkConf import InDet__TRT_TrackSegmentsMaker_BarrelCosmics
       InDetTRT_TrackSegmentsMaker = InDet__TRT_TrackSegmentsMaker_BarrelCosmics(name = 'InDetTRTSegmentsMaker'+extension,
                                                                                 TrtManagerLocation      = InDetKeys.TRT_Manager(),
                                                                                 TRT_ClustersContainer   = InDetKeys.TRT_DriftCircles(),
                                                                                 AssosiationTool         = InDetPrdAssociationTool,
                                                                                 UseAssosiationTool      = usePrdAssociationTool,
                                                                                 IsMagneticFieldOn       = InDetFlags.solenoidOn()) 

       ToolSvc += InDetTRT_TrackSegmentsMaker
       #InDetTRT_TrackSegmentsMaker.OutputLevel = VERBOSE
       if (InDetFlags.doPrintConfigurables()):
         print InDetTRT_TrackSegmentsMaker

    else:
      #
      # --- cut values
      #
      if extension == "_TRT":
        # TRT Subdetector segment finding
        MinNumberDCs   = NewTrackingCuts.minTRTonly()
        pTmin          = NewTrackingCuts.minPT()
        sharedFrac     = NewTrackingCuts.maxTRTonlyShared()
      else:
        # TRT-only/back-tracking segment finding
        MinNumberDCs   = NewTrackingCuts.minSecondaryTRTonTrk()
        pTmin          = NewTrackingCuts.minSecondaryPt()
        sharedFrac     = NewTrackingCuts.maxSecondaryTRTShared()
      #
      # --- offline version  of TRT segemnt making
      #
      from TRT_TrackSegmentsTool_xk.TRT_TrackSegmentsTool_xkConf import InDet__TRT_TrackSegmentsMaker_ATLxk
      InDetTRT_TrackSegmentsMaker = InDet__TRT_TrackSegmentsMaker_ATLxk(name                    = 'InDetTRT_SeedsMaker'+extension,
                                                                        TrtManagerLocation      = InDetKeys.TRT_Manager(),
                                                                        TRT_ClustersContainer   = InDetKeys.TRT_DriftCircles(),
                                                                        PropagatorTool          = InDetPatternPropagator,
                                                                        TrackExtensionTool      = InDetTRTExtensionTool,
                                                                        UseAssosiationTool      = usePrdAssociationTool,
                                                                        AssosiationTool         = InDetPrdAssociationTool,
                                                                        RemoveNoiseDriftCircles = InDetFlags.removeTRTNoise(),
                                                                        MinNumberDriftCircles   = MinNumberDCs,
                                                                        NumberMomentumChannel   = NewTrackingCuts.TRTSegFinderPtBins(),
                                                                        pTmin                   = pTmin,
                                                                        sharedFrac              = sharedFrac)
      ToolSvc += InDetTRT_TrackSegmentsMaker
      if (InDetFlags.doPrintConfigurables()):
        print InDetTRT_TrackSegmentsMaker

    #
    # --- TRT track reconstruction
    #
    from TRT_TrackSegmentsFinder.TRT_TrackSegmentsFinderConf import InDet__TRT_TrackSegmentsFinder
    
    if doPhase: 
      InDetTRT_TrackSegmentsFinderPhase = InDet__TRT_TrackSegmentsFinder(name              = 'InDetTRT_TrackSegmentsFinderPhase'+extension,
                                                                         SegmentsMakerTool   = InDetTRT_TrackSegmentsMakerPhase,
                                                                         SegmentsLocation    = BarrelSegments)

      topSequence += InDetTRT_TrackSegmentsFinderPhase
      if (InDetFlags.doPrintConfigurables()):
       print InDetTRT_TrackSegmentsFinderPhase

    else:

     if InDetFlags.doCaloSeededTRTSegments() or NewTrackingCuts.RoISeededBackTracking():
      InDetTRT_TrackSegmentsFinder = InDet__TRT_TrackSegmentsFinder(name              = 'InDetTRT_TrackSegmentsFinder'+extension,
                                                                    SegmentsMakerTool   = InDetTRT_TrackSegmentsMaker,
                                                                    SegmentsLocation    = BarrelSegments,
                                                                    useCaloSeeds        = True,
                                                                    InputClusterContainerName = InDetKeys.CaloClusterROIContainer())

     else:
      InDetTRT_TrackSegmentsFinder = InDet__TRT_TrackSegmentsFinder(name              = 'InDetTRT_TrackSegmentsFinder'+extension,
                                                                    SegmentsMakerTool   = InDetTRT_TrackSegmentsMaker,
                                                                    SegmentsLocation    = BarrelSegments) 


     topSequence += InDetTRT_TrackSegmentsFinder
     if (InDetFlags.doPrintConfigurables()):
      print InDetTRT_TrackSegmentsFinder
      
    #
    # --- load TRT validation alg
    #
    if InDetFlags.doTruth() and not InDetFlags.doCosmics():
      from InDetSegmentDriftCircleAssValidation.InDetSegmentDriftCircleAssValidationConf import InDet__SegmentDriftCircleAssValidation
      InDetSegmentDriftCircleAssValidation = InDet__SegmentDriftCircleAssValidation(name                 = "InDetSegmentDriftCircleAssValidation"+extension,
                                                                                    OrigTracksLocation   = BarrelSegments                  ,
                                                                                    TRT_DriftCirclesName = InDetKeys.TRT_DriftCircles()    ,
                                                                                    pTmin                = pTmin                           ,
                                                                                    Pseudorapidity       = 2.1                             , # end of TRT
                                                                                    RadiusMin            = 0.                              ,
                                                                                    RadiusMax            = 600.                            ,
                                                                                    MinNumberDCs         = MinNumberDCs)
      topSequence += InDetSegmentDriftCircleAssValidation
      if (InDetFlags.doPrintConfigurables()):
        print InDetSegmentDriftCircleAssValidation

    
    
