# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def ITkNnPixelClusterSplitProbToolCfg(flags, name="ITkNnPixelClusterSplitProbTool", **kwargs):
    acc = ComponentAccumulator()

    # --- new NN prob tool
    MultiplicityContent = [1 , 1 , 1]

    from InDetConfig.ITkTrackingCommonConfig import ITkTruthClusterizationFactoryCfg
    ITkTruthClusterizationFactory = acc.popToolsAndMerge(ITkTruthClusterizationFactoryCfg(flags))

    kwargs.setdefault("PriorMultiplicityContent", MultiplicityContent)
    kwargs.setdefault("NnClusterizationFactory", ITkTruthClusterizationFactory) #Truth-based for ITk for now

    ITkNnPixelClusterSplitProbTool = CompFactory.InDet.TruthPixelClusterSplitProbTool(name=name,**kwargs) #Truth-based for ITk for now

    acc.setPrivateTools(ITkNnPixelClusterSplitProbTool)
    return acc

def ITkNnPixelClusterSplitterCfg(flags, name="ITkNnPixelClusterSplitter", **kwargs):
    acc = ComponentAccumulator()

    from InDetConfig.ITkTrackingCommonConfig import ITkTruthClusterizationFactoryCfg
    ITkTruthClusterizationFactory = acc.popToolsAndMerge(ITkTruthClusterizationFactoryCfg(flags)) #Truth-based for ITk for now

    kwargs.setdefault("NnClusterizationFactory", ITkTruthClusterizationFactory)
    kwargs.setdefault("ThresholdSplittingIntoTwoClusters", 0.5)
    kwargs.setdefault("ThresholdSplittingIntoThreeClusters", 0.25)
    kwargs.setdefault("SplitOnlyOnBLayer", False)
    # --- new NN splitter
    ITkNnPixelClusterSplitter = CompFactory.InDet.TruthPixelClusterSplitter(name=name,**kwargs) #Truth-based for ITk for now

    acc.setPrivateTools(ITkNnPixelClusterSplitter)
    return acc

def ITkRecPreProcessingSiliconCfg(flags, **kwargs):
    acc = ComponentAccumulator()
    # ------------------------------------------------------------
    #
    # ----------- Data-Preparation stage
    #
    # ------------------------------------------------------------
    #
    # ----------- PrepRawData creation from Raw Data Objects
    #

    #
    # --- Slim BCM RDOs by zero-suppressing
    #
    if flags.Detector.EnableBCM:
        from InDetConfig.ITkTrackRecoConfig import ITk_BCM_ZeroSuppressionCfg
        acc.merge(ITk_BCM_ZeroSuppressionCfg(flags))

    #
    # -- Pixel Clusterization
    #
    if flags.Detector.EnableITkPixel:
        #
        # --- PixelClusterization algorithm
        #
        from InDetConfig.ITkTrackRecoConfig import ITkPixelClusterizationCfg
        acc.merge(ITkPixelClusterizationCfg(flags))

    #
    # --- Strip Clusterization
    #
    if flags.Detector.EnableITkStrip:
        #
        # --- Strip Clusterization algorithm
        #
        from InDetConfig.ITkTrackRecoConfig import ITkStripClusterizationCfg
        acc.merge(ITkStripClusterizationCfg(flags))

    if flags.ITk.Tracking.convertInDetClusters and flags.Detector.EnableITkPixel and flags.Detector.EnableITkStrip:
        #
        # --- Conversion algorithm for InDet clusters to xAOD clusters
        #
        from InDetConfig.ITkTrackRecoConfig import ITkInDetToXAODClusterConversionCfg
        acc.merge(ITkInDetToXAODClusterConversionCfg(flags))


    #
    # ----------- form SpacePoints from clusters in SCT and Pixels
    #
    #
    from InDetConfig.SiSpacePointFormationConfig import ITkSiTrackerSpacePointFinderCfg
    acc.merge(ITkSiTrackerSpacePointFinderCfg(flags))
    if flags.ITk.Tracking.produceNewSpacePointContainer:
        if flags.Detector.EnableITkPixel:
            from TrkConfig.ActsTrkSpacePointFormationConfig import ActsTrkPixelSpacePointFormationCfg
            acc.merge(ActsTrkPixelSpacePointFormationCfg(flags))
        if flags.Detector.EnableITkStrip:
            from TrkConfig.ActsTrkSpacePointFormationConfig import ActsTrkStripSpacePointFormationCfg
            acc.merge(ActsTrkStripSpacePointFormationCfg(flags))

    # this truth must only be done if you do PRD and SpacePointformation
    # If you only do the latter (== running on ESD) then the needed input (simdata)
    # is not in ESD but the resulting truth (clustertruth) is already there ...
    if flags.ITk.Tracking.doTruth:
        from InDetConfig.InDetTruthAlgsConfig import ITkPRD_MultiTruthMakerSiCfg
        acc.merge(ITkPRD_MultiTruthMakerSiCfg(flags))

    return acc


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.RDO_RUN2

    ConfigFlags.Detector.GeometryITkPixel   = True
    ConfigFlags.Detector.GeometryITkStrip   = True

    numThreads=1
    ConfigFlags.Concurrency.NumThreads=numThreads
    ConfigFlags.Concurrency.NumConcurrentEvents=numThreads

    ConfigFlags.loadAllDynamicFlags()

    ConfigFlags.lock()
    ConfigFlags.dump()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    top_acc = MainServicesCfg(ConfigFlags)

    top_acc.merge(ITkRecPreProcessingSiliconCfg(ConfigFlags))

    iovsvc = top_acc.getService('IOVDbSvc')
    iovsvc.OutputLevel=5

    top_acc.printConfig()
    top_acc.run(25)
    top_acc.store(open("test_ITkSiliconPreProcessing.pkl", "wb"))
