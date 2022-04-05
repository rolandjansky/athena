# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def NnPixelClusterSplitProbToolCfg(flags, name="NnPixelClusterSplitProbTool", **kwargs):
    acc = ComponentAccumulator()

    # --- new NN prob tool
    MultiplicityContent = [1 , 1 , 1]

    from InDetConfig.TrackingCommonConfig import NnClusterizationFactoryCfg
    NnClusterizationFactory = acc.getPrimaryAndMerge(NnClusterizationFactoryCfg(flags))

    kwargs.setdefault("PriorMultiplicityContent", MultiplicityContent)
    kwargs.setdefault("NnClusterizationFactory", NnClusterizationFactory)
    kwargs.setdefault("useBeamSpotInfo", flags.InDet.Tracking.useBeamSpotInfoNN)

    NnPixelClusterSplitProbTool = CompFactory.InDet.NnPixelClusterSplitProbTool(name=name,**kwargs)

    acc.setPrivateTools(NnPixelClusterSplitProbTool)
    return acc

def NnPixelClusterSplitterCfg(flags, name="NnPixelClusterSplitter", **kwargs):
    acc = ComponentAccumulator()

    from InDetConfig.TrackingCommonConfig import NnClusterizationFactoryCfg
    NnClusterizationFactory = acc.getPrimaryAndMerge(NnClusterizationFactoryCfg(flags))

    kwargs.setdefault("NnClusterizationFactory", NnClusterizationFactory)
    kwargs.setdefault("ThresholdSplittingIntoTwoClusters", 0.5)
    kwargs.setdefault("ThresholdSplittingIntoThreeClusters", 0.25)
    kwargs.setdefault("SplitOnlyOnBLayer", False)
    kwargs.setdefault("useBeamSpotInfo", flags.InDet.Tracking.useBeamSpotInfoNN)
    # --- new NN splitter
    NnPixelClusterSplitter = CompFactory.InDet.NnPixelClusterSplitter(name=name,**kwargs)

    acc.setPrivateTools(NnPixelClusterSplitter)
    return acc

def InDetRecPreProcessingSiliconCfg(flags, **kwargs):
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
        from InDetConfig.TrackRecoConfig import BCM_ZeroSuppressionCfg
        acc.merge(BCM_ZeroSuppressionCfg(flags))
    
    #
    # -- Pixel Clusterization
    #
    if flags.Detector.EnablePixel:
        #
        # --- PixelClusterization algorithm
        #
        from InDetConfig.TrackRecoConfig import PixelClusterizationCfg
        acc.merge(PixelClusterizationCfg(flags))
        if flags.InDet.doSplitReco :
            from InDetConfig.TrackRecoConfig import PixelClusterizationPUCfg
            acc.merge(PixelClusterizationPUCfg(flags))
    #
    # --- SCT Clusterization
    #
    if flags.Detector.EnableSCT:
        #
        # --- SCT_Clusterization algorithm
        #
        from InDetConfig.TrackRecoConfig import SCTClusterizationCfg
        acc.merge(SCTClusterizationCfg(flags))
        if flags.InDet.doSplitReco :
            from InDetConfig.TrackRecoConfig import SCTClusterizationPUCfg
            acc.merge(SCTClusterizationPUCfg(flags))
    #
    # ----------- form SpacePoints from clusters in SCT and Pixels
    #
    #
    from InDetConfig.SiSpacePointFormationConfig import InDetSiTrackerSpacePointFinderCfg
    acc.merge(InDetSiTrackerSpacePointFinderCfg(flags))

    # this truth must only be done if you do PRD and SpacePointformation
    # If you only do the latter (== running on ESD) then the needed input (simdata)
    # is not in ESD but the resulting truth (clustertruth) is already there ...
    if flags.InDet.doTruth:
        from InDetConfig.InDetTruthAlgsConfig import InDetPRD_MultiTruthMakerSiCfg
        acc.merge(InDetPRD_MultiTruthMakerSiCfg(flags))
        if flags.InDet.doSplitReco:
            from InDetConfig.InDetTruthAlgsConfig import InDetPRD_MultiTruthMakerSiPUCfg
            acc.merge(InDetPRD_MultiTruthMakerSiPUCfg(flags))

    return acc


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.RDO_RUN2

    ConfigFlags.InDet.Tracking.doPixelClusterSplitting = True

    numThreads=1
    ConfigFlags.Concurrency.NumThreads=numThreads
    ConfigFlags.Concurrency.NumConcurrentEvents=numThreads

    ConfigFlags.loadAllDynamicFlags()

    ConfigFlags.lock()
    ConfigFlags.dump()
    
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    top_acc = MainServicesCfg(ConfigFlags)

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    top_acc.merge(PoolReadCfg(ConfigFlags))

    top_acc.merge(InDetRecPreProcessingSiliconCfg(ConfigFlags))

    iovsvc = top_acc.getService('IOVDbSvc')
    iovsvc.OutputLevel=5

    top_acc.printConfig()
    top_acc.run(25)
    top_acc.store(open("test_SiliconPreProcessing.pkl", "wb"))
