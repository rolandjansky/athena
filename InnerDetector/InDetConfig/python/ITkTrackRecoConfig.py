# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import Format

##------------------------------------------------------------------------------
def ITk_BCM_ZeroSuppressionCfg(flags, name="ITk_BCM_ZeroSuppression", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("BcmContainerName", "BCM_RDOs")
    algo = CompFactory.BCM_ZeroSuppression(name=name, **kwargs)
    acc.addEventAlgo(algo, primary = True)
    return acc

##------------------------------------------------------------------------------
def ITkPixelClusterizationCfg(flags, name = "ITkPixelClusterization", **kwargs) :
    acc = ComponentAccumulator()
    merged_pixels_tool = acc.popToolsAndMerge(ITkMergedPixelsToolCfg(flags, **kwargs))
    ambi_finder = acc.popToolsAndMerge(ITkPixelGangedAmbiguitiesFinderCfg(flags, **kwargs))

    kwargs.setdefault("clusteringTool", merged_pixels_tool)
    kwargs.setdefault("gangedAmbiguitiesFinder", ambi_finder)
    kwargs.setdefault("DataObjectName", "ITkPixelRDOs")
    kwargs.setdefault("ClustersName", "ITkPixelClusters")
    kwargs.setdefault("AmbiguitiesMap", "ITkPixelClusterAmbiguitiesMap")

    acc.addEventAlgo(CompFactory.InDet.PixelClusterization(name=name, **kwargs))
    return acc
##------------------------------------------------------------------------------
def ITkPixelClusterizationPUCfg(flags, name="ITkPixelClusterizationPU", **kwargs) :
    kwargs.setdefault("DataObjectName", "ITkPixel_PU_RDOs")
    kwargs.setdefault("ClustersName", "ITkPixelPUClusters")
    kwargs.setdefault("AmbiguitiesMap", "ITkPixelClusterAmbiguitiesMapPU")
    return ITkPixelClusterizationCfg(flags, name=name, **kwargs)

##------------------------------------------------------------------------------
##------------------------------------------------------------------------------

def ITkStripClusterizationCfg(flags, name="ITkStripClusterization", **kwargs) :
    acc = ComponentAccumulator()

    # Need to get ITkStrip_ConditionsSummaryTool for e.g. ITkStripClusteringTool
    from SCT_ConditionsTools.ITkStripConditionsToolsConfig import ITkStripConditionsSummaryToolCfg
    ITkStripConditionsSummaryTool = acc.popToolsAndMerge(ITkStripConditionsSummaryToolCfg(flags))

    from SiLorentzAngleTool.ITkStripLorentzAngleConfig import ITkStripLorentzAngleCfg
    ITkStripLorentzAngleTool = acc.popToolsAndMerge( ITkStripLorentzAngleCfg(flags) )

    #### Clustering tool ######
    ITkClusterMakerTool = acc.popToolsAndMerge(ITkClusterMakerToolCfg(flags))
    ITkStripClusteringTool = CompFactory.InDet.SCT_ClusteringTool( name           = "ITkStripClusteringTool",
                                                                   globalPosAlg   = ITkClusterMakerTool,
                                                                   conditionsTool = ITkStripConditionsSummaryTool,
                                                                   LorentzAngleTool = ITkStripLorentzAngleTool,
                                                                   useRowInformation = True,
                                                                   SCTDetEleCollKey = "ITkStripDetectorElementCollection")

    kwargs.setdefault("clusteringTool", ITkStripClusteringTool)
    kwargs.setdefault("DataObjectName", 'ITkStripRDOs')
    kwargs.setdefault("ClustersName", 'ITkStripClusters')
    kwargs.setdefault("SCT_FlaggedCondData", "ITkStripFlaggedCondData")
    kwargs.setdefault("conditionsTool", ITkStripConditionsSummaryTool)

    acc.addEventAlgo( CompFactory.InDet.SCT_Clusterization(name=name, **kwargs))

    return acc

##------------------------------------------------------------------------------
##------------------------------------------------------------------------------
##------------------------------------------------------------------------------

def ITkStripClusterizationPUCfg(flags, name="ITkStripClusterizationPU", **kwargs) :
    kwargs.setdefault("DataObjectName", "ITkStrip_PU_RDOs" )
    kwargs.setdefault("ClustersName", "ITkStrip_PU_Clusters")
    return ITkStripClusterizationCfg(flags, name=name, **kwargs)

##------------------------------------------------------------------------------
def ITkPixelGangedAmbiguitiesFinderCfg(flags, **kwargs) :
    acc = ComponentAccumulator()

    from PixelGeoModelXml.ITkPixelGeoModelConfig import ITkPixelReadoutGeometryCfg
    acc.merge(ITkPixelReadoutGeometryCfg(flags))

    kwargs.setdefault("PixelDetEleCollKey", "ITkPixelDetectorElementCollection")

    ITkPixelGangedAmbiguitiesFinder = CompFactory.InDet.PixelGangedAmbiguitiesFinder( name = "ITkPixelGangedAmbiguitiesFinder", **kwargs)
    acc.setPrivateTools( ITkPixelGangedAmbiguitiesFinder )
    return acc

##------------------------------------------------------------------------------
def ITkMergedPixelsToolCfg(flags, **kwargs) :
      acc = ComponentAccumulator()
      # --- now load the framework for the clustering
      kwargs.setdefault("globalPosAlg", acc.popToolsAndMerge(ITkClusterMakerToolCfg(flags)))

      # PixelClusteringToolBase uses PixelConditionsSummaryTool
      from PixelConditionsTools.ITkPixelConditionsSummaryConfig import ITkPixelConditionsSummaryCfg
      ITkPixelConditionsSummary = acc.popToolsAndMerge(ITkPixelConditionsSummaryCfg(flags))
      kwargs.setdefault("PixelConditionsSummaryTool", ITkPixelConditionsSummary)

      kwargs.setdefault("PixelDetEleCollKey","ITkPixelDetectorElementCollection")

      ITkMergedPixelsTool = CompFactory.InDet.MergedPixelsTool(  name = "ITkMergedPixelsTool", **kwargs)
      acc.setPrivateTools(ITkMergedPixelsTool)
      return acc

##------------------------------------------------------------------------------
def ITkClusterMakerToolCfg(flags, name="ITkClusterMakerTool", **kwargs) :
    acc = ComponentAccumulator()

    from PixelConditionsAlgorithms.ITkPixelConditionsConfig import ITkPixelChargeCalibCondAlgCfg
    from PixelReadoutGeometry.PixelReadoutGeometryConfig import ITkPixelReadoutManagerCfg
    #ITkPixelCablingCondAlgCfg + ITkPixelReadoutSpeedAlgCfg needed?

    # This directly needs the following Conditions data:
    # PixelModuleData & PixelChargeCalibCondData
    acc.merge(ITkPixelChargeCalibCondAlgCfg(flags))
    acc.merge(ITkPixelReadoutManagerCfg(flags))

    from SiLorentzAngleTool.ITkPixelLorentzAngleConfig import ITkPixelLorentzAngleCfg
    ITkPixelLorentzAngleTool = acc.popToolsAndMerge(ITkPixelLorentzAngleCfg(flags))
    from SiLorentzAngleTool.ITkStripLorentzAngleConfig import ITkStripLorentzAngleCfg
    ITkStripLorentzAngleTool = acc.popToolsAndMerge(ITkStripLorentzAngleCfg(flags))

    kwargs.setdefault("PixelChargeCalibCondData", "ITkPixelChargeCalibCondData")
    kwargs.setdefault("PixelReadoutManager",acc.getService("ITkPixelReadoutManager"))
    kwargs.setdefault("PixelLorentzAngleTool", ITkPixelLorentzAngleTool)
    kwargs.setdefault("SCTLorentzAngleTool", ITkStripLorentzAngleTool)
    kwargs.setdefault("PixelOfflineCalibData", "")

    ITkClusterMakerTool = CompFactory.InDet.ClusterMakerTool(name = name, **kwargs)
    acc.setPrivateTools(ITkClusterMakerTool)
    return acc


def ITkTrackParticleCreatorToolCfg(flags, name="ITkTrackParticleCreatorTool", **kwargs):
    result = ComponentAccumulator()
    if "TrackToVertex" not in kwargs:
        from InDetConfig.TrackRecoConfig import TrackToVertexCfg
        kwargs["TrackToVertex"] = result.popToolsAndMerge(TrackToVertexCfg(flags))
    if "TrackSummaryTool" not in kwargs:
        from InDetConfig.ITkTrackingCommonConfig import ITkTrackSummaryToolSharedHitsCfg
        TrackSummaryTool = result.getPrimaryAndMerge(ITkTrackSummaryToolSharedHitsCfg(flags))
        kwargs["TrackSummaryTool"] = TrackSummaryTool
    kwargs.setdefault("BadClusterID", 3) # Select the mode to identify suspicous pixel cluster
    kwargs.setdefault("KeepParameters", True)
    kwargs.setdefault("KeepFirstParameters", False)
    # need to treat Vertex specifically because at the time of
    # the track particle creation the primary vertex does not yet exist.
    # The problem is solved by first creating track particles wrt. the beam line
    # and correcting the parameters after the vertex finding.
    kwargs.setdefault("PerigeeExpression", "BeamLine" if flags.ITk.Tracking.perigeeExpression=="Vertex"
                      else flags.ITk.Tracking.perigeeExpression)
    kwargs.setdefault("IBLParameterSvc", "")
    ITkTrackParticleCreatorTool = CompFactory.Trk.TrackParticleCreatorTool(name, **kwargs)
    result.addPublicTool(ITkTrackParticleCreatorTool, primary=True)
    return result

def ITkTrackCollectionCnvToolCfg(flags, name="ITkTrackCollectionCnvTool", ITkTrackParticleCreator = None, **kwargs):
    result = ComponentAccumulator()

    ITkTrackParticleCreator = result.getPrimaryAndMerge(ITkTrackParticleCreatorToolCfg(flags))

    kwargs.setdefault("TrackParticleCreator", ITkTrackParticleCreator)
    result.setPrivateTools(CompFactory.xAODMaker.TrackCollectionCnvTool(name, **kwargs))
    return result

def ITkTrackCollectionMergerAlgCfg(flags, name="ITkTrackCollectionMerger", InputCombinedTracks=None, CombinedITkClusterSplitProbContainer=None, **kwargs):
    result = ComponentAccumulator()

    kwargs.setdefault("TracksLocation", InputCombinedTracks)
    kwargs.setdefault("OutputTracksLocation", 'CombinedITkTracks')
    from InDetConfig.ITkTrackingCommonConfig import ITkPRDtoTrackMapToolGangedPixelsCfg
    ITkPRDtoTrackMapToolGangedPixels = result.popToolsAndMerge(ITkPRDtoTrackMapToolGangedPixelsCfg(flags))
    kwargs.setdefault("AssociationTool", ITkPRDtoTrackMapToolGangedPixels)
    kwargs.setdefault("AssociationMapName", 'ITkPRDToTrackMapCombinedITkTracks')
    kwargs.setdefault("UpdateSharedHits", True)
    kwargs.setdefault("UpdateAdditionalInfo", True)
    from InDetConfig.ITkTrackingCommonConfig import ITkTrackSummaryToolSharedHitsCfg
    TrackSummaryTool = result.getPrimaryAndMerge(ITkTrackSummaryToolSharedHitsCfg(flags, name="CombinedITkSplitProbTrackSummaryToolSharedHits"))
    TrackSummaryTool.InDetSummaryHelperTool.ClusterSplitProbabilityName = CombinedITkClusterSplitProbContainer
    kwargs.setdefault("SummaryTool", TrackSummaryTool)

    result.addEventAlgo(CompFactory.Trk.TrackCollectionMerger(name, **kwargs))
    return result

def ITkTrackParticleCnvAlgCfg(flags, name="ITkTrackParticleCnvAlg", TrackContainerName="CombinedITkTracks", OutputTrackParticleContainer="InDetTrackParticles", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("ConvertTracks", True)
    kwargs.setdefault("ConvertTrackParticles", False)
    kwargs.setdefault("TrackContainerName", TrackContainerName)
    kwargs.setdefault("xAODContainerName", OutputTrackParticleContainer)
    kwargs.setdefault("xAODTrackParticlesFromTracksContainerName", OutputTrackParticleContainer)
    if "TrackParticleCreator" not in kwargs:
        kwargs["TrackParticleCreator"] = result.getPrimaryAndMerge(ITkTrackParticleCreatorToolCfg(flags))
    if "TrackCollectionCnvTool" not in kwargs:
        kwargs["TrackCollectionCnvTool"] = result.popToolsAndMerge(ITkTrackCollectionCnvToolCfg(
            flags,
            TrackParticleCreator=kwargs["TrackParticleCreator"],
        ))

    if flags.ITk.Tracking.doTruth:
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

def CombinedTrackingPassFlagSets(flags):

    flags_set = []

    # Primary Pass
    if flags.ITk.Tracking.doFastTracking:
        flags = flags.cloneAndReplace("ITk.Tracking.ActivePass", "ITk.Tracking.FastPass")
    flags_set += [flags]

    # LRT
    if flags.ITk.Tracking.doLargeD0:
        flagsLRT = flags.cloneAndReplace("ITk.Tracking.ActivePass", "ITk.Tracking.LargeD0Pass")
        if flags.ITk.Tracking.doFastTracking:
            flagsLRT = flags.cloneAndReplace("ITk.Tracking.ActivePass", "ITk.Tracking.LargeD0FastPass")
        flags_set += [flagsLRT]

    # Photon conversion tracking reco
    if flags.Detector.EnableCalo and flags.ITk.Tracking.doConversionFinding:
        flagsConv = flags.cloneAndReplace("ITk.Tracking.ActivePass", "ITk.Tracking.ConversionFindingPass")
        flags_set += [flagsConv]

    return flags_set

def ITkTrackRecoCfg(flags):
    """Configures complete ID tracking """
    result = ComponentAccumulator()

    if flags.Input.Format is Format.BS:
        # TODO: ITk BS providers
        raise RuntimeError("BS imputs not supported")

    from InDetConfig.ITkSiliconPreProcessing import ITkRecPreProcessingSiliconCfg
    result.merge(ITkRecPreProcessingSiliconCfg(flags))

    flags_set = CombinedTrackingPassFlagSets(flags)
    InputCombinedITkTracks = [] # Tracks to be ultimately merged in InDetTrackParticle collection
    InputExtendedITkTracks = [] # Includes also tracks which end in standalone TrackParticle collections
    ClusterSplitProbContainer = ""

    for current_flags in flags_set:

        extension = current_flags.ITk.Tracking.ActivePass.extension
        TrackContainer = "Resolved" + extension + "Tracks"
        SiSPSeededTracks = "SiSPSeeded" + extension + "Tracks"

        from InDetConfig.ITkTrackingSiPatternConfig import ITkTrackingSiPatternCfg
        result.merge(ITkTrackingSiPatternCfg(current_flags,
                                             InputCollections = InputExtendedITkTracks,
                                             ResolvedTrackCollectionKey = TrackContainer,
                                             SiSPSeededTrackCollectionKey = SiSPSeededTracks,
                                             ClusterSplitProbContainer = ClusterSplitProbContainer))

        if current_flags.ITk.Tracking.ActivePass.storeSeparateContainer:
            if flags.ITk.doTruth:
                from InDetConfig.ITkTrackTruthConfig import ITkTrackTruthCfg
                result.merge(ITkTrackTruthCfg(current_flags,
                                              Tracks = TrackContainer,
                                              DetailedTruth = TrackContainer+"DetailedTruth",
                                              TracksTruth = TrackContainer+"TruthCollection"))

            result.merge(ITkTrackParticleCnvAlgCfg(current_flags,
                                                   name = extension + "TrackParticleCnvAlg",
                                                   TrackContainerName = TrackContainer,
                                                   OutputTrackParticleContainer = "InDet" + extension + "TrackParticles")) # Need specific handling for R3LargeD0 not to break downstream configs
        else:
            ClusterSplitProbContainer = "ITkAmbiguityProcessorSplitProb" + extension
            InputCombinedITkTracks += [TrackContainer]

        InputExtendedITkTracks += [TrackContainer]


    result.merge(ITkTrackCollectionMergerAlgCfg(flags,
                                                InputCombinedTracks = InputCombinedITkTracks,
                                                CombinedITkClusterSplitProbContainer = ClusterSplitProbContainer))

    if flags.ITk.Tracking.doTruth:
        from InDetConfig.ITkTrackTruthConfig import ITkTrackTruthCfg
        result.merge(ITkTrackTruthCfg(flags))

    result.merge(ITkTrackParticleCnvAlgCfg(flags))

    if flags.ITk.PriVertex.doVertexFinding:
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

    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.RDO_RUN2
    ConfigFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    top_acc = MainServicesCfg(ConfigFlags)

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    top_acc.merge(PoolReadCfg(ConfigFlags))

    if ConfigFlags.Input.isMC:
        from xAODTruthCnv.xAODTruthCnvConfigNew import GEN_AOD2xAODCfg
        top_acc.merge(GEN_AOD2xAODCfg(ConfigFlags))

    top_acc.merge(ITkTrackRecoCfg(ConfigFlags))

    from AthenaCommon.Constants import DEBUG
    top_acc.foreach_component("AthEventSeq/*").OutputLevel=DEBUG
    top_acc.printConfig(withDetails=True, summariseProps=True)
    top_acc.store(open("ITkTrackReco.pkl", "wb"))

    import sys
    if "--norun" not in sys.argv:
        sc = top_acc.run(5)
        if sc.isFailure():
            sys.exit(-1)
