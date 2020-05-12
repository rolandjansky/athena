# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

##------------------------------------------------------------------------------
def InDetBCM_ZeroSuppressionCfg(flags, **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("BcmContainerName", "BCM_RDOs")
    algo = CompFactory.BCM_ZeroSuppression("InDetBCM_ZeroSuppression", **kwargs)
    acc.addEventAlgo(algo, primary = True)
    return acc

##------------------------------------------------------------------------------
def InDetPixelClusterizationCfg(flags, **kwargs) :
    acc = ComponentAccumulator()
    sub_acc = InDetMergedPixelsToolCfg(flags, **kwargs)
    merged_pixels_tool = sub_acc.getPrimary()
    acc.merge(sub_acc)
    sub_acc = InDetPixelGangedAmbiguitiesFinderCfg(flags)
    ambi_finder=sub_acc.getPrimary()
    acc.merge(sub_acc)
    acc.addEventAlgo( CompFactory.InDet.PixelClusterization(   name                    = "InDetPixelClusterization",
                                                    clusteringTool          = merged_pixels_tool,
                                                    gangedAmbiguitiesFinder = ambi_finder,
                                                    DataObjectName          = "PixelRDOs",
                                                    ClustersName            = "PixelClusters" ))
    return acc
##------------------------------------------------------------------------------
def InDetPixelClusterizationPUCfg(flags, **kwargs) :
    kwargs.setdefault("name", "InDetPixelClusterizationPU")
    kwargs.setdefault("DataObjectName", "Pixel_PU_RDOs")
    kwargs.setdefault("ClustersName", "PixelPUClusters")
    kwargs.setdefault("AmbiguitiesMap", "PixelClusterAmbiguitiesMapPU")
    return InDetPixelClusterizationCfg(flags, **kwargs)

##------------------------------------------------------------------------------
##------------------------------------------------------------------------------

def InDet_SCTClusterizationCfg(flags, **kwargs) :
    acc = ComponentAccumulator()

    # Need to get SCT_ConditionsSummaryTool for e.g. SCT_ClusteringTool
    from InDetConfig.InDetRecToolConfig import InDetSCT_ConditionsSummaryToolCfg
    InDetSCT_ConditionsSummaryToolWithoutFlagged = acc.popToolsAndMerge(InDetSCT_ConditionsSummaryToolCfg(flags,withFlaggedCondTool=False))

    #### Clustering tool ######
    accbuf = InDetClusterMakerToolCfg(flags, **kwargs)
    InDetClusterMakerTool = accbuf.getPrimary()
    acc.merge(accbuf)
    InDetSCT_ClusteringTool = CompFactory.InDet.SCT_ClusteringTool(    name              = "InDetSCT_ClusteringTool",
                                                            globalPosAlg     = InDetClusterMakerTool,
                                                            conditionsTool = InDetSCT_ConditionsSummaryToolWithoutFlagged)
    if flags.InDet.selectSCTIntimeHits :
       if flags.InDet.InDet25nsec : 
          InDetSCT_ClusteringTool.timeBins = "01X" 
       else: 
          InDetSCT_ClusteringTool.timeBins = "X1X" 

    acc.addEventAlgo( CompFactory.InDet.SCT_Clusterization(    name                    = "InDetSCT_Clusterization",
                                                    clusteringTool          = InDetSCT_ClusteringTool,
                                                    DataObjectName          = 'SCT_RDOs', ##InDetKeys.SCT_RDOs(),
                                                    ClustersName            = 'SCT_Clusters', ##InDetKeys.SCT_Clusters(),
                                                    conditionsTool          = InDetSCT_ConditionsSummaryToolWithoutFlagged  ) )

    return acc


##------------------------------------------------------------------------------
##------------------------------------------------------------------------------
##------------------------------------------------------------------------------

def InDet_SCTClusterizationPUCfg(flags, **kwargs) :
    kwargs.setdefault("name", "InDetSCT_ClusterizationPU")
    kwargs.setdefault("DataObjectName", "SCT_PU_RDOs" )   #flags.InDetKeys.SCT_PU_RDOs
    kwargs.setdefault("ClustersName", "SCT_PU_Clusters") #flags.InDetKeys.SCT_PU_Clusters
    return InDet_SCTClusterizationCfg(flags, **kwargs)

##------------------------------------------------------------------------------
def InDetPixelGangedAmbiguitiesFinderCfg(flags) :
    acc = ComponentAccumulator()
    InDetPixelGangedAmbiguitiesFinder = CompFactory.InDet.PixelGangedAmbiguitiesFinder( name = "InDetPixelGangedAmbiguitiesFinder")
    acc.addPublicTool( InDetPixelGangedAmbiguitiesFinder, primary=True)
    return acc


##------------------------------------------------------------------------------
def InDetMergedPixelsToolCfg(flags, **kwargs) :
      acc = ComponentAccumulator()
      clusterSplitProbTool = None
      clusterSplitterTool  = None

      if flags.InDet.doPixelClusterSplitting :
         # --- Neutral Network version
         if flags.InDet.pixelClusterSplittingType == 'NeuralNet':
            useBeamConstraint = flags.InDet.useBeamConstraint

            from SiLorentzAngleTool.PixelLorentzAngleConfig import PixelLorentzAngleTool
 
            # --- new NN prob tool
            NnClusterizationFactory = CompFactory.InDet.NnClusterizationFactory(   name                         = "NnClusterizationFactory",
                                                                        PixelLorentzAngleTool        = PixelLorentzAngleTool( flags ),
                                                                        useToT                       = flags.InDet.doNNToTCalibration,
                                                                        NnCollectionReadKey          = "PixelClusterNN",
                                                                        NnCollectionWithTrackReadKey = "PixelClusterNNWithTrack")
            MultiplicityContent = [1 , 1 , 1]
            if flags.InDet.doSLHC:
                PixelClusterSplitProbTool=CompFactory.InDet.TruthPixelClusterSplitProbTool
            else:
                PixelClusterSplitProbTool=CompFactory.InDet.NnPixelClusterSplitProbTool
            NnPixelClusterSplitProbTool=PixelClusterSplitProbTool(name                     = "NnPixelClusterSplitProbTool",
                                                                           PriorMultiplicityContent = MultiplicityContent,
                                                                           NnClusterizationFactory  = NnClusterizationFactory,
                                                                           useBeamSpotInfo          = useBeamConstraint)
            # --- remember this prob tool  
            clusterSplitProbTool = NnPixelClusterSplitProbTool
            # --- new NN splitter
            if flags.InDet.doSLHC :
                PixelClusterSplitter=CompFactory.InDet.TruthPixelClusterSplitter
            else:
                PixelClusterSplitter=CompFactory.InDet.NnPixelClusterSplitter
            NnPixelClusterSplitter=PixelClusterSplitter(    name                                = "NnPixelClusterSplitter",
                                                            NnClusterizationFactory             = NnClusterizationFactory,
                                                            ThresholdSplittingIntoTwoClusters   = 0.5, ##InDet.pixelClusterSplitProb1, ###0.5, # temp.
                                                            ThresholdSplittingIntoThreeClusters = 0.25, ##InDet.pixelClusterSplitProb2, ###0.25, # temp.
                                                            SplitOnlyOnBLayer                   = False,
                                                            useBeamSpotInfo                     = useBeamConstraint)
            # remember splitter tool  
            clusterSplitterTool = NnPixelClusterSplitter
         # --- Neutral Network version ?
         elif flags.InDet.pixelClusterSplittingType == 'AnalogClus':      
            # new splitter tool & remember splitter tool    
            clusterSplitterTool=CompFactory.InDet.TotPixelClusterSplitter (name = "TotPixelClusterSplitter")
      
      # --- now load the framework for the clustering
      #InDetClusterMakerTool = CompFactory.InDet.ClusterMakerTool(name                 = "InDetClusterMakerTool")
      #acc.addPublicTool(InDetClusterMakerTool)
      accbuf = InDetClusterMakerToolCfg(flags)
      InDetClusterMakerTool = accbuf.getPrimary()
      acc.merge(accbuf)

      # PixelClusteringToolBase uses PixelConditionsSummaryTool
      from InDetConfig.InDetRecToolConfig import InDetPixelConditionsSummaryToolCfg
      accbuf = InDetPixelConditionsSummaryToolCfg(flags)
      conditionssummarytool = accbuf.popPrivateTools()
      # FIXME? Currently not assigning 
      acc.merge(accbuf)

      InDetMergedPixelsTool = CompFactory.InDet.MergedPixelsTool(  name                    = "InDetMergedPixelsTool", 
                                                        globalPosAlg            = InDetClusterMakerTool,
                                                        MinimalSplitSize        = 0,
                                                        MaximalSplitSize        = 49,
                                                        MinimalSplitProbability = 0,
                                                        DoIBLSplitting = True)
      # assign the tools if there are any                                                
      if not flags.InDet.doTIDE_Ambi and clusterSplitProbTool is not None : InDetMergedPixelsTool.SplitProbTool   = clusterSplitProbTool
      if not flags.InDet.doTIDE_Ambi and clusterSplitterTool is not None  : InDetMergedPixelsTool.ClusterSplitter = clusterSplitterTool

      acc.addPublicTool(InDetMergedPixelsTool, primary=True)
      return acc
##------------------------------------------------------------------------------
def InDetClusterMakerToolCfg(flags, **kwargs) :
    acc = ComponentAccumulator()
    InDetClusterMakerTool = CompFactory.InDet.ClusterMakerTool(name = "InDetClusterMakerTool")

    SCTLorentzAngleTool = CompFactory.SiLorentzAngleTool(name = "SCTLorentzAngleTool", DetectorName="SCT", SiLorentzAngleCondData="SCTSiLorentzAngleCondData")
    SCTLorentzAngleTool.UseMagFieldCache = True
    acc.addPublicTool(SCTLorentzAngleTool)
    acc.addPublicTool(InDetClusterMakerTool, primary=True)
    return acc

