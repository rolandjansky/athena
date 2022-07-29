"""Define methods to construct configured overlay copy algorithms

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def CopyCaloCalibrationHitContainerAlgCfg(flags, collectionName, name="CopyCaloCalibrationHitContainer", **kwargs):
    """Return a ComponentAccumulator for the CaloCalibrationHitContainer copying"""
    acc = ComponentAccumulator()

    kwargs.setdefault("collectionName", collectionName)
    kwargs.setdefault("SignalInputKey", flags.Overlay.SigPrefix + collectionName)
    kwargs.setdefault("OutputKey", collectionName)

    from SGComps.SGInputLoaderConfig import SGInputLoaderCfg
    acc.merge(SGInputLoaderCfg(flags, [f'CaloCalibrationHitContainer#{kwargs["SignalInputKey"]}']))

    # Copy CaloCalibrationHitContainer
    acc.addEventAlgo(CompFactory.CopyCaloCalibrationHitContainer(name + collectionName, **kwargs))

    # Re-map signal address
    from SGComps.AddressRemappingConfig import AddressRemappingCfg
    acc.merge(AddressRemappingCfg([
        f"CaloCalibrationHitContainer#{collectionName}->{flags.Overlay.SigPrefix}{collectionName}"
    ]))

    # Output
    if flags.Output.doWriteRDO:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
            f"CaloCalibrationHitContainer#{collectionName}"
        ]))

    # Add signal output
    if flags.Output.doWriteRDO_SGNL:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO_SGNL", ItemList=[
            f"CaloCalibrationHitContainer#{flags.Overlay.SigPrefix}{collectionName}"
        ]))

    return acc


def CopyJetTruthInfoAlgCfg(flags, name, **kwargs):
    """Return a ComponentAccumulator for the pile-up jets copying"""
    acc = ComponentAccumulator()

    if "OutputKey" not in kwargs or "BkgInputKey" not in kwargs:
        raise RuntimeError("'OutputKey' and 'BkgInputKey' need to be defined")

    from SGComps.SGInputLoaderConfig import SGInputLoaderCfg
    acc.merge(SGInputLoaderCfg(flags, [f'xAOD::JetContainer#{kwargs["BkgInputKey"]}']))

    # Copy jets
    acc.addEventAlgo(CompFactory.CopyJetTruthInfo(name, **kwargs))

    # Output
    if flags.Output.doWriteRDO:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
            f'xAOD::JetContainer#{kwargs["OutputKey"]}',
            f'xAOD::AuxContainerBase!#{kwargs["OutputKey"]}Aux.-constituentLinks.-constituentWeights',
        ]))

    return acc


def CopyInTimeAntiKt4JetTruthInfoCfg(flags, name="CopyInTimeAntiKt4JetTruthInfo", **kwargs):
    """Return a ComponentAccumulator for the in-time pile-up jets copying"""
    kwargs.setdefault("BkgInputKey", f"{flags.Overlay.BkgPrefix}InTimeAntiKt4TruthJets")
    kwargs.setdefault("OutputKey", "InTimeAntiKt4TruthJets")

    return CopyJetTruthInfoAlgCfg(flags, name, **kwargs)


def CopyOutOfTimeAntiKt4JetTruthInfoCfg(flags, name="CopyOutOfTimeAntiKt4JetTruthInfo", **kwargs):
    """Return a ComponentAccumulator for the out-of-time pile-up jets copying"""
    kwargs.setdefault("BkgInputKey", f"{flags.Overlay.BkgPrefix}OutOfTimeAntiKt4TruthJets")
    kwargs.setdefault("OutputKey", "OutOfTimeAntiKt4TruthJets")

    return CopyJetTruthInfoAlgCfg(flags, name, **kwargs)


def CopyInTimeAntiKt6JetTruthInfoCfg(flags, name="CopyInTimeAntiKt6JetTruthInfo", **kwargs):
    """Return a ComponentAccumulator for the in-time pile-up jets copying"""
    kwargs.setdefault("BkgInputKey", f"{flags.Overlay.BkgPrefix}InTimeAntiKt6TruthJets")
    kwargs.setdefault("OutputKey", "InTimeAntiKt6TruthJets")

    return CopyJetTruthInfoAlgCfg(flags, name, **kwargs)


def CopyOutOfTimeAntiKt6JetTruthInfoCfg(flags, name="CopyOutOfTimeAntiKt6JetTruthInfo", **kwargs):
    """Return a ComponentAccumulator for the out-of-time pile-up jets copying"""
    kwargs.setdefault("BkgInputKey", f"{flags.Overlay.BkgPrefix}OutOfTimeAntiKt6TruthJets")
    kwargs.setdefault("OutputKey", "OutOfTimeAntiKt6TruthJets")

    return CopyJetTruthInfoAlgCfg(flags, name, **kwargs)


def CopyJetTruthInfoCfg(flags, **kwargs):
    """Return a ComponentAccumulator for the CopyJetTruthInfo algorithms"""
    acc = ComponentAccumulator()
    if flags.Overlay.DataOverlay:
        return acc

    allowedContainers = [
        f"{flags.Overlay.BkgPrefix}InTimeAntiKt4TruthJets",
        f"{flags.Overlay.BkgPrefix}OutOfTimeAntiKt4TruthJets",
        f"{flags.Overlay.BkgPrefix}InTimeAntiKt6TruthJets",
        f"{flags.Overlay.BkgPrefix}OutOfTimeAntiKt6TruthJets"
    ]
    availableContainers = []

    # Detect the list of track record collections
    for container in allowedContainers:
        if not flags.Overlay.DataOverlay and container in flags.Input.Collections: #SecondaryCollections
            availableContainers.append(container)
    if allowedContainers[0] in availableContainers:
        acc.merge(CopyInTimeAntiKt4JetTruthInfoCfg(flags, **kwargs))
    if allowedContainers[1] in availableContainers:
        acc.merge(CopyOutOfTimeAntiKt4JetTruthInfoCfg(flags, **kwargs))
    if allowedContainers[2] in availableContainers:
        acc.merge(CopyInTimeAntiKt6JetTruthInfoCfg(flags, **kwargs))
    if allowedContainers[3] in availableContainers:
        acc.merge(CopyOutOfTimeAntiKt6JetTruthInfoCfg(flags, **kwargs))
    return acc


def CopyPileupParticleTruthInfoCfg(flags, name="CopyPileupParticleTruthInfo", **kwargs):
    """Return a ComponentAccumulator for the in-time pile-up jets copying"""
    acc = ComponentAccumulator()

    requiredContainer = f"{flags.Overlay.BkgPrefix}TruthPileupParticles"

    # Detect the list of track record collections
    if not flags.Overlay.DataOverlay and requiredContainer in flags.Input.Collections: # SecondaryCollections
        kwargs.setdefault("BkgInputKey", requiredContainer)
        kwargs.setdefault("OutputKey", "TruthPileupParticles")

        from SGComps.SGInputLoaderConfig import SGInputLoaderCfg
        acc.merge(SGInputLoaderCfg(flags, [f'xAOD::TruthParticleContainer#{kwargs["BkgInputKey"]}']))

        # Copy jets
        acc.addEventAlgo(CompFactory.CopyPileupParticleTruthInfo(name, **kwargs))

        if flags.Output.doWriteRDO:
            from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
            acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
                "xAOD::TruthParticleContainer#TruthPileupParticles",
                "xAOD::TruthParticleAuxContainer#TruthPileupParticlesAux."
            ]))
    return acc


def CopyMcEventCollectionCfg(flags, name="CopyMcEventCollection", **kwargs):
    """Return a ComponentAccumulator for the CopyMcEventCollection algorithm"""
    acc = ComponentAccumulator()

    kwargs.setdefault("RemoveBkgHardScatterTruth", True)

    inputs = []
    if flags.Overlay.DataOverlay:
        # Disable background for data overlay
        kwargs.setdefault("BkgInputKey", "")
    else:
        kwargs.setdefault("BkgInputKey", f"{flags.Overlay.BkgPrefix}TruthEvent")
        inputs.append(f'McEventCollection#{kwargs["BkgInputKey"]}')
    kwargs.setdefault("SignalInputKey", f"{flags.Overlay.SigPrefix}TruthEvent")
    inputs.append(f'McEventCollection#{kwargs["SignalInputKey"]}')

    if flags.Sim.DoFullChain and flags.Common.isOverlay:
        kwargs.setdefault("OutputKey", "BeamTruthEvent")
    else:
        kwargs.setdefault("OutputKey", "TruthEvent")

    from SGComps.SGInputLoaderConfig import SGInputLoaderCfg
    acc.merge(SGInputLoaderCfg(flags, inputs))

    # Merge and copy McEventCollection
    acc.addEventAlgo(CompFactory.CopyMcEventCollection(name, **kwargs))

    # Re-map signal address
    from SGComps.AddressRemappingConfig import AddressRemappingCfg
    acc.merge(AddressRemappingCfg([
        f"McEventCollection#TruthEvent->{flags.Overlay.SigPrefix}TruthEvent"
    ]))

    # Output
    if flags.Output.doWriteRDO:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
            "McEventCollection#TruthEvent"
        ]))

    # Add signal output
    if flags.Output.doWriteRDO_SGNL:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO_SGNL", ItemList=[
            f"McEventCollection#{flags.Overlay.SigPrefix}TruthEvent"
        ]))

    return acc


def CopyPixelClusterContainerAlgCfg(flags, **kwargs):
    """Return a ComponentAccumulator for the CopyPixelClusterContainer algorithm"""
    acc = ComponentAccumulator()

    kwargs.setdefault("InputKey","PixelClusters")
    kwargs.setdefault("OutputKey",flags.Overlay.BkgPrefix+"PixelClusters")

    alg=CompFactory.CopyPixelClusterContainer("CopyPixelClusterContainer", **kwargs)
    acc.addEventAlgo(alg)

    return acc

def CopySCT_ClusterContainerAlgCfg(flags, **kwargs):
    """Return a ComponentAccumulator for the CopySCT_ClusterContainer algorithm"""
    acc = ComponentAccumulator()

    kwargs.setdefault("InputKey","SCT_Clusters")
    kwargs.setdefault("OutputKey",flags.Overlay.BkgPrefix+"SCT_Clusters")

    alg=CompFactory.CopySCT_ClusterContainer("CopySCT_ClusterContainer", **kwargs)
    acc.addEventAlgo(alg)

    return acc

def CopyTRT_DriftCircleContainerAlgCfg(flags, **kwargs):
    """Return a ComponentAccumulator for the CopyTRT_DriftCircleContainer algorithm"""
    acc = ComponentAccumulator()

    kwargs.setdefault("InputKey","TRT_DriftCircles")
    kwargs.setdefault("OutputKey",flags.Overlay.BkgPrefix+"TRT_DriftCircles")

    alg=CompFactory.CopyTRT_DriftCircleContainer("CopyTRT_DriftCircleContainer", **kwargs)
    acc.addEventAlgo(alg)

    return acc

def CopyTrackCollectionAlgCfg(flags, collectionName, **kwargs):
    """Return a ComponentAccumulator for the TrackCollection copying"""
    acc = ComponentAccumulator()

    kwargs.setdefault("OutputKey",flags.Overlay.BkgPrefix + collectionName)
    kwargs.setdefault("InputKey", collectionName)

    alg = CompFactory.CopyTrackCollection("CopyTrackCollection"+collectionName)
    acc.addEventAlgo(alg)

    if flags.Output.doWriteRDO:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
            "TrackCollection#" + flags.Overlay.BkgPrefix+collectionName]))

    return acc


def CopyDetailedTrackTruthCollectionAlgCfg(flags, collectionName, **kwargs):
    """Return a ComponentAccumulator for the DetailedTrackTruthCollection copying"""
    acc = ComponentAccumulator()

    kwargs.setdefault("OutputKey",flags.Overlay.BkgPrefix + collectionName)
    kwargs.setdefault("InputKey", collectionName)

    alg = CompFactory.CopyDetailedTrackTruthCollection("CopyDetailedTrackTruthCollection"+collectionName)
    acc.addEventAlgo(alg)

    if flags.Output.doWriteRDO:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
            "DetailedTrackTruthCollection#" + flags.Overlay.BkgPrefix+collectionName
        ]))

    return acc

def CopyPRD_MultiTruthCollectionAlgCfg(flags, collectionName, **kwargs):
    """Return a ComponentAccumulator for the PRD_MultiTruthCollection copying"""
    acc = ComponentAccumulator()

    kwargs.setdefault("OutputKey",flags.Overlay.BkgPrefix + collectionName)
    kwargs.setdefault("InputKey", collectionName)

    alg = CompFactory.CopyPRD_MultiTruthCollection("CopyPRD_MultiTruthCollection"+collectionName)
    acc.addEventAlgo(alg)

    if flags.Output.doWriteRDO:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
            "PRD_MultiTruthCollection#" + flags.Overlay.BkgPrefix+collectionName
        ]))

    return acc


def CopyTimingsCfg(flags, name="CopyTimings", **kwargs):
    """Return a ComponentAccumulator for the CopyTimings algorithm"""
    acc = ComponentAccumulator()
    if "EVNTtoHITS_timings" not in flags.Input.SecondaryCollections:
        return acc

    # Disable background copying
    kwargs.setdefault("BkgInputKey", "")

    kwargs.setdefault("SignalInputKey", f"{flags.Overlay.SigPrefix}EVNTtoHITS_timings")
    kwargs.setdefault("OutputKey", "EVNTtoHITS_timings")

    from SGComps.SGInputLoaderConfig import SGInputLoaderCfg
    acc.merge(SGInputLoaderCfg(flags, [f'RecoTimingObj#{kwargs["SignalInputKey"]}']))

    # Copy RecoTimingObj
    acc.addEventAlgo(CompFactory.CopyTimings(name, **kwargs))

    # Re-map signal address
    from SGComps.AddressRemappingConfig import AddressRemappingCfg
    acc.merge(AddressRemappingCfg([
        f"RecoTimingObj#EVNTtoHITS_timings->{flags.Overlay.SigPrefix}EVNTtoHITS_timings"
    ]))

    # Output
    if flags.Output.doWriteRDO:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
            "RecoTimingObj#EVNTtoHITS_timings"
        ]))

    # Add signal output
    if flags.Output.doWriteRDO_SGNL:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO_SGNL", ItemList=[
            f"RecoTimingObj#{flags.Overlay.SigPrefix}EVNTtoHITS_timings"
        ]))

    return acc


def CopyTrackRecordCollectionAlgCfg(flags, collectionName, name="CopyTrackRecordCollection", **kwargs):
    """Return a ComponentAccumulator for the TrackRecordCollection copying"""
    acc = ComponentAccumulator()

    kwargs.setdefault("collectionName", collectionName)
    kwargs.setdefault("SignalInputKey", flags.Overlay.SigPrefix + collectionName)
    kwargs.setdefault("OutputKey", collectionName)

    from SGComps.SGInputLoaderConfig import SGInputLoaderCfg
    acc.merge(SGInputLoaderCfg(flags, [f'TrackRecordCollection#{kwargs["SignalInputKey"]}']))

    # Copy TrackRecordCollection
    acc.addEventAlgo(CompFactory.CopyTrackRecordCollection(name + collectionName, **kwargs))

    # Re-map signal address
    from SGComps.AddressRemappingConfig import AddressRemappingCfg
    acc.merge(AddressRemappingCfg([
        f"TrackRecordCollection#{collectionName}->{flags.Overlay.SigPrefix}{collectionName}"
    ]))

    # Output
    if flags.Output.doWriteRDO:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
            f"TrackRecordCollection#{collectionName}"
        ]))

    # Add signal output
    if flags.Output.doWriteRDO_SGNL:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO_SGNL", ItemList=[
            f"TrackRecordCollection#{flags.Overlay.SigPrefix}{collectionName}"
        ]))

    return acc

def CopyCaloCalibrationHitContainersCfg(flags, **kwargs):
    """Return overlay configuration for the CopyCalibrationHitContainer algorithms"""

    acc = ComponentAccumulator()

    allowedContainers = [
        "LArCalibrationHitActive", "LArCalibrationHitDeadMaterial", "LArCalibrationHitInactive"
        "TileCalibrationCellHitCnt", "TileCalibrationDMHitCnt",
        "TileCalibHitActiveCell", "TileCalibHitInactiveCell", "TileCalibHitDeadMaterial",
    ]
    availableContainers = []

    # Detect the list of calibration hit containers
    for container in allowedContainers:
        if (flags.Overlay.DataOverlay and container in flags.Input.Collections) \
            or (not flags.Overlay.DataOverlay and container in flags.Input.SecondaryCollections):
            availableContainers.append(container)

    for container in availableContainers:
        acc.merge(CopyCaloCalibrationHitContainerAlgCfg(flags, container, **kwargs))

    return acc

def CopyPixelClusterContainerCfg(flags, **kwargs):
    """Return overlay configuration for the CopyPixelClusterContainer algorithm"""

    acc = CopyPixelClusterContainerAlgCfg(flags, **kwargs)
    if flags.Output.doWriteRDO:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
            "InDet::PixelClusterContainer#Bkg_PixelClusters"
        ]))

    return acc

def CopySCT_ClusterContainerCfg(flags, **kwargs):
    """Return overlay configuration for the CopySCT_ClusterContainer algorithm"""

    acc = CopySCT_ClusterContainerAlgCfg(flags, **kwargs)
    if flags.Output.doWriteRDO:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
            "InDet::SCT_ClusterContainer#Bkg_SCT_Clusters"
        ]))

    return acc

def CopyTRT_DriftCircleContainerCfg(flags, **kwargs):
    """Return overlay configuration for the CopyTRT_DriftCircleContainer algorithm"""

    acc = CopyTRT_DriftCircleContainerAlgCfg(flags, **kwargs)
    if flags.Output.doWriteRDO:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
            "InDet::TRT_DriftCircleContainer#Bkg_TRT_DriftCircles"]))

    return acc


def CopyTrackRecordCollectionsCfg(flags, **kwargs):
    """Return overlay configuration for the TrackRecordCollection algorithms"""

    acc = ComponentAccumulator()

    allowedContainers = [
        "MuonExitLayer",
        "MuonEntryLayer",
        "CaloEntryLayer"
    ]
    availableContainers = []

    # Detect the list of track record collections
    for container in allowedContainers:
        if (flags.Overlay.DataOverlay and container in flags.Input.Collections) \
            or (not flags.Overlay.DataOverlay and container in flags.Input.SecondaryCollections):
            availableContainers.append(container)

    for container in availableContainers:
        acc.merge(CopyTrackRecordCollectionAlgCfg(flags, container, **kwargs))

    return acc

def CopyTrackCollectionsCfg(flags, **kwargs):
    """ Return overlay configuration for copying tracks"""

    acc = ComponentAccumulator()

    allowedContainers = [
        "CombinedInDetTracks",
        "DisappearingTracks",
        "ResolvedForwardTracks",
        "ResolvedLargeD0Tracks"
    ]

    availableContainers = []

    # Detect the list of track collections
    for container in allowedContainers:
        if (flags.Overlay.DataOverlay and container in flags.Input.Collections) \
            or (not flags.Overlay.DataOverlay and container in flags.Input.SecondaryCollections):
            availableContainers.append(container)

    for container in availableContainers:
        acc.merge(CopyTrackCollectionAlgCfg(flags, container, **kwargs))

    return acc

def CopyDetailedTrackTruthCollectionsCfg(flags, **kwargs):
    """ Return overlay configuration for copying detailed track truth"""

    acc = ComponentAccumulator()

    allowedContainers = [
        "DisappearingTracksDetailedTruth",
        "ResolvedForwardTracksDetailedTruth",
        "CombinedInDetTracksDetailedTruth",
        "ResolvedLargeD0TracksDetailedTruth"
    ]

    availableContainers = []

    # Detect the list of detailed track truth collections
    for container in allowedContainers:
        if (flags.Overlay.DataOverlay and container in flags.Input.Collections) \
            or (not flags.Overlay.DataOverlay and container in flags.Input.SecondaryCollections):
            availableContainers.append(container)

    for container in availableContainers:
        acc.merge(CopyDetailedTrackTruthCollectionAlgCfg(flags, container, **kwargs))

    return acc

def CopyPRD_MultiTruthCollectionsCfg(flags, **kwargs):
    """ Return overlay configuration for copying detailed track truth"""

    acc = ComponentAccumulator()

    allowedContainers = [
        "PRD_MultiTruthTRT",
        "PRD_MultiTruthPixel",
        "PRD_MultiTruthSCT"
    ]

    availableContainers = []

    # Detect the list of detailed track truth collections                                                                                                                
    for container in allowedContainers:
        if (flags.Overlay.DataOverlay and container in flags.Input.Collections) \
            or (not flags.Overlay.DataOverlay and container in flags.Input.SecondaryCollections):
            availableContainers.append(container)

    for container in availableContainers:
        acc.merge(CopyPRD_MultiTruthCollectionAlgCfg(flags, container, **kwargs))

    return acc
