
"""Define methods to construct configured TGC overlay algorithms

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def TGCDataOverlayExtraCfg(flags, **kwargs):
    """Return a ComponentAccumulator with TGC data overlay specifics"""
    acc = ComponentAccumulator()

    # We need to convert BS to RDO for data overlay
    from MuonConfig.MuonBytestreamDecodeConfig import TgcBytestreamDecodeCfg
    acc.merge(TgcBytestreamDecodeCfg(flags))

    return acc


def TgcOverlayAlgCfg(flags, name="TgcOverlay", **kwargs):
    """Return a ComponentAccumulator for TGCOverlay algorithm"""
    acc = ComponentAccumulator()

    kwargs.setdefault("BkgInputKey", flags.Overlay.BkgPrefix + "TGC_DIGITS")
    kwargs.setdefault("SignalInputKey", flags.Overlay.SigPrefix + "TGC_DIGITS")
    kwargs.setdefault("OutputKey", "TGC_DIGITS")

    # Do TGC overlay
    TgcOverlay = CompFactory.TgcOverlay
    alg = TgcOverlay(name, **kwargs)
    acc.addEventAlgo(alg)

    # Setup output
    if flags.Output.doWriteRDO:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
            "TgcRdoContainer#TGCRDO"
        ]))

    if flags.Output.doWriteRDO_SGNL:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO_SGNL", ItemList=[
            "TgcRdoContainer#" + flags.Overlay.SigPrefix + "TGCRDO"
        ]))

    return acc


def TgcTruthOverlayCfg(flags, name="TgcTruthOverlay", **kwargs):
    """Return a ComponentAccumulator for the TGC SDO overlay algorithm"""
    acc = ComponentAccumulator()

    # We do not need background TGC SDOs
    if flags.Overlay.DataOverlay:
        kwargs.setdefault("BkgInputKey", "")
    else:
        kwargs.setdefault("BkgInputKey",
                          flags.Overlay.BkgPrefix + "TGC_SDO")

    kwargs.setdefault("SignalInputKey",
                      flags.Overlay.SigPrefix + "TGC_SDO")
    kwargs.setdefault("OutputKey", "TGC_SDO")

    # Do TGC truth overlay
    MuonSimDataOverlay = CompFactory.MuonSimDataOverlay
    alg = MuonSimDataOverlay(name, **kwargs)
    acc.addEventAlgo(alg)

    # Setup output
    if flags.Output.doWriteRDO:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
            "MuonSimDataCollection#TGC_SDO"
        ]))

    if flags.Output.doWriteRDO_SGNL:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO_SGNL", ItemList=[
            "MuonSimDataCollection#" + flags.Overlay.SigPrefix + "TGC_SDO"
        ]))

    return acc


def TgcOverlayCfg(flags):
    """Configure and return a ComponentAccumulator for TGC overlay"""
    acc = ComponentAccumulator()

    # Add data overlay specifics
    if flags.Overlay.DataOverlay:
        acc.merge(TGCDataOverlayExtraCfg(flags))

    # Add TGC RDO to digit config
    from MuonConfig.MuonByteStreamCnvTestConfig import TgcRdoToTgcDigitCfg
    acc.merge(TgcRdoToTgcDigitCfg(flags))
    # Add TGC overlay digitization algorithm
    from MuonConfig.TGC_DigitizationConfig import TGC_OverlayDigitizationBasicCfg
    acc.merge(TGC_OverlayDigitizationBasicCfg(flags))
    # Add TGC overlay algorithm
    acc.merge(TgcOverlayAlgCfg(flags))
    # Add TGC truth overlay
    acc.merge(TgcTruthOverlayCfg(flags))
    # Add TGC digit to RDO config
    from MuonConfig.MuonByteStreamCnvTestConfig import TgcDigitToTgcRDOCfg
    acc.merge(TgcDigitToTgcRDOCfg(flags))

    return acc
