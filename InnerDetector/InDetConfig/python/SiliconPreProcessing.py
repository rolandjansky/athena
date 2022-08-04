# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.Enums import Format

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
        from InDetConfig.BCM_ZeroSuppressionConfig import BCM_ZeroSuppressionCfg
        acc.merge(BCM_ZeroSuppressionCfg(flags))
    
    #
    # -- Pixel Clusterization
    #
    if flags.Detector.EnablePixel and (   flags.Input.Format is Format.BS
                                       or 'PixelRDOs' in flags.Input.Collections):

        #
        # --- PixelClusterization algorithm
        #
        from InDetConfig.InDetPrepRawDataFormationConfig import PixelClusterizationCfg
        acc.merge(PixelClusterizationCfg(flags))
        if flags.InDet.doSplitReco :
            from InDetConfig.InDetPrepRawDataFormationConfig import PixelClusterizationPUCfg
            acc.merge(PixelClusterizationPUCfg(flags))
    #
    # --- SCT Clusterization
    #
    if flags.Detector.EnableSCT and (   flags.Input.Format is Format.BS
                                     or 'SCT_RDOs' in flags.Input.Collections):

        #
        # --- SCT_Clusterization algorithm
        #
        from InDetConfig.InDetPrepRawDataFormationConfig import SCTClusterizationCfg
        acc.merge(SCTClusterizationCfg(flags))
        if flags.InDet.doSplitReco :
            from InDetConfig.InDetPrepRawDataFormationConfig import SCTClusterizationPUCfg
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
    if (flags.InDet.doTruth
        and (not flags.Detector.EnableSCT   or 'SCT_SDO_Map'  in flags.Input.Collections)
        and (not flags.Detector.EnablePixel or 'PixelSDO_Map' in flags.Input.Collections)) :

        from InDetConfig.InDetTruthAlgsConfig import InDetPRD_MultiTruthMakerSiCfg
        acc.merge(InDetPRD_MultiTruthMakerSiCfg(flags))
        if flags.InDet.doSplitReco:
            from InDetConfig.InDetTruthAlgsConfig import InDetPRD_MultiTruthMakerSiPUCfg
            acc.merge(InDetPRD_MultiTruthMakerSiPUCfg(flags))

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
    if flags.Detector.EnableBCMPrime:
        from InDetConfig.BCM_ZeroSuppressionConfig import BCM_ZeroSuppressionCfg
        acc.merge(BCM_ZeroSuppressionCfg(flags))

    #
    # -- Pixel Clusterization
    #
    if flags.Detector.EnableITkPixel:
        #
        # --- PixelClusterization algorithm
        #
        from InDetConfig.InDetPrepRawDataFormationConfig import ITkPixelClusterizationCfg
        acc.merge(ITkPixelClusterizationCfg(flags))

    #
    # --- Strip Clusterization
    #
    if flags.Detector.EnableITkStrip:
        #
        # --- Strip Clusterization algorithm
        #
        from InDetConfig.InDetPrepRawDataFormationConfig import ITkStripClusterizationCfg
        acc.merge(ITkStripClusterizationCfg(flags))

    if flags.ITk.Tracking.convertInDetClusters and flags.Detector.EnableITkPixel and flags.Detector.EnableITkStrip:
        #
        # --- Conversion algorithm for InDet clusters to xAOD clusters
        #
        from InDetConfig.InDetPrepRawDataFormationConfig import ITkInDetToXAODClusterConversionCfg
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
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.RDO_RUN2

    ConfigFlags.InDet.Tracking.doPixelClusterSplitting = True

    numThreads=1
    ConfigFlags.Concurrency.NumThreads=numThreads
    ConfigFlags.Concurrency.NumConcurrentEvents=numThreads


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
