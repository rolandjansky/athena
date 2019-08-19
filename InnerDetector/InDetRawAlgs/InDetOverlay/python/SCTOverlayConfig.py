"""Define methods to construct configured SCT overlay algorithms

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def SCTOverlayAlgCfg(flags, name = "SCTOverlay", **kwargs):
    """Return a ComponentAccumulator for SCTOverlay algorithm"""
    acc = ComponentAccumulator()

    kwargs.setdefault("BkgInputKey", flags.Overlay.BkgPrefix + "SCT_RDOs")
    kwargs.setdefault("SignalInputKey", flags.Overlay.SigPrefix + "SCT_RDOs")
    kwargs.setdefault("OutputKey", "SCT_RDOs")

    kwargs.setdefault("includeBkg", True)

    # Do SCT overlay
    from InDetOverlay.InDetOverlayConf import SCTOverlay
    alg = SCTOverlay(name, **kwargs)
    acc.addEventAlgo(alg)

    # Setup output
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    acc.merge(OutputStreamCfg(flags, "RDO", ItemList = [
        "SCT_RDO_Container#SCT_RDOs"
    ]))

    return acc


def SCTTruthOverlayCfg(flags, name = "SCTSDOOverlay", **kwargs):
    """Return a ComponentAccumulator for the SCT SDO overlay algorithm"""
    acc = ComponentAccumulator()

    # We do not need background SCT SDOs
    kwargs.setdefault("BkgInputKey", "")

    kwargs.setdefault("SignalInputKey", flags.Overlay.BkgPrefix + "SCT_SDO_Map")
    kwargs.setdefault("OutputKey", "SCT_SDO_Map")

    # Do SCT truth overlay
    from InDetOverlay.InDetOverlayConf import InDetSDOOverlay
    alg = InDetSDOOverlay(name, **kwargs)
    acc.addEventAlgo(alg)

    # Setup output
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    acc.merge(OutputStreamCfg(flags, "RDO", ItemList = [
        "InDetSimDataCollection#SCT_SDO_Map"
    ]))

    return acc


def SCTOverlayCfg(flags):
    """Configure and return a ComponentAccumulator for SCT overlay"""
    acc = ComponentAccumulator()

    # Add SCT overlay digitization algorithm
    from SCT_Digitization.SCT_DigitizationConfigNew import SCT_DigitizationOverlayCfg
    acc.merge(SCT_DigitizationOverlayCfg(flags))
    # Add SCT overlay algorithm
    acc.merge(SCTOverlayAlgCfg(flags))
    # Add SCT truth overlay
    acc.merge(SCTTruthOverlayCfg(flags))

    return acc
