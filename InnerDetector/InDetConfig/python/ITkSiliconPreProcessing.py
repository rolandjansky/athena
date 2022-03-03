# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType

def ITkSiElementPropertiesTableCondAlgCfg(flags, name="ITkSiElementPropertiesTableCondAlg", **kwargs):
    # For strip DetectorElementCollection used
    from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripReadoutGeometryCfg
    acc = ITkStripReadoutGeometryCfg(flags)
    
    kwargs.setdefault("ReadKey", "ITkStripDetectorElementCollection")
    kwargs.setdefault("WriteKey", "ITkStripElementPropertiesTable")

    acc.addCondAlgo(CompFactory.InDet.SiElementPropertiesTableCondAlg(name = name, **kwargs))
    return acc

def ITkSiSpacePointMakerToolCfg(flags, name="ITkSiSpacePointMakerTool", **kwargs):
    acc = ComponentAccumulator()
    #
    # --- SiSpacePointMakerTool (public)
    #

    kwargs.setdefault("SCTGapParameter", 0.0015)

    ITkSiSpacePointMakerTool = CompFactory.InDet.SiSpacePointMakerTool(name = "ITkSiSpacePointMakerTool", **kwargs)
    acc.setPrivateTools(ITkSiSpacePointMakerTool)
    return acc

def ITkSiTrackerSpacePointFinderCfg(flags, name = "ITkSiTrackerSpacePointFinder", **kwargs):
    #
    # SiTrackerSpacePointFinder algorithm
    #

    # For strip DetectorElementCollection used
    from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripReadoutGeometryCfg
    acc = ITkStripReadoutGeometryCfg(flags)

    ITkSiSpacePointMakerTool = acc.popToolsAndMerge(ITkSiSpacePointMakerToolCfg(flags))

    kwargs.setdefault("SiSpacePointMakerTool", ITkSiSpacePointMakerTool)
    kwargs.setdefault("PixelsClustersName", 'ITkPixelClusters')
    kwargs.setdefault("SCT_ClustersName", 'ITkStripClusters')
    kwargs.setdefault("SCTPropertiesKey", "ITkStripElementPropertiesTable")
    kwargs.setdefault("SCTDetEleCollKey", "ITkStripDetectorElementCollection")
    kwargs.setdefault("SpacePointsPixelName", 'ITkPixelSpacePoints')
    kwargs.setdefault("SpacePointsSCTName", 'ITkStripSpacePoints')
    kwargs.setdefault("SpacePointsOverlapName", 'ITkOverlapSpacePoints')
    kwargs.setdefault("ProcessPixels", flags.Detector.EnableITkPixel)
    kwargs.setdefault("ProcessSCTs", flags.Detector.EnableITkStrip and (not flags.ITk.Tracking.doFastTracking or flags.ITk.Tracking.doLargeD0))
    kwargs.setdefault("ProcessOverlaps", flags.Detector.EnableITkStrip and (not flags.ITk.Tracking.doFastTracking or flags.ITk.Tracking.doLargeD0))

    if flags.Beam.Type is BeamType.Cosmics:
        kwargs.setdefault("ProcessOverlaps", False)
        kwargs.setdefault("OverrideBeamSpot", True)
        kwargs.setdefault("VertexZ", 0)
        kwargs.setdefault("VertexX", 0)
        kwargs.setdefault("VertexY", 99999999)
        kwargs.setdefault("OverlapLimitOpposite", 5)

    acc.addEventAlgo(CompFactory.InDet.SiTrackerSpacePointFinder(name = name, **kwargs))
    return acc

def ITkPRD_MultiTruthMakerSiCfg(flags, name="ITkPRD_MultiTruthMakerSi", **kwargs):
    acc = ComponentAccumulator()

    # For pixel + strip DetectorElementCollection used
    if flags.Detector.GeometryITkPixel:
        from PixelGeoModelXml.ITkPixelGeoModelConfig import ITkPixelReadoutGeometryCfg
        acc.merge(ITkPixelReadoutGeometryCfg(flags))

    if flags.Detector.GeometryITkStrip:
        from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripReadoutGeometryCfg
        acc.merge(ITkStripReadoutGeometryCfg(flags))

    if flags.ITk.Tracking.doTruth:
        kwargs.setdefault("PixelClusterContainerName", 'ITkPixelClusters')
        kwargs.setdefault("SCTClusterContainerName", 'ITkStripClusters')
        kwargs.setdefault("TRTDriftCircleContainerName", "")
        kwargs.setdefault("SimDataMapNamePixel", 'ITkPixelSDO_Map')
        kwargs.setdefault("SimDataMapNameSCT", 'ITkStripSDO_Map')
        kwargs.setdefault("SimDataMapNameTRT", "")
        kwargs.setdefault("TruthNamePixel", 'PRD_MultiTruthITkPixel')
        kwargs.setdefault("TruthNameSCT", 'PRD_MultiTruthITkStrip')
        kwargs.setdefault("TruthNameTRT", "")
         # a bit complicated, but this is how the truth maker gets to know which detector is on
        if not flags.Detector.EnableITkPixel:
            kwargs.setdefault("PixelClusterContainerName", "")
            kwargs.setdefault("SimDataMapNamePixel", "")
            kwargs.setdefault("TruthNamePixel", "")
        if not flags.Detector.EnableITkStrip:
            kwargs.setdefault("SCTClusterContainerName", "")
            kwargs.setdefault("SimDataMapNameSCT", "")
            kwargs.setdefault("TruthNameSCT", "")

    kwargs.setdefault("PixelDetEleCollKey", "ITkPixelDetectorElementCollection")
    kwargs.setdefault("SCTDetEleCollKey", "ITkStripDetectorElementCollection")

    ITkPRD_MultiTruthMakerSi = CompFactory.InDet.PRD_MultiTruthMaker(name = name, **kwargs)
    acc.addEventAlgo(ITkPRD_MultiTruthMakerSi)
    return acc

def ITkPRD_MultiTruthMakerSiPUCfg(flags, name="ITkPRD_MultiTruthMakerSiPU", **kwargs):
    acc = ComponentAccumulator()

    # For pixel + strip DetectorElementCollection used
    if flags.Detector.GeometryITkPixel:
        from PixelGeoModelXml.ITkPixelGeoModelConfig import ITkPixelReadoutGeometryCfg
        acc.merge(ITkPixelReadoutGeometryCfg(flags))

    if flags.Detector.GeometryITkStrip:
        from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripReadoutGeometryCfg
        acc.merge(ITkStripReadoutGeometryCfg(flags))

    if flags.ITk.Tracking.doTruth:
        kwargs.setdefault("PixelClusterContainerName", 'ITkPixelPUClusters')
        kwargs.setdefault("SCTClusterContainerName", 'ITkStripPUClusters')
        kwargs.setdefault("TRTDriftCircleContainerName", "")
        kwargs.setdefault("SimDataMapNamePixel", 'ITkPixel_PU_SDO_Map')
        kwargs.setdefault("SimDataMapNameSCT", 'ITkStrip_PU_SDO_Map')
        kwargs.setdefault("SimDataMapNameTRT", "")
        kwargs.setdefault("TruthNamePixel", 'PRD_PU_MultiTruthITkPixel')
        kwargs.setdefault("TruthNameSCT", 'PRD_PU_MultiTruthITkStrip')
        kwargs.setdefault("TruthNameTRT", "")
         # a bit complicated, but this is how the truth maker gets to know which detector is on
        if not flags.Detector.EnableITkPixel:
            kwargs.setdefault("PixelClusterContainerName", "")
            kwargs.setdefault("SimDataMapNamePixel", "")
            kwargs.setdefault("TruthNamePixel", "")
        if not flags.Detector.EnableITkStrip:
            kwargs.setdefault("SCTClusterContainerName", "")
            kwargs.setdefault("SimDataMapNameSCT", "")
            kwargs.setdefault("TruthNameSCT", "")

    kwargs.setdefault("PixelDetEleCollKey", "ITkPixelDetectorElementCollection")
    kwargs.setdefault("SCTDetEleCollKey", "ITkStripDetectorElementCollection")

    ITkPRD_MultiTruthMakerSi = CompFactory.InDet.PRD_MultiTruthMaker(name = name, **kwargs)
    acc.addEventAlgo(ITkPRD_MultiTruthMakerSi)
    return acc

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

    #
    # ----------- form SpacePoints from clusters in SCT and Pixels
    #
    #
    acc.merge(ITkSiElementPropertiesTableCondAlgCfg(flags))
    acc.merge(ITkSiTrackerSpacePointFinderCfg(flags))

    # this truth must only be done if you do PRD and SpacePointformation
    # If you only do the latter (== running on ESD) then the needed input (simdata)
    # is not in ESD but the resulting truth (clustertruth) is already there ...
    if flags.ITk.Tracking.doTruth:
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
