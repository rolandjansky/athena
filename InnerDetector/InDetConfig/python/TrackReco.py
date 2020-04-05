# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.UnifyProperties import unifySet,_propsToUnify
_propsToUnify['THistSvc.Output'] = unifySet

##------------------------------------------------------------------------------
def InDetBCM_ZeroSuppressionCfg(flags, **kwargs):
    from BCM_ZeroSuppression.BCM_ZeroSuppressionConf import BCM_ZeroSuppression
    acc = ComponentAccumulator()
    kwargs.setdefault("BcmContainerName", "BCM_RDOs")
    algo = BCM_ZeroSuppression("InDetBCM_ZeroSuppression", **kwargs)
    acc.addEventAlgo(algo, primary = True)
    return acc

##------------------------------------------------------------------------------
def InDetPixelClusterizationCfg(flags, **kwargs) :
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()
    from InDetPrepRawDataFormation.InDetPrepRawDataFormationConf import InDet__PixelClusterization
    sub_acc,merged_pixels_tool = InDetMergedPixelsToolCfg(flags, **kwargs)
    acc.merge(sub_acc)
    sub_acc,ambi_finder = InDetPixelGangedAmbiguitiesFinderCfg(flags)
    acc.merge(sub_acc)
    acc.addEventAlgo( InDet__PixelClusterization(   name                    = "InDetPixelClusterization",
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
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()
    #####################
    from SCT_ConditionsTools.SCT_ConditionsSummaryToolSetup import SCT_ConditionsSummaryToolSetup
    sct_ConditionsSummaryToolSetup = SCT_ConditionsSummaryToolSetup()
    sct_ConditionsSummaryToolSetup.setup()

    sct_ConditionsSummaryToolSetupWithoutFlagged = SCT_ConditionsSummaryToolSetup("InDetSCT_ConditionsSummaryToolWithoutFlagged")
    sct_ConditionsSummaryToolSetupWithoutFlagged.setup()
    InDetSCT_ConditionsSummaryToolWithoutFlagged = sct_ConditionsSummaryToolSetupWithoutFlagged.getTool()

    #### Clustering tool ######
    accbuf, InDetClusterMakerTool = InDetClusterMakerToolCfg(flags, **kwargs)
    acc.merge(accbuf)
    from SiClusterizationTool.SiClusterizationToolConf import InDet__SCT_ClusteringTool
    InDetSCT_ClusteringTool = InDet__SCT_ClusteringTool(    name              = "InDetSCT_ClusteringTool",
                                                            globalPosAlg     = InDetClusterMakerTool,
                                                            conditionsTool = InDetSCT_ConditionsSummaryToolWithoutFlagged)
    if flags.InDet.selectSCTIntimeHits :
       if flags.InDet.InDet25nsec : 
          InDetSCT_ClusteringTool.timeBins = "01X" 
       else: 
          InDetSCT_ClusteringTool.timeBins = "X1X" 

    from InDetPrepRawDataFormation.InDetPrepRawDataFormationConf import InDet__SCT_Clusterization
    acc.addEventAlgo( InDet__SCT_Clusterization(    name                    = "InDetSCT_Clusterization",
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
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()
    from SiClusterizationTool.SiClusterizationToolConf import InDet__PixelGangedAmbiguitiesFinder
    InDetPixelGangedAmbiguitiesFinder = InDet__PixelGangedAmbiguitiesFinder( name = "InDetPixelGangedAmbiguitiesFinder")
    acc.addPublicTool( InDetPixelGangedAmbiguitiesFinder )
    return acc,InDetPixelGangedAmbiguitiesFinder


##------------------------------------------------------------------------------
def InDetMergedPixelsToolCfg(flags, **kwargs) :
      from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
      acc = ComponentAccumulator()
      clusterSplitProbTool = None
      clusterSplitterTool  = None

      if flags.InDet.doPixelClusterSplitting :
         # --- Neutral Network version
         if flags.InDet.pixelClusterSplittingType == 'NeuralNet':
            useBeamConstraint = flags.InDet.useBeamConstraint

            from SiLorentzAngleTool.PixelLorentzAngleConfig import PixelLorentzAngleToolCfg
            # --- new NN prob tool
            from SiClusterizationTool.SiClusterizationToolConf import InDet__NnClusterizationFactory
            NnClusterizationFactory = InDet__NnClusterizationFactory(   name                         = "NnClusterizationFactory",
                                                                        PixelLorentzAngleTool        = PixelLorentzAngleToolCfg( flags ),
                                                                        useToT                       = flags.InDet.doNNToTCalibration,
                                                                        NnCollectionReadKey          = "PixelClusterNN",
                                                                        NnCollectionWithTrackReadKey = "PixelClusterNNWithTrack")

            MultiplicityContent = [1 , 1 , 1]
            if flags.InDet.doSLHC:
                from SiClusterizationTool.SiClusterizationToolConf import InDet__TruthPixelClusterSplitProbTool as PixelClusterSplitProbTool
            else:
                from SiClusterizationTool.SiClusterizationToolConf import InDet__NnPixelClusterSplitProbTool as PixelClusterSplitProbTool
            NnPixelClusterSplitProbTool=PixelClusterSplitProbTool(name                     = "NnPixelClusterSplitProbTool",
                                                                           PriorMultiplicityContent = MultiplicityContent,
                                                                           NnClusterizationFactory  = NnClusterizationFactory,
                                                                           useBeamSpotInfo          = useBeamConstraint)
            # --- remember this prob tool  
            clusterSplitProbTool = NnPixelClusterSplitProbTool
            # --- new NN splitter
            if flags.InDet.doSLHC :
                from SiClusterizationTool.SiClusterizationToolConf import InDet__TruthPixelClusterSplitter as PixelClusterSplitter
            else:
                from SiClusterizationTool.SiClusterizationToolConf import InDet__NnPixelClusterSplitter as PixelClusterSplitter
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
            # new splitter tool
            from SiClusterizationTool.SiClusterizationToolConf import InDet__TotPixelClusterSplitter
            TotPixelClusterSplitter=InDet__TotPixelClusterSplitter (name = "TotPixelClusterSplitter")
            # remember splitter tool    
            clusterSplitterTool = TotPixelClusterSplitter
      

      # --- now load the framework for the clustering
      #from SiClusterizationTool.SiClusterizationToolConf import InDet__ClusterMakerTool
      #InDetClusterMakerTool = InDet__ClusterMakerTool(name                 = "InDetClusterMakerTool")
      #acc.addPublicTool(InDetClusterMakerTool)
      accbuf, InDetClusterMakerTool = InDetClusterMakerToolCfg(flags, **kwargs)
      acc.merge(accbuf)

      from SiClusterizationTool.SiClusterizationToolConf import InDet__MergedPixelsTool
      InDetMergedPixelsTool = InDet__MergedPixelsTool(  name                    = "InDetMergedPixelsTool", 
                                                        globalPosAlg            = InDetClusterMakerTool,
                                                        MinimalSplitSize        = 0,
                                                        MaximalSplitSize        = 49,
                                                        MinimalSplitProbability = 0,
                                                        DoIBLSplitting = True)
      # assign the tools if there are any                                                
      if not flags.InDet.doTIDE_Ambi and clusterSplitProbTool is not None : InDetMergedPixelsTool.SplitProbTool   = clusterSplitProbTool
      if not flags.InDet.doTIDE_Ambi and clusterSplitterTool is not None  : InDetMergedPixelsTool.ClusterSplitter = clusterSplitterTool

      sub_acc = ComponentAccumulator()      
      acc.addPublicTool(InDetMergedPixelsTool)
      acc.merge(sub_acc)
      return acc,InDetMergedPixelsTool
##------------------------------------------------------------------------------
def InDetClusterMakerToolCfg(flags, **kwargs) :
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()
    from SiClusterizationTool.SiClusterizationToolConf import InDet__ClusterMakerTool
    InDetClusterMakerTool = InDet__ClusterMakerTool(name = "InDetClusterMakerTool")
    SiLorentzAngleTool=CompFactory.SiLorentzAngleTool

    SCTLorentzAngleTool = SiLorentzAngleTool(name = "SCTLorentzAngleTool", DetectorName="SCT", SiLorentzAngleCondData="SCTSiLorentzAngleCondData")
    SCTLorentzAngleTool.UseMagFieldSvc = True #may need also MagFieldSvc instance
    acc.addPublicTool(SCTLorentzAngleTool)
    acc.addPublicTool(InDetClusterMakerTool)
    return acc,InDetClusterMakerTool

