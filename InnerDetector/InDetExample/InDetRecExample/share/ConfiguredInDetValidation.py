
# Blocking the include for after first inclusion
include.block ('InDetRecExample/ConfiguredInDetValidation.py')

# ------------------------------------------------------------
#
# ----------- Truth Validation algorithms for tracks
#
# ------------------------------------------------------------

class  ConfiguredInDetValidation:

  def __init__ (self, nameExt = "",
                doStat = True, doClusVal = True,
                NewTrackingCuts = None,
                TrackCollectionKeys=[] , TrackCollectionTruthKeys=[],
                rmin = 0., rmax = 20.,
                McEventCollectionKey = "TruthEvent"):
    
    from InDetRecExample.InDetJobProperties import InDetFlags
    from InDetRecExample.InDetKeys import InDetKeys
    #
    # get ToolSvc and topSequence
    #
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    
    # ------------------------------------------------------------
    #
    # ------------- Do we do statistics for tracks ?
    #
    # ------------------------------------------------------------
    #
    if doStat:
      #
      # --- load statistics alg
      #
      from InDetRecStatistics.InDetRecStatisticsConf import InDet__InDetRecStatisticsAlg
      InDetRecStatistics = InDet__InDetRecStatisticsAlg (name                     = "InDetRecStatistics"+nameExt,
                                                         TrackCollectionKeys      = TrackCollectionKeys,
                                                         TrackTruthCollectionKeys = TrackCollectionTruthKeys,
                                                         McTrackCollectionKey     = McEventCollectionKey,
                                                         PrintSecondary           = True,
                                                         TruthToTrackTool         = (InDetTruthToTrack if InDetFlags.doTruth() else None),
                                                         UseTrackSummary          = True,
                                                         SummaryTool              = InDetTrackSummaryToolSharedHits, # this is a bug !!!
                                                         DoTruth                  = InDetFlags.doTruth(),
                                                         # maxEta                   = NewTrackingCuts.maxEta(),
                                                         # minPt                    = 2. * NewTrackingCuts.minPT(),
                                                         fakeTrackCut             = 0.8,    # ME: adapt cuts 
                                                         fakeTrackCut2            = 0.5,    # ME: adapt cuts 
                                                         matchTrackCut            = 0.5,    # ME: adapt cuts 
                                                         # maxEta                   = 2.5,    # ME: full coverage ends here
                                                         maxEta                   = NewTrackingCuts.maxEta(),
                                                         minPt                    = 1.*GeV, # ME: let's restrict to higher pt
                                                         maxRStartPrimary         = 25.0,   # ME: allows for IBL
                                                         maxRStartSecondary       = 560.0,  # ME: exit of SCT (we have TRT only)
                                                         maxZStartPrimary         = 320.0,  # ME: is in the beam pipe anyway
                                                         maxZStartSecondary       = 1500.0, # ME: enough forward hits
                                                         minREndPrimary           = 400.0,  # ME: between SCT 2 and 3 
                                                         minREndSecondary         = 1000.0, # ME: leaves in R
                                                         minZEndPrimary           = 2300.0, # ME: 2 disks in the forward
                                                         minZEndSecondary         = 2700.0) # ME: or leaves in z 
      if InDetFlags.doDBMstandalone() or nameExt=="DBM" or nameExt=="PUDBM":
        InDetRecStatistics.minPt              = .0*GeV
        InDetRecStatistics.maxEta             = 9999.
        InDetRecStatistics.maxRStartPrimary   = 9999999.
        InDetRecStatistics.maxRStartSecondary = 9999999.
        InDetRecStatistics.maxZStartPrimary   = 9999999.
        InDetRecStatistics.maxZStartSecondary = 9999999.
        InDetRecStatistics.minREndPrimary     = 0.
        InDetRecStatistics.minREndSecondary   = 0.
        InDetRecStatistics.minZEndPrimary     = 0.
        InDetRecStatistics.minZEndSecondary   = 0.

      elif (InDetFlags.doCosmics() or InDetFlags.doBeamHalo()):
        # change cuts for cosmics
        InDetRecStatistics.minPt              = .0*GeV
        InDetRecStatistics.maxEta             = 9999.
        InDetRecStatistics.maxEtaEndcap       = 9999.
        InDetRecStatistics.maxRStartPrimary   = 9999999.
        InDetRecStatistics.maxRStartSecondary = 9999999.
        InDetRecStatistics.maxZStartPrimary   = 9999999.
        InDetRecStatistics.maxZStartSecondary = 9999999.
        InDetRecStatistics.minREndPrimary     = 0.
        InDetRecStatistics.minREndSecondary   = 0.
        InDetRecStatistics.minZEndPrimary     = 0.
        InDetRecStatistics.minZEndSecondary   = 0.

        
      topSequence += InDetRecStatistics
      if (InDetFlags.doPrintConfigurables()):
        print InDetRecStatistics
            
    #
    # --- load cluster validation alg
    #
    if doClusVal:
      from InDetTrackClusterAssValidation.InDetTrackClusterAssValidationConf import InDet__TrackClusterAssValidation
      InDetTrackClusterAssValidation = InDet__TrackClusterAssValidation(name                   = "InDetTrackClusterAssValidation"+nameExt,
                                                                        TracksLocation         = TrackCollectionKeys             ,
                                                                        SpacePointsPixelName   = InDetKeys.PixelSpacePoints()    ,
                                                                        SpacePointsSCTName     = InDetKeys.SCT_SpacePoints()     ,
                                                                        SpacePointsOverlapName = InDetKeys.OverlapSpacePoints()  ,
                                                                        MomentumCut            = 2. * NewTrackingCuts.minPT()   ,
                                                                        RapidityCut            = NewTrackingCuts.maxEta()        ,
                                                                        RadiusMin              = rmin                            ,
                                                                        RadiusMax              = rmax                            ,
                                                                        MinNumberClusters      = NewTrackingCuts.minClusters()   ,
                                                                        MinNumberClustersTRT   = 0                               ,
                                                                        MinNumberSpacePoints   = 3                               ,
                                                                        usePixel               = DetFlags.haveRIO.pixel_on()     ,
                                                                        useSCT                 = DetFlags.haveRIO.SCT_on()       ,
                                                                        useTRT                 = DetFlags.haveRIO.TRT_on()       )

      if InDetFlags.doDBMstandalone() or  nameExt=="DBM" or nameExt=="PUDBM":
        InDetTrackClusterAssValidation.MomentumCut            = 0
        InDetTrackClusterAssValidation.RadiusMax              = 9999999.0
#        InDetTrackClusterAssValidation.RapidityCut            = 9999.9
        InDetTrackClusterAssValidation.MinNumberClustersTRT   = 0
        InDetTrackClusterAssValidation.MinNumberClusters      = 0
        InDetTrackClusterAssValidation.MinNumberSpacePoints   = 0
      
      elif InDetFlags.doCosmics() or InDetFlags.doBeamHalo():
        InDetTrackClusterAssValidation.MomentumCut            = NewTrackingCuts.minPT()
        InDetTrackClusterAssValidation.RadiusMax              = 9999999.0
        InDetTrackClusterAssValidation.RapidityCut            = 9999.9
        InDetTrackClusterAssValidation.MinNumberClustersTRT   = 0
        InDetTrackClusterAssValidation.MinNumberClusters      = 8
        #InDetTrackClusterAssValidation.MinNumberClustersTRT   = 0
        InDetTrackClusterAssValidation.MinNumberSpacePoints   = 4
        if InDetFlags.doBeamHalo():
          InDetTrackClusterAssValidation.MomentumCut            = 0
 
      topSequence += InDetTrackClusterAssValidation
      if (InDetFlags.doPrintConfigurables()):
        print InDetTrackClusterAssValidation

