# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def InDetSiElementPropertiesTableCondAlgCfg(flags, name="InDetSiElementPropertiesTableCondAlg", **kwargs):
    acc = ComponentAccumulator()
    acc.addCondAlgo(CompFactory.InDet.SiElementPropertiesTableCondAlg(name = name, **kwargs))
    return acc

def SiSpacePointMakerToolCfg(flags, name="InDetSiSpacePointMakerTool", **kwargs):
    acc = ComponentAccumulator()
    #
    # --- SiSpacePointMakerTool (public)
    #

    if flags.InDet.doSLHC:
        kwargs.setdefault("SCTGapParameter", 0.0015)

    if flags.Beam.Type == "cosmics" or flags.InDet.doBeamHalo:
        kwargs.setdefault("StripLengthTolerance", 0.05)
        kwargs.setdefault("UsePerpendicularProjection", True)
    
    InDetSiSpacePointMakerTool = CompFactory.InDet.SiSpacePointMakerTool(name = "InDetSiSpacePointMakerTool", **kwargs)
    acc.setPrivateTools(InDetSiSpacePointMakerTool)
    return acc

def InDetSiTrackerSpacePointFinderCfg(flags, name = "InDetSiTrackerSpacePointFinder", **kwargs):
    acc = ComponentAccumulator()
    #
    # SiTrackerSpacePointFinder algorithm
    #

    InDetSiSpacePointMakerTool = acc.popToolsAndMerge(SiSpacePointMakerToolCfg(flags))
    acc.addPublicTool(InDetSiSpacePointMakerTool) ## I decided to merge it

    kwargs.setdefault("SiSpacePointMakerTool", InDetSiSpacePointMakerTool)
    kwargs.setdefault("PixelsClustersName", 'PixelClusters') # InDetKeys.PixelClusters
    kwargs.setdefault("SCT_ClustersName", 'SCT_Clusters') # InDetKeys.SCT_Clusters
    kwargs.setdefault("SpacePointsPixelName", 'PixelSpacePoints') # InDetKeys.PixelSpacePoints
    kwargs.setdefault("SpacePointsSCTName", 'SCT_SpacePoints') # InDetKeys.SCT_SpacePoints
    kwargs.setdefault("SpacePointsOverlapName", 'OverlapSpacePoints') # InDetKeys.OverlapSpacePoints)
    kwargs.setdefault("ProcessPixels", flags.Detector.RecoPixel)
    kwargs.setdefault("ProcessSCTs", flags.Detector.RecoSCT)
    kwargs.setdefault("ProcessOverlaps", flags.Detector.RecoSCT)

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
        if (not flags.Detector.RecoPixel or not flags.InDet.doPixelPRDFormation):
            kwargs.setdefault("PixelClusterContainerName", "")
            kwargs.setdefault("SimDataMapNamePixel", "")
            kwargs.setdefault("TruthNamePixel", "")
        if (not flags.Detector.RecoSCT or not flags.InDet.doSCT_PRDFormation):
            kwargs.setdefault("SCTClusterContainerName", "")
            kwargs.setdefault("SimDataMapNameSCT", "")
            kwargs.setdefault("TruthNameSCT", "")

    InDetPRD_MultiTruthMakerSi = CompFactory.InDet.PRD_MultiTruthMaker(name = name, **kwargs)
    acc.addEventAlgo(InDetPRD_MultiTruthMakerSi)
    return acc

def InDetPRD_MultiTruthMakerSiPUCfg(flags, name="InDetPRD_MultiTruthMakerSiPU", **kwargs):
    acc = ComponentAccumulator()
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
        if (not flags.Detector.RecoPixel or not flags.InDet.doPixelPRDFormation):
            kwargs.setdefault("PixelClusterContainerName", "")
            kwargs.setdefault("SimDataMapNamePixel", "")
            kwargs.setdefault("TruthNamePixel", "")
        if (not flags.Detector.RecoSCT or not flags.InDet.doSCT_PRDFormation):
            kwargs.setdefault("SCTClusterContainerName", "")
            kwargs.setdefault("SimDataMapNameSCT", "")
            kwargs.setdefault("TruthNameSCT", "")

    InDetPRD_MultiTruthMakerSi = CompFactory.InDet.PRD_MultiTruthMaker(name = name, **kwargs)
    acc.addEventAlgo(InDetPRD_MultiTruthMakerSi)
    return acc


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files=defaultTestFiles.RDO

    ConfigFlags.Detector.RecoTRT = True
    ConfigFlags.Detector.RecoIBL = True

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

    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    top_acc.merge(MagneticFieldSvcCfg(ConfigFlags))

    from BeamSpotConditions.BeamSpotConditionsConfig import BeamSpotCondAlgCfg
    top_acc.merge(BeamSpotCondAlgCfg(ConfigFlags))

    from PixelGeoModel.PixelGeoModelConfig import PixelGeometryCfg
    top_acc.merge( PixelGeometryCfg(ConfigFlags) )

    from SCT_GeoModel.SCT_GeoModelConfig import SCT_GeometryCfg
    top_acc.merge(SCT_GeometryCfg(ConfigFlags))

    from PixelConditionsAlgorithms.PixelConditionsConfig import PixelDetectorElementCondAlgCfg, PixelAlignCondAlgCfg
    top_acc.merge(PixelAlignCondAlgCfg(ConfigFlags))
    top_acc.merge(PixelDetectorElementCondAlgCfg(ConfigFlags))

    from InDetConfig.TrackRecoConfig import PixelClusterizationCfg, SCTClusterizationCfg
    top_acc.merge(PixelClusterizationCfg(ConfigFlags))
    top_acc.merge(SCTClusterizationCfg(ConfigFlags))
    
    if ConfigFlags.InDet.doSpacePointFormation:
        top_acc.merge(InDetSiElementPropertiesTableCondAlgCfg(ConfigFlags))
        top_acc.merge(InDetSiTrackerSpacePointFinderCfg(ConfigFlags))

    if ConfigFlags.InDet.doPRDFormation and ConfigFlags.InDet.doSpacePointFormation:
        if ConfigFlags.InDet.doTruth:
            top_acc.merge(InDetPRD_MultiTruthMakerSiCfg(ConfigFlags))
            if ConfigFlags.InDet.doSplitReco:
                top_acc.merge(InDetPRD_MultiTruthMakerSiPUCfg(ConfigFlags))

    iovsvc = top_acc.getService('IOVDbSvc')
    iovsvc.OutputLevel=5

    top_acc.getService('StoreGateSvc').Dump = True
    top_acc.printConfig(withDetails = True, summariseProps = True)
    top_acc.run(25)
