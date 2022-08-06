"""Define methods to construct configured SCT overlay algorithms

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def SCTDataOverlayExtraCfg(flags, **kwargs):
    """Return a ComponentAccumulator with SCT data overlay specifics"""
    acc = ComponentAccumulator()

    # We need to convert BS to RDO for data overlay
    from SCT_RawDataByteStreamCnv.SCT_RawDataByteStreamCnvConfig import SCTOverlayRawDataProviderCfg
    acc.merge(SCTOverlayRawDataProviderCfg(flags, prefix="", **kwargs))

    # Add SCT event flag writer
    from SCT_RawDataByteStreamCnv.SCT_RawDataByteStreamCnvConfig import SCTEventFlagWriterCfg
    acc.merge(SCTEventFlagWriterCfg(flags, prefix=""))

    return acc


def SCTOverlayAlgCfg(flags, name="SCTOverlay", **kwargs):
    """Return a ComponentAccumulator for SCTOverlay algorithm"""
    acc = ComponentAccumulator()

    kwargs.setdefault("BkgInputKey", f"{flags.Overlay.BkgPrefix}SCT_RDOs")
    kwargs.setdefault("SignalInputKey", f"{flags.Overlay.SigPrefix}SCT_RDOs")
    kwargs.setdefault("OutputKey", "SCT_RDOs")

    if not flags.Overlay.DataOverlay:
        from SGComps.SGInputLoaderConfig import SGInputLoaderCfg
        acc.merge(SGInputLoaderCfg(flags, [f'SCT_RDO_Container#{kwargs["BkgInputKey"]}']))

    # Do SCT overlay
    acc.addEventAlgo(CompFactory.SCTOverlay(name, **kwargs))

    # Setup output
    if flags.Output.doWriteRDO:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
            "SCT_RDO_Container#SCT_RDOs"
        ]))

        if flags.Overlay.DataOverlay:
            acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
                "IDCInDetBSErrContainer#SCT_ByteStreamErrs"
            ]))

    if flags.Output.doWriteRDO_SGNL:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO_SGNL", ItemList=[
            f"SCT_RDO_Container#{flags.Overlay.SigPrefix}SCT_RDOs"
        ]))

    return acc


def SCTTruthOverlayCfg(flags, name="SCTSDOOverlay", **kwargs):
    """Return a ComponentAccumulator for the SCT SDO overlay algorithm"""
    acc = ComponentAccumulator()

    # We do not need background SCT SDOs
    kwargs.setdefault("BkgInputKey", "")

    kwargs.setdefault("SignalInputKey", f"{flags.Overlay.SigPrefix}SCT_SDO_Map")
    kwargs.setdefault("OutputKey", "SCT_SDO_Map")

    # Do SCT truth overlay
    acc.addEventAlgo(CompFactory.InDetSDOOverlay(name, **kwargs))

    # Setup output
    if flags.Output.doWriteRDO:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
            "InDetSimDataCollection#SCT_SDO_Map"
        ]))
    
    if flags.Output.doWriteRDO_SGNL:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO_SGNL", ItemList=[
            f"InDetSimDataCollection#{flags.Overlay.SigPrefix}SCT_SDO_Map"
        ]))

    return acc


def SCTOverlayCfg(flags):
    """Configure and return a ComponentAccumulator for SCT overlay"""
    acc = ComponentAccumulator()

    # Add data overlay specifics
    if flags.Overlay.DataOverlay:
        acc.merge(SCTDataOverlayExtraCfg(flags))

    # Add SCT overlay digitization algorithm
    from SCT_Digitization.SCT_DigitizationConfig import SCT_OverlayDigitizationBasicCfg
    acc.merge(SCT_OverlayDigitizationBasicCfg(flags))
    #if track overlay, don't run the standard overlay
    if not flags.Overlay.doTrackOverlay:
        # Add SCT overlay algorithm
        acc.merge(SCTOverlayAlgCfg(flags))
        # Add SCT truth overlay
        acc.merge(SCTTruthOverlayCfg(flags))

    return acc
