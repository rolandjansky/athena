
# ------------------------------------------------------------
#
# ----------- Data-Preparation stage
#
# ------------------------------------------------------------
#
# ----------- PrepRawData creation from Raw Data Objects
#

if not 'redoPatternRecoAndTracking' in dir():
  redoPatternRecoAndTracking = False

if InDetFlags.doPRDFormation():
   #
   # --- Slim BCM RDOs by zero-suppressing
   #   
   if DetFlags.makeRIO.BCM_on():
      from BCM_ZeroSuppression.BCM_ZeroSuppressionConf import BCM_ZeroSuppression
      InDetBCM_ZeroSuppression = BCM_ZeroSuppression(name             = "InDetBCM_ZeroSuppression",
                                                     BcmContainerName = InDetKeys.BCM_RDOs())
      topSequence += InDetBCM_ZeroSuppression
      if (InDetFlags.doPrintConfigurables()):
         print InDetBCM_ZeroSuppression
   
   if DetFlags.makeRIO.pixel_on() or DetFlags.makeRIO.SCT_on():
      #
      # --- ClusterMakerTool (public), needed by Pixel and SCT Clusterization
      #
      from SiClusterizationTool.SiClusterizationToolConf import InDet__ClusterMakerTool
      InDetClusterMakerTool = InDet__ClusterMakerTool(name                 = "InDetClusterMakerTool",
                                                      PixelCalibSvc        = None,
                                                      PixelOfflineCalibSvc = None,
                                                      UsePixelCalibCondDB  = False)

      if DetFlags.makeRIO.pixel_on() and not (athenaCommonFlags.isOnline() or InDetFlags.doSLHC()):
         InDetClusterMakerTool.PixelCalibSvc        = InDetPixelCalibSvc
         InDetClusterMakerTool.PixelOfflineCalibSvc = InDetPixelOfflineCalibSvc
         InDetClusterMakerTool.UsePixelCalibCondDB  = True

      ToolSvc += InDetClusterMakerTool
      if (InDetFlags.doPrintConfigurables()):
        print InDetClusterMakerTool
      
   #
   # -- Pixel Clusterization
   #
   if (DetFlags.makeRIO.pixel_on() and InDetFlags.doPixelPRDFormation()) or redoPatternRecoAndTracking:     
      #
      # --- MergedPixelTool (public)
      #
      minSplitProbability  = 0
      minSplitSize         = 3
      clusterSplitProbTool = None
      clusterSplitterTool  = None
      #
      # --- do we use new splittig or not ?
      #
      if InDetFlags.doPixelClusterSplitting():
         #
         # --- Neutral Network version ?
         #
         if InDetFlags.pixelClusterSplittingType() == 'NeuralNet':

            useBeamConstraint = InDetFlags.useBeamConstraint()
            
            # --- new NN prob tool
            MultiplicityContent = [1 , 1 , 1]
            if InDetFlags.doSLHC():
                from SiClusterizationTool.SiClusterizationToolConf import InDet__TruthPixelClusterSplitProbTool as PixelClusterSplitProbTool
            else:
                from SiClusterizationTool.SiClusterizationToolConf import InDet__NnPixelClusterSplitProbTool as PixelClusterSplitProbTool
            NnPixelClusterSplitProbTool=PixelClusterSplitProbTool(name                     = "NnPixelClusterSplitProbTool",
                                                                           PriorMultiplicityContent = MultiplicityContent,
                                                                           NnClusterizationFactory  = NnClusterizationFactory,
                                                                           useBeamSpotInfo          = useBeamConstraint)
            ToolSvc += NnPixelClusterSplitProbTool
            if (InDetFlags.doPrintConfigurables()):
              print NnPixelClusterSplitProbTool

            # --- remember this prob tool  
            clusterSplitProbTool = NnPixelClusterSplitProbTool
            
            # --- new NN splitter
            if InDetFlags.doSLHC():
                from SiClusterizationTool.SiClusterizationToolConf import InDet__TruthPixelClusterSplitter as PixelClusterSplitter
            else:
                from SiClusterizationTool.SiClusterizationToolConf import InDet__NnPixelClusterSplitter as PixelClusterSplitter
            NnPixelClusterSplitter=PixelClusterSplitter(name                                = "NnPixelClusterSplitter",
                                                                 NnClusterizationFactory             = NnClusterizationFactory,
                                                                 ThresholdSplittingIntoTwoClusters   = 0.5, # temp.
                                                                 ThresholdSplittingIntoThreeClusters = 0.25, # temp.
                                                                 SplitOnlyOnBLayer                   = False,
                                                                 useBeamSpotInfo                     = useBeamConstraint)

            
            ToolSvc += NnPixelClusterSplitter
            if (InDetFlags.doPrintConfigurables()):
              print NnPixelClusterSplitter

            # remember splitter tool  
            clusterSplitterTool = NnPixelClusterSplitter
            

         #
         # --- Neutral Network version ?
         #
         elif InDetFlags.pixelClusterSplittingType() == 'AnalogClus':      

            # new splitter tool
            from SiClusterizationTool.SiClusterizationToolConf import InDet__TotPixelClusterSplitter
            TotPixelClusterSplitter=InDet__TotPixelClusterSplitter (name="TotPixelClusterSplitter")

            ToolSvc += TotPixelClusterSplitter
            if (InDetFlags.doPrintConfigurables()):
              print TotPixelClusterSplitter

            # remember splitter tool    
            clusterSplitterTool = TotPixelClusterSplitter
                        
      #
      # --- now load the framework for the clustering
      #
      from SiClusterizationTool.SiClusterizationToolConf import InDet__MergedPixelsTool
      InDetMergedPixelsTool = InDet__MergedPixelsTool(name                    = "InDetMergedPixelsTool", 
                                                      globalPosAlg            = InDetClusterMakerTool,
                                                      MinimalSplitSize        = 0,
                                                      MaximalSplitSize        = 49,
                                                      MinimalSplitProbability = 0,
                                                      DoIBLSplitting = True,
                                                      SplitClusterAmbiguityMap= InDetKeys.SplitClusterAmbiguityMap())
      # assign the tools if there are any                                                
      if not InDetFlags.doTIDE_Ambi() and clusterSplitProbTool is not None : InDetMergedPixelsTool.SplitProbTool   = clusterSplitProbTool
      if not InDetFlags.doTIDE_Ambi() and clusterSplitterTool is not None  : InDetMergedPixelsTool.ClusterSplitter = clusterSplitterTool

      
      ToolSvc += InDetMergedPixelsTool
      if (InDetFlags.doPrintConfigurables()):
        print InDetMergedPixelsTool
                    
      #
      # --- PixelGangedAmbiguitiesFinder tool (public)
      #
      from SiClusterizationTool.SiClusterizationToolConf import InDet__PixelGangedAmbiguitiesFinder
      InDetPixelGangedAmbiguitiesFinder = InDet__PixelGangedAmbiguitiesFinder(name = "InDetPixelGangedAmbiguitiesFinder")
      ToolSvc += InDetPixelGangedAmbiguitiesFinder
      if (InDetFlags.doPrintConfigurables()):
        print InDetPixelGangedAmbiguitiesFinder
            
      #
      # --- PixelClusterization algorithm
      #
      from InDetPrepRawDataFormation.InDetPrepRawDataFormationConf import InDet__PixelClusterization
      InDetPixelClusterization = InDet__PixelClusterization(name                    = "InDetPixelClusterization",
                                                            clusteringTool          = InDetMergedPixelsTool,
                                                            gangedAmbiguitiesFinder = InDetPixelGangedAmbiguitiesFinder,
                                                            DetectorManagerName     = InDetKeys.PixelManager(), 
                                                            DataObjectName          = InDetKeys.PixelRDOs(),
                                                            ClustersName            = InDetKeys.PixelClusters()
                                                            AmbiguitiesMap = "PixelClusterAmbiguitiesMapPU")
      topSequence += InDetPixelClusterization
      if (InDetFlags.doPrintConfigurables()):
         print InDetPixelClusterization

      if InDetFlags.doSplitReco() :
        InDetPixelClusterizationPU = InDet__PixelClusterization(name                    = "InDetPixelClusterizationPU",
                                                                clusteringTool          = InDetMergedPixelsTool,
                                                                gangedAmbiguitiesFinder = InDetPixelGangedAmbiguitiesFinder,
                                                                DetectorManagerName     = InDetKeys.PixelManager(),
                                                                DataObjectName          = InDetKeys.PixelPURDOs(),
                                                                ClustersName            = InDetKeys.PixelPUClusters()
                                                                AmbiguitiesMap = "PixelClusterAmbiguitiesMapPU")
        topSequence += InDetPixelClusterizationPU
        if (InDetFlags.doPrintConfigurables()):
          print InDetPixelClusterizationPU

   #
   # --- SCT Clusterization
   #
   if DetFlags.makeRIO.SCT_on() and InDetFlags.doSCT_PRDFormation():
   
      #
      # --- SCT_ClusteringTool (public)
      #
      from SiClusterizationTool.SiClusterizationToolConf import InDet__SCT_ClusteringTool
      InDetSCT_ClusteringTool = InDet__SCT_ClusteringTool(name              = "InDetSCT_ClusteringTool",
                                                          globalPosAlg      = InDetClusterMakerTool,
                                                          conditionsService = InDetSCT_ConditionsSummarySvc)
      if InDetFlags.selectSCTIntimeHits():
         if InDetFlags.InDet25nsec(): 
            InDetSCT_ClusteringTool.timeBins = "01X" 
         else: 
            InDetSCT_ClusteringTool.timeBins = "X1X" 

      ToolSvc += InDetSCT_ClusteringTool
      if (InDetFlags.doPrintConfigurables()):
        print InDetSCT_ClusteringTool
            

      #
      # --- SCT_Clusterization algorithm
      #
      from InDetPrepRawDataFormation.InDetPrepRawDataFormationConf import InDet__SCT_Clusterization
      InDetSCT_Clusterization = InDet__SCT_Clusterization(name                    = "InDetSCT_Clusterization",
                                                          clusteringTool          = InDetSCT_ClusteringTool,
                                                          # ChannelStatus         = InDetSCT_ChannelStatusAlg,
                                                          DetectorManagerName     = InDetKeys.SCT_Manager(), 
                                                          DataObjectName          = InDetKeys.SCT_RDOs(),
                                                          ClustersName            = InDetKeys.SCT_Clusters(),
                                                          conditionsService       = InDetSCT_ConditionsSummarySvc,
                                                          FlaggedConditionService = InDetSCT_FlaggedConditionSvc)
      if InDetFlags.cutSCTOccupancy():
        InDetSCT_Clusterization.maxRDOs = 384 #77
      else:
        InDetSCT_Clusterization.maxRDOs = 0
      topSequence += InDetSCT_Clusterization
      if (InDetFlags.doPrintConfigurables()):
        print InDetSCT_Clusterization

      if InDetFlags.doSplitReco() :
        InDetSCT_ClusterizationPU = InDet__SCT_Clusterization(name                    = "InDetSCT_ClusterizationPU",
                                                              clusteringTool          = InDetSCT_ClusteringTool,
                                                              # ChannelStatus         = InDetSCT_ChannelStatusAlg,
                                                              DetectorManagerName     = InDetKeys.SCT_Manager(),
                                                              DataObjectName          = InDetKeys.SCT_PU_RDOs(),
                                                              ClustersName            = InDetKeys.SCT_PU_Clusters(),
                                                              conditionsService       = InDetSCT_ConditionsSummarySvc,
                                                              FlaggedConditionService = InDetSCT_FlaggedConditionSvc)
        if InDetFlags.cutSCTOccupancy():
          InDetSCT_ClusterizationPU.maxRDOs = 384 #77
        else:
          InDetSCT_ClusterizationPU.maxRDOs = 0
        topSequence += InDetSCT_ClusterizationPU
        if (InDetFlags.doPrintConfigurables()):
          print InDetSCT_ClusterizationPU

#
# ----------- form SpacePoints from clusters in SCT and Pixels
#
if InDetFlags.doSpacePointFormation():
   #
   # --- SiSpacePointMakerTool (public)
   #
   from SiSpacePointTool.SiSpacePointToolConf import InDet__SiSpacePointMakerTool
   InDetSiSpacePointMakerTool = InDet__SiSpacePointMakerTool(name = "InDetSiSpacePointMakerTool")

   if InDetFlags.doSLHC():
      InDetSiSpacePointMakerTool.SCTGapParameter = 0.0015

   if InDetFlags.doCosmics() or InDetFlags.doBeamHalo():
      InDetSiSpacePointMakerTool.StripLengthTolerance       = 0.05
      InDetSiSpacePointMakerTool.UsePerpendicularProjection = True

   ToolSvc += InDetSiSpacePointMakerTool
   if (InDetFlags.doPrintConfigurables()):
     print InDetSiSpacePointMakerTool
   
   #
   # SiTrackerSpacePointFinder algorithm
   #
   from SiSpacePointFormation.SiSpacePointFormationConf import InDet__SiTrackerSpacePointFinder
   InDetSiTrackerSpacePointFinder = InDet__SiTrackerSpacePointFinder(name                   = "InDetSiTrackerSpacePointFinder",
                                                                     SiSpacePointMakerTool  = InDetSiSpacePointMakerTool,
                                                                     PixelsClustersName     = InDetKeys.PixelClusters(),
                                                                     SCT_ClustersName       = InDetKeys.SCT_Clusters(),
                                                                     SpacePointsPixelName   = InDetKeys.PixelSpacePoints(),
                                                                     SpacePointsSCTName     = InDetKeys.SCT_SpacePoints(),
                                                                     SpacePointsOverlapName = InDetKeys.OverlapSpacePoints(),
                                                                     ProcessPixels          = DetFlags.haveRIO.pixel_on(),
                                                                     ProcessSCTs            = DetFlags.haveRIO.SCT_on(),
                                                                     ProcessOverlaps        = DetFlags.haveRIO.SCT_on())

#   if InDetFlags.doDBM():
#     InDetSiTrackerSpacePointFinderDBM = InDet__SiTrackerSpacePointFinder(name                   = "InDetSiTrackerSpacePointFinderDBM",
#                                                                          SiSpacePointMakerTool  = InDetSiSpacePointMakerTool,
#                                                                          PixelsClustersName     = InDetKeys.PixelClusters(),
#                                                                          SCT_ClustersName       = InDetKeys.SCT_Clusters(),
#                                                                          SpacePointsPixelName   = InDetKeys.PixelSpacePoints(),
#                                                                          SpacePointsSCTName     = InDetKeys.SCT_SpacePoints(),
#                                                                          SpacePointsOverlapName = InDetKeys.OverlapSpacePoints(),
#                                                                          ProcessPixels          = DetFlags.haveRIO.pixel_on(),
#                                                                          ProcessSCTs            = DetFlags.haveRIO.SCT_on(),
#                                                                          ProcessOverlaps        = DetFlags.haveRIO.SCT_on(),
#                                                                          OverlapLimitEtaMax     = 5.0,
#                                                                          OverlapLimitEtaMin     = 0
#                                                                          )
#     topSequence += InDetSiTrackerSpacePointFinderDBM

   if InDetFlags.doDBMstandalone():
      InDetSiTrackerSpacePointFinder.OverlapLimitEtaMax = 5.0
      InDetSiTrackerSpacePointFinder.OverlapLimitEtaMin = 0

   if InDetFlags.doCosmics():
      InDetSiTrackerSpacePointFinder.ProcessOverlaps      = False
      InDetSiTrackerSpacePointFinder.OverrideBeamSpot     = True
      InDetSiTrackerSpacePointFinder.VertexZ              = 0
      InDetSiTrackerSpacePointFinder.VertexX              = 0
      InDetSiTrackerSpacePointFinder.VertexY              = 99999999   
      InDetSiTrackerSpacePointFinder.OverlapLimitOpposite = 5

   topSequence += InDetSiTrackerSpacePointFinder
   if (InDetFlags.doPrintConfigurables()):
     print InDetSiTrackerSpacePointFinder
     if (InDetFlags.doDBM()):
       print InDetSiTrackerSpacePointFinderDBM

# this truth must only be done if you do PRD and SpacePointformation
# If you only do the latter (== running on ESD) then the needed input (simdata)
# is not in ESD but the resulting truth (clustertruth) is already there ...
if InDetFlags.doPRDFormation() and InDetFlags.doSpacePointFormation():
   if InDetFlags.doTruth():
      from InDetTruthAlgs.InDetTruthAlgsConf import InDet__PRD_MultiTruthMaker
      InDetPRD_MultiTruthMakerSi = InDet__PRD_MultiTruthMaker (name                        = 'InDetPRD_MultiTruthMakerSi',
                                                             PixelClusterContainerName   = InDetKeys.PixelClusters(),
                                                             SCTClusterContainerName     = InDetKeys.SCT_Clusters(),
                                                             TRTDriftCircleContainerName = "",
                                                             SimDataMapNamePixel         = InDetKeys.PixelSDOs(),
                                                             SimDataMapNameSCT           = InDetKeys.SCT_SDOs(),
                                                             SimDataMapNameTRT           = "",
                                                             TruthNamePixel              = InDetKeys.PixelClustersTruth(),
                                                             TruthNameSCT                = InDetKeys.SCT_ClustersTruth(),
                                                             TruthNameTRT                = "")
      # a bit complicated, but this is how the truth maker gets to know which detector is on
      if (not DetFlags.haveRIO.pixel_on() or not InDetFlags.doPixelPRDFormation()):
         InDetPRD_MultiTruthMakerSi.PixelClusterContainerName = ""
         InDetPRD_MultiTruthMakerSi.SimDataMapNamePixel       = ""
         InDetPRD_MultiTruthMakerSi.TruthNamePixel            = ""
      if (not DetFlags.haveRIO.SCT_on() or not InDetFlags.doSCT_PRDFormation()):
         InDetPRD_MultiTruthMakerSi.SCTClusterContainerName   = ""
         InDetPRD_MultiTruthMakerSi.SimDataMapNameSCT         = ""
         InDetPRD_MultiTruthMakerSi.TruthNameSCT              = ""

      topSequence += InDetPRD_MultiTruthMakerSi
      if (InDetFlags.doPrintConfigurables()):
        print InDetPRD_MultiTruthMakerSi

      if InDetFlags.doSplitReco() :
        InDetPRD_MultiTruthMakerSiPU = InDet__PRD_MultiTruthMaker (name                        = 'InDetPRD_MultiTruthMakerSiPU',
                                                               PixelClusterContainerName   = InDetKeys.PixelPUClusters(),
                                                               SCTClusterContainerName     = InDetKeys.SCT_PU_Clusters(),
                                                               TRTDriftCircleContainerName = "",
                                                               SimDataMapNamePixel         = InDetKeys.PixelPUSDOs(),
                                                               SimDataMapNameSCT           = InDetKeys.SCT_PU_SDOs(),
                                                               SimDataMapNameTRT           = "",
                                                               TruthNamePixel              = InDetKeys.PixelPUClustersTruth(),
                                                               TruthNameSCT                = InDetKeys.SCT_PU_ClustersTruth(),
                                                               TruthNameTRT                = "")
        # a bit complicated, but this is how the truth maker gets to know which detector is on
        if (not DetFlags.haveRIO.pixel_on() or not InDetFlags.doPixelPRDFormation()):
           InDetPRD_MultiTruthMakerSiPU.PixelClusterContainerName = ""
           InDetPRD_MultiTruthMakerSiPU.SimDataMapNamePixel       = ""
           InDetPRD_MultiTruthMakerSiPU.TruthNamePixel            = ""
        if (not DetFlags.haveRIO.SCT_on() or not InDetFlags.doSCT_PRDFormation()):
           InDetPRD_MultiTruthMakerSiPU.SCTClusterContainerName   = ""
           InDetPRD_MultiTruthMakerSiPU.SimDataMapNameSCT         = ""
           InDetPRD_MultiTruthMakerSiPU.TruthNameSCT              = ""

        print InDetPRD_MultiTruthMakerSiPU
        InDetPRD_MultiTruthMakerSiPU.OutputLevel=VERBOSE
        topSequence += InDetPRD_MultiTruthMakerSiPU
        if (InDetFlags.doPrintConfigurables()):
          print InDetPRD_MultiTruthMakerSiPU
