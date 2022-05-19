# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import Format

def ITkTrackParticleCreatorToolCfg(flags, name="ITkTrackParticleCreatorTool", **kwargs):
    result = ComponentAccumulator()
    if "TrackToVertex" not in kwargs:
        from InDetConfig.TrackRecoConfig import TrackToVertexCfg
        kwargs["TrackToVertex"] = result.popToolsAndMerge(TrackToVertexCfg(flags))
    if "TrackSummaryTool" not in kwargs:
        from TrkConfig.TrkTrackSummaryToolConfig import ITkTrackSummaryToolSharedHitsCfg
        TrackSummaryTool = result.popToolsAndMerge(ITkTrackSummaryToolSharedHitsCfg(flags))
        result.addPublicTool(TrackSummaryTool)
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

def ITkTrackCollectionMergerAlgCfg(flags, name="ITkTrackCollectionMerger",
                                   InputCombinedTracks=None,
                                   OutputCombinedTracks="CombinedITkTracks",
                                   AssociationMapName="ITkPRDToTrackMapCombinedITkTracks",
                                   CombinedITkClusterSplitProbContainer="",
                                   **kwargs):
    result = ComponentAccumulator()

    kwargs.setdefault("TracksLocation", InputCombinedTracks)
    kwargs.setdefault("OutputTracksLocation", OutputCombinedTracks)
    from InDetConfig.InDetAssociationToolsConfig import ITkPRDtoTrackMapToolGangedPixelsCfg
    ITkPRDtoTrackMapToolGangedPixels = result.popToolsAndMerge(ITkPRDtoTrackMapToolGangedPixelsCfg(flags))
    kwargs.setdefault("AssociationTool", ITkPRDtoTrackMapToolGangedPixels)
    kwargs.setdefault("AssociationMapName", AssociationMapName)
    kwargs.setdefault("UpdateSharedHits", True)
    kwargs.setdefault("UpdateAdditionalInfo", True)
    from TrkConfig.TrkTrackSummaryToolConfig import ITkTrackSummaryToolSharedHitsCfg
    TrackSummaryTool = result.popToolsAndMerge(ITkTrackSummaryToolSharedHitsCfg(flags, name="CombinedITkSplitProbTrackSummaryToolSharedHits"))
    TrackSummaryTool.InDetSummaryHelperTool.ClusterSplitProbabilityName = CombinedITkClusterSplitProbContainer
    result.addPublicTool(TrackSummaryTool)
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
    else:
        flags = flags.cloneAndReplace("ITk.Tracking.ActivePass", "ITk.Tracking.MainPass")
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

    from InDetConfig.SiliconPreProcessing import ITkRecPreProcessingSiliconCfg
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

    if flags.ITk.Tracking.writeExtendedPRDInfo:
        from InDetConfig.InDetPrepRawDataToxAODConfig import ITkPixelPrepDataToxAODCfg, ITkStripPrepDataToxAODCfg
        result.merge(ITkPixelPrepDataToxAODCfg(flags, ClusterSplitProbabilityName = ClusterSplitProbContainer))
        result.merge(ITkStripPrepDataToxAODCfg(flags))

        from DerivationFrameworkInDet.InDetToolsConfig import ITkTrackStateOnSurfaceDecoratorCfg
        TrackStateOnSurfaceDecorator = result.getPrimaryAndMerge(ITkTrackStateOnSurfaceDecoratorCfg(flags, name="ITkTrackStateOnSurfaceDecorator"))
        result.addEventAlgo(CompFactory.DerivationFramework.CommonAugmentation("ITkCommonKernel", AugmentationTools = [TrackStateOnSurfaceDecorator]))

        if flags.Input.isMC:
            from InDetPhysValMonitoring.InDetPhysValDecorationConfig import InDetPhysHitDecoratorAlgCfg
            result.merge(InDetPhysHitDecoratorAlgCfg(flags))

    # output
    result.merge(ITkTrackRecoOutputCfg(flags))

    return result

def ITkTrackRecoOutputCfg(flags):
    from OutputStreamAthenaPool.OutputStreamConfig import addToESD,addToAOD
    toAOD = []
    toESD = []

    # excluded track aux data
    excludedAuxData = "-clusterAssociation.-TTVA_AMVFVertices_forReco.-TTVA_AMVFWeights_forReco"
    # remove track decorations used internally by FTAG software
    excludedAuxData += ('.-TrackCompatibility.-JetFitter_TrackCompatibility_antikt4emtopo.-JetFitter_TrackCompatibility_antikt4empflow'
                        '.-btagIp_d0Uncertainty.-btagIp_z0SinThetaUncertainty.-btagIp_z0SinTheta.-btagIp_d0.-btagIp_trackMomentum.-btagIp_trackDisplacement'
                        '.-VxTrackAtVertex')

    # exclude IDTIDE/IDTRKVALID decorations
    excludedAuxData += '.-TrkBLX.-TrkBLY.-TrkBLZ.-TrkIBLX.-TrkIBLY.-TrkIBLZ.-TrkL1X.-TrkL1Y.-TrkL1Z.-TrkL2X.-TrkL2Y.-TrkL2Z'
    if not flags.ITk.Tracking.writeExtendedPRDInfo:
        excludedAuxData += '.-msosLink'

    toAOD += ["xAOD::TrackParticleContainer#InDetTrackParticles"]
    toAOD += [f"xAOD::TrackParticleAuxContainer#InDetTrackParticlesAux.{excludedAuxData}"]

    if flags.ITk.Tracking.writeExtendedPRDInfo:
        toAOD += [
            "xAOD::TrackMeasurementValidationContainer#ITkPixelClusters",
            "xAOD::TrackMeasurementValidationAuxContainer#ITkPixelClustersAux.",
            "xAOD::TrackMeasurementValidationContainer#ITkStripClusters",
            "xAOD::TrackMeasurementValidationAuxContainer#ITkStripClustersAux.",
            "xAOD::TrackStateValidationContainer#ITkPixelMSOSs",
            "xAOD::TrackStateValidationAuxContainer#ITkPixelMSOSsAux.",
            "xAOD::TrackStateValidationContainer#ITkStripMSOSs",
            "xAOD::TrackStateValidationAuxContainer#ITkStripMSOSsAux."
        ]

    result = ComponentAccumulator()
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
