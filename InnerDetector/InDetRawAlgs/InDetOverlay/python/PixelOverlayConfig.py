"""Define methods to construct configured Pixel overlay algorithms

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory



def PixelDataOverlayExtraCfg(flags, **kwargs):
    """Return a ComponentAccumulator with pixel data overlay specifics"""
    acc = ComponentAccumulator()

    # We need to convert BS to RDO for data overlay
    from PixelRawDataByteStreamCnv.PixelRawDataByteStreamCnvConfig import PixelRawDataProviderAlgCfg
    acc.merge(PixelRawDataProviderAlgCfg(flags, RDOKey=f"{flags.Overlay.BkgPrefix}PixelRDOs"))

    return acc


def PixelOverlayAlgCfg(flags, name="PixelOverlay", **kwargs):
    """Return a ComponentAccumulator for PixelOverlay algorithm"""
    acc = ComponentAccumulator()

    kwargs.setdefault("BkgInputKey", f"{flags.Overlay.BkgPrefix}PixelRDOs")
    kwargs.setdefault("SignalInputKey", f"{flags.Overlay.SigPrefix}PixelRDOs")
    kwargs.setdefault("OutputKey", "PixelRDOs")

    if not flags.Overlay.DataOverlay:
        from SGComps.SGInputLoaderConfig import SGInputLoaderCfg
        acc.merge(SGInputLoaderCfg(flags, [f'PixelRDO_Container#{kwargs["BkgInputKey"]}']))

    # Do Pixel overlay
    acc.addEventAlgo(CompFactory.PixelOverlay(name, **kwargs))

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
            f"PixelRDO_Container#{flags.Overlay.SigPrefix}PixelRDOs"
        ]))

    return acc


def PixelTruthOverlayCfg(flags, name="PixelSDOOverlay", **kwargs):
    """Return a ComponentAccumulator for the Pixel SDO overlay algorithm"""
    acc = ComponentAccumulator()

    # We do not need background Pixel SDOs
    kwargs.setdefault("BkgInputKey", "")

    kwargs.setdefault("SignalInputKey", f"{flags.Overlay.SigPrefix}PixelSDO_Map")
    kwargs.setdefault("OutputKey", "PixelSDO_Map")

    # Do Pixel truth overlay
    acc.addEventAlgo(CompFactory.InDetSDOOverlay(name, **kwargs))

    # Setup output
    if flags.Output.doWriteRDO:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
            "InDetSimDataCollection#PixelSDO_Map"
        ]))
    
    if flags.Output.doWriteRDO_SGNL:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO_SGNL", ItemList=[
            f"InDetSimDataCollection#{flags.Overlay.SigPrefix}PixelSDO_Map"
        ]))

    return acc


def PixelOverlayCfg(flags):
    """Configure and return a ComponentAccumulator for Pixel overlay"""
    acc = ComponentAccumulator()

    # Add data overlay specifics
    if flags.Overlay.DataOverlay:
        acc.merge(PixelDataOverlayExtraCfg(flags))

    # Add Pixel overlay digitization algorithm
    from PixelDigitization.PixelDigitizationConfig import PixelOverlayDigitizationBasicCfg
    acc.merge(PixelOverlayDigitizationBasicCfg(flags))
    #if track overlay, don't run the standard overlay
    if not flags.Overlay.doTrackOverlay:
        # Add Pixel overlay algorithm
        acc.merge(PixelOverlayAlgCfg(flags))
        # Add Pixel truth overlay
        acc.merge(PixelTruthOverlayCfg(flags))

    return acc
