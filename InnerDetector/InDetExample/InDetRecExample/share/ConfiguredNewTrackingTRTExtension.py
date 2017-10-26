# Blocking the include for after first inclusion
include.block ('InDetRecExample/ConfiguredNewTrackingTRTExtension.py')

# ------------------------------------------------------------
#
# ----------- Setup TRT Extension for New tracking
#
# ------------------------------------------------------------

class  ConfiguredNewTrackingTRTExtension:
   
   def __init__(self, NewTrackingCuts = None, SiTrackCollection = None, ExtendedTrackCollection = None, ExtendedTracksMap = None, TrackCollectionKeys=[], TrackCollectionTruthKeys=[] , doPhase = True):
      
      from InDetRecExample.InDetJobProperties import InDetFlags
      from AthenaCommon.DetFlags              import DetFlags
      from InDetRecExample.InDetKeys          import InDetKeys
      #
      # get ToolSvc and topSequence
      #
      from AthenaCommon.AppMgr                import ToolSvc
      from AthenaCommon.AlgSequence           import AlgSequence
      topSequence = AlgSequence()
      
      #    
      # ---------- TRT_TrackExtension
      #
      if InDetFlags.doTRTExtension() and InDetFlags.doTRTExtensionNew():


         #
         # Track extension to TRT algorithm
         #
         # set output extension map name
         OutputExtendedTracks = ExtendedTracksMap

         if doPhase:

           from TRT_TrackExtensionTool_xk.TRT_TrackExtensionTool_xkConf import InDet__TRT_TrackExtensionToolCosmics
           InDetTRTExtensionToolPhase = InDet__TRT_TrackExtensionToolCosmics(name                  = 'InDetTRT_ExtensionToolPhase',
                                                                             Propagator            = InDetPropagator,
                                                                             Extrapolator          = InDetExtrapolator,
                                                                             TRT_ClustersContainer = InDetKeys.TRT_DriftCirclesUncalibrated(),
                                                                             SearchNeighbour       = False, #needs debugging!!!
                                                                             RoadWidth             = 20.)

           ToolSvc += InDetTRTExtensionToolPhase
      

           from TRT_TrackExtensionAlg.TRT_TrackExtensionAlgConf import InDet__TRT_TrackExtensionAlg
           InDetTRTExtensionPhase    = InDet__TRT_TrackExtensionAlg (name                   = 'InDetTRT_ExtensionPhase'+NewTrackingCuts.extension(),
                                                                     InputTracksLocation    = SiTrackCollection,
                                                                     ExtendedTracksLocation = OutputExtendedTracks,
                                                                     TrackExtensionTool     = InDetTRTExtensionToolPhase)
           
           topSequence += InDetTRTExtensionPhase
           if (InDetFlags.doPrintConfigurables()):
            print InDetTRTExtensionPhase
           
         #
         else: 
          from TRT_TrackExtensionAlg.TRT_TrackExtensionAlgConf import InDet__TRT_TrackExtensionAlg
          InDetTRTExtension    = InDet__TRT_TrackExtensionAlg (name                   = 'InDetTRT_Extension'+NewTrackingCuts.extension(),
                                                              InputTracksLocation    = SiTrackCollection,
                                                              ExtendedTracksLocation = OutputExtendedTracks,
                                                              TrackExtensionTool     = InDetTRTExtensionTool)
          #InDetTRTExtension.OutputLevel = VERBOSE
          topSequence += InDetTRTExtension
          if (InDetFlags.doPrintConfigurables()):
            print InDetTRTExtension
   
      #
      # ------------ Track Extension Processor
      #
      if InDetFlags.doExtensionProcessor() and InDetFlags.doTRTExtensionNew():
            
         if InDetFlags.trtExtensionType() is 'DAF' :
            #
            # --- DAF Fitter setup
            #
            from TrkCompetingRIOsOnTrackTool.TrkCompetingRIOsOnTrackToolConf import Trk__CompetingRIOsOnTrackTool
            InDetCompetingRotCreator =  Trk__CompetingRIOsOnTrackTool( name                        = 'InDetCompetingRotCreator'+NewTrackingCuts.extension(),
                                                                       ToolForCompPixelClusters    = None,      # default
                                                                       ToolForCompSCT_Clusters     = None,      # default
                                                                       ToolForCompTRT_DriftCircles = InDetCompetingTRT_DC_Tool )
            ToolSvc += InDetCompetingRotCreator
            if (InDetFlags.doPrintConfigurables()):
               print InDetCompetingRotCreator
            #
            from TrkDeterministicAnnealingFilter.TrkDeterministicAnnealingFilterConf import Trk__DeterministicAnnealingFilter
            InDetExtensionFitter =  Trk__DeterministicAnnealingFilter( name = 'InDetDAF'+NewTrackingCuts.extension(),
                                                                       ToolForExtrapolation           = InDetExtrapolator,
                                                                       ToolForCompetingROTsCreation   = InDetCompetingRotCreator,
                                                                       ToolForUpdating                = InDetUpdator,
                                                                       AnnealingScheme                = [200., 81., 9., 4., 1., 1., 1.],
                                                                       DropOutlierCutValue            = 1.E-7,
                                                                       OutlierCutValue                = 0.01 )
            ToolSvc += InDetExtensionFitter
            if (InDetFlags.doPrintConfigurables()):
               print InDetExtensionFitter
         else:
            InDetExtensionFitter = InDetTrackFitter
            if NewTrackingCuts.mode() == "LowPt":
               InDetExtensionFitter = InDetTrackFitterLowPt

         #
         # --- load scoring for extension
         #
         if InDetFlags.doCosmics():
            from InDetTrackScoringTools.InDetTrackScoringToolsConf import InDet__InDetCosmicScoringTool
            InDetExtenScoringTool = InDet__InDetCosmicScoringTool(name                 = 'InDetCosmicExtenScoringTool',
                                                                  nWeightedClustersMin = 0,
                                                                  minTRTHits           = NewTrackingCuts.minTRTonTrk(),
                                                                  SummaryTool          = InDetTrackSummaryTool)
            
         else:
            have_calo_rois = InDetFlags.doBremRecovery() and InDetFlags.doCaloSeededBrem() and DetFlags.detdescr.Calo_allOn()
            from InDetTrackScoringTools.InDetTrackScoringToolsConf import InDet__InDetAmbiScoringTool
            InDetExtenScoringTool = InDet__InDetAmbiScoringTool(name                    = 'InDetExtenScoringTool'+NewTrackingCuts.extension(),
                                                                Extrapolator            = InDetExtrapolator,
                                                                SummaryTool             = InDetTrackSummaryTool,
                                                                DriftCircleCutTool      = InDetTRTDriftCircleCut,
                                                                useAmbigFcn             = True,  # this is NewTracking  
                                                                useTRT_AmbigFcn         = False,
                                                                minPt                   = NewTrackingCuts.minPT(),
                                                                maxRPhiImp              = NewTrackingCuts.maxPrimaryImpact(),
                                                                maxZImp                 = NewTrackingCuts.maxZImpact(),
                                                                maxEta                  = NewTrackingCuts.maxEta(),
                                                                minSiClusters           = NewTrackingCuts.minClusters(),
                                                                minPixel                = NewTrackingCuts.minPixel(),
                                                                maxSiHoles              = NewTrackingCuts.maxHoles(),
                                                                maxPixelHoles           = NewTrackingCuts.maxPixelHoles(),
                                                                maxSCTHoles             = NewTrackingCuts.maxSCTHoles(),
                                                                maxDoubleHoles          = NewTrackingCuts.maxDoubleHoles(),
                                                                usePixel                = NewTrackingCuts.usePixel(),
                                                                useSCT                  = NewTrackingCuts.useSCT(),
                                                                minTRTonTrk             = NewTrackingCuts.minTRTonTrk(),
                                                                minTRTPrecisionFraction = NewTrackingCuts.minTRTPrecFrac(),
                                                                doEmCaloSeed            = have_calo_rois)
            if not InDetExtenScoringTool.doEmCaloSeed:
               InDetExtenScoringTool.InputEmClusterContainerName = ''
            
            
            if InDetFlags.trackFitterType() in ['KalmanFitter', 'KalmanDNAFitter', 'ReferenceKalmanFitter']:
               InDetExtenScoringTool.minTRTPrecisionFraction = 0.2

         #InDetExtenScoringTool.OutputLevel = VERBOSE 
         ToolSvc += InDetExtenScoringTool
         if (InDetFlags.doPrintConfigurables()):
            print InDetExtenScoringTool     
         
         #
         # --- output track collection
         #
         self.__ForwardTrackCollection = ExtendedTrackCollection 
         #
         # --- get configured track extension processor
         #
         if doPhase:
         
          from InDetExtensionProcessor.InDetExtensionProcessorConf import InDet__InDetExtensionProcessor   
          InDetExtensionProcessorPhase = InDet__InDetExtensionProcessor ( name               = "InDetExtensionProcessorPhase"+NewTrackingCuts.extension(),
                                                                    TrackName          = SiTrackCollection,
                                                                    Cosmics            = True,
                                                                    ExtensionMap       = OutputExtendedTracks,
                                                                    NewTrackName       = self.__ForwardTrackCollection,
                                                                    TrackFitter        = InDetExtensionFitter,
                                                                    ScoringTool        = InDetExtenScoringTool,
                                                                    suppressHoleSearch = False,  # does not work properly
                                                                    tryBremFit         = InDetFlags.doBremRecovery(),
                                                                    caloSeededBrem     = InDetFlags.doCaloSeededBrem(),
                                                                    pTminBrem          = NewTrackingCuts.minPTBrem(),
                                                                    RefitPrds          = not (InDetFlags.refitROT() or (InDetFlags.trtExtensionType() is 'DAF')))
         #InDetExtensionProcessor.OutputLevel = VERBOSE
          if InDetFlags.materialInteractions():
             InDetExtensionProcessorPhase.matEffects = InDetFlags.materialInteractionsType()
          else:
             InDetExtensionProcessorPhase.matEffects = 0
   
          topSequence += InDetExtensionProcessorPhase
          if (InDetFlags.doPrintConfigurables()):
              print InDetExtensionProcessorPhase

         else:

          from InDetExtensionProcessor.InDetExtensionProcessorConf import InDet__InDetExtensionProcessor   
          InDetExtensionProcessor = InDet__InDetExtensionProcessor ( name               = "InDetExtensionProcessor"+NewTrackingCuts.extension(),
                                                                    TrackName          = SiTrackCollection,
                                                                    Cosmics            = InDetFlags.doCosmics(),
                                                                    ExtensionMap       = OutputExtendedTracks,
                                                                    NewTrackName       = self.__ForwardTrackCollection,
                                                                    TrackFitter        = InDetExtensionFitter,
                                                                    ScoringTool        = InDetExtenScoringTool,
                                                                    suppressHoleSearch = False,  # does not work properly
                                                                    tryBremFit         = InDetFlags.doBremRecovery(),
                                                                    caloSeededBrem     = InDetFlags.doCaloSeededBrem(),
                                                                    pTminBrem          = NewTrackingCuts.minPTBrem(),
                                                                    RefitPrds          = not (InDetFlags.refitROT() or (InDetFlags.trtExtensionType() is 'DAF')))
          #InDetExtensionProcessor.OutputLevel = VERBOSE
          if InDetFlags.materialInteractions():
             InDetExtensionProcessor.matEffects = InDetFlags.materialInteractionsType()
          else:
             InDetExtensionProcessor.matEffects = 0
   
          topSequence += InDetExtensionProcessor

          if (InDetFlags.doPrintConfigurables()):
             print InDetExtensionProcessor
         
         # --- Delete Resolved Si tracks and extension map
         from InDetRecExample.ConfiguredInDetSGDeletion import InDetSGDeletionAlg         
         InDetSGDeletionAlg(container = "TrackExtensionMap#", key = OutputExtendedTracks)
         if not InDetFlags.doMonitoringAlignment():
            InDetSGDeletionAlg(key = SiTrackCollection)
         
         #
         # ------------ Track truth.
         #
         if not InDetFlags.doSGDeletion():
            if InDetFlags.doTruth():
               #
               # set up the truth info for this container
               #
               include ("InDetRecExample/ConfiguredInDetTrackTruth.py")
               InDetTracksTruth = ConfiguredInDetTrackTruth(self.__ForwardTrackCollection,
                                                            self.__ForwardTrackCollection+"DetailedTruth",
                                                            self.__ForwardTrackCollection+"TruthCollection")
               #
               # add final output for statistics
               #
               TrackCollectionKeys      += [ InDetTracksTruth.Tracks() ]
               TrackCollectionTruthKeys += [ InDetTracksTruth.TracksTruth() ]
            else:
               TrackCollectionKeys      += [ self.__ForwardTrackCollection ]
               
         #
      else:
         self.__ForwardTrackCollection = SiTrackCollection

   def ForwardTrackCollection ( self ):
      return self.__ForwardTrackCollection
