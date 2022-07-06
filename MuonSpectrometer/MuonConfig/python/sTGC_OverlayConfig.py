
"""Define methods to construct configured sTGC overlay algorithms

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def sTGC_DataOverlayExtraCfg(flags, **kwargs):
    """Return a ComponentAccumulator with sTGC data overlay specifics"""
    acc = ComponentAccumulator()

    # We need to convert BS to RDO for data overlay
    from MuonConfig.MuonBytestreamDecodeConfig import sTgcBytestreamDecodeCfg
    acc.merge(sTgcBytestreamDecodeCfg(flags))

    return acc


def sTGC_OverlayAlgCfg(flags, name="STGC_Overlay", **kwargs):
    """Return a ComponentAccumulator for STGC_Overlay algorithm"""
    acc = ComponentAccumulator()

    kwargs.setdefault("BkgInputKey", f"{flags.Overlay.BkgPrefix}sTGC_DIGITS")
    kwargs.setdefault("SignalInputKey", f"{flags.Overlay.SigPrefix}sTGC_DIGITS")
    kwargs.setdefault("OutputKey", "sTGC_DIGITS")

    # Do sTGC overlay
    acc.addEventAlgo(CompFactory.STGC_Overlay(name, **kwargs))

    # Setup output
    if flags.Output.doWriteRDO:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
            "Muon::STGC_RawDataContainer#sTGCRDO"
        ]))

    if flags.Output.doWriteRDO_SGNL:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO_SGNL", ItemList=[
            f"Muon::STGC_RawDataContainer#{flags.Overlay.SigPrefix}sTGCRDO"
        ]))

    return acc


def sTGC_TruthOverlayCfg(flags, name="STGC_TruthOverlay", **kwargs):
    """Return a ComponentAccumulator for the sTGC SDO overlay algorithm"""
    acc = ComponentAccumulator()

    # We do not need background TGC SDOs
    if flags.Overlay.DataOverlay:
        kwargs.setdefault("BkgInputKey", "")
    else:
        kwargs.setdefault("BkgInputKey", f"{flags.Overlay.BkgPrefix}sTGC_SDO")

    if kwargs["BkgInputKey"]:
        from SGComps.SGInputLoaderConfig import SGInputLoaderCfg
        acc.merge(SGInputLoaderCfg(flags, [f'MuonSimDataCollection#{kwargs["BkgInputKey"]}']))

    kwargs.setdefault("SignalInputKey", f"{flags.Overlay.SigPrefix}sTGC_SDO")
    kwargs.setdefault("OutputKey", "sTGC_SDO")

    # Do TGC truth overlay
    acc.addEventAlgo(CompFactory.MuonSimDataOverlay(name, **kwargs))

    # Setup output
    if flags.Output.doWriteRDO:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
            "MuonSimDataCollection#sTGC_SDO"
        ]))

    if flags.Output.doWriteRDO_SGNL:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO_SGNL", ItemList=[
            f"MuonSimDataCollection#{flags.Overlay.SigPrefix}sTGC_SDO"
        ]))

    return acc


def sTGC_OverlayCfg(flags):
    """Configure and return a ComponentAccumulator for sTGC overlay"""
    acc = ComponentAccumulator()

    # Add data overlay specifics
    if flags.Overlay.DataOverlay:
        acc.merge(sTGC_DataOverlayExtraCfg(flags))

    # Add sTGC RDO to digit config
    from MuonConfig.MuonByteStreamCnvTestConfig import STGC_RdoToDigitCfg
    acc.merge(STGC_RdoToDigitCfg(flags))
    # Add sTGC overlay digitization algorithm
    from MuonConfig.sTGC_DigitizationConfig import sTGC_OverlayDigitizationBasicCfg
    acc.merge(sTGC_OverlayDigitizationBasicCfg(flags))
    # Add sTGC overlay algorithm
    acc.merge(sTGC_OverlayAlgCfg(flags))
    # Add sTGC truth overlay
    acc.merge(sTGC_TruthOverlayCfg(flags))
    # Add sTGC digit to RDO config
    from MuonConfig.MuonByteStreamCnvTestConfig import STGC_DigitToRDOCfg
    acc.merge(STGC_DigitToRDOCfg(flags))

    return acc
