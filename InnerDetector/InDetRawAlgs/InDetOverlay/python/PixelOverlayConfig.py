"""Define methods to construct configured Pixel overlay algorithms

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def PixelOverlayAlgCfg(flags, name = "PixelOverlay", **kwargs):
    """Return a ComponentAccumulator for PixelOverlay algorithm"""
    acc = ComponentAccumulator()

    kwargs.setdefault("BkgInputKey", flags.Overlay.BkgPrefix + "PixelRDOs")
    kwargs.setdefault("SignalInputKey", flags.Overlay.SigPrefix + "PixelRDOs")
    kwargs.setdefault("OutputKey", "PixelRDOs")

    # Do Pixel overlay
    PixelOverlay=CompFactory.PixelOverlay
    alg = PixelOverlay(name, **kwargs)
    acc.addEventAlgo(alg)

    # Setup output
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    acc.merge(OutputStreamCfg(flags, "RDO", ItemList = [
        "PixelRDO_Container#PixelRDOs"
    ]))

    return acc


def PixelTruthOverlayCfg(flags, name = "PixelSDOOverlay", **kwargs):
    """Return a ComponentAccumulator for the Pixel SDO overlay algorithm"""
    acc = ComponentAccumulator()

    # We do not need background Pixel SDOs
    kwargs.setdefault("BkgInputKey", "")

    kwargs.setdefault("SignalInputKey", flags.Overlay.BkgPrefix + "PixelSDO_Map")
    kwargs.setdefault("OutputKey", "PixelSDO_Map")

    # Do Pixel truth overlay
    InDetSDOOverlay=CompFactory.InDetSDOOverlay
    alg = InDetSDOOverlay(name, **kwargs)
    acc.addEventAlgo(alg)

    # Setup output
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    acc.merge(OutputStreamCfg(flags, "RDO", ItemList = [
        "InDetSimDataCollection#PixelSDO_Map"
    ]))

    return acc


def PixelOverlayCfg(flags):
    """Configure and return a ComponentAccumulator for Pixel overlay"""
    acc = ComponentAccumulator()

    # Add Pixel overlay digitization algorithm
    from PixelDigitization.PixelDigitizationConfigNew import PixelOverlayDigitizationCfg
    acc.merge(PixelOverlayDigitizationCfg(flags))
    # Add Pixel overlay algorithm
    acc.merge(PixelOverlayAlgCfg(flags))
    # Add Pixel truth overlay
    acc.merge(PixelTruthOverlayCfg(flags))

    return acc
