
# Blocking the include for after first inclusion
include.block ('InDetRecExample/ConfiguredNewTrackingSiPattern.py')

# ------------------------------------------------------------
#
# ----------- Setup Si Pattern for New tracking
#
# ------------------------------------------------------------

class  ConfiguredNewTrackingSiPattern:

   def __init__(self, InputCollections = None, ResolvedTrackCollectionKey = None, SiSPSeededTrackCollectionKey = None , NewTrackingCuts = None, TrackCollectionKeys=[] , TrackCollectionTruthKeys=[]):
      
      from InDetRecExample.InDetJobProperties import InDetFlags
      from InDetRecExample.InDetKeys          import InDetKeys
      #
      # --- get ToolSvc and topSequence
      #
      from AthenaCommon.AppMgr                import ToolSvc
      from AthenaCommon.AlgSequence           import AlgSequence
      topSequence = AlgSequence()

      #
      # --- decide if use the association tool
      #
      if (len(InputCollections) > 0) and (NewTrackingCuts.mode() == "LowPt" or NewTrackingCuts.mode() == "VeryLowPt" or NewTrackingCuts.mode() == "LargeD0" or NewTrackingCuts.mode() == "LowPtLargeD0" or NewTrackingCuts.mode() == "BeamGas" or NewTrackingCuts.mode() == "ForwardTracks" or NewTrackingCuts.mode() == "ForwardSLHCTracks"  or NewTrackingCuts.mode() == "PixelPrdAssociation" or NewTrackingCuts.mode() == "VeryForwardSLHCTracks" or NewTrackingCuts.mode() == "SLHCConversionFinding"):
         usePrdAssociationTool = True
      else:
         usePrdAssociationTool = False

      #
      # --- get list of already associated hits (always do this, even if no other tracking ran before)
      #
      if usePrdAssociationTool:
         from InDetTrackPRD_Association.InDetTrackPRD_AssociationConf import InDet__InDetTrackPRD_Association
         InDetPRD_Association = InDet__InDetTrackPRD_Association(name            = 'InDetPRD_Association'+NewTrackingCuts.extension(),
                                                                 AssociationTool = InDetPrdAssociationTool,
                                                                 TracksName      = list(InputCollections)) 
         topSequence += InDetPRD_Association
         if (InDetFlags.doPrintConfigurables()):
            print InDetPRD_Association

      # ------------------------------------------------------------
      #
      # ----------- SiSPSeededTrackFinder
      #
      # ------------------------------------------------------------

      if InDetFlags.doSiSPSeededTrackFinder():
         #
         # --- Space points seeds maker, use different ones for cosmics and collisions
         #
         if NewTrackingCuts.mode() == "DBM":
            from SiSpacePointsSeedTool_xk.SiSpacePointsSeedTool_xkConf import InDet__SiSpacePointsSeedMaker_ATLxk as SiSpacePointsSeedMaker
         elif InDetFlags.doCosmics():
            from SiSpacePointsSeedTool_xk.SiSpacePointsSeedTool_xkConf import InDet__SiSpacePointsSeedMaker_Cosmic as SiSpacePointsSeedMaker
         elif InDetFlags.doHeavyIon():
            from SiSpacePointsSeedTool_xk.SiSpacePointsSeedTool_xkConf import InDet__SiSpacePointsSeedMaker_HeavyIon as SiSpacePointsSeedMaker
         elif NewTrackingCuts.mode() == "LowPt" or NewTrackingCuts.mode() == "VeryLowPt" or (NewTrackingCuts.mode() == "Pixel" and InDetFlags.doMinBias()) :
            from SiSpacePointsSeedTool_xk.SiSpacePointsSeedTool_xkConf import InDet__SiSpacePointsSeedMaker_LowMomentum as SiSpacePointsSeedMaker
         elif NewTrackingCuts.mode() == "BeamGas":
            from SiSpacePointsSeedTool_xk.SiSpacePointsSeedTool_xkConf import InDet__SiSpacePointsSeedMaker_BeamGas as SiSpacePointsSeedMaker
         elif NewTrackingCuts.mode() == "SLHC" or NewTrackingCuts.mode() == "ForwardSLHCTracks" or NewTrackingCuts.mode() == "VeryForwardSLHCTracks" :
            from SiSpacePointsSeedTool_xk.SiSpacePointsSeedTool_xkConf import InDet__SiSpacePointsSeedMaker_ITK as SiSpacePointsSeedMaker
         else:
            from SiSpacePointsSeedTool_xk.SiSpacePointsSeedTool_xkConf import InDet__SiSpacePointsSeedMaker_ATLxk as SiSpacePointsSeedMaker

         InDetSiSpacePointsSeedMaker = SiSpacePointsSeedMaker (name                   = "InDetSpSeedsMaker"+NewTrackingCuts.extension(),
                                                               pTmin                  = NewTrackingCuts.minPT(),
                                                               maxdImpact             = NewTrackingCuts.maxPrimaryImpact(),
                                                               maxZ                   = NewTrackingCuts.maxZImpact(),
                                                               minZ                   = -NewTrackingCuts.maxZImpact(),
                                                               usePixel               = NewTrackingCuts.usePixel(),
                                                               SpacePointsPixelName   = InDetKeys.PixelSpacePoints(),
                                                               # useSCT                 = NewTrackingCuts.useSCT(),
                                                               useSCT                 = (NewTrackingCuts.useSCT() and NewTrackingCuts.useSCTSeeding()),
                                                               SpacePointsSCTName     = InDetKeys.SCT_SpacePoints(),
                                                               # useOverlapSpCollection = NewTrackingCuts.useSCT(),
                                                               useOverlapSpCollection = (NewTrackingCuts.useSCT() and NewTrackingCuts.useSCTSeeding()),
                                                               SpacePointsOverlapName = InDetKeys.OverlapSpacePoints(),
                                                               radMax                 = NewTrackingCuts.radMax(),
                                                               RapidityCut            = NewTrackingCuts.maxEta())
            
         if NewTrackingCuts.mode() == "Offline" or InDetFlags.doHeavyIon() or  NewTrackingCuts.mode() == "ForwardTracks":
            InDetSiSpacePointsSeedMaker.maxdImpactPPS = NewTrackingCuts.maxdImpactPPSSeeds()
            InDetSiSpacePointsSeedMaker.maxdImpactSSS = NewTrackingCuts.maxdImpactSSSSeeds()
         if usePrdAssociationTool:
            # not all classes have that property !!!
            InDetSiSpacePointsSeedMaker.UseAssociationTool = True
            InDetSiSpacePointsSeedMaker.AssociationTool    = InDetPrdAssociationTool
         if not InDetFlags.doCosmics():
            InDetSiSpacePointsSeedMaker.maxRadius1         = 0.75*NewTrackingCuts.radMax()
            InDetSiSpacePointsSeedMaker.maxRadius2         = NewTrackingCuts.radMax()
            InDetSiSpacePointsSeedMaker.maxRadius3         = NewTrackingCuts.radMax()
         if NewTrackingCuts.mode() == "LowPt" or NewTrackingCuts.mode() == "VeryLowPt" or (NewTrackingCuts.mode() == "Pixel" and InDetFlags.doMinBias()):
            try :
               InDetSiSpacePointsSeedMaker.pTmax              = NewTrackingCuts.maxPT()
            except:
               pass 
            InDetSiSpacePointsSeedMaker.mindRadius         = 4.0
         if NewTrackingCuts.mode() == "SLHC" or NewTrackingCuts.mode() == "SLHCConversionFinding":
            InDetSiSpacePointsSeedMaker.minRadius1         = 0
            InDetSiSpacePointsSeedMaker.minRadius2         = 0
            InDetSiSpacePointsSeedMaker.minRadius3         = 0
            InDetSiSpacePointsSeedMaker.maxRadius1         =1000.*Units.mm
            InDetSiSpacePointsSeedMaker.maxRadius2         =1000.*Units.mm
            InDetSiSpacePointsSeedMaker.maxRadius3         =1000.*Units.mm
         if NewTrackingCuts.mode() == "ForwardTracks" or NewTrackingCuts.mode() == "ForwardSLHCTracks" or NewTrackingCuts.mode() == "VeryForwardSLHCTracks":
            InDetSiSpacePointsSeedMaker.checkEta           = True
            InDetSiSpacePointsSeedMaker.etaMin             = NewTrackingCuts.minEta()
            InDetSiSpacePointsSeedMaker.etaMax             = NewTrackingCuts.maxEta()
            InDetSiSpacePointsSeedMaker.RapidityCut        = NewTrackingCuts.maxEta()
         if NewTrackingCuts.mode() == "DBM":
            InDetSiSpacePointsSeedMaker.etaMin             = NewTrackingCuts.minEta()
            InDetSiSpacePointsSeedMaker.etaMax             = NewTrackingCuts.maxEta()
            InDetSiSpacePointsSeedMaker.useDBM = True
         if NewTrackingCuts.mode() == "PixelThreeLayer":
            InDetSiSpacePointsSeedMaker.SkipIBLcut = True

                    
         #InDetSiSpacePointsSeedMaker.OutputLevel = VERBOSE
         ToolSvc += InDetSiSpacePointsSeedMaker
         if (InDetFlags.doPrintConfigurables()):
            print InDetSiSpacePointsSeedMaker
            
         #
         # --- Z-coordinates primary vertices finder (only for collisions)
         #
         if InDetFlags.useZvertexTool() and NewTrackingCuts.mode() != "DBM":
            from SiZvertexTool_xk.SiZvertexTool_xkConf import InDet__SiZvertexMaker_xk
            InDetZvertexMaker = InDet__SiZvertexMaker_xk(name          = 'InDetZvertexMaker'+NewTrackingCuts.extension(),
                                                         Zmax          = NewTrackingCuts.maxZImpact(),
                                                         Zmin          = -NewTrackingCuts.maxZImpact(),
                                                         minRatio      = 0.17) # not default
            InDetZvertexMaker.SeedMakerTool = InDetSiSpacePointsSeedMaker

            if InDetFlags.doHeavyIon():
               InDetZvertexMaker.HistSize = 2000
               ###InDetZvertexMaker.minContent = 200 
               InDetZvertexMaker.minContent = 30
               
            ToolSvc += InDetZvertexMaker
            if (InDetFlags.doPrintConfigurables()):
               print InDetZvertexMaker

         else:
            InDetZvertexMaker = None

         #
         # --- SCT and Pixel detector elements road builder
         #
         from SiDetElementsRoadTool_xk.SiDetElementsRoadTool_xkConf import InDet__SiDetElementsRoadMaker_xk
         InDetSiDetElementsRoadMaker = InDet__SiDetElementsRoadMaker_xk(name               = 'InDetSiRoadMaker'+NewTrackingCuts.extension(),
                                                                        PropagatorTool     = InDetPatternPropagator,
                                                                        usePixel           = NewTrackingCuts.usePixel(),
                                                                        PixManagerLocation = InDetKeys.PixelManager(),
                                                                        useSCT             = NewTrackingCuts.useSCT(), 
                                                                        SCTManagerLocation = InDetKeys.SCT_Manager(),         
                                                                        RoadWidth          = NewTrackingCuts.RoadWidth())
         #InDetSiDetElementsRoadMaker.OutputLevel = VERBOSE
         ToolSvc += InDetSiDetElementsRoadMaker
         if (InDetFlags.doPrintConfigurables()):
            print      InDetSiDetElementsRoadMaker

         #
         # --- Local track finding using sdCaloSeededSSSpace point seed
         #

         useBremMode = NewTrackingCuts.mode() == "Offline" or NewTrackingCuts.mode() == "SLHC" or NewTrackingCuts.mode() == "DBM"
         from SiTrackMakerTool_xk.SiTrackMakerTool_xkConf import InDet__SiTrackMaker_xk as SiTrackMaker
         InDetSiTrackMaker = SiTrackMaker(name                      = 'InDetSiTrackMaker'+NewTrackingCuts.extension(),
                                          useSCT                    = NewTrackingCuts.useSCT(),
                                          usePixel                  = NewTrackingCuts.usePixel(),
                                          RoadTool                  = InDetSiDetElementsRoadMaker,
                                          CombinatorialTrackFinder  = InDetSiComTrackFinder,
                                          pTmin                     = NewTrackingCuts.minPT(),
                                          pTminBrem                 = NewTrackingCuts.minPTBrem(),
                                          pTminSSS                  = InDetFlags.pT_SSScut(),
                                          nClustersMin              = NewTrackingCuts.minClusters(),
                                          nHolesMax                 = NewTrackingCuts.nHolesMax(),
                                          nHolesGapMax              = NewTrackingCuts.nHolesGapMax(),
                                          SeedsFilterLevel          = NewTrackingCuts.seedFilterLevel(),
                                          Xi2max                    = NewTrackingCuts.Xi2max(),
                                          Xi2maxNoAdd               = NewTrackingCuts.Xi2maxNoAdd(),
                                          nWeightedClustersMin      = NewTrackingCuts.nWeightedClustersMin(),
                                          CosmicTrack               = InDetFlags.doCosmics(),
                                          Xi2maxMultiTracks         = NewTrackingCuts.Xi2max(), # was 3.
                                          useSSSseedsFilter         = InDetFlags.doSSSfilter(), 
                                          doMultiTracksProd         = True,
                                          useBremModel              = InDetFlags.doBremRecovery() and useBremMode, # only for NewTracking the brem is debugged !!!
                                          doCaloSeededBrem          = InDetFlags.doCaloSeededBrem(),
                                          doHadCaloSeedSSS          = InDetFlags.doHadCaloSeededSSS(),
                                          phiWidth                  = NewTrackingCuts.phiWidthBrem(),
                                          etaWidth                  = NewTrackingCuts.etaWidthBrem(),
                                          InputClusterContainerName = InDetKeys.CaloClusterROIContainer(), # "InDetCaloClusterROIs" 
                                          InputHadClusterContainerName = InDetKeys.HadCaloClusterROIContainer(), # "InDetCaloClusterROIs" 
                                          UseAssociationTool        = usePrdAssociationTool)

         if NewTrackingCuts.mode() == "SLHC" or NewTrackingCuts.mode() == "ForwardSLHCTracks" or NewTrackingCuts.mode() == "VeryForwardSLHCTracks" :
            InDetSiTrackMaker.ITKGeometry = True

         if NewTrackingCuts.mode() == "DBM":
            InDetSiTrackMaker.MagneticFieldMode = "NoField"
            InDetSiTrackMaker.useBremModel = False
            InDetSiTrackMaker.doMultiTracksProd = False
            InDetSiTrackMaker.TrackPatternRecoInfo = 'SiSPSeededFinder'			
            InDetSiTrackMaker.pTminSSS = -1
            InDetSiTrackMaker.CosmicTrack = False
            InDetSiTrackMaker.useSSSseedsFilter = False
            InDetSiTrackMaker.doCaloSeededBrem = False
            InDetSiTrackMaker.doHadCaloSeedSSS = False
            InDetSiTrackMaker.UseAssociationTool = False
					
         elif InDetFlags.doCosmics():
           InDetSiTrackMaker.TrackPatternRecoInfo = 'SiSpacePointsSeedMaker_Cosmic'
	   
         elif InDetFlags.doHeavyIon():
           InDetSiTrackMaker.TrackPatternRecoInfo = 'SiSpacePointsSeedMaker_HeavyIon'
         
         elif NewTrackingCuts.mode() == "LowPt":
           InDetSiTrackMaker.TrackPatternRecoInfo = 'SiSpacePointsSeedMaker_LowMomentum'

         elif NewTrackingCuts.mode() == "VeryLowPt" or (NewTrackingCuts.mode() == "Pixel" and InDetFlags.doMinBias()):
           InDetSiTrackMaker.TrackPatternRecoInfo = 'SiSpacePointsSeedMaker_VeryLowMomentum'           

         elif NewTrackingCuts.mode() == "BeamGas":
           InDetSiTrackMaker.TrackPatternRecoInfo = 'SiSpacePointsSeedMaker_BeamGas'
 
         elif NewTrackingCuts.mode() == "ForwardTracks":
           InDetSiTrackMaker.TrackPatternRecoInfo = 'SiSpacePointsSeedMaker_ForwardTracks'

         elif NewTrackingCuts.mode() == "ForwardSLHCTracks":
           InDetSiTrackMaker.TrackPatternRecoInfo = 'SiSpacePointsSeedMaker_ForwardSLHCTracks'

         elif NewTrackingCuts.mode() == "VeryForwardSLHCTracks": 
           InDetSiTrackMaker.TrackPatternRecoInfo = 'SiSpacePointsSeedMaker_VeryForwardSLHCTracks' 

         elif NewTrackingCuts.mode() == "SLHCConversionFinding":
           InDetSiTrackMaker.TrackPatternRecoInfo = 'SiSpacePointsSeedMaker_SLHCConversionTracks'

         elif NewTrackingCuts.mode() == "LargeD0" or NewTrackingCuts.mode() == "LowPtLargeD0":
           InDetSiTrackMaker.TrackPatternRecoInfo = 'SiSpacePointsSeedMaker_LargeD0'
        
         else:
           InDetSiTrackMaker.TrackPatternRecoInfo = 'SiSPSeededFinder'
					  
         if InDetFlags. doStoreTrackSeeds():
              InDetSiTrackMaker.SeedSegmentsWrite=True
              InDetSiTrackMaker.SeedToTrackConversion=InDet_SeedToTrackConversion
         #InDetSiTrackMaker.OutputLevel = VERBOSE				  
         ToolSvc += InDetSiTrackMaker
         if (InDetFlags.doPrintConfigurables()):
            print InDetSiTrackMaker
         #
         # set output track collection name
        #
         self.__SiTrackCollection = SiSPSeededTrackCollectionKey
         #
         # --- Setup Track finder using space points seeds
         #

         from SiSPSeededTrackFinder.SiSPSeededTrackFinderConf import InDet__SiSPSeededTrackFinder

         if NewTrackingCuts.mode() == "ForwardSLHCTracks" or NewTrackingCuts.mode() == "ForwardTracks":

          InDetSiSPSeededTrackFinder = InDet__SiSPSeededTrackFinder(name           = 'InDetSiSpTrackFinder'+NewTrackingCuts.extension(),
                                                                    TrackTool      = InDetSiTrackMaker,
                                                                    TracksLocation = self.__SiTrackCollection,
                                                                    SeedsTool      = InDetSiSpacePointsSeedMaker,
                                                                    useZvertexTool = InDetFlags.useZvertexTool(),
                                                                    ZvertexTool    = InDetZvertexMaker,
                                                                    useNewStrategy = False,
                                                                    useMBTSTimeDiff = InDetFlags.useMBTSTimeDiff(),
                                                                    useZBoundFinding = False)
          if InDetFlags.doHeavyIon() :
           InDetSiSPSeededTrackFinder.FreeClustersCut = 2 #Heavy Ion optimization from Igor
         
         else:
          InDetSiSPSeededTrackFinder = InDet__SiSPSeededTrackFinder(name           = 'InDetSiSpTrackFinder'+NewTrackingCuts.extension(),
                                                                    TrackTool      = InDetSiTrackMaker,
                                                                    TracksLocation = self.__SiTrackCollection,
                                                                    SeedsTool      = InDetSiSpacePointsSeedMaker,
                                                                    useZvertexTool = InDetFlags.useZvertexTool() and NewTrackingCuts.mode() != "DBM",
                                                                    ZvertexTool    = InDetZvertexMaker,
                                                                    useNewStrategy = InDetFlags.useNewSiSPSeededTF() and NewTrackingCuts.mode() != "DBM",
                                                                    useMBTSTimeDiff = InDetFlags.useMBTSTimeDiff(),
                                                                    useZBoundFinding = NewTrackingCuts.doZBoundary() and NewTrackingCuts.mode() != "DBM")   

          if InDetFlags.doHeavyIon() :
           InDetSiSPSeededTrackFinder.FreeClustersCut = 2 #Heavy Ion optimization from Igor

         #InDetSiSPSeededTrackFinder.OutputLevel =VERBOSE 
         topSequence += InDetSiSPSeededTrackFinder
         if (InDetFlags.doPrintConfigurables()):
            print InDetSiSPSeededTrackFinder

         if not InDetFlags.doSGDeletion():
            if InDetFlags.doTruth():
               #
               # set up the truth info for this container
               #
               include ("InDetRecExample/ConfiguredInDetTrackTruth.py")
               InDetTracksTruth = ConfiguredInDetTrackTruth(self.__SiTrackCollection,
                                                            self.__SiTrackCollection+"DetailedTruth",
                                                            self.__SiTrackCollection+"TruthCollection")
               #
               # add final output for statistics
               #
               TrackCollectionKeys      += [ InDetTracksTruth.Tracks() ]
               TrackCollectionTruthKeys += [ InDetTracksTruth.TracksTruth() ]
            else:
               TrackCollectionKeys      += [ self.__SiTrackCollection ]
               
      # ------------------------------------------------------------
      #
      # ---------- Ambiguity solving
      #
      # ------------------------------------------------------------

      if InDetFlags.doAmbiSolving():
         #
         # --- load InnerDetector TrackSelectionTool
         #
         
         prob1 = InDetFlags.pixelClusterSplitProb1()
         prob2 = InDetFlags.pixelClusterSplitProb2()
         nhitsToAllowSplitting = 9

         if geoFlags.Run() == 1:
            prob1 = InDetFlags.pixelClusterSplitProb1_run1()
            prob2 = InDetFlags.pixelClusterSplitProb2_run1() 
            nhitsToAllowSplitting = 8

         if InDetFlags.doTIDE_Ambi() and not (NewTrackingCuts.mode() == "ForwardSLHCTracks" or NewTrackingCuts.mode() == "ForwardTracks" or NewTrackingCuts.mode() == "DBM"):
           from InDetAmbiTrackSelectionTool.InDetAmbiTrackSelectionToolConf import InDet__InDetDenseEnvAmbiTrackSelectionTool as AmbiTrackSelectionTool
         else:
           from InDetAmbiTrackSelectionTool.InDetAmbiTrackSelectionToolConf import InDet__InDetAmbiTrackSelectionTool as AmbiTrackSelectionTool
         InDetAmbiTrackSelectionTool = AmbiTrackSelectionTool(name                = 'InDetAmbiTrackSelectionTool'+NewTrackingCuts.extension(),
                                                              AssociationTool     = InDetPrdAssociationTool,
                                                              DriftCircleCutTool  = InDetTRTDriftCircleCut,
                                                              minHits             = NewTrackingCuts.minClusters(),
                                                              minNotShared        = NewTrackingCuts.minSiNotShared(),
                                                              maxShared           = NewTrackingCuts.maxShared(),
                                                              minTRTHits          = 0, # used for Si only tracking !!!
                                                              sharedProbCut       = 0.10,
                                                              UseParameterization = False,
                                                              Cosmics             = InDetFlags.doCosmics(),
                                                              doPixelSplitting    = InDetFlags.doPixelClusterSplitting() and NewTrackingCuts.mode != "DBM")
         if InDetFlags.doTIDE_Ambi() and not (NewTrackingCuts.mode() == "ForwardSLHCTracks" or NewTrackingCuts.mode() == "ForwardTracks" or NewTrackingCuts.mode() == "DBM"):
           InDetAmbiTrackSelectionTool.sharedProbCut             = prob1
           InDetAmbiTrackSelectionTool.sharedProbCut2            = prob2
           InDetAmbiTrackSelectionTool.minSiHitsToAllowSplitting = nhitsToAllowSplitting
           InDetAmbiTrackSelectionTool.minUniqueSCTHits          = 4
           InDetAmbiTrackSelectionTool.minTrackChi2ForSharedHits = 3
           InDetAmbiTrackSelectionTool.InputHadClusterContainerName = InDetKeys.HadCaloClusterROIContainer()
           InDetAmbiTrackSelectionTool.doHadCaloSeed             = False   #Only split in cluster in region of interest
           InDetAmbiTrackSelectionTool.minPtSplit                = InDetFlags.pixelClusterSplitMinPt()       #Only allow split clusters on track withe pt greater than this MeV
           InDetAmbiTrackSelectionTool.phiWidth                  = 0.2     #Split cluster ROI size
           InDetAmbiTrackSelectionTool.etaWidth                  = 0.2     #Split cluster ROI size
           InDetAmbiTrackSelectionTool.InputEmClusterContainerName = InDetKeys.CaloClusterROIContainer()
           InDetAmbiTrackSelectionTool.doEmCaloSeed              = False   #Only split in cluster in region of interest
           InDetAmbiTrackSelectionTool.minPtConv                 = 10000   #Only allow split clusters on track withe pt greater than this MeV
           InDetAmbiTrackSelectionTool.phiWidthEM                = 0.05     #Split cluster ROI size
           InDetAmbiTrackSelectionTool.etaWidthEM                = 0.05     #Split cluster ROI size


         if NewTrackingCuts.mode() == "DBM":
           InDetAmbiTrackSelectionTool.Cosmics = False
           InDetAmbiTrackSelectionTool.UseParameterization   = False
           InDetAmbiTrackSelectionTool.doPixelSplitting      = False
           InDetAmbiTrackSelectionTool.maxShared             = 1000
           InDetAmbiTrackSelectionTool.maxTracksPerSharedPRD = 2
           InDetAmbiTrackSelectionTool.minHits               = 0
           InDetAmbiTrackSelectionTool.minNotShared          = 0
           InDetAmbiTrackSelectionTool.minScoreShareTracks   = 0.0
           InDetAmbiTrackSelectionTool.minTRTHits            = 0
           InDetAmbiTrackSelectionTool.sharedProbCut         = 0.1
         if InDetFlags.doTIDE_AmbiTrackMonitoring() and InDetFlags.doTIDE_Ambi() and not (NewTrackingCuts.mode() == "ForwardSLHCTracks" or NewTrackingCuts.mode() == "ForwardTracks" or NewTrackingCuts.mode() == "PixelPrdAssociation" or NewTrackingCuts.mode() == "DBM" or  NewTrackingCuts.mode() == "PixelFourLayer" or  NewTrackingCuts.mode() == "PixelThreeLayer"):
           InDetAmbiTrackSelectionTool.ObserverTool             = TrackObserverTool     #observerTool
           InDetAmbiTrackSelectionTool.MonitorAmbiguitySolving  = True
        
         # if NewTrackingCuts.mode() == "ForwardTracks":
         #    InDetAmbiTrackSelectionTool.OutputLevel = VERBOSE

         ToolSvc += InDetAmbiTrackSelectionTool
         if (InDetFlags.doPrintConfigurables()):
            print InDetAmbiTrackSelectionTool
         #
         # --- set up different Scoring Tool for collisions and cosmics
         #
         if InDetFlags.doCosmics() and NewTrackingCuts.mode() != "DBM":
            from InDetTrackScoringTools.InDetTrackScoringToolsConf import InDet__InDetCosmicScoringTool
            InDetAmbiScoringTool = InDet__InDetCosmicScoringTool(name                 = 'InDetCosmicsScoringTool'+NewTrackingCuts.extension(),
                                                                 nWeightedClustersMin = NewTrackingCuts.nWeightedClustersMin(),
                                                                 minTRTHits           = 0,
                                                                 SummaryTool          = InDetTrackSummaryTool)
         else:
            from InDetTrackScoringTools.InDetTrackScoringToolsConf import InDet__InDetAmbiScoringTool
            InDetAmbiScoringTool = InDet__InDetAmbiScoringTool(name                    = 'InDetAmbiScoringTool'+NewTrackingCuts.extension(),
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
                                                               InputEmClusterContainerName = InDetKeys.CaloClusterROIContainer(),
                                                               doEmCaloSeed            = True and InDetFlags.doCaloSeededBrem(),
                                                               minTRTonTrk             = 0,
                                                               minTRTPrecisionFraction = 0);
            # allow for some overlap for low-pt tracking
            #if InDetFlags.doLowPt() and not NewTrackingCuts.mode() == "LowPt":
            #   InDetAmbiScoringTool.minPt = NewTrackingCuts.minPT()-100.*Units.MeV

         # if NewTrackingCuts.mode() == "ForwardTracks":
         #   InDetAmbiScoringTool.OutputLevel = VERBOSE   

         ToolSvc += InDetAmbiScoringTool
         if (InDetFlags.doPrintConfigurables()):
            print InDetAmbiScoringTool
         #
         # --- load Ambiguity Processor
         #
         useBremMode = NewTrackingCuts.mode() == "Offline" or NewTrackingCuts.mode() == "SLHC"

         if InDetFlags.doTIDE_Ambi() and not (NewTrackingCuts.mode() == "ForwardSLHCTracks" or NewTrackingCuts.mode() == "ForwardTracks" or NewTrackingCuts.mode() == "DBM"):
           from TrkAmbiguityProcessor.TrkAmbiguityProcessorConf import Trk__DenseEnvironmentsAmbiguityProcessorTool as ProcessorTool
           use_low_pt_fitter =  True if NewTrackingCuts.mode() == "LowPt" or NewTrackingCuts.mode() == "VeryLowPt" or (NewTrackingCuts.mode() == "Pixel" and InDetFlags.doMinBias()) else False
           fitter_list=[( InDetTrackFitter if not use_low_pt_fitter else InDetTrackFitterLowPt )]
           if InDetFlags.doRefitInvalidCov() :
              from AthenaCommon import CfgGetter
              fitter_list.append(CfgGetter.getPublicTool('KalmanFitter'))
              fitter_list.append(CfgGetter.getPublicTool('ReferenceKalmanFitter'))

           InDetAmbiguityProcessor = ProcessorTool(name               = 'InDetAmbiguityProcessor'+NewTrackingCuts.extension(),
	                                                 Fitter             = fitter_list ,
	                                                 ScoringTool        = InDetAmbiScoringTool,
	                                                 SelectionTool      = InDetAmbiTrackSelectionTool,
	                                                 SuppressHoleSearch = False,
	                                                 tryBremFit         = InDetFlags.doBremRecovery() and useBremMode and NewTrackingCuts.mode() != "DBM",
	                                                 caloSeededBrem     = InDetFlags.doCaloSeededBrem() and NewTrackingCuts.mode() != "DBM",
	                                                 pTminBrem          = NewTrackingCuts.minPTBrem(),
	                                                 RefitPrds          = True, 
	                                                 RejectTracksWithInvalidCov=InDetFlags.doRejectInvalidCov())
         else:
           from TrkAmbiguityProcessor.TrkAmbiguityProcessorConf import Trk__SimpleAmbiguityProcessorTool as ProcessorTool
           InDetAmbiguityProcessor = ProcessorTool(name               = 'InDetAmbiguityProcessor'+NewTrackingCuts.extension(),
                                                 Fitter             = InDetTrackFitter,
                                                 ScoringTool        = InDetAmbiScoringTool,
                                                 SelectionTool      = InDetAmbiTrackSelectionTool,
                                                 SuppressHoleSearch = False,
                                                 tryBremFit         = InDetFlags.doBremRecovery() and useBremMode and NewTrackingCuts.mode() != "DBM",
                                                 caloSeededBrem     = InDetFlags.doCaloSeededBrem() and NewTrackingCuts.mode() != "DBM",
                                                 pTminBrem          = NewTrackingCuts.minPTBrem(),
                                                 RefitPrds          = True)
	
         if InDetFlags.doTIDE_Ambi() and not (NewTrackingCuts.mode() == "ForwardSLHCTracks" or NewTrackingCuts.mode() == "ForwardTracks" or NewTrackingCuts.mode() == "DBM"):
           InDetAmbiguityProcessor.SplitProbTool             = NnPixelClusterSplitProbTool
           InDetAmbiguityProcessor.sharedProbCut             = prob1
           InDetAmbiguityProcessor.sharedProbCut2            = prob2
           InDetAmbiguityProcessor.SplitClusterAmbiguityMap  = InDetKeys.SplitClusterAmbiguityMap()
           if InDetFlags.doTIDE_RescalePixelCovariances() :
            InDetAmbiguityProcessor.applydRcorrection = True

         if NewTrackingCuts.mode() == "Pixel" or NewTrackingCuts.mode() == "DBM":
            InDetAmbiguityProcessor.SuppressHoleSearch = True
         if NewTrackingCuts.mode() == "LowPt" or NewTrackingCuts.mode() == "VeryLowPt" or (NewTrackingCuts.mode() == "Pixel" and InDetFlags.doMinBias()):
            if InDetAmbiguityProcessor.getName().find('Dense') :
               pass
            else :
               InDetAmbiguityProcessor.Fitter = InDetTrackFitterLowPt
             
         if InDetFlags.materialInteractions():
            InDetAmbiguityProcessor.MatEffects = InDetFlags.materialInteractionsType()
         else:
            InDetAmbiguityProcessor.MatEffects = 0

         # if NewTrackingCuts.mode() == "ForwardTracks":
         #    InDetAmbiguityProcessor.OutputLevel = VERBOSE
         
         if InDetFlags.doTIDE_AmbiTrackMonitoring() and InDetFlags.doTIDE_Ambi() and not (NewTrackingCuts.mode() == "ForwardSLHCTracks" or NewTrackingCuts.mode() == "ForwardTracks" or NewTrackingCuts.mode() == "PixelPrdAssociation" or NewTrackingCuts.mode() == "DBM" or  NewTrackingCuts.mode() == "PixelFourLayer" or  NewTrackingCuts.mode() == "PixelThreeLayer"):
            InDetAmbiguityProcessor.ObserverTool             = TrackObserverTool     #observerTool
            InDetAmbiguityProcessor.MonitorAmbiguitySolving  = True

         ToolSvc += InDetAmbiguityProcessor
         if (InDetFlags.doPrintConfigurables()):
            print InDetAmbiguityProcessor
         #
         # --- set input and output collection
         #
         InputTrackCollection     = self.__SiTrackCollection
         self.__SiTrackCollection = ResolvedTrackCollectionKey
         #
         # --- configure Ambiguity solver
         #
         from TrkAmbiguitySolver.TrkAmbiguitySolverConf import Trk__TrkAmbiguitySolver
         InDetAmbiguitySolver = Trk__TrkAmbiguitySolver(name               = 'InDetAmbiguitySolver'+NewTrackingCuts.extension(),
                                                        TrackInput         = [ InputTrackCollection ],
                                                        TrackOutput        = self.__SiTrackCollection,
                                                        AmbiguityProcessor = InDetAmbiguityProcessor)
         topSequence += InDetAmbiguitySolver
         if (InDetFlags.doPrintConfigurables()):
            print InDetAmbiguitySolver

         #
         # --- Delete Silicon Sp-Seeded tracks
         #
         from InDetRecExample.ConfiguredInDetSGDeletion import InDetSGDeletionAlg
         InDetSGDeletionAlg(key = SiSPSeededTrackCollectionKey)

         if ( ( NewTrackingCuts.mode() in ["Pixel", "SCT"] ) or not InDetFlags.doSGDeletion()):
            if InDetFlags.doTruth():
               #
               # set up the truth info for this container
               #
               include ("InDetRecExample/ConfiguredInDetTrackTruth.py")
               InDetTracksTruth = ConfiguredInDetTrackTruth(self.__SiTrackCollection,
                                                            self.__SiTrackCollection+"DetailedTruth",
                                                            self.__SiTrackCollection+"TruthCollection")
               #
               # add final output for statistics
               #
               TrackCollectionKeys      += [ InDetTracksTruth.Tracks() ]
               TrackCollectionTruthKeys += [ InDetTracksTruth.TracksTruth() ]
            else:
               TrackCollectionKeys      += [ self.__SiTrackCollection ]
      
   def SiTrackCollection ( self ):
      try:
         return self.__SiTrackCollection
      except AttributeError:
         raise AttributeError("ConfiguredNewTrackingSiPattern: "\
                  " Neither InDetFlags.doSiSPSeededTrackFinder nor InDetFlags.doAmbiSolving"\
                  " are True, meaning the __SiTrackCollection data member has not been declared.")
               

