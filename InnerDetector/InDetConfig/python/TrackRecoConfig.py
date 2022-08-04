# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType, Format

def TrackToVertexCfg(flags, name="AtlasTrackToVertexTool", **kwargs):
    result = ComponentAccumulator()
    if "Extrapolator" not in kwargs:
        from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
        kwargs["Extrapolator"] = result.popToolsAndMerge(AtlasExtrapolatorCfg(flags))
    from BeamSpotConditions.BeamSpotConditionsConfig import BeamSpotCondAlgCfg
    result.merge(BeamSpotCondAlgCfg(flags))
    result.setPrivateTools(CompFactory.Reco.TrackToVertex(name, **kwargs))
    return result

def TrackCollectionMergerAlgCfg(flags, name="InDetTrackCollectionMerger",
                                InputCombinedTracks=None,
                                OutputCombinedTracks="",
                                AssociationMapName="",
                                CombinedInDetClusterSplitProbContainer="",
                                **kwargs):
    result = ComponentAccumulator()

    if flags.Overlay.doTrackOverlay:
        kwargs.setdefault("DoTrackOverlay",True)
        if "Disappearing" in name:
            InputCombinedTracks+=flags.Overlay.BkgPrefix+"DisappearingTracks"
        else:
            InputCombinedTracks+=flags.Overlay.BkgPrefix+"CombinedInDetTracks"
    kwargs.setdefault("TracksLocation", InputCombinedTracks)
    kwargs.setdefault("OutputTracksLocation", OutputCombinedTracks)
    from InDetConfig.InDetAssociationToolsConfig import InDetPRDtoTrackMapToolGangedPixelsCfg
    InDetPRDtoTrackMapToolGangedPixels = result.popToolsAndMerge(InDetPRDtoTrackMapToolGangedPixelsCfg(flags))
    kwargs.setdefault("AssociationTool", InDetPRDtoTrackMapToolGangedPixels)
    kwargs.setdefault("AssociationMapName", AssociationMapName)
    kwargs.setdefault("UpdateSharedHits", True)
    kwargs.setdefault("UpdateAdditionalInfo", True)
    kwargs.setdefault("DoTrackOverlay",flags.Overlay.doTrackOverlay)
    from TrkConfig.TrkTrackSummaryToolConfig import InDetTrackSummaryToolSharedHitsCfg
    TrackSummaryTool = result.popToolsAndMerge(InDetTrackSummaryToolSharedHitsCfg(flags, name=OutputCombinedTracks+"SummaryToolSharedHits"))
    TrackSummaryTool.InDetSummaryHelperTool.ClusterSplitProbabilityName = CombinedInDetClusterSplitProbContainer
    result.addPublicTool(TrackSummaryTool)
    kwargs.setdefault("SummaryTool", TrackSummaryTool)

    result.addEventAlgo(CompFactory.Trk.TrackCollectionMerger(name, **kwargs))
    return result

# Configuration not supported, to be recommissioned if needed
def ReFitTrackAlgCfg(flags, name="InDetRefitTrack", InputTrackCollection="CombinedInDetTracks", OutputTrackCollection="RefittedTracks", **kwargs):
    result = ComponentAccumulator()

    from TrkConfig.CommonTrackFitterConfig import InDetTrackFitterCfg, InDetTrackFitterTRTCfg
    InDetTrackFitter = result.popToolsAndMerge(InDetTrackFitterCfg(flags))
    InDetTrackFitterTRT = result.popToolsAndMerge(InDetTrackFitterTRTCfg(flags))
    from TrkConfig.TrkTrackSummaryToolConfig import InDetTrackSummaryToolSharedHitsCfg
    TrackSummaryTool = result.popToolsAndMerge(InDetTrackSummaryToolSharedHitsCfg(flags))
    from InDetConfig.InDetAssociationToolsConfig import InDetPRDtoTrackMapToolGangedPixelsCfg
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
    else:
        flags = flags.cloneAndReplace("InDet.Tracking.ActivePass", "InDet.Tracking.MainPass")

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

def ClusterSplitProbabilityContainerName(flags):

    flags_set = CombinedTrackingPassFlagSets(flags)
    extension = flags_set[-1].InDet.Tracking.ActivePass.extension
    ClusterSplitProbContainer = "InDetAmbiguityProcessorSplitProb" + extension
    if len(flags_set)==1 and flags.InDet.Tracking.doBackTracking: # Only primary pass + back-tracking
        ClusterSplitProbContainer = "InDetTRT_SeededAmbiguityProcessorSplitProb"

    return ClusterSplitProbContainer

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
    from InDetConfig.TRTStandaloneConfig import TRTStandaloneCfg
    from InDetConfig.TRTExtensionConfig import NewTrackingTRTExtensionCfg
    from xAODTrackingCnv.xAODTrackingCnvConfig import TrackParticleCnvAlgPIDCheckCfg

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

        result.merge(TRTStandaloneCfg(flagsTRT,
                                      extension = "_TRT",
                                      InputCollections = [],
                                      BarrelSegments = "TRTSegmentsTRT"))

    # @TODO add TRTPhase computation somewhere (needed for cosmics)

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
            result.merge(TRTStandaloneCfg(current_flags,
                                          extension = "",
                                          InputCollections = InputCombinedInDetTracks,
                                          BarrelSegments = "TRTSegments"))
            InputCombinedInDetTracks += ["TRTStandaloneTracks"]
            InputExtendedInDetTracks += ["TRTStandaloneTracks"]
            continue # Skip rest of config for the TRTStandalone pass

        ResolvedTracks = "Resolved" + extension + "Tracks"
        #for track overlay, save resolved track name for final merged track collection
        if flags.Overlay.doTrackOverlay and current_flags.InDet.Tracking.ActivePass.storeSeparateContainer:
            ResolvedTracks = flags.Overlay.sigPrefix + ResolvedTracks
        # Tweak to match old config key
        if "LargeD0" in extension:
            ResolvedTracks = "ResolvedLargeD0Tracks"
            if flags.Overlay.doTrackOverlay:
                ResolvedTracks = flags.Overlay.sigPrefix + "ResolvedLargeD0Tracks"
        SiSPSeededTracks = "SiSPSeeded" + extension + "Tracks" # Old config had inconsistent "SiSPSeeded" vs "SiSpSeeded" keys

        result.merge(TrackingSiPatternCfg(current_flags,
                                          InputCollections = InputExtendedInDetTracks,
                                          ResolvedTrackCollectionKey = ResolvedTracks,
                                          SiSPSeededTrackCollectionKey = SiSPSeededTracks,
                                          ClusterSplitProbContainer = ClusterSplitProbContainer))

        TrackContainer = ResolvedTracks
        if flags.Overlay.doTrackOverlay and current_flags.InDet.Tracking.ActivePass.storeSeparateContainer:
            TrackContainer = "Resolved" + extension + "Tracks"
            if "LargeD0" in extension:
                TrackContainer = "ResolvedLargeD0Tracks"

        if current_flags.InDet.Tracking.ActivePass.useTRTExtension:
            ExtendedTracks = "Extended" + extension + "Tracks"
            # Tweaks to match old config key
            if extension=="Disappearing":
                ExtendedTracks = "ExtendedTracksDisappearing"
            elif "LargeD0" in extension:
                ExtendedTracks = "ExtendedLargeD0Tracks"
            ExtendedTracksMap = "ExtendedTracksMap" + extension

            result.merge(NewTrackingTRTExtensionCfg(current_flags,
                                                    SiTrackCollection = ResolvedTracks,
                                                    ExtendedTrackCollection = ExtendedTracks,
                                                    ExtendedTracksMap = ExtendedTracksMap))

            TrackContainer = ExtendedTracks

        if current_flags.InDet.Tracking.ActivePass.storeSeparateContainer:
            # Dummy Merger to fill additional info for PRD-associated pixel tracklets
            # Can also run on all separate collections like R3LargeD0 but kept consistent with legacy config
            if extension=="Disappearing":
                InputTracks = [TrackContainer]
                if flags.Overlay.doTrackOverlay:
                    InputTracks += [flags.Overlay.bkgPrefix+extension+"Tracks"]
                TrackContainer = extension+"Tracks"
                result.merge(TrackCollectionMergerAlgCfg(current_flags, name = "InDetTrackCollectionMerger"+extension,
                                                         InputCombinedTracks = InputTracks,
                                                         OutputCombinedTracks = TrackContainer,
                                                         CombinedInDetClusterSplitProbContainer = "InDetAmbiguityProcessorSplitProb" + extension))
            elif flags.Overlay.doTrackOverlay:
                #schedule merger to combine signal and background tracks
                InputTracks=[flags.Overlay.sigPrefix+TrackContainer,flags.Overlay.bkgPrefix+TrackContainer]
                result.merge(TrackCollectionMergerAlgCfg(current_flags, name = "InDetTrackCollectionMerger"+extension,
                                                         InputCombinedTracks = InputTracks,
                                                         OutputCombinedTracks = "Resolved" + extension + "Tracks",
                                                         CombinedInDetClusterSplitProbContainer = "InDetAmbiguityProcessorSplitProb" + extension))
            if flags.InDet.doTruth:
                result.merge(InDetTrackTruthCfg(current_flags,
                                                Tracks = TrackContainer,
                                                DetailedTruth = TrackContainer+"DetailedTruth",
                                                TracksTruth = TrackContainer+"TruthCollection"))

            result.merge(TrackParticleCnvAlgPIDCheckCfg(current_flags,
                                                        name = extension + "TrackParticleCnvAlg",
                                                        TrackContainerName = TrackContainer,
                                                        xAODTrackParticlesFromTracksContainerName = "InDetLargeD0TrackParticles" if "LargeD0" in extension
                                                        else "InDet" + extension + "TrackParticles")) # Need specific handling for R3LargeD0 not to break downstream configs

        else:
            ClusterSplitProbContainer = "InDetAmbiguityProcessorSplitProb" + extension
            InputCombinedInDetTracks += [TrackContainer]

        InputExtendedInDetTracks += [TrackContainer]

        # BackTracking only after primary pass
        if isPrimaryPass:
            if flags.InDet.Tracking.doTRTSegments:
                from InDetConfig.TRTSegmentFindingConfig import TRTSegmentFindingCfg
                result.merge(TRTSegmentFindingCfg(current_flags,
                                                  extension = "",
                                                  InputCollections = InputCombinedInDetTracks,
                                                  BarrelSegments = "TRTSegments"))

            # BackTracking
            if flags.InDet.Tracking.doBackTracking:
                from InDetConfig.BackTrackingConfig import BackTrackingCfg
                result.merge(BackTrackingCfg(current_flags,
                                             InputCollections = InputCombinedInDetTracks,
                                             ClusterSplitProbContainer = ClusterSplitProbContainer))
                ClusterSplitProbContainer = "InDetTRT_SeededAmbiguityProcessorSplitProb"
                InputCombinedInDetTracks += ["ResolvedTRTSeededTracks"]
                InputExtendedInDetTracks += ["ResolvedTRTSeededTracks"]

            if flags.InDet.doTruth and (flags.InDet.Tracking.doPseudoTracking or flags.InDet.Tracking.doIdealPseudoTracking): ## Do we need the dotruth flags...?
                from InDetConfig.TruthTrackingConfig import TruthTrackingCfg
                result.merge(TruthTrackingCfg(flags))

                ## Old config only scheduled InDetTrackTruth for IdealPseudoTracking, while the TrackParticleCnvAlg requires it if "doTruth" is enabled
                if flags.InDet.doTruth: ## needed if flag above is removed
                    from InDetConfig.TrackTruthConfig import InDetTrackTruthCfg
                    result.merge(InDetTrackTruthCfg(flags, 'InDetPseudoTracks', 'InDetPseudoTracksTruthCollection', 'InDetPseudoTracksTruthCollection'))

                result.merge(TrackParticleCnvAlgPIDCheckCfg(current_flags,
                                                           name = "PseudoTrackParticleCnvAlg",
                                                           TrackContainerName = "InDetPseudoTracks",
                                                           xAODTrackParticlesFromTracksContainerName = "InDetPseudoTrackParticles"))

            isPrimaryPass = False

    if flags.Overlay.doTrackOverlay:
        InputCombinedInDetTracks += [flags.Overlay.bkgPrefix+"CombinedInDetTracks"]
    result.merge(TrackCollectionMergerAlgCfg(flags,
                                             InputCombinedTracks = InputCombinedInDetTracks,
                                             OutputCombinedTracks = "CombinedInDetTracks",
                                             AssociationMapName = "PRDtoTrackMapCombinedInDetTracks",
                                             CombinedInDetClusterSplitProbContainer = ClusterSplitProbabilityContainerName(flags)))

    if flags.InDet.doTruth:
        from InDetConfig.TrackTruthConfig import InDetTrackTruthCfg
        result.merge(InDetTrackTruthCfg(flags))

    from xAODTrackingCnv.xAODTrackingCnvConfig import TrackParticleCnvAlgCfg
    result.merge(TrackParticleCnvAlgCfg(flags))

    if flags.InDet.Tracking.doStoreTrackSeeds:
        from xAODTrackingCnv.xAODTrackingCnvConfig import TrackParticleCnvAlgNoPIDCfg
        result.merge(TrackParticleCnvAlgNoPIDCfg(flags,
                                                 name = "SiSPSeedSegmentsCnvAlg",
                                                 TrackContainerName = "SiSPSeedSegments",
                                                 xAODTrackParticlesFromTracksContainerName = "SiSPSeedSegmentsTrackParticles"))

    if flags.InDet.PriVertex.doVertexFinding:
        from InDetConfig.VertexFindingConfig import primaryVertexFindingCfg
        result.merge(primaryVertexFindingCfg(flags))

    if flags.InDet.Tracking.writeExtendedPRDInfo:
        from InDetConfig.InDetPrepRawDataToxAODConfig import InDetPixelPrepDataToxAODCfg, InDetSCT_PrepDataToxAODCfg, InDetTRT_PrepDataToxAODCfg
        result.merge(InDetPixelPrepDataToxAODCfg(flags, ClusterSplitProbabilityName = ClusterSplitProbabilityContainerName(flags)))
        result.merge(InDetSCT_PrepDataToxAODCfg(flags))
        result.merge(InDetTRT_PrepDataToxAODCfg(flags))

        from DerivationFrameworkInDet.InDetToolsConfig import TrackStateOnSurfaceDecoratorCfg
        TrackStateOnSurfaceDecorator = result.getPrimaryAndMerge(TrackStateOnSurfaceDecoratorCfg(flags, name="TrackStateOnSurfaceDecorator"))
        result.addEventAlgo(CompFactory.DerivationFramework.CommonAugmentation("InDetCommonKernel", AugmentationTools = [TrackStateOnSurfaceDecorator]))

        if flags.Input.isMC:
            from InDetPhysValMonitoring.InDetPhysValDecorationConfig import InDetPhysHitDecoratorAlgCfg
            result.merge(InDetPhysHitDecoratorAlgCfg(flags))

    # output
    result.merge(InDetTrackRecoOutputCfg(flags))

    return result


# these are used internally by the FTAG software. We generally don't
# want to save them since they can be reconstructed by rerunning
# flavor tagging.
FTAG_AUXDATA = [
    'VxTrackAtVertex',
    'TrackCompatibility',
    'JetFitter_TrackCompatibility_antikt4emtopo',
    'JetFitter_TrackCompatibility_antikt4empflow',
    'btagIp_d0Uncertainty',
    'btagIp_z0SinThetaUncertainty',
    'btagIp_z0SinTheta',
    'btagIp_d0',
    'btagIp_trackMomentum',
    'btagIp_trackDisplacement',
    'btagIp_invalidIp',
 ]

def InDetTrackRecoOutputCfg(flags):
    from OutputStreamAthenaPool.OutputStreamConfig import addToESD,addToAOD
    toAOD = []
    toESD = []

    # FIXME: special branches without flags
    special = False

    # excluded track aux data
    excludedAuxData = "-clusterAssociation.-TTVA_AMVFVertices_forReco.-TTVA_AMVFWeights_forReco"
    # remove track decorations used internally by FTAG software
    excludedAuxData += '.-'.join([''] + FTAG_AUXDATA)

    if not special: #not flags.InDet.keepFirstParameters or flags.InDet.keepAdditionalHitsOnTrackParticle
        excludedAuxData += '.-trackParameterCovarianceMatrices.-parameterX.-parameterY.-parameterZ.-parameterPX.-parameterPY.-parameterPZ.-parameterPosition'

    # exclude TTVA decorations 
    excludedAuxData += '.-TTVA_AMVFVertices.-TTVA_AMVFWeights'

    # exclude IDTIDE/IDTRKVALID decorations
    excludedAuxData += '.-TrkBLX.-TrkBLY.-TrkBLZ.-TrkIBLX.-TrkIBLY.-TrkIBLZ.-TrkL1X.-TrkL1Y.-TrkL1Z.-TrkL2X.-TrkL2Y.-TrkL2Z'
    if not flags.InDet.Tracking.writeExtendedPRDInfo:
        excludedAuxData += '.-msosLink'

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
    if flags.InDet.doTRTPhase:
        toESD += ["ComTime#TRT_Phase"]

    # Save PRD
    toESD += [
        "InDet::SCT_ClusterContainer#SCT_Clusters",
        "InDet::PixelClusterContainer#PixelClusters",
        "InDet::TRT_DriftCircleContainer#TRT_DriftCircles",
        "InDet::PixelGangedClusterAmbiguities#PixelClusterAmbiguitiesMap",
    ]
    if flags.InDet.Tracking.doPixelClusterSplitting:
        toESD += ["InDet::PixelGangedClusterAmbiguities#SplitClusterAmbiguityMap"]
    toESD += ["IDCInDetBSErrContainer#SCT_FlaggedCondData"]
    toESD += ["Trk::ClusterSplitProbabilityContainer#" + ClusterSplitProbabilityContainerName(flags)]

    # add tracks
    if flags.InDet.Tracking.doStoreTrackSeeds:
        toESD += ["TrackCollection#SiSPSeedSegments"]

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

    if flags.InDet.Tracking.doPseudoTracking:
        toESD += ["TrackCollection#InDetPseudoTracks"]
        if flags.InDet.doTruth:
            toESD += ["TrackTruthCollection#InDetPseudoTracksTruthCollection"]
            toESD += ["DetailedTrackTruthCollection#InDetPseudoTracksDetailedTruth"]

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

    toESD += ["TrackCollection#CombinedInDetTracks"]

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
    if flags.InDet.Tracking.doPseudoTracking:
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

    if flags.InDet.Tracking.writeExtendedPRDInfo:
        toAOD += [
            "xAOD::TrackMeasurementValidationContainer#PixelClusters",
            "xAOD::TrackMeasurementValidationAuxContainer#PixelClustersAux.",
            "xAOD::TrackMeasurementValidationContainer#SCT_Clusters",
            "xAOD::TrackMeasurementValidationAuxContainer#SCT_ClustersAux.",
            "xAOD::TrackMeasurementValidationContainer#TRT_DriftCircles",
            "xAOD::TrackMeasurementValidationAuxContainer#TRT_DriftCirclesAux.",
            "xAOD::TrackStateValidationContainer#PixelMSOSs",
            "xAOD::TrackStateValidationAuxContainer#PixelMSOSsAux.",
            "xAOD::TrackStateValidationContainer#SCT_MSOSs",
            "xAOD::TrackStateValidationAuxContainer#SCT_MSOSsAux.",
            "xAOD::TrackStateValidationContainer#TRT_MSOSs",
            "xAOD::TrackStateValidationAuxContainer#TRT_MSOSsAux."
        ]

    result = ComponentAccumulator()
    result.merge(addToESD(flags, toESD + toAOD))
    result.merge(addToAOD(flags, toAOD))

    return result


if __name__ == "__main__":
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
        from xAODTruthCnv.xAODTruthCnvConfig import GEN_AOD2xAODCfg
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
