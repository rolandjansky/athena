"""Define methods to construct configured overlay copy algorithms

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def CopyCaloCalibrationHitContainerAlgCfg(flags, collectionName, name="CopyCaloCalibrationHitContainer", **kwargs):
    """Return a ComponentAccumulator for the CaloCalibrationHitContainer copying"""
    acc = ComponentAccumulator()

    kwargs.setdefault("collectionName", collectionName)
    kwargs.setdefault("SignalInputKey", flags.Overlay.SigPrefix + collectionName)
    kwargs.setdefault("OutputKey", collectionName)

    # Copy CaloCalibrationHitContainer
    CopyCaloCalibrationHitContainer=CompFactory.CopyCaloCalibrationHitContainer
    alg = CopyCaloCalibrationHitContainer(name + collectionName, **kwargs)
    acc.addEventAlgo(alg)

    # Re-map signal address
    from SGComps.AddressRemappingConfig import AddressRemappingCfg
    acc.merge(AddressRemappingCfg([
        "CaloCalibrationHitContainer#" + collectionName + "->" + flags.Overlay.SigPrefix + collectionName
    ]))

    return acc


def CopyInTimeJetTruthInfoCfg(flags, name="CopyInTimeJetTruthInfo", **kwargs):
    """Return a ComponentAccumulator for the in-time pile-up jets copying"""
    acc = ComponentAccumulator()

    kwargs.setdefault("BkgInputKey", flags.Overlay.BkgPrefix + "InTimeAntiKt4TruthJets")
    kwargs.setdefault("OutputKey", "InTimeAntiKt4TruthJets")

    # Copy jets
    CopyJetTruthInfo=CompFactory.CopyJetTruthInfo
    alg = CopyJetTruthInfo(name, **kwargs)
    acc.addEventAlgo(alg)

    return acc


def CopyOutOfTimeJetTruthInfoCfg(flags, name="CopyOutOfTimeJetTruthInfo", **kwargs):
    """Return a ComponentAccumulator for the out-of-time pile-up jets copying"""
    acc = ComponentAccumulator()

    kwargs.setdefault("BkgInputKey", flags.Overlay.BkgPrefix + "OutOfTimeAntiKt4TruthJets")
    kwargs.setdefault("OutputKey", "OutOfTimeAntiKt4TruthJets")

    # Copy jets
    CopyJetTruthInfo=CompFactory.CopyJetTruthInfo
    alg = CopyJetTruthInfo(name, **kwargs)
    acc.addEventAlgo(alg)

    return acc


def CopyJetTruthInfoAlgsCfg(flags, **kwargs):
    """Return a ComponentAccumulator for the CopyJetTruthInfo algorithms"""
    acc = ComponentAccumulator()
    acc.merge(CopyInTimeJetTruthInfoCfg(flags, **kwargs))
    acc.merge(CopyOutOfTimeJetTruthInfoCfg(flags, **kwargs))
    return acc


def CopyMcEventCollectionAlgCfg(flags, name = "CopyMcEventCollection", **kwargs):
    """Return a ComponentAccumulator for the CopyMcEventCollection algorithm"""
    acc = ComponentAccumulator()

    if flags.Overlay.DataOverlay:
        # Disable background for data overlay
        kwargs.setdefault("BkgInputKey", "")
    else:
        kwargs.setdefault("BkgInputKey", "" + flags.Overlay.BkgPrefix + "TruthEvent")
    kwargs.setdefault("SignalInputKey", flags.Overlay.SigPrefix + "TruthEvent")
    kwargs.setdefault("OutputKey", "TruthEvent")

    # Merge and copy McEventCollection
    CopyMcEventCollection=CompFactory.CopyMcEventCollection
    alg = CopyMcEventCollection(name, **kwargs)
    acc.addEventAlgo(alg)

    # Re-map signal address
    from SGComps.AddressRemappingConfig import AddressRemappingCfg
    acc.merge(AddressRemappingCfg([
        "McEventCollection#TruthEvent->" + flags.Overlay.SigPrefix + "TruthEvent"
    ]))

    return acc


def CopyTimingsAlgCfg(flags, name = "CopyTimings", **kwargs):
    """Return a ComponentAccumulator for the CopyTimings algorithm"""
    acc = ComponentAccumulator()

    # Disable background copying
    kwargs.setdefault("BkgInputKey", "")

    kwargs.setdefault("SignalInputKey", flags.Overlay.SigPrefix + "EVNTtoHITS_timings")
    kwargs.setdefault("OutputKey", "EVNTtoHITS_timings")

    # Copy RecoTimingObj
    CopyTimings=CompFactory.CopyTimings
    alg = CopyTimings(name, **kwargs)
    acc.addEventAlgo(alg)

    # Re-map signal address
    from SGComps.AddressRemappingConfig import AddressRemappingCfg
    acc.merge(AddressRemappingCfg([
        "RecoTimingObj#EVNTtoHITS_timings->" + flags.Overlay.SigPrefix + "EVNTtoHITS_timings"
    ]))

    return acc


def CopyTrackRecordCollectionAlgCfg(flags, collectionName, name="CopyTrackRecordCollection", **kwargs):
    """Return a ComponentAccumulator for the TrackRecordCollection copying"""
    acc = ComponentAccumulator()

    kwargs.setdefault("collectionName", collectionName)
    kwargs.setdefault("SignalInputKey", flags.Overlay.SigPrefix + collectionName)
    kwargs.setdefault("OutputKey", collectionName)

    # Copy TrackRecordCollection
    CopyTrackRecordCollection=CompFactory.CopyTrackRecordCollection
    alg = CopyTrackRecordCollection(name + collectionName, **kwargs)
    acc.addEventAlgo(alg)

    # Re-map signal address
    from SGComps.AddressRemappingConfig import AddressRemappingCfg
    acc.merge(AddressRemappingCfg([
        "TrackRecordCollection#" + collectionName + "->" + flags.Overlay.SigPrefix + collectionName
    ]))

    return acc


def CopyCaloCalibrationHitContainerOutputCfg(flags, collectionName, **kwargs):
    """Return CopyCaloCalibrationHitContainer output configuration"""

    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    acc = OutputStreamCfg(flags, "RDO",
                          ItemList=["CaloCalibrationHitContainer#" + collectionName])

    # Add signal output
    if flags.Output.doWriteRDO_SGNL:
        outConfig = OutputStreamCfg(flags, "RDO_SGNL",
                                    ItemList=["CaloCalibrationHitContainer#" + flags.Overlay.SigPrefix + collectionName])
        acc.merge(outConfig)

    return acc


def CopyJetTruthInfoOutputCfg(flags, **kwargs):
    """Return CopyTimings output configuration"""

    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    acc = OutputStreamCfg(flags, "RDO",
                          ItemList=[
                              "xAOD::JetContainer#InTimeAntiKt4TruthJets",
                              "xAOD::JetAuxContainer#InTimeAntiKt4TruthJetsAux.",
                              "xAOD::JetContainer#OutOfTimeAntiKt4TruthJets",
                              "xAOD::JetAuxContainer#OutOfTimeAntiKt4TruthJetsAux."
                          ])

    return acc


def CopyMcEventCollectionOutputCfg(flags, **kwargs):
    """Return CopyMcEventCollection output configuration"""

    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    acc = OutputStreamCfg(flags, "RDO",
                          ItemList=["McEventCollection#TruthEvent"])

    # Add signal output
    if flags.Output.doWriteRDO_SGNL:
        outConfig = OutputStreamCfg(flags, "RDO_SGNL",
                                    ItemList=["McEventCollection#" + flags.Overlay.SigPrefix + "TruthEvent"])
        acc.merge(outConfig)

    return acc


def CopyTimingsOutputCfg(flags, **kwargs):
    """Return CopyTimings output configuration"""

    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    acc = OutputStreamCfg(flags, "RDO",
                          ItemList=["RecoTimingObj#EVNTtoHITS_timings"])

    # Add signal output
    if flags.Output.doWriteRDO_SGNL:
        outConfig = OutputStreamCfg(flags, "RDO_SGNL",
                                    ItemList=["RecoTimingObj#" + flags.Overlay.SigPrefix + "EVNTtoHITS_timings"])
        acc.merge(outConfig)

    return acc


def CopyTrackRecordCollectionOutputCfg(flags, collectionName, **kwargs):
    """Return CopyTrackRecordCollection output configuration"""

    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    acc = OutputStreamCfg(flags, "RDO",
                          ItemList=["TrackRecordCollection#" + collectionName])

    # Add signal output
    if flags.Output.doWriteRDO_SGNL:
        outConfig = OutputStreamCfg(flags, "RDO_SGNL",
                                    ItemList=["TrackRecordCollection#" + flags.Overlay.SigPrefix + collectionName])
        acc.merge(outConfig)

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
        if container in flags.Input.SecondaryCollections:
            availableContainers.append(container)

    for container in availableContainers:
        acc.merge(CopyCaloCalibrationHitContainerAlgCfg(flags, container, **kwargs))
        acc.merge(CopyCaloCalibrationHitContainerOutputCfg(flags, container, **kwargs))

    return acc


def CopyJetTruthInfoCfg(flags, **kwargs):
    """Return overlay configuration for the CopyJetTruthInfo algorithms"""

    if flags.Overlay.DataOverlay:
        return ComponentAccumulator()

    acc = CopyInTimeJetTruthInfoCfg(flags, **kwargs)
    acc.merge(CopyOutOfTimeJetTruthInfoCfg(flags, **kwargs))
    acc.merge(CopyJetTruthInfoOutputCfg(flags, **kwargs))

    return acc


def CopyMcEventCollectionCfg(flags, **kwargs):
    """Return overlay configuration for the CopyMcEventCollection algorithm"""

    acc = CopyMcEventCollectionAlgCfg(flags, **kwargs)
    acc.merge(CopyMcEventCollectionOutputCfg(flags, **kwargs))

    return acc


def CopyTimingsCfg(flags, **kwargs):
    """Return overlay configuration for the CopyTimings algorithm"""

    acc = CopyTimingsAlgCfg(flags, **kwargs)
    acc.merge(CopyTimingsOutputCfg(flags, **kwargs))

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
        if container in flags.Input.SecondaryCollections:
            availableContainers.append(container)

    for container in availableContainers:
        acc.merge(CopyTrackRecordCollectionAlgCfg(flags, container, **kwargs))
        acc.merge(CopyTrackRecordCollectionOutputCfg(flags, container, **kwargs))

    return acc
