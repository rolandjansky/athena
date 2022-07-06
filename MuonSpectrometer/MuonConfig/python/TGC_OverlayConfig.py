
"""Define methods to construct configured TGC overlay algorithms

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def TGC_DataOverlayExtraCfg(flags, **kwargs):
    """Return a ComponentAccumulator with TGC data overlay specifics"""
    acc = ComponentAccumulator()

    # We need to convert BS to RDO for data overlay
    from MuonConfig.MuonBytestreamDecodeConfig import TgcBytestreamDecodeCfg
    acc.merge(TgcBytestreamDecodeCfg(flags))

    return acc


def TGC_OverlayAlgCfg(flags, name="TgcOverlay", **kwargs):
    """Return a ComponentAccumulator for TGCOverlay algorithm"""
    acc = ComponentAccumulator()

    kwargs.setdefault("BkgInputKey", f"{flags.Overlay.BkgPrefix}TGC_DIGITS")
    kwargs.setdefault("SignalInputKey", f"{flags.Overlay.SigPrefix}TGC_DIGITS")
    kwargs.setdefault("OutputKey", "TGC_DIGITS")

    # Do TGC overlay
    acc.addEventAlgo(CompFactory.TgcOverlay(name, **kwargs))

    # Setup output
    if flags.Output.doWriteRDO:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
            "TgcRdoContainer#TGCRDO"
        ]))

    if flags.Output.doWriteRDO_SGNL:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO_SGNL", ItemList=[
            f"TgcRdoContainer#{flags.Overlay.SigPrefix}TGCRDO"
        ]))

    return acc


def TGC_TruthOverlayCfg(flags, name="TgcTruthOverlay", **kwargs):
    """Return a ComponentAccumulator for the TGC SDO overlay algorithm"""
    acc = ComponentAccumulator()

    # We do not need background TGC SDOs
    if flags.Overlay.DataOverlay:
        kwargs.setdefault("BkgInputKey", "")
    else:
        kwargs.setdefault("BkgInputKey", f"{flags.Overlay.BkgPrefix}TGC_SDO")

    if kwargs["BkgInputKey"]:
        from SGComps.SGInputLoaderConfig import SGInputLoaderCfg
        acc.merge(SGInputLoaderCfg(flags, [f'MuonSimDataCollection#{kwargs["BkgInputKey"]}']))

    kwargs.setdefault("SignalInputKey", f"{flags.Overlay.SigPrefix}TGC_SDO")
    kwargs.setdefault("OutputKey", "TGC_SDO")

    # Do TGC truth overlay
    acc.addEventAlgo(CompFactory.MuonSimDataOverlay(name, **kwargs))

    # Setup output
    if flags.Output.doWriteRDO:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
            "MuonSimDataCollection#TGC_SDO"
        ]))

    if flags.Output.doWriteRDO_SGNL:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO_SGNL", ItemList=[
            f"MuonSimDataCollection#{flags.Overlay.SigPrefix}TGC_SDO"
        ]))

    return acc


def TGC_OverlayCfg(flags):
    """Configure and return a ComponentAccumulator for TGC overlay"""
    acc = ComponentAccumulator()

    # Add data overlay specifics
    if flags.Overlay.DataOverlay:
        acc.merge(TGC_DataOverlayExtraCfg(flags))

    # Add TGC RDO to digit config
    from MuonConfig.MuonByteStreamCnvTestConfig import TgcRdoToTgcDigitCfg
    acc.merge(TgcRdoToTgcDigitCfg(flags))
    # Add TGC overlay digitization algorithm
    from MuonConfig.TGC_DigitizationConfig import TGC_OverlayDigitizationBasicCfg
    acc.merge(TGC_OverlayDigitizationBasicCfg(flags))
    # Add TGC overlay algorithm
    acc.merge(TGC_OverlayAlgCfg(flags))
    # Add TGC truth overlay
    acc.merge(TGC_TruthOverlayCfg(flags))
    # Add TGC digit to RDO config
    from MuonConfig.MuonByteStreamCnvTestConfig import TgcDigitToTgcRDOCfg
    acc.merge(TgcDigitToTgcRDOCfg(flags))

    return acc
