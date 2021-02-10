# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Blocking the include for after first inclusion
include.block ('InDetRecExample/ConfiguredNewTrackingSiPattern.py')

# ------------------------------------------------------------
#
# ----------- Setup Si Pattern for New tracking
#
# ------------------------------------------------------------

class  ConfiguredNewTrackingSiPattern:

   def __init__(self, InputCollections = None, ResolvedTrackCollectionKey = None, SiSPSeededTrackCollectionKey = None , NewTrackingCuts = None, TrackCollectionKeys=[] , TrackCollectionTruthKeys=[]):
      
      from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags as commonGeoFlags
      from InDetRecExample.InDetJobProperties import InDetFlags
      from InDetRecExample.InDetKeys          import InDetKeys
      #
      # --- get ToolSvc and topSequence
      #
      from AthenaCommon.AppMgr import ServiceMgr as svcMgr
      from AthenaCommon.AppMgr                import ToolSvc
      from AthenaCommon.AlgSequence           import AlgSequence
      topSequence = AlgSequence()


      #
      # --- decide if use the association tool
      #
      if (len(InputCollections) > 0) and (NewTrackingCuts.mode() == "LowPt" or NewTrackingCuts.mode() == "VeryLowPt" or NewTrackingCuts.mode() == "LowPtRoI" or NewTrackingCuts.mode() == "LargeD0" or NewTrackingCuts.mode() == "R3LargeD0" or NewTrackingCuts.mode() == "SLHCLargeD0" or NewTrackingCuts.mode() == "LowPtLargeD0" or NewTrackingCuts.mode() == "DisplacedSoftPion" or NewTrackingCuts.mode() == "PixelThreeLayer" or NewTrackingCuts.mode() == "BeamGas" or NewTrackingCuts.mode() == "ForwardTracks"  or NewTrackingCuts.mode() == "PixelPrdAssociation" or NewTrackingCuts.mode() == "ROIConv"):

         usePrdAssociationTool = True
      else:
         usePrdAssociationTool = False


      #
      #--------- Configure EtaDependentCuts
      #


      if InDetFlags.useEtaDependentCuts() and (NewTrackingCuts.mode() == "SLHC" or NewTrackingCuts.mode() == "SLHCLargeD0"):

         from InDetEtaDependentCuts.Configuration_EtaDependentCuts import ConfiguredEtaDependentCuts
         InDetEtaDependentCutsSvc = ConfiguredEtaDependentCuts(NewTrackingCuts)

         svcMgr += InDetEtaDependentCutsSvc

         if InDetFlags.doPrintConfigurables():
            print InDetEtaDependentCutsSvc


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
         elif NewTrackingCuts.mode() == "LowPtRoI" :
            from SiSpacePointsSeedTool_xk.SiSpacePointsSeedTool_xkConf import InDet__SiSpacePointsSeedMaker_ATLxk as SiSpacePointsSeedMaker
         elif NewTrackingCuts.mode() == "BeamGas":
            from SiSpacePointsSeedTool_xk.SiSpacePointsSeedTool_xkConf import InDet__SiSpacePointsSeedMaker_BeamGas as SiSpacePointsSeedMaker
         elif NewTrackingCuts.mode() == "SLHC" or NewTrackingCuts.mode() == "ROIConv" or NewTrackingCuts.mode() == "SLHCLargeD0":
            from SiSpacePointsSeedTool_xk.SiSpacePointsSeedTool_xkConf import InDet__SiSpacePointsSeedMaker_ITK as SiSpacePointsSeedMaker
         elif NewTrackingCuts.mode() == "DisplacedSoftPion" :
            from SiSpacePointsSeedTool_xk.SiSpacePointsSeedTool_xkConf import InDet__SiSpacePointsSeedMaker_TrkSeeded as SiSpacePointsSeedMaker
         else:
            from SiSpacePointsSeedTool_xk.SiSpacePointsSeedTool_xkConf import InDet__SiSpacePointsSeedMaker_ATLxk as SiSpacePointsSeedMaker

         InDetSiSpacePointsSeedMaker = SiSpacePointsSeedMaker (name                   = "InDetSpSeedsMaker"+NewTrackingCuts.extension(),
                                                               usePixel               = NewTrackingCuts.usePixel(),
                                                               SpacePointsPixelName   = InDetKeys.PixelSpacePoints(),
                                                               # useSCT                 = NewTrackingCuts.useSCT(),
                                                               useSCT                 = (NewTrackingCuts.useSCT() and NewTrackingCuts.useSCTSeeding()),
                                                               SpacePointsSCTName     = InDetKeys.SCT_SpacePoints(),
                                                               useOverlapSpCollection = (NewTrackingCuts.useSCT() and NewTrackingCuts.useSCTSeeding()),
                                                               SpacePointsOverlapName = InDetKeys.OverlapSpacePoints(),
                                                               radMax                 = NewTrackingCuts.radMax(),
                                                               pTmin                  =  NewTrackingCuts.minPTSeed(),
                                                               maxdImpact             =  NewTrackingCuts.maxPrimaryImpactSeed(),
                                                               maxZ                   =  NewTrackingCuts.maxZImpactSeed(),
                                                               minZ                   = -NewTrackingCuts.maxZImpactSeed(),
                                                               etaMax                 = NewTrackingCuts.maxEta())

          
         if NewTrackingCuts.mode() == "Offline" or InDetFlags.doHeavyIon() or  NewTrackingCuts.mode() == "ForwardTracks":
            InDetSiSpacePointsSeedMaker.maxdImpactPPS = NewTrackingCuts.maxdImpactPPSSeeds()
            InDetSiSpacePointsSeedMaker.maxdImpactSSS = NewTrackingCuts.maxdImpactSSSSeeds()
         if NewTrackingCuts.mode() == "R3LargeD0":
            InDetSiSpacePointsSeedMaker.usePixel = False
            InDetSiSpacePointsSeedMaker.etaMax = NewTrackingCuts.maxEta() 
         if usePrdAssociationTool:
            # not all classes have that property !!!
            InDetSiSpacePointsSeedMaker.UseAssociationTool = True
            InDetSiSpacePointsSeedMaker.AssociationTool    = InDetPrdAssociationTool
         if not (InDetFlags.doCosmics() or NewTrackingCuts.mode() == "ROIConv") and not (NewTrackingCuts.mode() == "SLHC" or NewTrackingCuts.mode() == "SLHCLargeD0"):
            InDetSiSpacePointsSeedMaker.maxRadius1         = 0.75*NewTrackingCuts.radMax()
            InDetSiSpacePointsSeedMaker.maxRadius2         = NewTrackingCuts.radMax()
            InDetSiSpacePointsSeedMaker.maxRadius3         = NewTrackingCuts.radMax()
         if NewTrackingCuts.mode() == "LowPt" or NewTrackingCuts.mode() == "VeryLowPt" or (NewTrackingCuts.mode() == "Pixel" and InDetFlags.doMinBias()):
            try :
               InDetSiSpacePointsSeedMaker.pTmax              = NewTrackingCuts.maxPT()
            except:
               pass 
            InDetSiSpacePointsSeedMaker.mindRadius         = 4.0
         if NewTrackingCuts.mode() == "LowPtRoI" :
            try :
               InDetSiSpacePointsSeedMaker.pTmax              = NewTrackingCuts.maxPT()
            except:
               pass
            InDetSiSpacePointsSeedMaker.mindRadius         = 4.0
         if NewTrackingCuts.mode() == "ForwardTracks":
            InDetSiSpacePointsSeedMaker.checkEta           = True
            InDetSiSpacePointsSeedMaker.etaMin             = NewTrackingCuts.minEta()
         if NewTrackingCuts.mode() == "DBM":
            InDetSiSpacePointsSeedMaker.etaMin             = NewTrackingCuts.minEta()
            InDetSiSpacePointsSeedMaker.useDBM = True
         if NewTrackingCuts.mode() == "PixelThreeLayer" or (InDetFlags.doImprovedPixelPrdAssociation() and NewTrackingCuts.mode() == "PixelPrdAssociation") :
            InDetSiSpacePointsSeedMaker.SkipIBLcut = True
         if NewTrackingCuts.mode() == "DisplacedSoftPion" :
            InDetSiSpacePointsSeedMaker.maxSeedsForSpacePoint = 50
            InDetSiSpacePointsSeedMaker.DeltaThetaRoISP       = 0.8
            InDetSiSpacePointsSeedMaker.DeltaPhiRoISP         = 0.8
            InDetSiSpacePointsSeedMaker.RoISeedTool           = RoISeedTool
         if NewTrackingCuts.mode() == "SLHCLargeD0":
            InDetSiSpacePointsSeedMaker.maxSeedsForSpacePoint=5
            InDetSiSpacePointsSeedMaker.isLRT=True
         if InDetFlags.doFastTracking() :
            InDetSiSpacePointsSeedMaker.useFastTracking       = True
            InDetSiSpacePointsSeedMaker.maxSeedsForSpacePoint = 3
            if NewTrackingCuts.mode() == "SLHC" :
               InDetSiSpacePointsSeedMaker.useSCT                = False
            elif NewTrackingCuts.mode() == "SLHCLargeD0":
               InDetSiSpacePointsSeedMaker.usePixel              = False
               InDetSiSpacePointsSeedMaker.etaMax = NewTrackingCuts.maxEta()

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


         # ------------------------------------------------------------
         #
         # ----------- Loading of ZWindowRoISeedTool for LowPtRoI
         #
         # ------------------------------------------------------------

         if NewTrackingCuts.mode() == "LowPtRoI" :

            # ZWindowRoI Tool needed for InDet__SiSpacePointsSeeded tool            
            if (len(InputCollections) == 0) :
               InputZWindowTracks = ""
               print "InputCollections is empty, ZWindowRoI tool needs at least one"
            else :
               InputZWindowTracks = InputCollections[0] ##list(InputCollections)
               print "InputCollections is not empty, ZWindowRoI tool will use the first track colletion in the list"
               print "Size of the track collection ",len(InputCollections)," The collection is ",InputCollections

            if InDetFlags.LowPtRoIStrategy() == 1:
               from SiSpacePointsSeedTool_xk.SiSpacePointsSeedTool_xkConf import InDet__ZWindowRoISeedTool
               ZWindowRoISeedTool = InDet__ZWindowRoISeedTool (name  = 'InDetZWindowRoISeedTool',
                                                               InputTracksCollection     = InputZWindowTracks,
                                                               LeadingMinTrackPt         = 18.*Units.GeV,
                                                               SubleadingMinTrackPt      = 12.5*Units.GeV,
                                                               TracksMaxEta              = 2.5,
                                                               TracksMaxD0               = 9999.,
                                                               MaxDeltaZTracksPair       = 1.0,
                                                               TrackZ0Window             = InDetFlags.LowPtRoIWindow() )
            elif InDetFlags.LowPtRoIStrategy() == 2:
               from SiSpacePointsSeedTool_xk.SiSpacePointsSeedTool_xkConf import InDet__TruthHSRoISeedTool
               ZWindowRoISeedTool = InDet__TruthHSRoISeedTool (name = "InDetZWindowRoISeedTool",
                                                               InputTruthEventsCollection = "TruthEvents",
                                                               TrackZ0Window             = InDetFlags.LowPtRoIWindow() )
            elif InDetFlags.LowPtRoIStrategy() == 3:
               from SiSpacePointsSeedTool_xk.SiSpacePointsSeedTool_xkConf import InDet__FileRoISeedTool
               ZWindowRoISeedTool = InDet__FileRoISeedTool (name = "InDetZWindowRoISeedTool",
                                                               InputFileName = InDetFlags.LowPtRoIFile(),
                                                               TrackZ0Window             = InDetFlags.LowPtRoIWindow() )
            ToolSvc += ZWindowRoISeedTool

            from SiSpacePointsSeedTool_xk.SiSpacePointsSeedTool_xkConf import InDet__RandomRoISeedTool
            RandomRoISeedTool = InDet__RandomRoISeedTool(name = "RandomRoISeedTool",
                                                         TrackZ0Window             = InDetFlags.LowPtRoIWindow() )
            ToolSvc += RandomRoISeedTool

            #ZWindowRoISeedTool.OutputLevel = VERBOSE
            #ServiceMgr.MessageSvc.debugLimit = 1000000
            #ServiceMgr.MessageSvc.verboseLimit = 1000000

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
         
         if commonGeoFlags.Run()=="RUN4":
            InDetSiDetElementsRoadMaker.ITkGeometry = True 

         ToolSvc += InDetSiDetElementsRoadMaker
         if (InDetFlags.doPrintConfigurables()):
            print      InDetSiDetElementsRoadMaker

         #
         # --- Local track finding using sdCaloSeededSSSpace point seed
         #

         useBremMode = NewTrackingCuts.mode() == "Offline" or NewTrackingCuts.mode() == "SLHC" or NewTrackingCuts.mode() == "DBM"
         
         if commonGeoFlags.Run()=="RUN4":
           from SiTrackMakerTool_xk.SiTrackMakerTool_xkConf import InDet__SiTrackMakerITk_xk as SiTrackMaker
         else:
           from SiTrackMakerTool_xk.SiTrackMakerTool_xkConf import InDet__SiTrackMaker_xk as SiTrackMaker
         InDetSiTrackMaker = SiTrackMaker(name                      = 'InDetSiTrackMaker'+NewTrackingCuts.extension(),
                                          useSCT                    = NewTrackingCuts.useSCT(),
                                          usePixel                  = NewTrackingCuts.usePixel(),
                                          RoadTool                  = InDetSiDetElementsRoadMaker,
                                          CombinatorialTrackFinder  = InDetSiComTrackFinder,
                                          pTminSSS                  = InDetFlags.pT_SSScut(),
                                          SeedsFilterLevel          = NewTrackingCuts.seedFilterLevel(),
                                          CosmicTrack               = InDetFlags.doCosmics(),
                                          useSSSseedsFilter         = InDetFlags.doSSSfilter(), 
                                          doMultiTracksProd         = True,
                                          useBremModel              = InDetFlags.doBremRecovery() and useBremMode, # only for NewTracking the brem is debugged !!!
                                          doCaloSeededBrem          = InDetFlags.doCaloSeededBrem(),
                                          doHadCaloSeedSSS          = InDetFlags.doHadCaloSeededSSS(),
                                          InputClusterContainerName = InDetKeys.CaloClusterROIContainer(), # "InDetCaloClusterROIs" 
                                          InputHadClusterContainerName = InDetKeys.HadCaloClusterROIContainer(), # "InDetCaloClusterROIs" 
                                          UseAssociationTool        = usePrdAssociationTool)
         if not NewTrackingCuts.useEtaDependentCuts():
            InDetSiTrackMaker.pTmin                     = NewTrackingCuts.minPT()
            InDetSiTrackMaker.pTminBrem                 = NewTrackingCuts.minPTBrem()
            InDetSiTrackMaker.Xi2max                    = NewTrackingCuts.Xi2max()
            InDetSiTrackMaker.Xi2maxNoAdd               = NewTrackingCuts.Xi2maxNoAdd()
            InDetSiTrackMaker.Xi2maxMultiTracks         = NewTrackingCuts.Xi2max()
            InDetSiTrackMaker.nClustersMin              = NewTrackingCuts.minClusters()
            InDetSiTrackMaker.nHolesMax                 = NewTrackingCuts.nHolesMax()
            InDetSiTrackMaker.nHolesGapMax              = NewTrackingCuts.nHolesGapMax()
            InDetSiTrackMaker.phiWidth                  = NewTrackingCuts.phiWidthBrem()
            InDetSiTrackMaker.etaWidth                  = NewTrackingCuts.etaWidthBrem()
            InDetSiTrackMaker.nWeightedClustersMin      = NewTrackingCuts.nWeightedClustersMin()
         else:
            InDetSiTrackMaker.etaBins                   = NewTrackingCuts.etaBins()
            InDetSiTrackMaker.pTBins                    = NewTrackingCuts.minPT()
            InDetSiTrackMaker.nClustersMin              = min(NewTrackingCuts.minClusters())
            InDetSiTrackMaker.pTmin                     = NewTrackingCuts.minPT()[0]
            InDetSiTrackMaker.pTminBrem                 = NewTrackingCuts.minPTBrem()[0]
            InDetSiTrackMaker.Xi2max                    = NewTrackingCuts.Xi2max()[0]
            InDetSiTrackMaker.Xi2maxNoAdd               = NewTrackingCuts.Xi2maxNoAdd()[0]
            InDetSiTrackMaker.Xi2maxMultiTracks         = NewTrackingCuts.Xi2max()[0]
            InDetSiTrackMaker.nHolesMax                 = NewTrackingCuts.nHolesMax()[0]
            InDetSiTrackMaker.nHolesGapMax              = NewTrackingCuts.nHolesGapMax()[0]
            InDetSiTrackMaker.phiWidth                  = NewTrackingCuts.phiWidthBrem()[0]
            InDetSiTrackMaker.etaWidth                  = NewTrackingCuts.etaWidthBrem()[0]
            InDetSiTrackMaker.nWeightedClustersMin      = NewTrackingCuts.nWeightedClustersMin()[0]
            
         if NewTrackingCuts.mode() == "PixelThreeLayer" or (InDetFlags.doImprovedPixelPrdAssociation() and NewTrackingCuts.mode() == "PixelPrdAssociation"):
            InDetSiTrackMaker.CleanSpuriousSCTClus = True

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

         elif NewTrackingCuts.mode() == "LowPtRoI":
           InDetSiTrackMaker.TrackPatternRecoInfo = 'SiSpacePointsSeedMaker_LowMomentum'

         elif NewTrackingCuts.mode() == "VeryLowPt" or (NewTrackingCuts.mode() == "Pixel" and InDetFlags.doMinBias()):
           InDetSiTrackMaker.TrackPatternRecoInfo = 'SiSpacePointsSeedMaker_VeryLowMomentum'           

         elif NewTrackingCuts.mode() == "BeamGas":
           InDetSiTrackMaker.TrackPatternRecoInfo = 'SiSpacePointsSeedMaker_BeamGas'
 
         elif NewTrackingCuts.mode() == "ForwardTracks":
           InDetSiTrackMaker.TrackPatternRecoInfo = 'SiSpacePointsSeedMaker_ForwardTracks'

         elif NewTrackingCuts.mode() == "ROIConv":
           InDetSiTrackMaker.TrackPatternRecoInfo = 'SiSpacePointsSeedMaker_ROIConvTracks'

         elif NewTrackingCuts.mode() == "LargeD0" or NewTrackingCuts.mode() == "R3LargeD0" or NewTrackingCuts.mode() == "LowPtLargeD0" or NewTrackingCuts.mode() == "SLHCLargeD0":
           InDetSiTrackMaker.TrackPatternRecoInfo = 'SiSpacePointsSeedMaker_LargeD0'

         elif NewTrackingCuts.mode() == "DisplacedSoftPion":
           InDetSiTrackMaker.TrackPatternRecoInfo = 'SiSpacePointsSeedMaker_TrkSeeded'       

         else:
           InDetSiTrackMaker.TrackPatternRecoInfo = 'SiSPSeededFinder'
           
         if InDetFlags. doStoreTrackSeeds() and not NewTrackingCuts.mode() == "LowPtRoI":
              InDetSiTrackMaker.SeedSegmentsWrite=True
              InDetSiTrackMaker.SeedToTrackConversion=InDet_SeedToTrackConversion
         if InDetFlags. doStoreTrackSeeds() and NewTrackingCuts.mode() == "LowPtRoI":
              InDetSiTrackMaker.SeedSegmentsWrite=True
              InDetSiTrackMaker.SeedToTrackConversion=InDet_SeedToTrackConversionLowPtRoI         #InDetSiTrackMaker.OutputLevel = VERBOSE				  
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

         if NewTrackingCuts.mode() == "ROIConv":

            from AthenaCommon.AppMgr import ServiceMgr
            from RegionSelector.RegSelSvcDefault import RegSelSvcDefault
            InDetRegSelSvc             = RegSelSvcDefault()
            InDetRegSelSvc.enablePixel = DetFlags.pixel_on()
            InDetRegSelSvc.enableSCT   = DetFlags.SCT_on()
            ServiceMgr += InDetRegSelSvc
            if (InDetFlags.doPrintConfigurables()):
               print InDetRegSelSvc


            InDetSiSPSeededTrackFinder = InDet__SiSPSeededTrackFinder(name           = 'InDetSiSpTrackFinder'+NewTrackingCuts.extension(),
                                                                      TrackTool      = InDetSiTrackMaker,
                                                                      TracksLocation = self.__SiTrackCollection,
                                                                      SeedsTool      = InDetSiSpacePointsSeedMaker,
                                                                      useZvertexTool = InDetFlags.useZvertexTool(),
                                                                      ZvertexTool    = InDetZvertexMaker,
                                                                      useConvSeeded = True,
                                                                      useMBTSTimeDiff = InDetFlags.useMBTSTimeDiff(),
                                                                      useZBoundFinding = True,
                                                                      RegSelSvc        = InDetRegSelSvc)


         elif NewTrackingCuts.mode() == "ForwardTracks":

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

         elif NewTrackingCuts.mode() == "LowPtRoI" :
          from SiSPSeededTrackFinder.SiSPSeededTrackFinderConf import InDet__SiSPSeededTrackFinderRoI
          InDetSiSPSeededTrackFinder = InDet__SiSPSeededTrackFinderRoI(name            = 'InDetSiSpTrackFinder'+NewTrackingCuts.extension(),
                                                                    TrackTool          = InDetSiTrackMaker,
                                                                    TracksLocation     = self.__SiTrackCollection,
                                                                    SeedsTool          = InDetSiSpacePointsSeedMaker,
                                                                    VxOutputName    = InDetKeys.xAODLowPtRoIVertexContainer(),
                                                                    ZWindowRoISeedTool = ZWindowRoISeedTool,
                                                                    RandomRoISeedTool = RandomRoISeedTool,
                                                                    RoIWidth = InDetFlags.LowPtRoIWindow())
          #InDetSiSpSeededTrackFinder.OutputLevel = DEBUG

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
          
          if commonGeoFlags.Run()=="RUN4":
              InDetSiSPSeededTrackFinder.ITKGeometry = True
              if InDetFlags.doFastTracking() and (NewTrackingCuts.mode() == "SLHC" or NewTrackingCuts.mode() == "SLHCLargeD0"):
                InDetSiSPSeededTrackFinder.doFastTracking = True
                InDetSiSPSeededTrackFinder.InDetEtaDependentCutsSvc = InDetEtaDependentCutsSvc

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

         if InDetFlags.doTIDE_Ambi() and not ( NewTrackingCuts.mode() == "ForwardTracks" or NewTrackingCuts.mode() == "DBM"):
           from InDetAmbiTrackSelectionTool.InDetAmbiTrackSelectionToolConf import InDet__InDetDenseEnvAmbiTrackSelectionTool as AmbiTrackSelectionTool
         elif not InDetFlags.doTIDE_Ambi() and (NewTrackingCuts.mode()=="SLHC" or NewTrackingCuts.mode()=="SLHCLargeD0") and not DetFlags.makeRIO.pixel_on() and DetFlags.readRIOPool.pixel_on() and DetFlags.haveRIO.pixel_on():
           from InDetAmbiTrackSelectionTool.InDetAmbiTrackSelectionToolConf import InDet__InDetDenseEnvAmbiTrackSelectionTool as AmbiTrackSelectionTool
         else:
           from InDetAmbiTrackSelectionTool.InDetAmbiTrackSelectionToolConf import InDet__InDetAmbiTrackSelectionTool as AmbiTrackSelectionTool
         InDetAmbiTrackSelectionTool = AmbiTrackSelectionTool(name                = 'InDetAmbiTrackSelectionTool'+NewTrackingCuts.extension(),
                                                              AssociationTool     = InDetPrdAssociationTool,
                                                              DriftCircleCutTool  = InDetTRTDriftCircleCut,
                                                              minTRTHits          = 0, # used for Si only tracking !!!
                                                              sharedProbCut       = 0.10,
                                                              UseParameterization = False,
                                                              Cosmics             = InDetFlags.doCosmics(),
                                                              doPixelSplitting    = InDetFlags.doPixelClusterSplitting() and NewTrackingCuts.mode != "DBM")

         if not NewTrackingCuts.useEtaDependentCuts():
           InDetAmbiTrackSelectionTool.minHits             = NewTrackingCuts.minClusters()
           InDetAmbiTrackSelectionTool.minNotShared        = NewTrackingCuts.minSiNotShared()
           InDetAmbiTrackSelectionTool.maxShared           = NewTrackingCuts.maxShared()
         else:
           InDetAmbiTrackSelectionTool.InDetEtaDependentCutsSvc = InDetEtaDependentCutsSvc

         if InDetFlags.doTIDE_Ambi() and not (NewTrackingCuts.mode() == "ForwardTracks" or NewTrackingCuts.mode() == "DBM"):
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
         if InDetFlags.doTIDE_AmbiTrackMonitoring() and InDetFlags.doTIDE_Ambi() and not (NewTrackingCuts.mode() == "ForwardTracks" or NewTrackingCuts.mode() == "PixelPrdAssociation" or NewTrackingCuts.mode() == "DBM" or  NewTrackingCuts.mode() == "PixelFourLayer" or  NewTrackingCuts.mode() == "PixelThreeLayer"):
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
                                                               maxEta                  = NewTrackingCuts.maxEta(),
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
         
         if not NewTrackingCuts.useEtaDependentCuts():
           InDetAmbiScoringTool.minPt                   = NewTrackingCuts.minPT()
           InDetAmbiScoringTool.minSiClusters           = NewTrackingCuts.minClusters()
           InDetAmbiScoringTool.minPixel                = NewTrackingCuts.minPixel()                                   
           InDetAmbiScoringTool.maxSiHoles              = NewTrackingCuts.maxHoles()
           InDetAmbiScoringTool.maxPixelHoles           = NewTrackingCuts.maxPixelHoles()
           InDetAmbiScoringTool.maxSCTHoles             = NewTrackingCuts.maxSCTHoles()
           InDetAmbiScoringTool.maxDoubleHoles          = NewTrackingCuts.maxDoubleHoles()
           InDetAmbiScoringTool.maxRPhiImp              = NewTrackingCuts.maxPrimaryImpact()
           InDetAmbiScoringTool.maxZImp                 = NewTrackingCuts.maxZImpact()
         else:
           InDetAmbiScoringTool.InDetEtaDependentCutsSvc = InDetEtaDependentCutsSvc
           InDetAmbiScoringTool.useITkAmbigFcn           = True

         ToolSvc += InDetAmbiScoringTool
         if (InDetFlags.doPrintConfigurables()):
            print InDetAmbiScoringTool
         #
         # --- load Ambiguity Processor
         #
         useBremMode = NewTrackingCuts.mode() == "Offline" or NewTrackingCuts.mode() == "SLHC" or NewTrackingCuts.mode() == "SLHCLargeD0"

         if InDetFlags.doTIDE_Ambi() and not (NewTrackingCuts.mode() == "ForwardTracks" or NewTrackingCuts.mode() == "DBM"):
           from TrkAmbiguityProcessor.TrkAmbiguityProcessorConf import Trk__DenseEnvironmentsAmbiguityProcessorTool as ProcessorTool
           use_low_pt_fitter =  True if NewTrackingCuts.mode() == "LowPt" or NewTrackingCuts.mode() == "VeryLowPt" or NewTrackingCuts.mode() == "LowPtRoI" or (NewTrackingCuts.mode() == "Pixel" and InDetFlags.doMinBias()) else False
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
                                                 RefitPrds          = True)
         if not NewTrackingCuts.useEtaDependentCuts():
           InDetAmbiguityProcessor.pTminBrem          = NewTrackingCuts.minPTBrem()
         else:
           InDetAmbiguityProcessor.pTminBrem          = NewTrackingCuts.minPTBrem()[0]
           
         if InDetFlags.doTIDE_Ambi() and not (NewTrackingCuts.mode() == "ForwardTracks" or NewTrackingCuts.mode() == "DBM"):
           InDetAmbiguityProcessor.SplitProbTool             = NnPixelClusterSplitProbTool
           InDetAmbiguityProcessor.sharedProbCut             = prob1
           InDetAmbiguityProcessor.sharedProbCut2            = prob2
           InDetAmbiguityProcessor.SplitClusterAmbiguityMap  = InDetKeys.SplitClusterAmbiguityMap()
           if InDetFlags.doTIDE_RescalePixelCovariances() :
            InDetAmbiguityProcessor.applydRcorrection = True

         if NewTrackingCuts.mode() == "Pixel" or NewTrackingCuts.mode() == "DBM":
            InDetAmbiguityProcessor.SuppressHoleSearch = True
         if NewTrackingCuts.mode() == "LowPt" or NewTrackingCuts.mode() == "VeryLowPt" or NewTrackingCuts.mode() == "LowPtRoI" or (NewTrackingCuts.mode() == "Pixel" and InDetFlags.doMinBias()):
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
         
         if InDetFlags.doTIDE_AmbiTrackMonitoring() and InDetFlags.doTIDE_Ambi() and not (NewTrackingCuts.mode() == "ForwardTracks" or NewTrackingCuts.mode() == "PixelPrdAssociation" or NewTrackingCuts.mode() == "DBM" or  NewTrackingCuts.mode() == "PixelFourLayer" or  NewTrackingCuts.mode() == "PixelThreeLayer"):
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
       
      elif InDetFlags.doFastTracking() and (NewTrackingCuts.mode() == "SLHC" or NewTrackingCuts.mode() == "SLHCLargeD0"):
        if InDetFlags.doFastTrackingFit():
          #
          # defining setup without ambiguity solving for fast tracking reconstuction
          #
          
          ### We only allow refitting of the tracks to exploit the cluster calibration constants
          from TrkAmbiguityProcessor.TrkAmbiguityProcessorConf import Trk__SimpleFitterTool as ProcessorTool
          fitter_list=[InDetTrackFitter]
          if InDetFlags.doRefitInvalidCov() :
            from AthenaCommon import CfgGetter
            fitter_list.append(CfgGetter.getPublicTool('KalmanFitter'))
            fitter_list.append(CfgGetter.getPublicTool('ReferenceKalmanFitter'))
          
          useBremMode = NewTrackingCuts.mode() == "Offline" or NewTrackingCuts.mode() == "SLHC" or NewTrackingCuts.mode() == "SLHCLargeD0"
          
          from InDetAmbiTrackSelectionTool.InDetAmbiTrackSelectionToolConf import InDet__InDetDenseEnvAmbiTrackSelectionTool as AmbiTrackSelectionTool
          
          
          SimpleFitterTool = ProcessorTool(name               = 'SimpleFitterTool'+NewTrackingCuts.extension(),
                                           Fitter             = fitter_list ,
                                           SummaryTool        = InDetTrackSummaryTool,
                                           AssociationTool    = InDetPrdAssociationTool,
                                           tryBremFit         = InDetFlags.doBremRecovery() and useBremMode and NewTrackingCuts.mode() != "DBM",
                                           caloSeededBrem     = InDetFlags.doCaloSeededBrem() and NewTrackingCuts.mode() != "DBM",
                                           pTminBrem = NewTrackingCuts.minPTBrem()[0] if NewTrackingCuts.useEtaDependentCuts() else NewTrackingCuts.minPTBrem(),
                                           RejectTracksWithInvalidCov=InDetFlags.doRejectInvalidCov())
          
          ToolSvc += SimpleFitterTool
          if (InDetFlags.doPrintConfigurables()):
             print SimpleFitterTool
          #
          # --- set input and output collection
          #
          InputTrackCollection     = self.__SiTrackCollection
          self.__SiTrackCollection = ResolvedTrackCollectionKey
          #
          # --- configure Ambiguity solver to use the simple fitter tool
          # this means it only runs the final fit w/o resolving ambiguities
          #
          from TrkAmbiguitySolver.TrkAmbiguitySolverConf import Trk__TrkAmbiguitySolver
          InDetAmbiguitySolver = Trk__TrkAmbiguitySolver(name               = 'InDetFitter'+NewTrackingCuts.extension(),
                                                         TrackInput         = [ InputTrackCollection ],
                                                         TrackOutput        = self.__SiTrackCollection,
                                                         AmbiguityProcessor = SimpleFitterTool)
          topSequence += InDetAmbiguitySolver
          if (InDetFlags.doPrintConfigurables()):
             print InDetAmbiguitySolver
          
        else: # not refitting tracks, dump out the existing track collection
          #
          # --- set input and output collection
          #
          InputTrackCollection     = self.__SiTrackCollection
          self.__SiTrackCollection = ResolvedTrackCollectionKey
          
          #
          # defining setup without ambiguity solving for fast tracking reconstuction
          #
          from TrkCollectionAliasAlg.TrkCollectionAliasAlgConf import Trk__TrkCollectionAliasAlg
          InDetCopyAlgForAmbi = Trk__TrkCollectionAliasAlg (name             = "InDetCopyAlgForAmbi"+NewTrackingCuts.extension(),
                                                            CollectionName   = InputTrackCollection,
                                                            AliasName        = ResolvedTrackCollectionKey) 
          topSequence += InDetCopyAlgForAmbi
          if (InDetFlags.doPrintConfigurables()):
             print InDetCopyAlgForAmbi
        
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
               

