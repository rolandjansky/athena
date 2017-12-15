
# --------------------------------------------      
#
# --- produce tracking ntuple
#
# --------------------------------------------      

# --- truth ?
if InDetFlags.doTruth() :
  TruthToTrackTool = InDetTruthToTrack
else:
  TruthToTrackTool = None
  
# --------------------------------------------      

if InDetFlags.doNtupleCreation():

  if InDetFlags.doTrkNtuple() or InDetFlags.doPixelTrkNtuple() or InDetFlags.doSctTrkNtuple() or InDetFlags.doTrtTrkNtuple():
    # --- include helper tools
    from TrkValTools.TrkValToolsConf import Trk__ResidualValidationNtupleHelper
    ResidualNtupleHelper      = Trk__ResidualValidationNtupleHelper( name = 'InDetResidualValHelper' )
    ToolSvc += ResidualNtupleHelper
    if (InDetFlags.doPrintConfigurables()):
      print ResidualNtupleHelper
    #
    from TrkValTools.TrkValToolsConf import Trk__HitPositionNtupleHelper
    HitPositionNtupleHelper   = Trk__HitPositionNtupleHelper( name = 'InDetHitPosValHelper' )
    ToolSvc += HitPositionNtupleHelper
    if (InDetFlags.doPrintConfigurables()):
      print HitPositionNtupleHelper
    #
    from TrkValTools.TrkValToolsConf import Trk__TrackPositionNtupleHelper
    TrackPositionNtupleHelper = Trk__TrackPositionNtupleHelper( name = 'InDetTrackPosValHelper' )
    ToolSvc += TrackPositionNtupleHelper
    if (InDetFlags.doPrintConfigurables()):
      print TrackPositionNtupleHelper
  
    PixelNtupleHelperToolsList = []
    SCTNtupleHelperToolsList   = []
    TRTNtupleHelperToolsList   = []
  
    if DetFlags.haveRIO.pixel_on():
      PixelNtupleHelperToolsList   = [ResidualNtupleHelper]
    if DetFlags.haveRIO.SCT_on():
      SCTNtupleHelperToolsList     = [ResidualNtupleHelper]
    if DetFlags.haveRIO.TRT_on():
      TRTNtupleHelperToolsList     = [ResidualNtupleHelper]
    GeneralNtupleHelperToolsList = [HitPositionNtupleHelper, TrackPositionNtupleHelper]

    minNumberOfPixelHitsForTracksInNtuple = 1 # for now, crashes if set to zero, under investigation

    # --- set ntuple structore for track variables
    from TrkValTools.TrkValToolsConf import Trk__TrackInformationNtupleTool
    InDetTrackInfoNtupleTool = Trk__TrackInformationNtupleTool(name="InDetTrackInfoNtupleTool")
    ToolSvc += InDetTrackInfoNtupleTool
    if InDetFlags.doPrintConfigurables():
        print InDetTrackInfoNtupleTool

    from TrkValTools.TrkValToolsConf import Trk__PerigeeParametersNtupleTool
    InDetPerigeeNtupleTool = Trk__PerigeeParametersNtupleTool \
         (name                     = "InDetPerigeeNtupleTool",
          DoTruth                  = InDetFlags.doTruth(),
          ExtrapolatorTool         = InDetExtrapolator)
    ToolSvc += InDetPerigeeNtupleTool 
    if InDetFlags.doPrintConfigurables():
        print InDetPerigeeNtupleTool

    from TrkValTools.TrkValToolsConf import Trk__MeasurementVectorNtupleTool
    InDetMeastsNtupleTool = Trk__MeasurementVectorNtupleTool(name                     = 'InDetMeastsNtupleTool',
                                                             HoleSearchTool           = InDetHoleSearchTool,
                                                             UpdatorTool              = InDetUpdator,
                                                             PixelNtupleHelperTools   = PixelNtupleHelperToolsList,
                                                             SCTNtupleHelperTools     = SCTNtupleHelperToolsList,
                                                             TRTNtupleHelperTools     = TRTNtupleHelperToolsList,
                                                             GeneralNtupleHelperTools = GeneralNtupleHelperToolsList,
                                                             DoHoleSearch             = True)
    ToolSvc += InDetMeastsNtupleTool
    if InDetFlags.doPrintConfigurables():
      print InDetMeastsNtupleTool

    # --- include track selection tool
    from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetTrackSelectorTool
    TrkValTrackSelectorTool = InDet__InDetTrackSelectorTool(name                  = 'InDetTrkValTrackSelectorTool',
                                                            minPt                 = InDetNewTrackingCuts.minPT(),
                                                            IPz0Max               = 1000.,
                                                            maxZ0                 = 1000.,
                                                            maxD0                 = 2000.,
                                                            maxD0overSigmaD0      = 500.,
                                                            numberOfPixelHits     = minNumberOfPixelHitsForTracksInNtuple,
                                                            numberOfInnermostPixelLayerHits    = 0,
                                                            TrackSummaryTool      = InDetTrackSummaryTool)
    #if (InDetFlags.doLowPt()):
    #  TrkValTrackSelectorTool.minPt = InDetNewTrackingCutsLowPt.minPT()

    if rec.Commissioning():
      TrkValTrackSelectorTool.minPt                   =     1000.0
      TrkValTrackSelectorTool.IPz0Max                 = 9999999.0
      TrkValTrackSelectorTool.maxZ0                   = 9999999.0
      TrkValTrackSelectorTool.maxD0                   = 9999999.0
      TrkValTrackSelectorTool.maxD0overSigmaD0        = 9999999.0
      TrkValTrackSelectorTool.numberOfPixelHits       =       1
      TrkValTrackSelectorTool.numberOfInnermostPixelLayerHits      =       0
      
    ToolSvc += TrkValTrackSelectorTool
    if (InDetFlags.doPrintConfigurables()):
      print TrkValTrackSelectorTool

    # --- include track ntuple alg

    if InDetFlags.doCosmics():
      # --- include InDetReconstructableSelector and adjust parameters to cosmics setups
      from TrkValTools.TrkValToolsConf import Trk__InDetReconstructableSelector
      TrkInDetReconstructableSelector = Trk__InDetReconstructableSelector(name               = 'InDetReconstructableSelector',
                                                                          # OutputLevel          = VERBOSE,
                                                                          MinPt              = 0.5,
                                                                          MaxRStartAll       = 2000000, #very loose cuts
                                                                          MaxZStartAll       = 2000000)
      ToolSvc += TrkInDetReconstructableSelector
      if (InDetFlags.doPrintConfigurables()):
        print TrkInDetReconstructableSelector

    from TrkValAlgs.TrkValAlgsConf import Trk__TrackValidationNtupleWriter
    TrkValNtupleWriter = Trk__TrackValidationNtupleWriter(name                 = 'InDetValNtupleWriter',
                                                          ValidationNtupleTools = [ InDetTrackInfoNtupleTool, InDetPerigeeNtupleTool, InDetMeastsNtupleTool ],
                                                          NtupleFileName       = 'TRKVAL',
                                                          NtupleDirectoryName  = 'Validation',
                                                          DoTruth              = InDetFlags.doTruth(),
                                                          TruthToTrackTool     = TruthToTrackTool,
                                                          TrackSelectorTool    = TrkValTrackSelectorTool)

    # third case in if statement for production of TrkNtuple without rerunning the reco
    if not InDetFlags.doCosmics() \
           or (InDetFlags.doCosmics() and InDetFlags.doNewTracking()) \
           or (InDetFlags.doCosmics() and not InDetFlags.doNewTracking()):
      TrkValNtupleWriter.TrackCollection      = [ InDetKeys.UnslimmedTracks() ]
      TrkValNtupleWriter.TrackTruthCollection = [ InDetKeys.UnslimmedTracksTruth() ]

    from TrkValTools.TrkValToolsConf import Trk__EventToTrackLinkNtupleTool
    EventToTrackLinkNtupleTool = Trk__EventToTrackLinkNtupleTool(name='EventToTrackLinkNtupleTool')
    ToolSvc += EventToTrackLinkNtupleTool
    from TrkValTools.TrkValToolsConf import Trk__EventPropertyNtupleTool
    EventPropertyNtupleTool = Trk__EventPropertyNtupleTool(name='EventPropertyNtupleTool',
                                                           FillTrtPhase = InDetFlags.doTRTPhaseCalculation())
    ToolSvc += EventPropertyNtupleTool
    TrkValNtupleWriter.EventPropertyNtupleTools = [ EventPropertyNtupleTool, EventToTrackLinkNtupleTool ]

    if rec.Production() and globalflags.DataSource == "data":
      TrkValNtupleWriter.TrackCollection = [InDetKeys.ResolvedTracks()]  # saves space

    if InDetFlags.doRefit() :
      TrkValNtupleWriter.TrackCollection      += [ InDetKeys.RefittedTracks() ]
      TrkValNtupleWriter.TrackTruthCollection += [ InDetKeys.RefittedTracksTruth() ]

    topSequence += TrkValNtupleWriter
    if (InDetFlags.doPrintConfigurables()):
      print TrkValNtupleWriter

  # configure sub detector tracking independent ntuple trees

  if InDetFlags.doSctClusterNtuple():
    from InDetTrackValidation.InDetTrackValidationConf import InDet__SCT_ClusterValidationNtupleWriter
    SctNtupleWriter = InDet__SCT_ClusterValidationNtupleWriter(name                 = 'InDetSctClusterValidationNtupleWriter',
                                                               NtupleFileName       = 'TRKVAL',
                                                               NtupleDirectoryName  = 'Validation',
                                                               NtupleTreeName       = 'SctRIOs',
                                                               DoHitsOnTracks       = True,                                                               
                                                               SCT_ClusterContainer =  InDetKeys.SCT_Clusters())
    topSequence += SctNtupleWriter
    if (InDetFlags.doPrintConfigurables()):
      print SctNtupleWriter
 
  # --------------------------------------------      

  if InDetFlags.doVtxNtuple():  
    from TrkVertexFitterValidationUtils.TrkVertexFitterValidationUtilsConf import Trk__TrkPriVxPurityTool
    InDetPriVxPurityTool =  Trk__TrkPriVxPurityTool(name                 = "InDetPriVxPurityTool",
                                                    VertexRTolerance     = 0.001,
                                                    VertexZTolerance     = 0.001,
                                                    SimTrackMapName      = "TrackParticleTruthCollection",
                                                    MonteCarloCollection = InDetKeys.McEventCollection())
    ToolSvc += InDetPriVxPurityTool
    if (InDetFlags.doPrintConfigurables()):
      print InDetPriVxPurityTool

    from TrkVertexFitterValidation.TrkVertexFitterValidationConf import Trk__PUVertexTest
    InDetVertexNTupleWriter = Trk__PUVertexTest(name = "InDetVertexNTupleWriter",
                                                NtupleFileName       = 'TRKVAL',
                                                NtupleDirectoryName  = 'Validation',
                                                NtupleTreeName       = 'VertexTree',
                                                VertexCollectionName = InDetKeys.PrimaryVertices(),
                                                MonteCarloCollection = InDetKeys.McEventCollection(),
                                                PurityTool           = InDetPriVxPurityTool,
                                                TrackSelector        = InDetTrackSelectorTool,
						McAvailable          = InDetFlags.doTruth() ,
                                                TracksName = InDetKeys.Tracks())
    topSequence += InDetVertexNTupleWriter
    if (InDetFlags.doPrintConfigurables()):
      print InDetVertexNTupleWriter

  # --------------------------------------------      

  if InDetFlags.doConvVtxNtuple():
    from TrkVertexFitterValidation.TrkVertexFitterValidationConf import Trk__ConvVertexTest
    InDetConversionNTupleWriter = Trk__ConvVertexTest(name                 = "InDetConversionNTupleWriter",
                                                      NtupleFileName       = 'TRKVAL',
                                                      NtupleDirectoryName  = 'Validation',
                                                      NtupleTreeName       = 'ConversionTree',
                                                      VertexCollectionName = InDetKeys.Conversions(),
                                                      MonteCarloCollection = InDetKeys.McEventCollection())
    topSequence += InDetConversionNTupleWriter
    if (InDetFlags.doPrintConfigurables()):
      print InDetConversionNTupleWriter

  # --------------------------------------------      

  if InDetFlags.doV0VtxNtuple():
    from TrkVertexAnalysisUtils.TrkVertexAnalysisUtilsConf import Trk__V0Tools
    InDetV0Tools = Trk__V0Tools(name = "InDetV0Tools")
    ToolSvc += InDetV0Tools
    if (InDetFlags.doPrintConfigurables()):
      print InDetV0Tools

    from TrkVertexFitterValidation.TrkVertexFitterValidationConf import Trk__V0VertexTest
    InDetV0NTupleWriter = Trk__V0VertexTest(name                 = "InDetV0NTupleWriter",
                                            NtupleFileName       = 'TRKVAL',
                                            NtupleDirectoryName  = 'Validation',
                                            NtupleTreeName       = 'V0Tree',
                                            McAvailable          = InDetFlags.doTruth(),
                                            maxRxy               = 900.,
                                            maxEta               = 2.5,
                                            minPt                = 500.,
                                            VertexCollectionName = InDetKeys.V0Candidates(),
                                            MonteCarloCollection = InDetKeys.McEventCollection(),
                                            V0Tools              = InDetV0Tools,
                                            Extrapolator         = InDetExtrapolator)

    topSequence += InDetV0NTupleWriter
    if (InDetFlags.doPrintConfigurables()):
      print InDetV0NTupleWriter


  # --- and the setup
  #
  theApp.HistogramPersistency = 'ROOT'
  if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()
  ServiceMgr.THistSvc.Output += [ "TRKVAL DATAFILE='" + InDetKeys.trkValidationNtupleName() + "' TYPE='ROOT' OPT='RECREATE'" ]
  theApp.Dlls += [ 'RootHistCnv' ]

# --------------------------------------------      
#
# --- standard plots
#
# --------------------------------------------      
if InDetFlags.doStandardPlots():
  if len(TrackCollectionKeys) > 0:
    if not (InDetFlags.doDBMstandalone() or InDetFlags.doDBM()):
    #
    # -- track selections 
    #
    # for standard good quality cuts
      from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
      InDetTrackSelectorToolGood = InDet__InDetDetailedTrackSelectorTool(name                 = "InDetDetailedTrackSelectorToolGood",
                                                                         pTMin                = 0,
                                                                         d0MaxPreselection    = 10,
                                                                         IPd0Max              = 2,
                                                                         IPz0Max              = 10000,
                                                                         etaMax               = 2.5,
                                                                         nHitSi               = 7,
                                                                         nHitBLayer           = 0,
                                                                         nHitPix              = 0,
                                                                         nHitTrtHighEFractionMax             = 1,
                                                                         nHitTrtHighEFractionWithOutliersMax = 1,
                                                                         useTrackSummaryInfo  = True,
                                                                         InDetTestBLayerTool  = InDetRecTestBLayerTool,
                                                                         TrackSummaryTool     = InDetTrackSummaryTool,
                                                                         Extrapolator         = InDetExtrapolator,
                                                                         TrtMaxEtaAcceptance  = 1.9)

      ToolSvc+=InDetTrackSelectorToolGood
      if (InDetFlags.doPrintConfigurables()):
        print      InDetTrackSelectorToolGood

    # for track selection as done by b-tagging group
      InDetTrackSelectorToolBtag = InDet__InDetDetailedTrackSelectorTool(name                 = "InDetDetailedTrackSelectorToolBtag",
                                                                         pTMin                = 0,
                                                                         IPd0Max              = 1,
                                                                         d0MaxPreselection    = 10,
                                                                         IPz0Max              = 40000,
                                                                         etaMax               = 2.5,
                                                                         nHitBLayer           = 1,
                                                                         nHitPix              = 2,
                                                                         nHitSi               = 7,
                                                                         nHitTrtHighEFractionMax             = 1,
                                                                         nHitTrtHighEFractionWithOutliersMax = 1,
                                                                         # -- do not give tool ! InDetTestBLayerTool  = InDetRecTestBLayerTool,
                                                                         TrackSummaryTool     = InDetTrackSummaryTool,
                                                                         Extrapolator         = InDetExtrapolator,
                                                                         TrtMaxEtaAcceptance  = 1.9)       

      ToolSvc+=InDetTrackSelectorToolBtag
      if (InDetFlags.doPrintConfigurables()):
        print  InDetTrackSelectorToolBtag

    # --- add an AthenaMonManager algorithm to the list of algorithms to be ran
    from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
    InDetTrackPerfMonManager = AthenaMonManager( name                = "InDetTrackPerfMonManager",
                                                 FileKey             = "stat",
                                                 ManualDataTypeSetup = True,
                                                 DataType            = "userDefined", # use this for collision data for now
                                                 Environment         = "user",
                                                 ManualRunLBSetup    = True,
                                                 Run                 = 1,
                                                 LumiBlock           = 1)
  
    topSequence += InDetTrackPerfMonManager
    if (InDetFlags.doPrintConfigurables()):
      print InDetTrackPerfMonManager
    

      
    
    # --- Setup the output histogram file(s)
    if not hasattr(ServiceMgr, 'THistSvc'):
      from GaudiSvc.GaudiSvcConf import THistSvc
      ServiceMgr += THistSvc()
    THistSvc = Service( "THistSvc" )
    histOutput = "InDetPerformanceMon DATAFILE='" + InDetKeys.StandardPlotHistName() + "' OPT='RECREATE'"
    THistSvc.Output += [histOutput]
    InDetTrackPerfMonManager.FileKey = "InDetPerformanceMon"
    
    from InDetPerformanceRTT.InDetPerformanceRTTConf import IDStandardPerformance as InDetStandardPerformance

    # all tracks
    InDetStandardPerformanceAll = InDetStandardPerformance (name               = "InDetStandardPerformanceAll",
                                                            tracksName         = InDetKeys.UnslimmedTracks(),
                                                            tracksTruthName    = InDetKeys.UnslimmedTracksTruth(),
                                                            SummaryTool        = InDetTrackSummaryToolSharedHits,
                                                            HoleSearch         = InDetHoleSearchTool,
                                                            useTrackSelection  = False,
                                                            HistDirectoryName  = "AllTracks",
                                                            TruthToTrackTool   = TruthToTrackTool,
                                                            doUpgrade          = InDetFlags.doSLHC(),
                                                            DoTruth            = InDetFlags.doTruth())

    if InDetFlags.doDBMstandalone() or InDetFlags.doDBM(): 
      InDetStandardPerformanceAll.tracksName = InDetKeys.DBMTracks() 
      InDetStandardPerformanceAll.tracksTruthName = InDetKeys.DBMTracksTruth()

#    if InDetFlags.doDBM():
#      InDetStandardPerformanceDBM = InDetStandardPerformance (name               = "InDetStandardPerformanceDBM",
#                                                            tracksName         = InDetKeys.DBMTracks(),
#                                                            tracksTruthName    = InDetKeys.DBMTracksTruth(),
#                                                            SummaryTool        = InDetTrackSummaryToolSharedHits,
#                                                            HoleSearch         = InDetHoleSearchTool,
#                                                            useTrackSelection  = False,
#                                                            HistDirectoryName  = "DBMTracks",
#                                                            TruthToTrackTool   = TruthToTrackTool,
#                                                            doUpgrade          = False,
#                                                            DoTruth            = InDetFlags.doTruth())
#      ToolSvc += InDetStandardPerformanceDBM
#      InDetTrackPerfMonManager.AthenaMonTools += [ InDetStandardPerformanceDBM ]

    ToolSvc += InDetStandardPerformanceAll
    if (InDetFlags.doPrintConfigurables()):
      print    InDetStandardPerformanceAll
#      if InDetFlags.doDBM():
#        print InDetStandardPerformanceDBM
        
    InDetTrackPerfMonManager.AthenaMonTools += [ InDetStandardPerformanceAll ]
        
  #monitoring truth-/pseudo-tracks in particular
    if InDetFlags.doPseudoTracking() :
      InDetStandardPerformancePseudoTracks = InDetStandardPerformance (name               = "InDetStandardPerformancePseudoTracks",
                                                                       tracksName         = InDetKeys.PseudoTracks(),
                                                                       tracksTruthName    = InDetKeys.PseudoTracksTruth(),
                                                                       SummaryTool        = InDetTrackSummaryToolSharedHits,
                                                                       HoleSearch         = InDetHoleSearchTool,
                                                                       useTrackSelection  = False,
                                                                       HistDirectoryName  = "PseudoTracksTracks",
                                                                       TruthToTrackTool   = TruthToTrackTool,
                                                                       doUpgrade          = InDetFlags.doSLHC(),
                                                                       DoTruth            = InDetFlags.doTruth())
      if InDetFlags.doSplitReco() :
        InDetStandardPerformancePseudoTracks.TruthParticleContainerName = "TruthEvent_PU"
      ToolSvc += InDetStandardPerformancePseudoTracks
      if (InDetFlags.doPrintConfigurables()):
        print    InDetStandardPerformancePseudoTracks
      InDetTrackPerfMonManager.AthenaMonTools += [ InDetStandardPerformancePseudoTracks ]

    # selected tracks passing good quality cuts
    if not (InDetFlags.doDBMstandalone() or InDetFlags.doDBM()):
      InDetStandardPerformanceGood = InDetStandardPerformance (name                = "InDetStandardPerformanceGood",
                                                               tracksName          = InDetKeys.UnslimmedTracks(),
                                                               tracksTruthName     = InDetKeys.UnslimmedTracksTruth(),
                                                               SummaryTool         = InDetTrackSummaryToolSharedHits,
                                                               HoleSearch          = InDetHoleSearchTool,
                                                               useTrackSelection   = True,
                                                               TrackSelectorTool   = InDetTrackSelectorToolGood,
                                                               HistDirectoryName   = "SelectedGoodTracks",
                                                               TruthToTrackTool    = TruthToTrackTool,
                                                               doUpgrade           = InDetFlags.doSLHC(),
                                                               DoTruth             = InDetFlags.doTruth())
    
      ToolSvc += InDetStandardPerformanceGood
      if (InDetFlags.doPrintConfigurables()):
        print    InDetStandardPerformanceGood
    
      InDetTrackPerfMonManager.AthenaMonTools += [ InDetStandardPerformanceGood ]

    # selected tracks passing b-tagging cuts
      InDetStandardPerformanceBtag = InDetStandardPerformance (name               = "InDetStandardPerformanceBtag",
                                                               tracksName         = InDetKeys.UnslimmedTracks(),
                                                               tracksTruthName    = InDetKeys.UnslimmedTracksTruth(),
                                                               SummaryTool        = InDetTrackSummaryToolSharedHits,
                                                               HoleSearch         = InDetHoleSearchTool,
                                                               useTrackSelection  = True,
                                                               TrackSelectorTool  = InDetTrackSelectorToolBtag,
                                                               HistDirectoryName  = "SelectedBtagTracks",
                                                               TruthToTrackTool   = TruthToTrackTool,
                                                               doUpgrade          = InDetFlags.doSLHC(),
                                                               DoTruth            = InDetFlags.doTruth())
    
      ToolSvc += InDetStandardPerformanceBtag
      if (InDetFlags.doPrintConfigurables()):
        print    InDetStandardPerformanceBtag
    
      InDetTrackPerfMonManager.AthenaMonTools += [ InDetStandardPerformanceBtag ]

    # selected Good IPatRec tracks
    if InDetFlags.doiPatRec():
      InDetStandardPerformanceGoodIPatRec = InDetStandardPerformance (name               = "InDetStandardPerformanceGoodIPatRec",
                                                                      tracksName         = "ConvertedIPatTracks",
                                                                      tracksTruthName    = "ConvertedIPatTracksTruth",
                                                                      SummaryTool        = InDetTrackSummaryToolSharedHits,
                                                                      HoleSearch         = InDetHoleSearchTool,
                                                                      useTrackSelection  = True,
                                                                      TrackSelectorTool  = InDetTrackSelectorToolGood,
                                                                      HistDirectoryName  = "SelectedGoodIPatTracks",
                                                                      TruthToTrackTool   = TruthToTrackTool,
                                                                      DoTruth            = InDetFlags.doTruth())
    
      ToolSvc += InDetStandardPerformanceGoodIPatRec
      if (InDetFlags.doPrintConfigurables()):
        print    InDetStandardPerformanceGoodIPatRec
    
      InDetTrackPerfMonManager.AthenaMonTools += [ InDetStandardPerformanceGoodIPatRec ]




# --------------------------------------------
#
# --- Physics Validation Monitoring
#
# --------------------------------------------

if InDetFlags.doPhysValMon():

  from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
  InDetPhysValMonManager = AthenaMonManager (name                = "InDetPhysValMonManager",
                                             FileKey             = "PhysValMon",
                                             ManualDataTypeSetup = True,
                                             DataType            = "userDefined", # use this for collision data for now
                                             Environment         = "user",
                                             ManualRunLBSetup    = True,
                                             Run                 = 1,
                                             LumiBlock           = 1)
  topSequence += InDetPhysValMonManager
  if (InDetFlags.doPrintConfigurables()):
    print InDetPhysValMonManager

  from InDetPhysValMonitoring.InDetPhysValMonitoringConf import InDetPhysValMonitoringTool
  if InDetFlags.doDBMstandalone():
    InDetPhysValMonTool = InDetPhysValMonitoringTool (useTrackSelection   = False,
                                                      TrackParticleContainerName = "InDetDBMTrackParticles", 
                                                      TrackSelectionTool = None, 
                                                      TruthSelectionTool = None, 
                                                      jetContainerName = "")
  else:
    InDetPhysValMonTool = InDetPhysValMonitoringTool (useTrackSelection   = True,
                                                      TrackSelectionTool   = InDetTrackSelectorTool)
#                                                      TrackParticleContainerName      = InDetKeys.Tracks())
#      InputTrackCollectionTruth = InDetKeys.TracksTruth()
#  if InDetFlags.doDBM():
#    InDetPhysValMonToolDBM = InDetPhysValMonitoringTool (useTrackSelection   = False,
#                                                         TrackParticleContainerName   = "InDetDBMTrackParticles",
#                                                         TruthParticleContainerName = "DBMTracksTruth",
#                                                         TrackSelectionTool = None,
#                                                         TruthSelectionTool = None,
#                                                         jetContainerName = "")

#    ToolSvc += InDetPhysValMonToolDBM
#    InDetPhysValMonManager.AthenaMonTools += [InDetPhysValMonToolDBM]

  ToolSvc += InDetPhysValMonTool
  InDetPhysValMonManager.AthenaMonTools += [InDetPhysValMonTool]
  if (InDetFlags.doPrintConfigurables()):
    print InDetPhysValMonTool
#    if InDetFlags.doDBM():
#      print InDetPhysValMonToolDBM
#monitoring pile-up particles separately if splitReco is used (fast chain)
  if InDetFlags.doSplitReco():
    InDetPhysValMonToolPU = InDetPhysValMonitoringTool (useTrackSelection   = True,
                                                        TrackSelectionTool   = InDetTrackSelectorTool,
                                                        TruthParticleContainerName = "SpclMCPU")
    ToolSvc += InDetPhysValMonToolPU
    InDetPhysValMonManager.AthenaMonTools += [InDetPhysValMonToolPU]
    if (InDetFlags.doPrintConfigurables()):
      print InDetPhysValMonToolPU	

  # --- Setup the output histogram file(s)
  if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()
  THistSvc = Service( "THistSvc" )
  THistSvc.Output += ["PhysValMon DATAFILE='"+InDetKeys.PhysValMonHistName()+"' OPT='RECREATE'"]
