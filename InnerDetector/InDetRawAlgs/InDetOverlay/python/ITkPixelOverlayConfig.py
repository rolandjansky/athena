"""Define methods to construct configured ITk Pixel overlay algorithms

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def ITkPixelOverlayAlgCfg(flags, name="ITkPixelOverlay", **kwargs):
    """Return a ComponentAccumulator for ITk PixelOverlay algorithm"""
    acc = ComponentAccumulator()

    kwargs.setdefault("BkgInputKey", f"{flags.Overlay.BkgPrefix}ITkPixelRDOs")
    kwargs.setdefault("SignalInputKey", f"{flags.Overlay.SigPrefix}ITkPixelRDOs")
    kwargs.setdefault("OutputKey", "ITkPixelRDOs")

    if not flags.Overlay.DataOverlay:
        from SGComps.SGInputLoaderConfig import SGInputLoaderCfg
        acc.merge(SGInputLoaderCfg(flags, [f'PixelRDO_Container#{kwargs["BkgInputKey"]}']))

    # Do Pixel overlay
    acc.addEventAlgo(CompFactory.PixelOverlay(name, **kwargs))

    # Setup output
    if flags.Output.doWriteRDO:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
            "PixelRDO_Container#ITkPixelRDOs"
        ]))

        if flags.Overlay.DataOverlay:
            acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
                "IDCInDetBSErrContainer#ITkPixelByteStreamErrs"
            ]))

    if flags.Output.doWriteRDO_SGNL:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO_SGNL", ItemList=[
            f"PixelRDO_Container#{flags.Overlay.SigPrefix}ITkPixelRDOs"
        ]))

    return acc


def ITkPixelTruthOverlayCfg(flags, name="ITkPixelSDOOverlay", **kwargs):
    """Return a ComponentAccumulator for the ITk Pixel SDO overlay algorithm"""
    acc = ComponentAccumulator()

    # We do not need background Pixel SDOs
    kwargs.setdefault("BkgInputKey", "")

    kwargs.setdefault("SignalInputKey", f"{flags.Overlay.SigPrefix}ITkPixelSDO_Map")
    kwargs.setdefault("OutputKey", "ITkPixelSDO_Map")

    # Do Pixel truth overlay
    acc.addEventAlgo(CompFactory.InDetSDOOverlay(name, **kwargs))

    # Setup output
    if flags.Output.doWriteRDO:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
            "InDetSimDataCollection#ITkPixelSDO_Map"
        ]))
    
    if flags.Output.doWriteRDO_SGNL:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO_SGNL", ItemList=[
            f"InDetSimDataCollection#{flags.Overlay.SigPrefix}ITkPixelSDO_Map"
        ]))

    return acc


def ITkPixelOverlayCfg(flags):
    """Configure and return a ComponentAccumulator for Pixel overlay"""
    acc = ComponentAccumulator()

    # Add ITk Pixel overlay digitization algorithm
    from PixelDigitization.ITkPixelDigitizationConfig import ITkPixelOverlayDigitizationBasicCfg
    acc.merge(ITkPixelOverlayDigitizationBasicCfg(flags))
    # Add ITk Pixel overlay algorithm
    acc.merge(ITkPixelOverlayAlgCfg(flags))
    # Add ITk Pixel truth overlay
    acc.merge(ITkPixelTruthOverlayCfg(flags))

    return acc
