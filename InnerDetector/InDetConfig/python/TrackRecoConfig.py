# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

##------------------------------------------------------------------------------
def BCM_ZeroSuppressionCfg(flags, name="InDetBCM_ZeroSuppression", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("BcmContainerName", "BCM_RDOs")
    algo = CompFactory.BCM_ZeroSuppression(name=name, **kwargs)
    acc.addEventAlgo(algo, primary = True)
    return acc

##------------------------------------------------------------------------------
def PixelClusterizationCfg(flags, name = "InDetPixelClusterization", **kwargs) :
    acc = ComponentAccumulator()
    merged_pixels_tool = acc.popToolsAndMerge(MergedPixelsToolCfg(flags, **kwargs))
    ambi_finder = acc.popToolsAndMerge(PixelGangedAmbiguitiesFinderCfg(flags))

    # Region selector tools for Pixel
    from RegionSelector.RegSelToolConfig import regSelTool_Pixel_Cfg
    RegSelTool_Pixel = acc.popToolsAndMerge(regSelTool_Pixel_Cfg(flags))

    kwargs.setdefault("clusteringTool", merged_pixels_tool)
    kwargs.setdefault("gangedAmbiguitiesFinder", ambi_finder)
    kwargs.setdefault("DataObjectName", "PixelRDOs")
    kwargs.setdefault("ClustersName", "PixelClusters")
    kwargs.setdefault("RegSelTool", RegSelTool_Pixel)

    acc.addEventAlgo(CompFactory.InDet.PixelClusterization(name=name, **kwargs))
    return acc
##------------------------------------------------------------------------------
def PixelClusterizationPUCfg(flags, name="InDetPixelClusterizationPU", **kwargs) :
    kwargs.setdefault("DataObjectName", "Pixel_PU_RDOs")
    kwargs.setdefault("ClustersName", "PixelPUClusters")
    kwargs.setdefault("AmbiguitiesMap", "PixelClusterAmbiguitiesMapPU")
    return PixelClusterizationCfg(flags, name=name, **kwargs)

##------------------------------------------------------------------------------
##------------------------------------------------------------------------------

def SCTClusterizationCfg(flags, name="InDetSCT_Clusterization", **kwargs) :
    acc = ComponentAccumulator()

    # Need to get SCT_ConditionsSummaryTool for e.g. SCT_ClusteringTool
    from SCT_ConditionsTools.SCT_ConditionsToolsConfig import SCT_ConditionsSummaryToolCfg
    InDetSCT_ConditionsSummaryToolWithoutFlagged = acc.popToolsAndMerge(SCT_ConditionsSummaryToolCfg(flags, withFlaggedCondTool=False))

    #### Clustering tool ######
    InDetClusterMakerTool = acc.popToolsAndMerge(ClusterMakerToolCfg(flags))
    InDetSCT_ClusteringTool = CompFactory.InDet.SCT_ClusteringTool( name           = "InDetSCT_ClusteringTool",
                                                                    globalPosAlg   = InDetClusterMakerTool,
                                                                    conditionsTool = InDetSCT_ConditionsSummaryToolWithoutFlagged)
    if flags.InDet.selectSCTIntimeHits :
       if flags.InDet.InDet25nsec :
          InDetSCT_ClusteringTool.timeBins = "01X"
       else:
          InDetSCT_ClusteringTool.timeBins = "X1X"

    kwargs.setdefault("clusteringTool", InDetSCT_ClusteringTool)
    kwargs.setdefault("DataObjectName", 'SCT_RDOs') ##InDetKeys.SCT_RDOs()
    kwargs.setdefault("ClustersName", 'SCT_Clusters') ##InDetKeys.SCT_Clusters()
    kwargs.setdefault("conditionsTool", InDetSCT_ConditionsSummaryToolWithoutFlagged)

    acc.addEventAlgo( CompFactory.InDet.SCT_Clusterization(name=name, **kwargs))

    return acc

##------------------------------------------------------------------------------
##------------------------------------------------------------------------------
##------------------------------------------------------------------------------

def SCTClusterizationPUCfg(flags, name="InDetSCT_ClusterizationPU", **kwargs) :
    kwargs.setdefault("DataObjectName", "SCT_PU_RDOs" )   #flags.InDetKeys.SCT_PU_RDOs
    kwargs.setdefault("ClustersName", "SCT_PU_Clusters")  #flags.InDetKeys.SCT_PU_Clusters
    return SCTClusterizationCfg(flags, name=name, **kwargs)

##------------------------------------------------------------------------------
def PixelGangedAmbiguitiesFinderCfg(flags) :
    from PixelGeoModel.PixelGeoModelConfig import PixelReadoutGeometryCfg
    acc = PixelReadoutGeometryCfg(flags)

    InDetPixelGangedAmbiguitiesFinder = CompFactory.InDet.PixelGangedAmbiguitiesFinder( name = "InDetPixelGangedAmbiguitiesFinder")
    acc.setPrivateTools( InDetPixelGangedAmbiguitiesFinder )
    return acc

##------------------------------------------------------------------------------
def MergedPixelsToolCfg(flags, **kwargs) :
      acc = ComponentAccumulator()
      # --- now load the framework for the clustering
      kwargs.setdefault("globalPosAlg", acc.popToolsAndMerge(ClusterMakerToolCfg(flags)) )

      # PixelClusteringToolBase uses PixelConditionsSummaryTool
      from PixelConditionsTools.PixelConditionsSummaryConfig import PixelConditionsSummaryCfg
      kwargs.setdefault("PixelConditionsSummaryTool", acc.popToolsAndMerge(PixelConditionsSummaryCfg(flags)) )

      # Enable duplcated RDO check for data15 because duplication mechanism was used.
      if len(flags.Input.ProjectName)>=6 and flags.Input.ProjectName[:6]=="data15":
          kwargs.setdefault("CheckDuplicatedRDO", True )

      InDetMergedPixelsTool = CompFactory.InDet.MergedPixelsTool(  name = "InDetMergedPixelsTool", **kwargs)

      acc.setPrivateTools(InDetMergedPixelsTool)
      return acc

##------------------------------------------------------------------------------
def ClusterMakerToolCfg(flags, name="InDetClusterMakerTool", **kwargs) :
    acc = ComponentAccumulator()

    # This directly needs the following Conditions data:
    # PixelChargeCalibCondData & PixelOfflineCalibData
    from PixelConditionsAlgorithms.PixelConditionsConfig import PixelChargeCalibCondAlgCfg, PixelOfflineCalibCondAlgCfg
    acc.merge(PixelChargeCalibCondAlgCfg(flags))
    acc.merge(PixelOfflineCalibCondAlgCfg(flags))

    from PixelReadoutGeometry.PixelReadoutGeometryConfig import PixelReadoutManagerCfg
    acc.merge(PixelReadoutManagerCfg(flags))

    from SiLorentzAngleTool.PixelLorentzAngleConfig import PixelLorentzAngleCfg
    PixelLorentzAngleTool = acc.popToolsAndMerge(PixelLorentzAngleCfg(flags))
    from SiLorentzAngleTool.SCT_LorentzAngleConfig import SCT_LorentzAngleCfg
    SCTLorentzAngleTool = acc.popToolsAndMerge( SCT_LorentzAngleCfg(flags) )

    kwargs.setdefault("PixelLorentzAngleTool", PixelLorentzAngleTool)
    kwargs.setdefault("SCTLorentzAngleTool", SCTLorentzAngleTool)

    InDetClusterMakerTool = CompFactory.InDet.ClusterMakerTool(name = name, **kwargs)
    acc.setPrivateTools(InDetClusterMakerTool)
    return acc


def TrackToVertexCfg(flags, name="AtlasTrackToVertexTool", **kwargs):
    result = ComponentAccumulator()
    if "Extrapolator" not in kwargs:
        from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
        kwargs["Extrapolator"] = result.popToolsAndMerge(AtlasExtrapolatorCfg(flags))
    from BeamSpotConditions.BeamSpotConditionsConfig import BeamSpotCondAlgCfg
    result.merge(BeamSpotCondAlgCfg(flags))
    result.setPrivateTools(CompFactory.Reco.TrackToVertex(name, **kwargs))
    return result

def TrackParticleCreatorToolCfg(flags, name="InDetxAODParticleCreatorTool", **kwargs):
    if flags.Detector.GeometryITk:
        name = name.replace("InDet", "ITk")
        from InDetConfig.ITkTrackRecoConfig import ITkTrackParticleCreatorToolCfg
        return ITkTrackParticleCreatorToolCfg(flags, name, **kwargs)

    result = ComponentAccumulator()
    if "TrackToVertex" not in kwargs:
        kwargs["TrackToVertex"] = result.popToolsAndMerge(TrackToVertexCfg(flags))
    if "TrackSummaryTool" not in kwargs:
        from InDetConfig.TrackingCommonConfig import InDetTrackSummaryToolSharedHitsCfg
        TrackSummaryTool = result.getPrimaryAndMerge(InDetTrackSummaryToolSharedHitsCfg(flags))
        kwargs["TrackSummaryTool"] = TrackSummaryTool
    p_expr = flags.InDet.perigeeExpression
    kwargs.setdefault("BadClusterID", flags.InDet.pixelClusterBadClusterID)
    kwargs.setdefault("KeepParameters", True)
    kwargs.setdefault("KeepFirstParameters", False)
    kwargs.setdefault(
        "PerigeeExpression",
        p_expr if p_expr != "Vertex" else "BeamLine")
    result.addPublicTool(CompFactory.Trk.TrackParticleCreatorTool(name, **kwargs), primary = True)
    return result

def TrackCollectionCnvToolCfg(flags, name="TrackCollectionCnvTool", TrackParticleCreator = None):
    if flags.Detector.GeometryITk:
        name = name.replace("InDet", "ITk")
        from InDetConfig.ITkTrackRecoConfig import ITkTrackCollectionCnvToolCfg
        return ITkTrackCollectionCnvToolCfg(flags, name, TrackParticleCreator)

    result = ComponentAccumulator()
    if TrackParticleCreator is None:
        TrackParticleCreator = result.getPrimaryAndMerge(TrackParticleCreatorToolCfg(flags))
    result.setPrivateTools(CompFactory.xAODMaker.TrackCollectionCnvTool(
        name,
        TrackParticleCreator=TrackParticleCreator,
    ))
    return result

def TrackCollectionMergerAlgCfg(flags, name="InDetTrackCollectionMerger", InputCombinedTracks=None, **kwargs):
    result = ComponentAccumulator()

    kwargs.setdefault("TracksLocation", InputCombinedTracks)
    kwargs.setdefault("OutputTracksLocation", "CombinedInDetTracks")
    from InDetConfig.TrackingCommonConfig import InDetPRDtoTrackMapToolGangedPixelsCfg
    InDetPRDtoTrackMapToolGangedPixels = result.popToolsAndMerge(InDetPRDtoTrackMapToolGangedPixelsCfg(flags))
    kwargs.setdefault("AssociationTool", InDetPRDtoTrackMapToolGangedPixels)
    kwargs.setdefault("AssociationMapName", "PRDtoTrackMapCombinedInDetTracks")
    kwargs.setdefault("UpdateSharedHits", True)
    kwargs.setdefault("UpdateAdditionalInfo", True)
    from InDetConfig.TrackingCommonConfig import InDetTrackSummaryToolSharedHitsCfg
    TrackSummaryTool = result.getPrimaryAndMerge(InDetTrackSummaryToolSharedHitsCfg(flags))
    kwargs.setdefault("SummaryTool", TrackSummaryTool)

    result.addEventAlgo(CompFactory.Trk.TrackCollectionMerger(name, **kwargs))
    return result

def TrackParticleCnvAlgCfg(flags, name="TrackParticleCnvAlg", TrackContainerName="CombinedInDetTracks", OutputTrackParticleContainer="InDetTrackParticles", **kwargs):
    if flags.Detector.GeometryITk:
        name = name.replace("InDet", "ITk")
        from InDetConfig.ITkTrackRecoConfig import ITkTrackParticleCnvAlgCfg
        return ITkTrackParticleCnvAlgCfg(flags, name, TrackContainerName, OutputTrackParticleContainer, **kwargs)

    result = ComponentAccumulator()
    kwargs.setdefault("ConvertTracks", True)
    kwargs.setdefault("ConvertTrackParticles", False)
    kwargs.setdefault("TrackContainerName", TrackContainerName)
    kwargs.setdefault("xAODContainerName", OutputTrackParticleContainer)
    kwargs.setdefault("xAODTrackParticlesFromTracksContainerName", OutputTrackParticleContainer)
    if "TrackParticleCreator" not in kwargs:
        kwargs["TrackParticleCreator"] = result.getPrimaryAndMerge(TrackParticleCreatorToolCfg(flags))
    if "TrackCollectionCnvTool" not in kwargs:
        kwargs["TrackCollectionCnvTool"] = result.popToolsAndMerge(TrackCollectionCnvToolCfg(
            flags,
            TrackParticleCreator=kwargs["TrackParticleCreator"],
        ))

    if flags.InDet.doTruth:
        kwargs.setdefault("TrackTruthContainerName", f"{TrackContainerName}TruthCollection")
        kwargs.setdefault("AddTruthLink", True)

        if "MCTruthClassifier" not in kwargs:
            from MCTruthClassifier.MCTruthClassifierConfig import MCTruthClassifierCfg
            kwargs["MCTruthClassifier"] = result.popToolsAndMerge(
                MCTruthClassifierCfg(flags))

    else:
        kwargs.setdefault("AddTruthLink", False)
    result.addEventAlgo(CompFactory.xAODMaker.TrackParticleCnvAlg(name, **kwargs))
    return result

def InDetTrackRecoCfg(flags):
    if flags.Detector.GeometryITk:
        from InDetConfig.ITkTrackRecoConfig import ITkTrackRecoCfg
        return ITkTrackRecoCfg(flags)

    """Configures complete ID tracking """
    result = ComponentAccumulator()

    if flags.Input.Format == "BS":
        from PixelRawDataByteStreamCnv.PixelRawDataByteStreamCnvConfig import PixelRawDataProviderAlgCfg
        result.merge(PixelRawDataProviderAlgCfg(flags))
        from SCT_RawDataByteStreamCnv.SCT_RawDataByteStreamCnvConfig import SCTRawDataProviderCfg, SCTEventFlagWriterCfg
        result.merge(SCTRawDataProviderCfg(flags))
        result.merge(SCTEventFlagWriterCfg(flags))
        from TRT_RawDataByteStreamCnv.TRT_RawDataByteStreamCnvConfig import TRTRawDataProviderCfg
        result.merge(TRTRawDataProviderCfg(flags))

    from InDetConfig.SiliconPreProcessing import InDetRecPreProcessingSiliconCfg
    result.merge(InDetRecPreProcessingSiliconCfg(flags))

    ClusterSplitProbContainer=''
    InputCombinedInDetTracks = []

    from InDetConfig.TrackingSiPatternConfig import TrackingSiPatternCfg
    from InDetConfig.TrackTruthConfig import InDetTrackTruthCfg

    # ------------------------------------------------------------
    #
    # ----------- Subdetector pattern from New Tracking
    #
    # ------------------------------------------------------------

    # Pixel track segment finding
    if flags.InDet.Tracking.doTrackSegmentsPixel:
        flagsPixel = flags.cloneAndReplace("InDet.Tracking.Pass", "InDet.Tracking.PixelPass")
        PixelTrackContainer = "ResolvedPixelTracks"
    
        result.merge(TrackingSiPatternCfg(flagsPixel,
                                          InputCollections = [],
                                          ResolvedTrackCollectionKey = PixelTrackContainer,
                                          SiSPSeededTrackCollectionKey = "SiSPSeededPixelTracks",
                                          ClusterSplitProbContainer = ClusterSplitProbContainer))
        ClusterSplitProbContainer = "InDetAmbiguityProcessorSplitProb" + flagsPixel.Tracking.Pass.extension

        if flags.InDet.doTruth:
            result.merge(InDetTrackTruthCfg(flagsPixel,
                                            Tracks = PixelTrackContainer,
                                            DetailedTruth = PixelTrackContainer+"DetailedTruth",
                                            TracksTruth = PixelTrackContainer+"TruthCollection"))

    # SCT track segment finding
    if flags.InDet.Tracking.doTrackSegmentsSCT:
        flagsSCT = flags.cloneAndReplace("InDet.Tracking.Pass", "InDet.TrackingSCTPass")
        SCTTrackContainer = "ResolvedSCTTracks"

        result.merge(TrackingSiPatternCfg(flagsSCT,
                                          InputCollections = [],
                                          ResolvedTrackCollectionKey = SCTTrackContainer,
                                          SiSPSeededTrackCollectionKey = "SiSPSeededSCTTracks",
                                          ClusterSplitProbContainer = ClusterSplitProbContainer))
        ClusterSplitProbContainer = "InDetAmbiguityProcessorSplitProb" + flagsSCT.Tracking.Pass.extension

    # TRT track segment finding
    if flags.InDet.Tracking.doTrackSegmentsTRT:
        flagsTRT = flags.cloneAndReplace("InDet.Tracking.Pass", "InDet.TrackingTRTPass")
        from InDetConfig.TRTSegmentFindingConfig import TRTSegmentFindingCfg
        result.merge(TRTSegmentFindingCfg(flagsTRT,
                                          extension = "_TRT",
                                          InputCollections = [],
                                          BarrelSegments = "TRTSegmentsTRT"))

        from InDetConfig.TRTStandaloneConfig import TRTStandaloneCfg
        result.merge(TRTStandaloneCfg(flagsTRT,
                                      extension = "_TRT",
                                      InputCollections = [],
                                      BarrelSegments = "TRTSegmentsTRT"))

    # ------------------------------------------------------------
    #
    # ----------- Main passes for standard reconstruction
    #
    # ------------------------------------------------------------

    if flags.InDet.Tracking.doHighPileup:
        flags = flags.cloneAndReplace("InDet.Tracking.Pass", "InDet.Tracking.HighPileupPass")

    result.merge(TrackingSiPatternCfg(flags,
                                      InputCollections = [],
                                      ResolvedTrackCollectionKey = "ResolvedTracks",
                                      SiSPSeededTrackCollectionKey = "SiSPSeededTracks",
                                      ClusterSplitProbContainer = ClusterSplitProbContainer))
    ClusterSplitProbContainer = "InDetAmbiguityProcessorSplitProb"

    # TRT extension
    if flags.InDet.Tracking.doTRTExtension:
        from InDetConfig.TRTExtensionConfig import NewTrackingTRTExtensionCfg
        result.merge(NewTrackingTRTExtensionCfg(flags,
                                                SiTrackCollection = "ResolvedTracks",
                                                ExtendedTrackCollection = "ExtendedTracks",
                                                ExtendedTracksMap = "ExtendedTracksMap",
                                                doPhase=False))
        InputCombinedInDetTracks += ["ExtendedTracks"]
    else:
        InputCombinedInDetTracks += ["ResolvedTracks"]

    # TRT segments
    # Different from TRT track segments at the top as takes as input previously built track collections
    if flags.InDet.Tracking.doTRTSegments:
        from InDetConfig.TRTSegmentFindingConfig import TRTSegmentFindingCfg
        result.merge(TRTSegmentFindingCfg(flags,
                                          extension = "",
                                          InputCollections = InputCombinedInDetTracks,
                                          BarrelSegments = "TRTSegments"))

    # BackTracking
    if flags.InDet.Tracking.doBackTracking:
        from InDetConfig.BackTrackingConfig import BackTrackingCfg
        result.merge(BackTrackingCfg(flags,
                                     InputCollections = InputCombinedInDetTracks,
                                     ClusterSplitProbContainer = ClusterSplitProbContainer))
        ClusterSplitProbContainer = "InDetTRT_SeededAmbiguityProcessorSplitProb"
        InputCombinedInDetTracks += ["ResolvedTRTSeededTracks"]

    # LRT
    ClusterSplitProbContainerLargeD0 = ""
    if flags.InDet.Tracking.doLargeD0 or flags.InDet.Tracking.doR3LargeD0 or flags.InDet.Tracking.doLowPtLargeD0:

        if flags.InDet.Tracking.doLowPtLargeD0:
            flagsLRT = flags.cloneAndReplace("InDet.Tracking.Pass", "InDet.Tracking.LowPtLargeD0Pass")
        elif flags.InDet.Tracking.doR3LargeD0:
            flagsLRT = flags.cloneAndReplace("InDet.Tracking.Pass", "InDet.Tracking.R3LargeD0Pass")
        else:
            flagsLRT = flags.cloneAndReplace("InDet.Tracking.Pass", "InDet.Tracking.LargeD0Pass")

        LRTTrackContainer = "ResolvedLargeD0Tracks"
        result.merge(TrackingSiPatternCfg(flagsLRT,
                                          InputCollections = InputCombinedInDetTracks,
                                          ResolvedTrackCollectionKey = LRTTrackContainer,
                                          SiSPSeededTrackCollectionKey = "SiSpSeededLargeD0Tracks",
                                          ClusterSplitProbContainer = ClusterSplitProbContainer))
        ClusterSplitProbContainerLargeD0 = "InDetAmbiguityProcessorSplitProb" + flagsLRT.InDet.Tracking.Pass.extension

        if flags.InDet.Tracking.doTRTExtension:
            result.merge(NewTrackingTRTExtensionCfg(flagsLRT,
                                                    SiTrackCollection="ResolvedLargeD0Tracks",
                                                    ExtendedTrackCollection="ExtendedLargeD0Tracks",
                                                    ExtendedTracksMap="ExtendedTracksMapLargeD0",
                                                    doPhase=False))
            LRTTrackContainer = "ExtendedLargeD0Tracks"

        if flags.InDet.Tracking.storeSeparateLargeD0Container:
            if flags.InDet.doTruth:
                result.merge(InDetTrackTruthCfg(flagsLRT,
                                                Tracks = LRTTrackContainer,
                                                DetailedTruth = LRTTrackContainer+"DetailedTruth",
                                                TracksTruth = LRTTrackContainer+"TruthCollection"))
            result.merge(TrackParticleCnvAlgCfg(flagsLRT,
                                                name="LargeD0TrackParticleCnvAlg",
                                                TrackContainerName=LRTTrackContainer,
                                                OutputTrackParticleContainer="InDetLargeD0TrackParticles"))
        else:
            ClusterSplitProbContainer = ClusterSplitProbContainerLargeD0
            InputCombinedInDetTracks += [LRTTrackContainer]

    # Low pt
    if flags.InDet.Tracking.doLowPt:
        flagsLowPt = flags.cloneAndReplace("InDet.Tracking.Pass", "InDet.Tracking.LowPtPass")
        LowPtTrackContainer = "ResolvedLowPtTracks"
        result.merge(TrackingSiPatternCfg(flagsLowPt,
                                          InputCollections = InputCombinedInDetTracks,
                                          ResolvedTrackCollectionKey = LowPtTrackContainer,
                                          SiSPSeededTrackCollectionKey = "SiSpSeededLowPtTracks",
                                          ClusterSplitProbContainer = ClusterSplitProbContainer))
        ClusterSplitProbContainer = "InDetAmbiguityProcessorSplitProb"+flagsLowPt.InDet.Tracking.Pass.extension

        if flags.InDet.Tracking.doTRTExtension:
            result.merge(NewTrackingTRTExtensionCfg(flagsLowPt,
                                                    SiTrackCollection = LowPtTrackContainer,
                                                    ExtendedTrackCollection = "ExtendedLowPtTracks",
                                                    ExtendedTracksMap = "ExtendedTracksMapLowPt",
                                                    doPhase = False))
            LowPtTrackContainer = "ExtendedLowPtTracks"

        InputCombinedInDetTracks += [LowPtTrackContainer]

    # Very low pt
    if flags.InDet.Tracking.doVeryLowPt:
        flagsVeryLowPt = flags.cloneAndReplace("InDet.Tracking.Pass", "InDet.Tracking.VeryLowPtPass")
        result.merge(TrackingSiPatternCfg(flagsVeryLowPt,
                                          InputCollections = InputCombinedInDetTracks,
                                          ResolvedTrackCollectionKey = "ResolvedVeryLowPtTracks",
                                          SiSPSeededTrackCollectionKey = "SiSpSeededVeryLowPtTracks",
                                          ClusterSplitProbContainer = ClusterSplitProbContainer))
        ClusterSplitProbContainer = "InDetAmbiguityProcessorSplitProb"+flagsVeryLowPt.InDet.Tracking.Pass.extension
        InputCombinedInDetTracks += ["ResolvedVeryLowPtTracks"]

    # TRT standalone
    # Different from TRT track segments at the top as takes as input previously built track collections
    if flags.InDet.Tracking.doTRTStandalone:
        flagsTRTStandalone = flags.cloneAndReplace("InDet.Tracking.Pass", "InDet.Tracking.TRTStandalonePass")
        from InDetConfig.TRTStandaloneConfig import TRTStandaloneCfg
        result.merge(TRTStandaloneCfg(flagsTRTStandalone,
                                      extension = "",
                                      InputCollections = InputCombinedInDetTracks,
                                      BarrelSegments = "TRTSegments"))
        InputCombinedInDetTracks += ["TRTStandaloneTracks"]

    # Forward tracklets
    if flags.InDet.Tracking.doForwardTracks:
        # Add tracks that are not saved to the InputCombinedInDetTracks
        InputForwardInDetTracks = []
        InputForwardInDetTracks += InputCombinedInDetTracks
        if flags.InDet.Tracking.storeSeparateLargeD0Container:
            InputForwardInDetTracks += ["ExtendedLargeD0Tracks"]

        flagsForward = flags.cloneAndReplace("InDet.Tracking.Pass", "InDet.Tracking.ForwardTracksPass")
        ForwardTrackContainer = "ResolvedForwardTracks"
        result.merge(TrackingSiPatternCfg(flagsForward,
                                          InputCollections = InputForwardInDetTracks,
                                          ResolvedTrackCollectionKey = ForwardTrackContainer,
                                          SiSPSeededTrackCollectionKey = "SiSpSeededForwardTracks",
                                          ClusterSplitProbContainer = ClusterSplitProbContainer))
        InputForwardInDetTracks += [ForwardTrackContainer]
        ClusterSplitProbContainer = "InDetAmbiguityProcessorSplitProb"+flagsForward.InDet.Tracking.Pass.extension

        if flags.InDet.doTruth:
            from InDetConfig.TrackTruthConfig import InDetTrackTruthCfg
            result.merge(InDetTrackTruthCfg(flagsForward,
                                            Tracks = ForwardTrackContainer,
                                            DetailedTruth = ForwardTrackContainer+"DetailedTruth",
                                            TracksTruth = ForwardTrackContainer+"TruthCollection"))
        result.merge(TrackParticleCnvAlgCfg(flagsForward,
                                            name = "ForwardTrackParticleCnvAlg",
                                            TrackContainerName = ForwardTrackContainer,
                                            OutputTrackParticleContainer = "InDetForwardTrackParticles"))

    if flags.InDet.Tracking.doTrackSegmentsDisappearing:
        InputPixelInDetTracks = []
        InputPixelInDetTracks += InputCombinedInDetTracks
        # Add tracks that are not saved to the InputCombinedInDetTracks
        if flags.InDet.Tracking.doForwardTracks:
            InputPixelInDetTracks += ["ResolvedForwardTracks"]
        if flags.InDet.Tracking.storeSeparateLargeD0Container:
            InputPixelInDetTracks += ["ExtendedLargeD0Tracks"]

        flagsDisappearing = flags.cloneAndReplace("InDet.Tracking.Pass", "InDet.Tracking.DisappearingPass")
        DisappearingTrackContainer = "ResolvedDisappearingTracks"
        result.merge(TrackingSiPatternCfg(flagsDisappearing,
                                          InputCollections = InputPixelInDetTracks,
                                          ResolvedTrackCollectionKey = DisappearingTrackContainer,
                                          SiSPSeededTrackCollectionKey = "SiSpSeededDisappearingTracks",
                                          ClusterSplitProbContainer = ClusterSplitProbContainer))
        ClusterSplitProbContainer = "InDetAmbiguityProcessorSplitProb"+flagsDisappearing.InDet.Tracking.Pass.extension

        if flags.InDet.Tracking.doTRTExtension:
            result.merge(NewTrackingTRTExtensionCfg(flagsDisappearing,
                                                    SiTrackCollection = DisappearingTrackContainer,
                                                    ExtendedTrackCollection = "ExtendedTracksDisappearing",
                                                    ExtendedTracksMap = "ExtendedTracksMapDisappearing",
                                                    doPhase = False))
            DisappearingTrackContainer = "ExtendedTracksDisappearing"

        if flags.InDet.doTruth:
            from InDetConfig.TrackTruthConfig import InDetTrackTruthCfg
            result.merge(InDetTrackTruthCfg(flagsDisappearing,
                                            Tracks = DisappearingTrackContainer,
                                            DetailedTruth = DisappearingTrackContainer+"DetailedTruth",
                                            TracksTruth = DisappearingTrackContainer+"TruthCollection"))
        result.merge(TrackParticleCnvAlgCfg(flagsDisappearing,
                                            name = "DisappearingTrackParticleCnvAlg",
                                            TrackContainerName = DisappearingTrackContainer,
                                            OutputTrackParticleContainer = "InDetDisappearingTrackParticles"))

    # Beam gas
    if flags.InDet.Tracking.doBeamGas:
        flagsBeamGas = flags.cloneAndReplace("InDet.Tracking.Pass", "InDet.Tracking.BeamGasPass")
        BeamGasTrackContainer = "ResolvedBeamGasTracks"
        result.merge(TrackingSiPatternCfg(flagsBeamGas,
                                          InputCollections = InputCombinedInDetTracks,
                                          ResolvedTrackCollectionKey = BeamGasTrackContainer,
                                          SiSPSeededTrackCollectionKey = "SiSpSeededBeamGasTracks",
                                          ClusterSplitProbContainer = ClusterSplitProbContainer))
        ClusterSplitProbContainer = "InDetAmbiguityProcessorSplitProb"+flagsBeamGas.InDet.Tracking.Pass.extension

        if flags.InDet.Tracking.doTRTExtension:
            result.merge(NewTrackingTRTExtensionCfg(flagsLowPt,
                                                    SiTrackCollection = BeamGasTrackContainer,
                                                    ExtendedTrackCollection = "ExtendedBeamGasTracks",
                                                    ExtendedTracksMap = "ExtendedTracksMapBeamGas",
                                                    doPhase = False))
            LowPtTrackContainer = "ExtendedBeamGasTracks"
        InputCombinedInDetTracks += [LowPtTrackContainer]

    result.merge(TrackCollectionMergerAlgCfg(flags, InputCombinedTracks=InputCombinedInDetTracks))

    if flags.InDet.doTruth:
        from InDetConfig.TrackTruthConfig import InDetTrackTruthCfg
        result.merge(InDetTrackTruthCfg(flags))

    # TODO add followup algs
    result.merge(TrackParticleCnvAlgCfg(flags))

    if flags.InDet.PriVertex.doVertexFinding:
        from InDetConfig.VertexFindingConfig import primaryVertexFindingCfg
        result.merge(primaryVertexFindingCfg(flags))

    from OutputStreamAthenaPool.OutputStreamConfig import addToESD,addToAOD
    toAOD = ["xAOD::TrackParticleContainer#InDetTrackParticles", "xAOD::TrackParticleAuxContainer#InDetTrackParticlesAux."]
    toESD = []
    result.merge(addToESD(flags, toAOD+toESD))
    result.merge(addToAOD(flags, toAOD))
    return result


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    # Disable calo for this test
    ConfigFlags.Detector.EnableCalo = False

    # TODO add these flags in future
    # ConfigFlags.addFlag('InDet.doAmbiSolving', True)
    # ConfigFlags.addFlag('InDet.useHolesFromPattern', False)
    # ConfigFlags.addFlag('InDet.holeSearchInGX2Fit', True)

    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.RDO
    ConfigFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    top_acc = MainServicesCfg(ConfigFlags)

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    top_acc.merge(PoolReadCfg(ConfigFlags))

    #######################################################################
    #################### Additional Configuration  ########################
    if "EventInfo" not in ConfigFlags.Input.Collections:
        from xAODEventInfoCnv.xAODEventInfoCnvConfig import EventInfoCnvAlgCfg
        top_acc.merge(EventInfoCnvAlgCfg(ConfigFlags))

    if ConfigFlags.Input.isMC:
        from xAODTruthCnv.xAODTruthCnvConfigNew import GEN_AOD2xAODCfg
        top_acc.merge(GEN_AOD2xAODCfg(ConfigFlags))

    top_acc.merge(InDetTrackRecoCfg(ConfigFlags))
    from AthenaCommon.Constants import DEBUG
    top_acc.foreach_component("AthEventSeq/*").OutputLevel=DEBUG
    top_acc.printConfig(withDetails=True, summariseProps=True)
    top_acc.store(open("TrackRecoConfig.pkl", "wb"))

    import sys
    if "--norun" not in sys.argv:
        sc = top_acc.run(1)
        if sc.isFailure():
            sys.exit(-1)
