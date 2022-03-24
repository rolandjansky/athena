# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType, Format

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
       if flags.Beam.BunchSpacing<=25 and flags.Beam.Type is BeamType.Collisions:
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
    kwargs.setdefault("BadClusterID", 3) # Select the mode to identify suspicous pixel cluster
    kwargs.setdefault("KeepParameters", True)
    kwargs.setdefault("KeepFirstParameters", False)
    # need to treat Vertex specifically because at the time of
    # the track particle creation the primary vertex does not yet exist.
    # The problem is solved by first creating track particles wrt. the beam line
    # and correcting the parameters after the vertex finding.
    kwargs.setdefault("PerigeeExpression", "BeamLine" if flags.InDet.Tracking.perigeeExpression=="Vertex"
                      else flags.InDet.Tracking.perigeeExpression)
    result.addPublicTool(CompFactory.Trk.TrackParticleCreatorTool(name, **kwargs), primary = True)
    return result

def RecTrackParticleContainerCnvToolCfg(flags, name="RecTrackParticleContainerCnvTool", TrackParticleCreator = None):
    result = ComponentAccumulator()
    if TrackParticleCreator is None:
        TrackParticleCreator = result.getPrimaryAndMerge(TrackParticleCreatorToolCfg(flags))
    result.setPrivateTools(CompFactory.xAODMaker.RecTrackParticleContainerCnvTool(name,
                                                                                  TrackParticleCreator=TrackParticleCreator,
    ))
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

def TrackCollectionMergerAlgCfg(flags, name="InDetTrackCollectionMerger", InputCombinedTracks=None, CombinedInDetClusterSplitProbContainer=None, **kwargs):
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
    TrackSummaryTool = result.getPrimaryAndMerge(InDetTrackSummaryToolSharedHitsCfg(flags, name="CombinedInDetSplitProbTrackSummaryToolSharedHits"))
    TrackSummaryTool.InDetSummaryHelperTool.ClusterSplitProbabilityName = CombinedInDetClusterSplitProbContainer
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

# Configuration not supported, to be recommissioned if needed
def ReFitTrackAlgCfg(flags, name="InDetRefitTrack", InputTrackCollection="CombinedInDetTracks", OutputTrackCollection="RefittedTracks", **kwargs):
    result = ComponentAccumulator()

    from InDetConfig.TrackingCommonConfig import InDetTrackFitterCfg, InDetTrackFitterTRTCfg, InDetTrackSummaryToolSharedHitsCfg, InDetPRDtoTrackMapToolGangedPixelsCfg
    InDetTrackFitter = result.popToolsAndMerge(InDetTrackFitterCfg(flags))
    InDetTrackFitterTRT = result.popToolsAndMerge(InDetTrackFitterTRTCfg(flags))
    TrackSummaryTool = result.getPrimaryAndMerge(InDetTrackSummaryToolSharedHitsCfg(flags))
    InDetPRDtoTrackMapToolGangedPixels = result.popToolsAndMerge(InDetPRDtoTrackMapToolGangedPixelsCfg(flags))
    kwargs.setdefault("FitterTool", InDetTrackFitter)
    kwargs.setdefault("FitterToolTRT", InDetTrackFitterTRT)
    kwargs.setdefault("SummaryTool", TrackSummaryTool)
    kwargs.setdefault("AssociationTool", InDetPRDtoTrackMapToolGangedPixels)
    kwargs.setdefault("TrackName", InputTrackCollection)
    kwargs.setdefault("NewTrackName", OutputTrackCollection)
    kwargs.setdefault("useParticleHypothesisFromTrack", True)
    kwargs.setdefault("matEffects", flags.InDet.materialInteractionsType if flags.InDet.materialInteractions else 0)

    result.addEventAlgo(CompFactory.Trk.ReFitTrack(name, **kwargs))
    return result

def CombinedTrackingPassFlagSets(flags):

    flags_set = []

    # Primary Pass
    if flags.InDet.Tracking.doDBMstandalone:
        flags = flags.cloneAndReplace("InDet.Tracking.ActivePass", "InDet.Tracking.DBMPass")
    elif flags.InDet.Tracking.doVtxLumi:
        flags = flags.cloneAndReplace("InDet.Tracking.ActivePass", "InDet.Tracking.VtxLumiPass")
    elif flags.InDet.Tracking.doVtxBeamSpot:
        flags = flags.cloneAndReplace("InDet.Tracking.ActivePass", "InDet.Tracking.VtxBeamSpotPass")
    elif flags.Beam.Type is BeamType.Cosmics:
        flags = flags.cloneAndReplace("InDet.Tracking.ActivePass", "InDet.Tracking.CosmicsPass")
    elif flags.Reco.EnableHI:
        flags = flags.cloneAndReplace("InDet.Tracking.ActivePass", "InDet.Tracking.HeavyIonPass")
    elif flags.InDet.Tracking.doHighPileup:
        flags = flags.cloneAndReplace("InDet.Tracking.ActivePass", "InDet.Tracking.HighPileupPass")
    elif flags.InDet.Tracking.doMinBias:
        flags = flags.cloneAndReplace("InDet.Tracking.ActivePass", "InDet.Tracking.MinBiasPass")
    elif flags.InDet.Tracking.doRobustReco:
        flags = flags.cloneAndReplace("InDet.Tracking.ActivePass", "InDet.Tracking.RobustRecoPass")

    flags_set += [flags]

    # LRT pass
    if flags.InDet.Tracking.doLargeD0 or flags.InDet.Tracking.doR3LargeD0 or flags.InDet.Tracking.doLowPtLargeD0:

        if flags.InDet.Tracking.doLowPtLargeD0:
            flagsLRT = flags.cloneAndReplace("InDet.Tracking.ActivePass", "InDet.Tracking.LowPtLargeD0Pass")
        elif flags.InDet.Tracking.doR3LargeD0:
            flagsLRT = flags.cloneAndReplace("InDet.Tracking.ActivePass", "InDet.Tracking.R3LargeD0Pass")
        else:
            flagsLRT = flags.cloneAndReplace("InDet.Tracking.ActivePass", "InDet.Tracking.LargeD0Pass")

        flags_set += [flagsLRT]

    # LowPt pass
    if flags.InDet.Tracking.doLowPt:
        flagsLowPt = flags.cloneAndReplace("InDet.Tracking.ActivePass", "InDet.Tracking.LowPtPass")
        flags_set += [flagsLowPt]

    # VeryLowPt pass
    if flags.InDet.Tracking.doVeryLowPt:
        flagsVeryLowPt = flags.cloneAndReplace("InDet.Tracking.ActivePass", "InDet.Tracking.VeryLowPtPass")
        flags_set += [flagsVeryLowPt]

    # TRT standalone pass
    if flags.InDet.Tracking.doTRTStandalone:
        flagsTRTStandalone = flags.cloneAndReplace("InDet.Tracking.ActivePass", "InDet.Tracking.TRTStandalonePass")
        flags_set += [flagsTRTStandalone]

    # Forward tracklets
    if flags.InDet.Tracking.doForwardTracks:
        flagsForward = flags.cloneAndReplace("InDet.Tracking.ActivePass", "InDet.Tracking.ForwardPass")
        flags_set += [flagsForward]

    # Disappearing pixel tracklets
    if flags.InDet.Tracking.doTrackSegmentsDisappearing:
        flagsDisappearing = flags.cloneAndReplace("InDet.Tracking.ActivePass", "InDet.Tracking.DisappearingPass")
        flags_set += [flagsDisappearing]

    # Beam gas
    if flags.InDet.Tracking.doBeamGas:
        flagsBeamGas = flags.cloneAndReplace("InDet.Tracking.ActivePass", "InDet.Tracking.BeamGasPass")
        flags_set += [flagsBeamGas]

    return flags_set



def InDetTrackRecoCfg(flags):
    if flags.Detector.GeometryITk:
        from InDetConfig.ITkTrackRecoConfig import ITkTrackRecoCfg
        return ITkTrackRecoCfg(flags)

    """Configures complete ID tracking """
    result = ComponentAccumulator()

    if flags.Input.Format is Format.BS:
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
        flagsPixel = flags.cloneAndReplace("InDet.Tracking.ActivePass", "InDet.Tracking.PixelPass")
        PixelTrackContainer = "ResolvedPixelTracks"
    
        result.merge(TrackingSiPatternCfg(flagsPixel,
                                          InputCollections = [],
                                          ResolvedTrackCollectionKey = PixelTrackContainer,
                                          SiSPSeededTrackCollectionKey = "SiSPSeededPixelTracks",
                                          ClusterSplitProbContainer = ClusterSplitProbContainer))
        ClusterSplitProbContainer = "InDetAmbiguityProcessorSplitProb" + flagsPixel.Tracking.ActivePass.extension

        if flags.InDet.doTruth:
            result.merge(InDetTrackTruthCfg(flagsPixel,
                                            Tracks = PixelTrackContainer,
                                            DetailedTruth = PixelTrackContainer+"DetailedTruth",
                                            TracksTruth = PixelTrackContainer+"TruthCollection"))

    # SCT track segment finding
    if flags.InDet.Tracking.doTrackSegmentsSCT:
        flagsSCT = flags.cloneAndReplace("InDet.Tracking.ActivePass", "InDet.TrackingSCTPass")
        SCTTrackContainer = "ResolvedSCTTracks"

        result.merge(TrackingSiPatternCfg(flagsSCT,
                                          InputCollections = [],
                                          ResolvedTrackCollectionKey = SCTTrackContainer,
                                          SiSPSeededTrackCollectionKey = "SiSPSeededSCTTracks",
                                          ClusterSplitProbContainer = ClusterSplitProbContainer))
        ClusterSplitProbContainer = "InDetAmbiguityProcessorSplitProb" + flagsSCT.Tracking.ActivePass.extension

    # TRT track segment finding
    if flags.InDet.Tracking.doTrackSegmentsTRT:
        flagsTRT = flags.cloneAndReplace("InDet.Tracking.ActivePass", "InDet.Tracking.TRTPass")
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

    flags_set = CombinedTrackingPassFlagSets(flags)
    InputCombinedInDetTracks = [] # Tracks to be ultimately merged in InDetTrackParticle collection
    InputExtendedInDetTracks = [] # Includes also tracks which end in standalone TrackParticle collections
    ClusterSplitProbContainer = ""
    isPrimaryPass = True

    for current_flags in flags_set:

        extension = current_flags.InDet.Tracking.ActivePass.extension

        if flags.InDet.Tracking.doTRTStandalone and extension=="TRTStandalone":
            from InDetConfig.TRTStandaloneConfig import TRTStandaloneCfg
            result.merge(TRTStandaloneCfg(current_flags,
                                          extension = "",
                                          InputCollections = InputCombinedInDetTracks,
                                          BarrelSegments = "TRTSegments"))
            InputCombinedInDetTracks += ["TRTStandaloneTracks"]
            InputExtendedInDetTracks += ["TRTStandaloneTracks"]
            continue # Skip rest of config for the TRTStandalone pass

        ResolvedTracks = "Resolved" + extension + "Tracks"
        # Tweak to match old config key
        if "LargeD0" in extension:
            ResolvedTracks = "ResolvedLargeD0Tracks"
        SiSPSeededTracks = "SiSPSeeded" + extension + "Tracks" # Old config had inconsistent "SiSPSeeded" vs "SiSpSeeded" keys

        result.merge(TrackingSiPatternCfg(current_flags,
                                          InputCollections = InputExtendedInDetTracks,
                                          ResolvedTrackCollectionKey = ResolvedTracks,
                                          SiSPSeededTrackCollectionKey = SiSPSeededTracks,
                                          ClusterSplitProbContainer = ClusterSplitProbContainer))

        TrackContainer = ResolvedTracks

        if current_flags.InDet.Tracking.ActivePass.useTRTExtension:
            ExtendedTracks = "Extended" + extension + "Tracks"
            # Tweaks to match old config key
            if extension=="Disappearing":
                ExtendedTracks = "ExtendedTracksDisappearing"
            elif "LargeD0" in extension:
                ExtendedTracks = "ExtendedLargeD0Tracks"
            ExtendedTracksMap = "ExtendedTracksMap" + extension

            from InDetConfig.TRTExtensionConfig import NewTrackingTRTExtensionCfg
            result.merge(NewTrackingTRTExtensionCfg(current_flags,
                                                    SiTrackCollection = ResolvedTracks,
                                                    ExtendedTrackCollection = ExtendedTracks,
                                                    ExtendedTracksMap = ExtendedTracksMap))

            TrackContainer = ExtendedTracks

        if current_flags.InDet.Tracking.ActivePass.storeSeparateContainer:
            if flags.InDet.doTruth:
                result.merge(InDetTrackTruthCfg(current_flags,
                                                Tracks = TrackContainer,
                                                DetailedTruth = TrackContainer+"DetailedTruth",
                                                TracksTruth = TrackContainer+"TruthCollection"))

            result.merge(TrackParticleCnvAlgCfg(current_flags,
                                                name = extension + "TrackParticleCnvAlg",
                                                TrackContainerName = TrackContainer,
                                                OutputTrackParticleContainer = "InDetLargeD0TrackParticles" if "LargeD0" in extension
                                                else "InDet" + extension + "TrackParticles")) # Need specific handling for R3LargeD0 not to break downstream configs
        else:
            ClusterSplitProbContainer = "InDetAmbiguityProcessorSplitProb" + extension
            InputCombinedInDetTracks += [TrackContainer]

        InputExtendedInDetTracks += [TrackContainer]

        # BackTracking only after primary pass
        if isPrimaryPass:
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
                InputExtendedInDetTracks += ["ResolvedTRTSeededTracks"]
            isPrimaryPass = False




    result.merge(TrackCollectionMergerAlgCfg(flags,
                                             InputCombinedTracks = InputCombinedInDetTracks,
                                             CombinedInDetClusterSplitProbContainer = ClusterSplitProbContainer))

    if flags.InDet.doTruth:
        from InDetConfig.TrackTruthConfig import InDetTrackTruthCfg
        result.merge(InDetTrackTruthCfg(flags))

    result.merge(TrackParticleCnvAlgCfg(flags))

    if flags.InDet.Tracking.doStoreTrackSeeds:
        result.merge(TrackParticleCnvAlgCfg(flags,
                                            name = "SiSPSeedSegmentsCnvAlg",
                                            TrackContainerName = "SiSPSeedSegments",
                                            OutputTrackParticleContainer = "SiSPSeedSegmentsTrackParticles"))

    if flags.InDet.PriVertex.doVertexFinding:
        from InDetConfig.VertexFindingConfig import primaryVertexFindingCfg
        result.merge(primaryVertexFindingCfg(flags))

    # output
    result.merge(InDetTrackRecoOutputCfg(flags))

    return result


def InDetTrackRecoOutputCfg(flags):
    from OutputStreamAthenaPool.OutputStreamConfig import addToESD,addToAOD
    toAOD = []
    toESD = []

    # FIXME: special branches without flags
    special = False

    # excluded track aux data
    excludedAuxData = "-clusterAssociation.-TTVA_AMVFVertices_forReco.-TTVA_AMVFWeights_forReco"
    # remove track decorations used internally by FTAG software
    excludedAuxData += ('.-TrackCompatibility.-JetFitter_TrackCompatibility_antikt4emtopo.-JetFitter_TrackCompatibility_antikt4empflow'
                        '.-btagIp_d0Uncertainty.-btagIp_z0SinThetaUncertainty.-btagIp_z0SinTheta.-btagIp_d0.-btagIp_trackMomentum.-btagIp_trackDisplacement'
                        '.-VxTrackAtVertex')

    if not special: #not flags.InDet.keepFirstParameters or flags.InDet.keepAdditionalHitsOnTrackParticle
        excludedAuxData += '.-trackParameterCovarianceMatrices.-parameterX.-parameterY.-parameterZ.-parameterPX.-parameterPY.-parameterPZ.-parameterPosition'

    # exclude TTVA decorations 
    excludedAuxData += '.-TTVA_AMVFVertices.-TTVA_AMVFWeights'

    # exclude IDTIDE/IDTRKVALID decorations
    excludedAuxData += '.-TrkBLX.-TrkBLY.-TrkBLZ.-TrkIBLX.-TrkIBLY.-TrkIBLZ.-TrkL1X.-TrkL1Y.-TrkL1Z.-TrkL2X.-TrkL2Y.-TrkL2Z.-msosLink'

    # exclude IDTIDE decorations
    excludedAuxData += ('.-IDTIDE1_biased_PVd0Sigma.-IDTIDE1_biased_PVz0Sigma.-IDTIDE1_biased_PVz0SigmaSinTheta.-IDTIDE1_biased_d0.-IDTIDE1_biased_d0Sigma'
                        '.-IDTIDE1_biased_z0.-IDTIDE1_biased_z0Sigma.-IDTIDE1_biased_z0SigmaSinTheta.-IDTIDE1_biased_z0SinTheta.-IDTIDE1_unbiased_PVd0Sigma.-IDTIDE1_unbiased_PVz0Sigma'
                        '.-IDTIDE1_unbiased_PVz0SigmaSinTheta.-IDTIDE1_unbiased_d0.-IDTIDE1_unbiased_d0Sigma.-IDTIDE1_unbiased_z0.-IDTIDE1_unbiased_z0Sigma.-IDTIDE1_unbiased_z0SigmaSinTheta'
                        '.-IDTIDE1_unbiased_z0SinTheta')


    ##### ESD #####
    # Save full and zero-suppressed BCM rdos
    # (the latter is needed to allow writting to AOD and the former will hopefully be removed in future):
    if not flags.InDet.Tracking.doDBMstandalone:
        toESD += [
            "BCM_RDO_Container#BCM_RDOs",
            "BCM_RDO_Container#BCM_CompactDOs",
        ]

    # In case of cosmics we save the RDOs as well
    if special:  # flags.InDet.writeRDOs:
        toESD += [
            "PixelRDO_Container#PixelRDOs",
            "SCT_RDO_Container#SCT_RDOs",
            # "TRT_RDO_Container#TRT_RDOs",
        ]

    # write phase calculation into ESD
    if flags.Beam.Type is BeamType.Cosmics:
        toESD += ["ComTime#TRT_Phase"]

    # Save PRD
    if special:  # flags.InDet.writePRDs: globalflags.DataSource == 'data' and flags.InDet.doHeavyIon()
        toESD += [
            "InDet::SCT_ClusterContainer#SCT_Clusters",
            "InDet::PixelClusterContainer#PixelClusters",
            "InDet::TRT_DriftCircleContainer#TRT_DriftCircles",
            "InDet::PixelGangedClusterAmbiguities#PixelClusterAmbiguitiesMap",
        ]
        if flags.InDet.Tracking.doPixelClusterSplitting:
            toESD += ["InDet::PixelGangedClusterAmbiguities#SplitClusterAmbiguityMap"]
        toESD += ["IDCInDetBSErrContainer#SCT_FlaggedCondData"]
        toESD += ["Trk::ClusterSplitProbabilityContainer#*"]  # TODO: proper name

    # add tracks
    if flags.InDet.Tracking.doStoreTrackSeeds and special:  # flags.InDet.doWriteTracksToESD:
        toESD += ["TrackCollection#SiSPSeedSegments"]

    if special:  # flags.InDet.doWriteTracksToESD:
        toESD += ["TrackCollection#SiSPSeededTracks"]

        if flags.InDet.Tracking.doTrackSegmentsPixel:
            toESD += ["TrackCollection#ResolvedPixelTracks"]
            if flags.InDet.doTruth:
                toESD += ["TrackTruthCollection#ResolvedPixelTracksTruthCollection"]
                toESD += ["DetailedTrackTruthCollection#ResolvedPixelTracksDetailedTruth"]

        if flags.InDet.Tracking.doTrackSegmentsSCT:
            toESD += ["TrackCollection#ResolvedSCTTracks"]
            if flags.InDet.doTruth:
                toESD += ["TrackTruthCollection#ResolvedSCTTracksTruthCollection"]
                toESD += ["DetailedTrackTruthCollection#ResolvedSCTTracksDetailedTruth"]

        if flags.InDet.Tracking.doTrackSegmentsTRT:
            toESD += ["TrackCollection#StandaloneTRTTracks"]
            if flags.InDet.doTruth:
                toESD += ["TrackTruthCollection#StandaloneTRTTracksTruthCollection"]
                toESD += ["DetailedTrackTruthCollection#StandaloneTRTTracksDetailedTruth"]

        if special:  # flags.InDet.doPseudoTracking:
            toESD += ["TrackCollection#InDetPseudoTracks"]
            if flags.InDet.doTruth:
                toESD += ["TrackTruthCollection#InDetPseudoTracksTruthCollection"]
                toESD += ["DetailedTrackTruthCollection#InDetPseudoTracksDetailedTruth"]

        if special:  # flags.InDet.doTIDE_AmbiTrackMonitoring:
            toESD += ["TrackCollection#ObservedTracksCollection"]
            if flags.InDet.doTruth:
                toESD += ["TrackTruthCollection#InDetObservedTrackTruthCollection"]
                toESD += ["DetailedTrackTruthCollection#InDetObservedTrackDetailedTruth"]

        if flags.InDet.Tracking.doDBMstandalone:
            toESD += ["TrackCollection#ResolvedDBMTracks"]
            if flags.InDet.doTruth:
                toESD += ["TrackTruthCollection#ResolvedDBMTracksTruthCollection"]
                toESD += ["DetailedTrackTruthCollection#ResolvedDBMTracksDetailedTruth"]

        # add the forward tracks for combined muon reconstruction
        if flags.InDet.Tracking.doForwardTracks:
            toESD += ["TrackCollection#ResolvedForwardTracks"]
            if flags.InDet.doTruth:
                toESD += ["TrackTruthCollection#ResolvedForwardTracksTruthCollection"]
                toESD += ["DetailedTrackTruthCollection#ResolvedForwardTracksDetailedTruth"]

        if flags.InDet.Tracking.doBeamGas:
            # TODO
            pass

        if flags.InDet.Tracking.doBeamHalo:
            # TODO
            pass

        if flags.InDet.Tracking.doTrackSegmentsDisappearing:
            toESD += ["TrackCollection#DisappearingTracks"]
            if flags.InDet.doTruth:
                toESD += ["TrackTruthCollection#DisappearingTracksTruthCollection"]
                toESD += ["DetailedTrackTruthCollection#DisappearingTracksDetailedTruth"]

        # Add TRT Segments (only if standalone is off).
        # TODO: no TP converter?
        # if not flags.InDet.doTRTStandalone:
        #    toESD += ["Trk::SegmentCollection#TRTSegments"]

        # Save (Detailed) Track Truth
        if flags.InDet.doTruth:
            toESD += ["TrackTruthCollection#TrackTruthCollection"]
            toESD += ["DetailedTrackTruthCollection#DetailedTrackTruth"]

            # save PRD MultiTruth
            toESD += [
                "PRD_MultiTruthCollection#PRD_MultiTruthPixel",
                "PRD_MultiTruthCollection#PRD_MultiTruthSCT",
                "PRD_MultiTruthCollection#PRD_MultiTruthTRT",
            ]

    if not flags.Input.isMC:
        toESD += [
            "InDetBSErrContainer#PixelByteStreamErrs",
            "TRT_BSErrContainer#TRT_ByteStreamErrs",
            "TRT_BSIdErrContainer#TRT_ByteStreamIdErrs",
            "IDCInDetBSErrContainer#SCT_ByteStreamErrs",
        ]

    ##### AOD #####
    toAOD += ["xAOD::TrackParticleContainer#InDetTrackParticles"]
    toAOD += [f"xAOD::TrackParticleAuxContainer#InDetTrackParticlesAux.{excludedAuxData}"]
    toAOD += ["xAOD::TrackParticleContainer#InDetForwardTrackParticles"]
    toAOD += [f"xAOD::TrackParticleAuxContainer#InDetForwardTrackParticlesAux.{excludedAuxData}"]
    toAOD += ["xAOD::TrackParticleContainer#InDetLargeD0TrackParticles"]
    toAOD += [f"xAOD::TrackParticleAuxContainer#InDetLargeD0TrackParticlesAux.{excludedAuxData}"]
    if flags.InDet.Tracking.doTrackSegmentsPixel:
        toAOD += ["xAOD::TrackParticleContainer#InDetPixelTrackParticles"]
        toAOD += [f"xAOD::TrackParticleAuxContainer#InDetPixelTrackParticlesAux.{excludedAuxData}"]
    if flags.InDet.Tracking.doTrackSegmentsDisappearing:
        toAOD += ["xAOD::TrackParticleContainer#InDetDisappearingTrackParticles"]
        toAOD += [f"xAOD::TrackParticleAuxContainer#InDetDisappearingTrackParticlesAux.{excludedAuxData}"]
    if flags.InDet.Tracking.doTrackSegmentsSCT:
        toAOD += ["xAOD::TrackParticleContainer#InDetSCTTrackParticles"]
        toAOD += [f"xAOD::TrackParticleAuxContainer#InDetSCTTrackParticlesAux.{excludedAuxData}"]
    if flags.InDet.Tracking.doTrackSegmentsTRT:
        toAOD += ["xAOD::TrackParticleContainer#InDetTRTTrackParticles"]
        toAOD += [f"xAOD::TrackParticleAuxContainer#InDetTRTTrackParticlesAux.{excludedAuxData}"]
    if flags.InDet.Tracking.doDBMstandalone: 
        toAOD += ["xAOD::TrackParticleContainer#InDetDBMTrackParticles"] 
        toAOD += [f"xAOD::TrackParticleAuxContainer#InDetDBMTrackParticlesAux.{excludedAuxData}"] 
        toAOD += ["TrackCollection#ResolvedDBMTracks"] 
        if flags.InDet.doTruth:
            toAOD += ["TrackTruthCollection#ResolvedDBMTracksTruthCollection"] 
            toAOD += ["DetailedTrackTruthCollection#ResolvedDBMTracksDetailedTruth"] 
    if special:  # flags.InDet.doPseudoTracking:
        toAOD += ["xAOD::TrackParticleContainer#InDetPseudoTrackParticles"]
        toAOD += [f"xAOD::TrackParticleAuxContainer#InDetPseudoTrackParticlesAux.{excludedAuxData}"]
        if flags.InDet.doTruth:
            toAOD += ["TrackTruthCollection#InDetPseudoTrackTruthCollection"]
            toAOD += ["DetailedTrackTruthCollection#InDetPseudoTrackDetailedTruth"]
    if special:  # flags.InDet.doTIDE_AmbiTrackMonitoring:
        toAOD += ["xAOD::TrackParticleContainer#InDetObservedTrackParticles"]
        toAOD += [f"xAOD::TrackParticleAuxContainer#InDetObservedTrackParticlesAux.{excludedAuxData}"]
        if flags.InDet.doTruth:
            toAOD += ["TrackTruthCollection#InDetObservedTrackTruthCollection"]
            toAOD += ["DetailedTrackTruthCollection#ObservedDetailedTracksTruth"]

    result = ComponentAccumulator()
    result.merge(addToESD(flags, toESD + toAOD))
    result.merge(addToAOD(flags, toAOD))

    return result


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    # Disable calo for this test
    ConfigFlags.Detector.EnableCalo = False

    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.RDO_RUN2
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
