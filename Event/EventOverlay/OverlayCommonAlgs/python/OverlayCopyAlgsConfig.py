"""Define methods to construct configured overlay copy algorithms

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def CopyMcEventCollectionCfg(flags, name = "CopyMcEventCollection", **kwargs):
    """Return a ComponentAccumulator for CopyMcEventCollection algorithm"""
    acc = ComponentAccumulator()

    if flags.Overlay.DataOverlay:
        # Disable background for data overlay
        kwargs.setdefault("BkgInputKey", "")
    else:
        kwargs.setdefault("BkgInputKey", "" + flags.Overlay.BkgPrefix + "TruthEvent")
    kwargs.setdefault("SignalInputKey", flags.Overlay.SigPrefix + "TruthEvent")
    kwargs.setdefault("OutputKey", "TruthEvent")

    # Merge and copy McEventCollection
    from OverlayCommonAlgs.OverlayCommonAlgsConf import CopyMcEventCollection
    alg = CopyMcEventCollection(name, **kwargs)
    acc.addEventAlgo(alg)

    # Re-map signal address
    from SGComps.AddressRemappingConfig import AddressRemappingCfg
    acc.merge(AddressRemappingCfg([
        "McEventCollection#TruthEvent->" + flags.Overlay.SigPrefix + "TruthEvent"
    ]))

    # Setup output
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    acc.merge(OutputStreamCfg(flags, "RDO", ItemList = [
        "McEventCollection#TruthEvent"
    ]))

    return acc
