"""Define methods to construct configured EventInfoOverlay algorithms

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def EventInfoCnvAlgCfg(flags, name="EventInfoCnvAlg", **kwargs):
    """Return a ComponentAccumulator for EventInfoCnvAlg algorithm"""
    acc = ComponentAccumulator() 

    kwargs.setdefault("AODKey", "McEventInfo")
    kwargs.setdefault("xAODKey", flags.Overlay.SigPrefix + "EventInfo")

    from xAODEventInfoCnv.xAODEventInfoCnvConf import xAODMaker__EventInfoCnvAlg
    alg = xAODMaker__EventInfoCnvAlg(name, **kwargs)
    acc.addEventAlgo(alg)

    return acc


def EventInfoOverlayCfg(flags, name="EventInfoOverlay", **kwargs):
    """Return a ComponentAccumulator for EventInfoOverlay algorithm"""
    acc = ComponentAccumulator() 

    kwargs.setdefault("BkgInputKey", flags.Overlay.BkgPrefix + "EventInfo")
    kwargs.setdefault("SignalInputKey", flags.Overlay.SigPrefix + "EventInfo")
    kwargs.setdefault("OutputKey", "EventInfo")

    kwargs.setdefault("DataOverlay", flags.Overlay.DataOverlay)

    # Do the xAOD::EventInfo overlay
    from OverlayCommonAlgs.OverlayCommonAlgsConf import EventInfoOverlay
    alg = EventInfoOverlay(name, **kwargs)
    acc.addEventAlgo(alg)

    # Re-map signal address
    from SGComps.AddressRemappingConfig import AddressRemappingCfg
    acc.merge(AddressRemappingCfg([
        "xAOD::EventInfo#EventInfo->" + flags.Overlay.SigPrefix + "EventInfo",
        "xAOD::EventAuxInfo#EventInfoAux.->" + flags.Overlay.SigPrefix + "EventInfoAux.",
    ]))

    return acc
