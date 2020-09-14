"""Define methods to construct configured Pixel overlay algorithms

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def PixelRawDataProviderAlgCfg(flags, name="PixelRawDataProvider", **kwargs):
    """Return a ComponentAccumulator for pixel raw data provider"""
    # Temporary until available in the central location
    acc = ComponentAccumulator()

    kwargs.setdefault("RDOKey", flags.Overlay.BkgPrefix + "PixelRDOs")

    PixelRawDataProvider = CompFactory.PixelRawDataProvider
    alg = PixelRawDataProvider(name, **kwargs)
    acc.addEventAlgo(alg)

    return acc


def PixelDataOverlayExtraCfg(flags, **kwargs):
    """Return a ComponentAccumulator with pixel data overlay specifics"""
    acc = ComponentAccumulator()

    # We need to convert BS to RDO for data overlay
    acc.merge(PixelRawDataProviderAlgCfg(flags))

    return acc


def PixelOverlayAlgCfg(flags, name="PixelOverlay", **kwargs):
    """Return a ComponentAccumulator for PixelOverlay algorithm"""
    acc = ComponentAccumulator()

    kwargs.setdefault("BkgInputKey", flags.Overlay.BkgPrefix + "PixelRDOs")
    kwargs.setdefault("SignalInputKey", flags.Overlay.SigPrefix + "PixelRDOs")
    kwargs.setdefault("OutputKey", "PixelRDOs")

    # Do Pixel overlay
    PixelOverlay = CompFactory.PixelOverlay
    alg = PixelOverlay(name, **kwargs)
    acc.addEventAlgo(alg)

    # Setup output
    if flags.Output.doWriteRDO:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
            "PixelRDO_Container#PixelRDOs"
        ]))

        if flags.Overlay.DataOverlay:
            acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
                "IDCInDetBSErrContainer#PixelByteStreamErrs"
            ]))

    if flags.Output.doWriteRDO_SGNL:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO_SGNL", ItemList=[
            "PixelRDO_Container#" + flags.Overlay.SigPrefix + "PixelRDOs"
        ]))

    return acc


def PixelTruthOverlayCfg(flags, name="PixelSDOOverlay", **kwargs):
    """Return a ComponentAccumulator for the Pixel SDO overlay algorithm"""
    acc = ComponentAccumulator()

    # We do not need background Pixel SDOs
    kwargs.setdefault("BkgInputKey", "")

    kwargs.setdefault("SignalInputKey",
                      flags.Overlay.SigPrefix + "PixelSDO_Map")
    kwargs.setdefault("OutputKey", "PixelSDO_Map")

    # Do Pixel truth overlay
    InDetSDOOverlay = CompFactory.InDetSDOOverlay
    alg = InDetSDOOverlay(name, **kwargs)
    acc.addEventAlgo(alg)

    # Setup output
    if flags.Output.doWriteRDO:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
            "InDetSimDataCollection#PixelSDO_Map"
        ]))
    
    if flags.Output.doWriteRDO_SGNL:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO_SGNL", ItemList=[
            "InDetSimDataCollection#" + flags.Overlay.SigPrefix + "PixelSDO_Map"
        ]))

    return acc


def PixelOverlayCfg(flags):
    """Configure and return a ComponentAccumulator for Pixel overlay"""
    acc = ComponentAccumulator()

    # Add data overlay specifics
    if flags.Overlay.DataOverlay:
        acc.merge(PixelDataOverlayExtraCfg(flags))

    # Add Pixel overlay digitization algorithm
    from PixelDigitization.PixelDigitizationConfigNew import PixelOverlayDigitizationBasicCfg
    acc.merge(PixelOverlayDigitizationBasicCfg(flags))
    # Add Pixel overlay algorithm
    acc.merge(PixelOverlayAlgCfg(flags))
    # Add Pixel truth overlay
    acc.merge(PixelTruthOverlayCfg(flags))

    return acc
