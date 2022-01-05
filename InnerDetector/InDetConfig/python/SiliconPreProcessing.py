# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def InDetSiElementPropertiesTableCondAlgCfg(flags, name="InDetSiElementPropertiesTableCondAlg", **kwargs):
    # For SCT DetectorElementCollection used
    from SCT_GeoModel.SCT_GeoModelConfig import SCT_ReadoutGeometryCfg
    acc = SCT_ReadoutGeometryCfg(flags)

    acc.addCondAlgo(CompFactory.InDet.SiElementPropertiesTableCondAlg(name = name, **kwargs))
    return acc

def SiSpacePointMakerToolCfg(flags, name="InDetSiSpacePointMakerTool", **kwargs):
    acc = ComponentAccumulator()
    #
    # --- SiSpacePointMakerTool (public)
    #

    if flags.Beam.Type == "cosmics" or flags.InDet.Tracking.doBeamHalo:
        kwargs.setdefault("StripLengthTolerance", 0.05)
        kwargs.setdefault("UsePerpendicularProjection", True)
    
    InDetSiSpacePointMakerTool = CompFactory.InDet.SiSpacePointMakerTool(name = "InDetSiSpacePointMakerTool", **kwargs)
    acc.setPrivateTools(InDetSiSpacePointMakerTool)
    return acc

def InDetSiTrackerSpacePointFinderCfg(flags, name = "InDetSiTrackerSpacePointFinder", **kwargs):
    #
    # SiTrackerSpacePointFinder algorithm
    #

    # For SCT DetectorElementCollection used
    from SCT_GeoModel.SCT_GeoModelConfig import SCT_ReadoutGeometryCfg
    acc = SCT_ReadoutGeometryCfg(flags)

    from BeamSpotConditions.BeamSpotConditionsConfig import BeamSpotCondAlgCfg
    acc.merge(BeamSpotCondAlgCfg(flags))

    InDetSiSpacePointMakerTool = acc.popToolsAndMerge(SiSpacePointMakerToolCfg(flags))

    kwargs.setdefault("SiSpacePointMakerTool", InDetSiSpacePointMakerTool)
    kwargs.setdefault("PixelsClustersName", 'PixelClusters') # InDetKeys.PixelClusters
    kwargs.setdefault("SCT_ClustersName", 'SCT_Clusters') # InDetKeys.SCT_Clusters
    kwargs.setdefault("SpacePointsPixelName", 'PixelSpacePoints') # InDetKeys.PixelSpacePoints
    kwargs.setdefault("SpacePointsSCTName", 'SCT_SpacePoints') # InDetKeys.SCT_SpacePoints
    kwargs.setdefault("SpacePointsOverlapName", 'OverlapSpacePoints') # InDetKeys.OverlapSpacePoints)
    kwargs.setdefault("ProcessPixels", flags.Detector.EnablePixel)
    kwargs.setdefault("ProcessSCTs", flags.Detector.EnableSCT)
    kwargs.setdefault("ProcessOverlaps", flags.Detector.EnableSCT)

    if flags.InDet.doDBMstandalone:
        kwargs.setdefault("OverlapLimitEtaMax", 5.0)
        kwargs.setdefault("OverlapLimitEtaMin", 0)

    if flags.Beam.Type == "cosmics":
        kwargs.setdefault("ProcessOverlaps", False)
        kwargs.setdefault("OverrideBeamSpot", True)
        kwargs.setdefault("VertexZ", 0)
        kwargs.setdefault("VertexX", 0)
        kwargs.setdefault("VertexY", 99999999)
        kwargs.setdefault("OverlapLimitOpposite", 5)

    acc.addEventAlgo(CompFactory.InDet.SiTrackerSpacePointFinder(name = name, **kwargs))
    return acc

def InDetPRD_MultiTruthMakerSiCfg(flags, name="InDetPRD_MultiTruthMakerSi", **kwargs):
    acc = ComponentAccumulator()

    # For pixel + SCT DetectorElementCollection used
    from PixelGeoModel.PixelGeoModelConfig import PixelReadoutGeometryCfg
    acc.merge(PixelReadoutGeometryCfg( flags ))
    from SCT_GeoModel.SCT_GeoModelConfig import SCT_ReadoutGeometryCfg
    acc.merge(SCT_ReadoutGeometryCfg( flags ))

    if flags.InDet.doTruth:
        kwargs.setdefault("PixelClusterContainerName", 'PixelClusters') # InDetKeys.PixelClusters()
        kwargs.setdefault("SCTClusterContainerName", 'SCT_Clusters') # InDetKeys.SCT_Clusters()
        kwargs.setdefault("TRTDriftCircleContainerName", "")
        kwargs.setdefault("SimDataMapNamePixel", 'PixelSDO_Map') # InDetKeys.PixelSDOs()
        kwargs.setdefault("SimDataMapNameSCT", 'SCT_SDO_Map') # InDetKeys.SCT_SDOs()
        kwargs.setdefault("SimDataMapNameTRT", "")
        kwargs.setdefault("TruthNamePixel", 'PRD_MultiTruthPixel') # InDetKeys.PixelClustersTruth()
        kwargs.setdefault("TruthNameSCT", 'PRD_MultiTruthSCT') # InDetKeys.SCT_ClustersTruth()
        kwargs.setdefault("TruthNameTRT", "")
        # a bit complicated, but this is how the truth maker gets to know which detector is on
        if not flags.Detector.EnablePixel:
            kwargs.setdefault("PixelClusterContainerName", "")
            kwargs.setdefault("SimDataMapNamePixel", "")
            kwargs.setdefault("TruthNamePixel", "")
        if not flags.Detector.EnableSCT:
            kwargs.setdefault("SCTClusterContainerName", "")
            kwargs.setdefault("SimDataMapNameSCT", "")
            kwargs.setdefault("TruthNameSCT", "")

    InDetPRD_MultiTruthMakerSi = CompFactory.InDet.PRD_MultiTruthMaker(name = name, **kwargs)
    acc.addEventAlgo(InDetPRD_MultiTruthMakerSi)
    return acc

def InDetPRD_MultiTruthMakerSiPUCfg(flags, name="InDetPRD_MultiTruthMakerSiPU", **kwargs):
    acc = ComponentAccumulator()

    # For pixel + SCT DetectorElementCollection used
    from PixelGeoModel.PixelGeoModelConfig import PixelReadoutGeometryCfg
    acc.merge(PixelReadoutGeometryCfg( flags ))
    from SCT_GeoModel.SCT_GeoModelConfig import SCT_ReadoutGeometryCfg
    acc.merge(SCT_ReadoutGeometryCfg( flags ))

    if flags.InDet.doTruth:
        kwargs.setdefault("PixelClusterContainerName", 'PixelPUClusters') # InDetKeys.PixelPUClusters()
        kwargs.setdefault("SCTClusterContainerName", 'SCT_PU_Clusters') # InDetKeys.SCT_PU_Clusters()
        kwargs.setdefault("TRTDriftCircleContainerName", "")
        kwargs.setdefault("SimDataMapNamePixel", 'Pixel_PU_SDO_Map') # InDetKeys.PixelPUSDOs()
        kwargs.setdefault("SimDataMapNameSCT", 'SCT_PU_SDO_Map') # InDetKeys.SCT_PU_SDOs()
        kwargs.setdefault("SimDataMapNameTRT", "")
        kwargs.setdefault("TruthNamePixel", 'PRD_PU_MultiTruthPixel') # InDetKeys.PixelPUClustersTruth()
        kwargs.setdefault("TruthNameSCT", 'PRD_PU_MultiTruthSCT') # InDetKeys.SCT_PU_ClustersTruth()
        kwargs.setdefault("TruthNameTRT", "")
         # a bit complicated, but this is how the truth maker gets to know which detector is on
        if not flags.Detector.EnablePixel:
            kwargs.setdefault("PixelClusterContainerName", "")
            kwargs.setdefault("SimDataMapNamePixel", "")
            kwargs.setdefault("TruthNamePixel", "")
        if not flags.Detector.EnableSCT:
            kwargs.setdefault("SCTClusterContainerName", "")
            kwargs.setdefault("SimDataMapNameSCT", "")
            kwargs.setdefault("TruthNameSCT", "")

    InDetPRD_MultiTruthMakerSi = CompFactory.InDet.PRD_MultiTruthMaker(name = name, **kwargs)
    acc.addEventAlgo(InDetPRD_MultiTruthMakerSi)
    return acc

def NnPixelClusterSplitProbToolCfg(flags, name="NnPixelClusterSplitProbTool", **kwargs):
    acc = ComponentAccumulator()

    # --- new NN prob tool
    MultiplicityContent = [1 , 1 , 1]

    from InDetConfig.TrackingCommonConfig import NnClusterizationFactoryCfg
    NnClusterizationFactory = acc.getPrimaryAndMerge(NnClusterizationFactoryCfg(flags))

    kwargs.setdefault("PriorMultiplicityContent", MultiplicityContent)
    kwargs.setdefault("NnClusterizationFactory", NnClusterizationFactory)
    kwargs.setdefault("useBeamSpotInfo", flags.InDet.useBeamSpotInfoNN)

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
    kwargs.setdefault("useBeamSpotInfo", flags.InDet.useBeamSpotInfoNN)
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
    acc.merge(InDetSiElementPropertiesTableCondAlgCfg(flags))
    acc.merge(InDetSiTrackerSpacePointFinderCfg(flags))

    # this truth must only be done if you do PRD and SpacePointformation
    # If you only do the latter (== running on ESD) then the needed input (simdata)
    # is not in ESD but the resulting truth (clustertruth) is already there ...
    if flags.InDet.doTruth:
        acc.merge(InDetPRD_MultiTruthMakerSiCfg(flags))
        if flags.InDet.doSplitReco:
            acc.merge(InDetPRD_MultiTruthMakerSiPUCfg(flags))

    return acc


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files=defaultTestFiles.RDO

    ConfigFlags.InDet.doPixelClusterSplitting = True

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
