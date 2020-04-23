#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
#           Setup of precision tracking

from __future__ import print_function

from AthenaCommon.Include import include
include.block("InDetTrigRecExample/EFInDetConfig.py")
include("InDetTrigRecExample/InDetTrigRec_jobOptions.py") # this is needed to get InDetTrigFlags
#from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags

from AthenaCommon.Logging import logging 
log = logging.getLogger("EFIDTracking")

#Start using already decided naming conventions
#TODO: remap might not be needed in the end once this is consistent with FTF configuration
#def remap_signature( signature ):
#   if signature == 'electron':
#       return 'Electron'
#   else:
#        return signature

#def makeInDetPrecisionTracking( whichSignature, verifier = False, inputFTFtracks='TrigFastTrackFinder_Tracks', outputTrackPrefixName = "HLT_ID", rois = 'EMViewRoIs' ):
#  doTRTextension = False 
#  ptAlgs = [] #List containing all the precision tracking algorithms hence every new added alg has to be appended to the list


  #-----------------------------------------------------------------------------
  #                        Naming conventions





  #-----------------------------------------------------------------------------
  #                        Verifying input data for the algorithms
  
  #If run in views need to check data dependancies!
  #NOTE: this seems necessary only when PT is called from a different view than FTF otherwise causes stalls
#  if verifier:
#         verifier.DataObjects += [  ( 'InDet::PixelGangedClusterAmbiguities' , 'StoreGateSvc+TrigPixelClusterAmbiguitiesMap' ),
#                                  ( 'TrackCollection' , 'StoreGateSvc+' + inputFTFtracks ) ] 
      
  
  #-----------------------------------------------------------------------------
  #                        Pattern recognition stage
#def __init__(self, InputCollections = None, ResolvedTrackCollectionKey = None, SiSPSeededTrackCollectionKey = None , NewTrackingCuts = None, TrackCollectionKeys=[] , TrackCollectionTruthKeys=[]):

#Start using already decided naming conventions
#TODO: remap might not be needed in the end once this is consistent with FTF configuration
#TODO unify with PT remap
#def remap_signature( signature ):
#   if signature == 'electron':
#       return 'Electron'
#   else:
#        return signature


def makeInDetPatternRecognition( whichSignature, rois = 'EMViewRoIs', InputCollections=None,  NewTrackingCuts = None ):
      from InDetRecExample.InDetJobProperties import InDetFlags
      #Global keys/names for collections 
      from InDetRecExample.InDetKeys          import InDetKeys #FIXME: to be replaced with Trig keys?
      from .InDetTrigCollectionKeys import  TrigPixelKeys, TrigSCTKeys

      from AthenaCommon.DetFlags import DetFlags

      from AthenaCommon.AppMgr import ToolSvc

      prefix     = 'InDetTrigMT'
      suffix =  '_%s'%whichSignature if whichSignature else '' 
      #Final output track collection
      #SiSPSeededTrackCollectionKey = "SiSPSeededTracks_%s"%whichSignature 
      outEFIDTracks             = "HLT_IDTrkTrack_%s_%s"         %( whichSignature, 'EFID')
      outEFIDTrackParticles     = "HLT_IDTrack_%s_%s"            %( whichSignature, 'EFID')



      viewAlgs = [] #list of all algs running in this module
      #
      # --- decide if use the association tool
      #
      #FIXME: Make the same decision as offline (based on the tracking cuts)?
      #if (len(InputCollections) > 0) and (NewTrackingCuts.mode() == "LowPt" or NewTrackingCuts.mode() == "VeryLowPt" or NewTrackingCuts.mode() == "LargeD0" or NewTrackingCuts.mode() == "LowPtLargeD0" or NewTrackingCuts.mode() == "BeamGas" or NewTrackingCuts.mode() == "ForwardTracks" or NewTrackingCuts.mode() == "ForwardSLHCTracks"  or NewTrackingCuts.mode() == "Disappearing" or NewTrackingCuts.mode() == "VeryForwardSLHCTracks" or NewTrackingCuts.mode() == "SLHCConversionFinding"):

      #Why not use association tool? what are the cases when not needed?
      #usePrdAssociationTool = True
      #else:
      usePrdAssociationTool = False


      import InDetRecExample.TrackingCommon as TrackingCommon
      if usePrdAssociationTool:
         print ('Running SiSPseedTrackFinder!')
         #FIXME: switch to naming based on tracking

         #FIXME: If so:
         # 1] Get a new association tool
         #associationTool = TrackingCommon.getInDetTrigPRDtoTrackMapToolGangedPixels(),

         # 2] Use the ganged pixel from here?
         #from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigPrdAssociationTool

         # 3] Create the new one as in offline tracking:
         InDetTrigPrdAssociation = TrackingCommon.getInDetTrackPRD_Association(namePrefix     = prefix,
                                                                               nameSuffix     = suffix,
                                                                               TracksName     = list(InputCollections))#This is readHandle #What input collection Thought there are no tracks at this point??! 
         # 4] if so do I use normal or ganged?
         #from InDetAssociationTools.InDetAssociationToolsConf import InDet__InDetPRD_AssociationToolGangedPixels
         #InDetTrigPrdAssociationl = InDet__InDetPRD_AssociationToolGangedPixels(name = "%sPrdAssociationTool%s"%(prefix,suffix),
         #                                                                          PixelClusterAmbiguitiesMapName = TrigPixelKeys.PRDtoTrackMap )
         viewAlgs.append( InDetTrigPrdAssociation )


      #FIXME? use trigger flags?
      #if InDetFlags.doSiSPSeededTrackFinder():
      doSiSPSeededTrackFinder = True
      if doSiSPSeededTrackFinder:
         print ('Running SiSPseedTrackFinder!')

         #FIXME: Need to add different options based on the 
         from SiSpacePointsSeedTool_xk.SiSpacePointsSeedTool_xkConf import InDet__SiSpacePointsSeedMaker_ATLxk as SiSpacePointsSeedMaker


         InDetSiSpacePointsSeedMaker = SiSpacePointsSeedMaker (      name                   = "%sInDetSpSeedsMaker%s"%(prefix, suffix), #+NewTrackingCuts.extension(),
                                                                     pTmin                  = NewTrackingCuts.minPT(),
                                                                     maxdImpact             = NewTrackingCuts.maxPrimaryImpact(),
                                                                     maxZ                   = NewTrackingCuts.maxZImpact(),
                                                                     minZ                   = -NewTrackingCuts.maxZImpact(),
                                                                     usePixel               = NewTrackingCuts.usePixel(),
                                                                     SpacePointsPixelName   = TrigPixelKeys.SpacePoints,
                                                                     # useSCT                 = NewTrackingCuts.useSCT(),
                                                                     useSCT                 = (NewTrackingCuts.useSCT() and NewTrackingCuts.useSCTSeeding()),
                                                                     SpacePointsSCTName     = TrigSCTKeys.SpacePoints,
                                                                     # useOverlapSpCollection = NewTrackingCuts.useSCT(),
                                                                     useOverlapSpCollection = (NewTrackingCuts.useSCT() and NewTrackingCuts.useSCTSeeding()),
                                                                     SpacePointsOverlapName = InDetKeys.OverlapSpacePoints(),
                                                                     radMax                 = NewTrackingCuts.radMax(),
                                                                     RapidityCut            = NewTrackingCuts.maxEta())

         ToolSvc += InDetSiSpacePointsSeedMaker
         #FIXME consider specific settings here
         #if NewTrackingCuts.mode() == "Offline" or InDetFlags.doHeavyIon() or  NewTrackingCuts.mode() == "ForwardTracks":
         #        InDetSiSpacePointsSeedMaker.maxdImpactPPS = NewTrackingCuts.maxdImpactPPSSeeds()
         #        InDetSiSpacePointsSeedMaker.maxdImpactSSS = NewTrackingCuts.maxdImpactSSSSeeds()

         if usePrdAssociationTool:
         #   # not all classes have that property !!!
            InDetSiSpacePointsSeedMaker.PRDtoTrackMap      = TrigPixelKeys.PRDtoTrackMap #InDetTrigPrdAssociationTool

         #if not InDetFlags.doCosmics():
         #   InDetSiSpacePointsSeedMaker.maxRadius1         = 0.75*NewTrackingCuts.radMax()
         #   InDetSiSpacePointsSeedMaker.maxRadius2         = NewTrackingCuts.radMax()
         #   InDetSiSpacePointsSeedMaker.maxRadius3         = NewTrackingCuts.radMax()
         #if NewTrackingCuts.mode() == "LowPt" or NewTrackingCuts.mode() == "VeryLowPt" or (NewTrackingCuts.mode() == "Pixel" and InDetFlags.doMinBias()):
         #   try :
         #      InDetSiSpacePointsSeedMaker.pTmax              = NewTrackingCuts.maxPT()
         #   except:
         #      pass 
         #   InDetSiSpacePointsSeedMaker.mindRadius         = 4.0
         #if NewTrackingCuts.mode() == "SLHC" or NewTrackingCuts.mode() == "SLHCConversionFinding":
         #   InDetSiSpacePointsSeedMaker.minRadius1         = 0
         #   InDetSiSpacePointsSeedMaker.minRadius2         = 0
         #   InDetSiSpacePointsSeedMaker.minRadius3         = 0
         #   InDetSiSpacePointsSeedMaker.maxRadius1         =1000.*Units.mm
         #   InDetSiSpacePointsSeedMaker.maxRadius2         =1000.*Units.mm
         #   InDetSiSpacePointsSeedMaker.maxRadius3         =1000.*Units.mm
         #if NewTrackingCuts.mode() == "ForwardTracks" or NewTrackingCuts.mode() == "ForwardSLHCTracks" or NewTrackingCuts.mode() == "VeryForwardSLHCTracks":
         #   InDetSiSpacePointsSeedMaker.checkEta           = True
         #   InDetSiSpacePointsSeedMaker.etaMin             = NewTrackingCuts.minEta()
         #   InDetSiSpacePointsSeedMaker.etaMax             = NewTrackingCuts.maxEta()
         #   InDetSiSpacePointsSeedMaker.RapidityCut        = NewTrackingCuts.maxEta()
         #if NewTrackingCuts.mode() == "DBM":
         #   InDetSiSpacePointsSeedMaker.etaMin             = NewTrackingCuts.minEta()
         #   InDetSiSpacePointsSeedMaker.etaMax             = NewTrackingCuts.maxEta()
         #   InDetSiSpacePointsSeedMaker.useDBM = True
 


         #Z finder for the vtx
         #
         # --- Z-coordinates primary vertices finder (only for collisions)
         #
         if InDetFlags.useZvertexTool() and NewTrackingCuts.mode() != "DBM":
            from SiZvertexTool_xk.SiZvertexTool_xkConf import InDet__SiZvertexMaker_xk
            InDetZvertexMaker = InDet__SiZvertexMaker_xk(name          = '%sInDetZvertexMaker%s'%(prefix,suffix),
                                                         Zmax          = NewTrackingCuts.maxZImpact(),
                                                         Zmin          = -NewTrackingCuts.maxZImpact(),
                                                         minRatio      = 0.17) # not default
            InDetZvertexMaker.SeedMakerTool = InDetSiSpacePointsSeedMaker

            if InDetFlags.doHeavyIon():
               InDetZvertexMaker.HistSize = 2000
               ###InDetZvertexMaker.minContent = 200 
               InDetZvertexMaker.minContent = 30
               
            ToolSvc += InDetZvertexMaker
            #if (InDetFlags.doPrintConfigurables()):
            #   printfunc (InDetZvertexMaker)

         else:
            InDetZvertexMaker = None

         #
      #   # --- SCT and Pixel detector elements road builder
      #   #
         #FIXME which propagator
         # 1] 
         from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigPropagator

         # 2] How about offline

         from SiDetElementsRoadTool_xk.SiDetElementsRoadTool_xkConf import InDet__SiDetElementsRoadMaker_xk
         InDetSiDetElementsRoadMaker = InDet__SiDetElementsRoadMaker_xk(name               = '%sInDetSiRoadMaker%s'%(prefix,suffix),#+NewTrackingCuts.extension(),
                                                                        PropagatorTool     = InDetTrigPropagator,#InDetPatternPropagator,
                                                                        usePixel           = NewTrackingCuts.usePixel(),
                                                                        PixManagerLocation = InDetKeys.PixelManager(),
                                                                        useSCT             = NewTrackingCuts.useSCT(), 
                                                                        SCTManagerLocation = InDetKeys.SCT_Manager(), #FIXME change the name?        
                                                                        RoadWidth          = NewTrackingCuts.RoadWidth())

         ToolSvc += InDetSiDetElementsRoadMaker
         #if (InDetFlags.doPrintConfigurables()):
         #   printfunc (     InDetSiDetElementsRoadMaker)
         # Condition algorithm for InDet__SiDetElementsRoadMaker_xk
         if DetFlags.haveRIO.pixel_on():
         #FIXME:
         #pixelOn = True
         #if pixelOn:
             # Condition algorithm for SiCombinatorialTrackFinder_xk
            from AthenaCommon.AlgSequence import AthSequencer
            condSeq = AthSequencer("AthCondSeq")
            if not hasattr(condSeq, "InDetSiDetElementBoundaryLinksPixelCondAlg"):
                from SiCombinatorialTrackFinderTool_xk.SiCombinatorialTrackFinderTool_xkConf import InDet__SiDetElementBoundaryLinksCondAlg_xk
                condSeq += InDet__SiDetElementBoundaryLinksCondAlg_xk(name     = "InDetSiDetElementBoundaryLinksPixelCondAlg",
                                                                      ReadKey  = "PixelDetectorElementCollection",
                                                                      WriteKey = "PixelDetElementBoundaryLinks_xk",
                                                                      UsePixelDetectorManager = True)

         if NewTrackingCuts.useSCT():
            from AthenaCommon.AlgSequence import AthSequencer
            condSeq = AthSequencer("AthCondSeq")
            if not hasattr(condSeq, "InDet__SiDetElementsRoadCondAlg_xk"):
               from SiDetElementsRoadTool_xk.SiDetElementsRoadTool_xkConf import InDet__SiDetElementsRoadCondAlg_xk
               condSeq += InDet__SiDetElementsRoadCondAlg_xk(name = "InDet__SiDetElementsRoadCondAlg_xk")

            if not hasattr(condSeq, "InDetSiDetElementBoundaryLinksSCTCondAlg"):
               from SiCombinatorialTrackFinderTool_xk.SiCombinatorialTrackFinderTool_xkConf import InDet__SiDetElementBoundaryLinksCondAlg_xk
               condSeq += InDet__SiDetElementBoundaryLinksCondAlg_xk(name = "InDetSiDetElementBoundaryLinksSCTCondAlg",
                                                                     ReadKey = "SCT_DetectorElementCollection",
                                                                     WriteKey = "SCT_DetElementBoundaryLinks_xk")


      #   #
      #   # --- Local track finding using sdCaloSeededSSSpace point seed
      #   #
      #   # @TODO ensure that PRD association map is used if usePrdAssociationTool is set
         is_dbm = InDetFlags.doDBMstandalone() or NewTrackingCuts.extension()=='DBM'
         rot_creator_digital = TrackingCommon.getInDetRotCreatorDigital() if not is_dbm else TrackingCommon.getInDetRotCreatorDBM()


         from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigSCTConditionsSummaryTool, InDetTrigPatternUpdator

         from SiCombinatorialTrackFinderTool_xk.SiCombinatorialTrackFinderTool_xkConf import InDet__SiCombinatorialTrackFinder_xk
         track_finder = InDet__SiCombinatorialTrackFinder_xk(name                  = '%sInDetSiComTrackFinder%s'%(prefix,suffix),#+NewTrackingCuts.extension(),
                                                             PropagatorTool        = InDetTrigPropagator,#InDetPatternPropagator,
                                                             UpdatorTool           = InDetTrigPatternUpdator,#InDetPatternUpdator,
                                                             SctSummaryTool        = InDetTrigSCTConditionsSummaryTool,
                                                             RIOonTrackTool        = rot_creator_digital,
                                                             usePixel              = DetFlags.haveRIO.pixel_on(),
                                                             useSCT                = DetFlags.haveRIO.SCT_on() if not is_dbm else False,
                                                             PixelClusterContainer = TrigPixelKeys.Clusters,#InDetKeys.PixelClusters(),
                                                             SCT_ClusterContainer  = TrigSCTKeys.Clusters )#InDetKeys.SCT_Clusters())

         ToolSvc += track_finder
         if is_dbm :
            track_finder.MagneticFieldMode     = "NoField"
            track_finder.TrackQualityCut       = 9.3

         
         #track_finder.SctSummaryTool = InDetTrigSCTConditionsSummaryTool
         #if (DetFlags.haveRIO.SCT_on()):
         #   track_finder.SctSummaryTool = InDetSCT_ConditionsSummaryTool
         #else:
         #   track_finder.SctSummaryTool = None

      #   ToolSvc += track_finder

         useBremMode = NewTrackingCuts.mode() == "Offline" or NewTrackingCuts.mode() == "SLHC" or NewTrackingCuts.mode() == "DBM"
         from SiTrackMakerTool_xk.SiTrackMakerTool_xkConf import InDet__SiTrackMaker_xk as SiTrackMaker
         InDetSiTrackMaker = SiTrackMaker(name                          = '%sInDetSiTrackMaker%s'%(prefix,suffix),#+NewTrackingCuts.extension(),
                                          useSCT                        = NewTrackingCuts.useSCT(),
                                          usePixel                      = NewTrackingCuts.usePixel(),
                                          RoadTool                      = InDetSiDetElementsRoadMaker,
                                          CombinatorialTrackFinder      = track_finder,
                                          pTmin                         = NewTrackingCuts.minPT(),
                                          pTminBrem                     = NewTrackingCuts.minPTBrem(),
                                          pTminSSS                      = InDetFlags.pT_SSScut(),
                                          nClustersMin                  = NewTrackingCuts.minClusters(),
                                          nHolesMax                     = NewTrackingCuts.nHolesMax(),
                                          nHolesGapMax                  = NewTrackingCuts.nHolesGapMax(),
                                          SeedsFilterLevel              = NewTrackingCuts.seedFilterLevel(),
                                          Xi2max                        = NewTrackingCuts.Xi2max(),
                                          Xi2maxNoAdd                   = NewTrackingCuts.Xi2maxNoAdd(),
                                          nWeightedClustersMin          = NewTrackingCuts.nWeightedClustersMin(),
                                          CosmicTrack                   = InDetFlags.doCosmics(),
                                          Xi2maxMultiTracks             = NewTrackingCuts.Xi2max(), # was 3.
                                          useSSSseedsFilter             = InDetFlags.doSSSfilter(),
                                          doMultiTracksProd             = True,
                                          useBremModel                  = InDetFlags.doBremRecovery() and useBremMode, # only for NewTracking the brem is debugged !!!
                                          doCaloSeededBrem              = InDetFlags.doCaloSeededBrem(),
                                          doHadCaloSeedSSS              = InDetFlags.doHadCaloSeededSSS(),
                                          phiWidth                      = NewTrackingCuts.phiWidthBrem(),
                                          etaWidth                      = NewTrackingCuts.etaWidthBrem(),
                                          InputClusterContainerName     = InDetKeys.CaloClusterROIContainer(), # "InDetCaloClusterROIs" 
                                          InputHadClusterContainerName  = InDetKeys.HadCaloClusterROIContainer(), # "InDetCaloClusterROIs" 
                                          UseAssociationTool            = usePrdAssociationTool)

         ToolSvc += InDetSiTrackMaker

      #FIXME: do only cosmics for now, but change later
      #   if NewTrackingCuts.mode() == "SLHC" or NewTrackingCuts.mode() == "ForwardSLHCTracks" or NewTrackingCuts.mode() == "VeryForwardSLHCTracks" :
      #      InDetSiTrackMaker.ITKGeometry = True

      #   if NewTrackingCuts.mode() == "DBM":
      #      InDetSiTrackMaker.MagneticFieldMode = "NoField"
      #      InDetSiTrackMaker.useBremModel = False
      #      InDetSiTrackMaker.doMultiTracksProd = False
      #      InDetSiTrackMaker.TrackPatternRecoInfo = 'SiSPSeededFinder'       
      #      InDetSiTrackMaker.pTminSSS = -1
      #      InDetSiTrackMaker.CosmicTrack = False
      #      InDetSiTrackMaker.useSSSseedsFilter = False
      #      InDetSiTrackMaker.doCaloSeededBrem = False
      #      InDetSiTrackMaker.doHadCaloSeedSSS = False

         #elif InDetFlags.doCosmics():
         #  InDetSiTrackMaker.TrackPatternRecoInfo = 'SiSpacePointsSeedMaker_Cosmic'
         InDetSiTrackMaker.TrackPatternRecoInfo = 'SiSpacePointsSeedMaker_Cosmic'
      #
      #   elif InDetFlags.doHeavyIon():
      #     InDetSiTrackMaker.TrackPatternRecoInfo = 'SiSpacePointsSeedMaker_HeavyIon'
      #   
      #   elif NewTrackingCuts.mode() == "LowPt":
      #     InDetSiTrackMaker.TrackPatternRecoInfo = 'SiSpacePointsSeedMaker_LowMomentum'

      #   elif NewTrackingCuts.mode() == "VeryLowPt" or (NewTrackingCuts.mode() == "Pixel" and InDetFlags.doMinBias()):
      #     InDetSiTrackMaker.TrackPatternRecoInfo = 'SiSpacePointsSeedMaker_VeryLowMomentum'           

      #   elif NewTrackingCuts.mode() == "BeamGas":
      #     InDetSiTrackMaker.TrackPatternRecoInfo = 'SiSpacePointsSeedMaker_BeamGas'
 
      #   elif NewTrackingCuts.mode() == "ForwardTracks":
      #     InDetSiTrackMaker.TrackPatternRecoInfo = 'SiSpacePointsSeedMaker_ForwardTracks'

      #   elif NewTrackingCuts.mode() == "ForwardSLHCTracks":
      #     InDetSiTrackMaker.TrackPatternRecoInfo = 'SiSpacePointsSeedMaker_ForwardSLHCTracks'

      #   elif NewTrackingCuts.mode() == "VeryForwardSLHCTracks": 
      #     InDetSiTrackMaker.TrackPatternRecoInfo = 'SiSpacePointsSeedMaker_VeryForwardSLHCTracks' 

      #   elif NewTrackingCuts.mode() == "SLHCConversionFinding":
      #     InDetSiTrackMaker.TrackPatternRecoInfo = 'SiSpacePointsSeedMaker_SLHCConversionTracks'

      #   elif NewTrackingCuts.mode() == "LargeD0" or NewTrackingCuts.mode() == "LowPtLargeD0":
      #     InDetSiTrackMaker.TrackPatternRecoInfo = 'SiSpacePointsSeedMaker_LargeD0'
      #  
      #   else:
      #     InDetSiTrackMaker.TrackPatternRecoInfo = 'SiSPSeededFinder'
      #           
         if InDetFlags.doStoreTrackSeeds():
              from SeedToTrackConversionTool.SeedToTrackConversionToolConf import InDet__SeedToTrackConversionTool
              InDet_SeedToTrackConversion = InDet__SeedToTrackConversionTool(name       = "%sInDet_SeedToTrackConversion%s"%(prefix, suffix),
                                                                             OutputName = InDetKeys.SiSPSeedSegments()+NewTrackingCuts.extension())
              InDetSiTrackMaker.SeedToTrackConversion = InDet_SeedToTrackConversion
              InDetSiTrackMaker.SeedSegmentsWrite = True

      #   #InDetSiTrackMaker.OutputLevel = VERBOSE             
      #   ToolSvc += InDetSiTrackMaker
      #   if (InDetFlags.doPrintConfigurables()):
      #      printfunc (InDetSiTrackMaker)
      #   #
      #   # set output track collection name
      #  #
      #   self.__SiTrackCollection = SiSPSeededTrackCollectionKey
      #   #
      #   # --- Setup Track finder using space points seeds
      #   #

         #FIXME: which summary tool to use??
         # trackSummaryTool = TrackingCommon.getInDetTrackSummaryToolNoHoleSearch()
         #ToolSvc += trackSummaryTool
         from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackSummaryTool


         from SiSPSeededTrackFinder.SiSPSeededTrackFinderConf import InDet__SiSPSeededTrackFinder
         InDetSiSPSeededTrackFinder = InDet__SiSPSeededTrackFinder(name                 = '%sInDetSiSpTrackFinder%s'%(prefix, suffix),# +NewTrackingCuts.extension(),
                                                                    TrackTool           = InDetSiTrackMaker,
                                                                    PRDtoTrackMap       = TrigPixelKeys.PRDtoTrackMap if usePrdAssociationTool else '',
                                                                    SpacePointsPixelName= TrigPixelKeys.SpacePoints,
                                                                    SpacePointsSCTName  = TrigSCTKeys.SpacePoints,
                                                                    TrackSummaryTool    = InDetTrigTrackSummaryTool,
                                                                    TracksLocation      = outEFIDTracks,
                                                                    SeedsTool           = InDetSiSpacePointsSeedMaker,
                                                                    useZvertexTool      = InDetFlags.useZvertexTool(),
                                                                    ZvertexTool         = InDetZvertexMaker )

         if NewTrackingCuts.mode() == "ForwardSLHCTracks" or NewTrackingCuts.mode() == "ForwardTracks":
                                                                    InDetSiSPSeededTrackFinder.useNewStrategy     = False
                                                                    InDetSiSPSeededTrackFinder.useMBTSTimeDiff    = InDetFlags.useMBTSTimeDiff()
                                                                    InDetSiSPSeededTrackFinder.useZBoundFinding   = False
      #    if InDetFlags.doHeavyIon() :
      #     InDetSiSPSeededTrackFinder.FreeClustersCut = 2 #Heavy Ion optimization from Igor
         else:
                                                                    InDetSiSPSeededTrackFinder.useZvertexTool      = InDetFlags.useZvertexTool() and NewTrackingCuts.mode() != "DBM"
                                                                    InDetSiSPSeededTrackFinder.useNewStrategy      = InDetFlags.useNewSiSPSeededTF() and NewTrackingCuts.mode() != "DBM"
                                                                    InDetSiSPSeededTrackFinder.useMBTSTimeDiff     = InDetFlags.useMBTSTimeDiff()
                                                                    InDetSiSPSeededTrackFinder.useZBoundFinding    = NewTrackingCuts.doZBoundary() and NewTrackingCuts.mode() != "DBM"   



         
         viewAlgs.append( InDetSiSPSeededTrackFinder )
         
         #for alg in viewAlgs:
         #   print alg

      #    if InDetFlags.doHeavyIon() :
      #     InDetSiSPSeededTrackFinder.FreeClustersCut = 2 #Heavy Ion optimization from Igor

      #   #InDetSiSPSeededTrackFinder.OutputLevel =VERBOSE 
      #   topSequence += InDetSiSPSeededTrackFinder
      #   if (InDetFlags.doPrintConfigurables()):
      #      printfunc (InDetSiSPSeededTrackFinder)

      #   if not InDetFlags.doSGDeletion():
      #      if InDetFlags.doTruth():
      #         #
      #         # set up the truth info for this container
      #         #
      #         include ("InDetRecExample/ConfiguredInDetTrackTruth.py")
      #         InDetTracksTruth = ConfiguredInDetTrackTruth(self.__SiTrackCollection,
      #                                                      self.__SiTrackCollection+"DetailedTruth",
      #                                                      self.__SiTrackCollection+"TruthCollection")
      #         #
      #         # add final output for statistics
      #         #
      #         TrackCollectionKeys      += [ InDetTracksTruth.Tracks() ]
      #         TrackCollectionTruthKeys += [ InDetTracksTruth.TracksTruth() ]
      #      else:
      #         TrackCollectionKeys      += [ self.__SiTrackCollection ]
      #         


      #Convert final track collection to xAOD track particles
      from .InDetTrigCommon import getTrackParticleCnv  
      viewAlgs.append( getTrackParticleCnv( prefix, suffix + "_EFID", outEFIDTracks, outEFIDTrackParticles ) )


      #print viewAlgs
      #print 'VIEWS!', len(viewAlgs)
      #print(len(viewAlgs))

      return  viewAlgs

  #-----------------------------------------------------------------------------
  #                        Ambiguity solving stage
      #TODO:

