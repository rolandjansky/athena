
"""Define methods to construct configured MM overlay algorithms

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def MM_DataOverlayExtraCfg(flags, **kwargs):
    """Return a ComponentAccumulator with MM data overlay specifics"""
    acc = ComponentAccumulator()

    # We need to convert BS to RDO for data overlay
    from MuonConfig.MuonBytestreamDecodeConfig import MmBytestreamDecodeCfg
    acc.merge(MmBytestreamDecodeCfg(flags))

    return acc


def MM_OverlayAlgCfg(flags, name="MM_Overlay", **kwargs):
    """Return a ComponentAccumulator for MM_Overlay algorithm"""
    acc = ComponentAccumulator()

    kwargs.setdefault("BkgInputKey", f"{flags.Overlay.BkgPrefix}MM_DIGITS")
    kwargs.setdefault("SignalInputKey", f"{flags.Overlay.SigPrefix}MM_DIGITS")
    kwargs.setdefault("OutputKey", "MM_DIGITS")

    kwargs.setdefault("IntegrationWindow", 30) # in ns

    # Do MM overlay
    acc.addEventAlgo(CompFactory.MM_Overlay(name, **kwargs))

    # Setup output
    if flags.Output.doWriteRDO:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
            "Muon::MM_RawDataContainer#MMRDO"
        ]))

    if flags.Output.doWriteRDO_SGNL:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO_SGNL", ItemList=[
            f"Muon::MM_RawDataContainer#{flags.Overlay.SigPrefix}MMRDO"
        ]))

    return acc


def MM_TruthOverlayCfg(flags, name="MM_TruthOverlay", **kwargs):
    """Return a ComponentAccumulator for the MM SDO overlay algorithm"""
    acc = ComponentAccumulator()

    # We do not need background MDT SDOs
    if flags.Overlay.DataOverlay:
        kwargs.setdefault("BkgInputKey", "")
    else:
        kwargs.setdefault("BkgInputKey", f"{flags.Overlay.BkgPrefix}MM_SDO")

    if kwargs["BkgInputKey"]:
        from SGComps.SGInputLoaderConfig import SGInputLoaderCfg
        acc.merge(SGInputLoaderCfg(flags, [f'MuonSimDataCollection#{kwargs["BkgInputKey"]}']))

    kwargs.setdefault("SignalInputKey", f"{flags.Overlay.SigPrefix}MM_SDO")
    kwargs.setdefault("OutputKey", "MM_SDO")

    # Do MDT truth overlay
    acc.addEventAlgo(CompFactory.MuonSimDataOverlay(name, **kwargs))

    # Setup output
    if flags.Output.doWriteRDO:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
            "MuonSimDataCollection#MM_SDO"
        ]))

    if flags.Output.doWriteRDO_SGNL:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO_SGNL", ItemList=[
            f"MuonSimDataCollection#{flags.Overlay.SigPrefix}MM_SDO"
        ]))

    return acc


def MM_OverlayCfg(flags):
    """Configure and return a ComponentAccumulator for MM overlay"""
    acc = ComponentAccumulator()

    # Add data overlay specifics
    if flags.Overlay.DataOverlay:
        acc.merge(MM_DataOverlayExtraCfg(flags))

    # Add MM RDO to digit config
    from MuonConfig.MuonByteStreamCnvTestConfig import MM_RdoToDigitCfg
    acc.merge(MM_RdoToDigitCfg(flags))
    # Add MM overlay digitization algorithm
    from MuonConfig.MM_DigitizationConfig import MM_OverlayDigitizationBasicCfg
    acc.merge(MM_OverlayDigitizationBasicCfg(flags))
    # Add MM overlay algorithm
    acc.merge(MM_OverlayAlgCfg(flags))
    # Add MM truth overlay
    acc.merge(MM_TruthOverlayCfg(flags))
    # Add MM digit to RDO config
    from MuonConfig.MuonByteStreamCnvTestConfig import MM_DigitToRDOCfg
    acc.merge(MM_DigitToRDOCfg(flags))

    return acc
