
"""Define methods to construct configured CSC overlay algorithms

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def CSCDataOverlayExtraCfg(flags, **kwargs):
    """Return a ComponentAccumulator with CSC data overlay specifics"""
    acc = ComponentAccumulator()

    # We need to convert BS to RDO for data overlay
    from MuonConfig.MuonBytestreamDecodeConfig import CscBytestreamDecodeCfg
    acc.merge(CscBytestreamDecodeCfg(flags))

    return acc


def CscOverlayAlgCfg(flags, name="CscOverlay", **kwargs):
    """Return a ComponentAccumulator for CSCOverlay algorithm"""
    from MuonConfig.MuonCalibrationConfig import CscCalibToolCfg
    acc = CscCalibToolCfg(flags)
    kwargs.setdefault("CalibTool", acc.popPrivateTools())

    from MuonConfig.MuonCSC_CnvToolsConfig import MuonCscRDODecoderCfg
    kwargs.setdefault("CscRdoDecoderTool", acc.popToolsAndMerge(MuonCscRDODecoderCfg(flags)))

    kwargs.setdefault("BkgInputKey", flags.Overlay.BkgPrefix + "CSCRDO")
    kwargs.setdefault("SignalInputKey", flags.Overlay.SigPrefix + "CSCRDO")
    kwargs.setdefault("OutputKey", "CSCRDO")

    kwargs.setdefault("isDataOverlay", flags.Overlay.DataOverlay)

    # Do CSC overlay
    CscOverlay = CompFactory.CscOverlay
    alg = CscOverlay(name, **kwargs)
    acc.addEventAlgo(alg)

    # Setup output
    if flags.Output.doWriteRDO:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
            "CscRawDataContainer#CSCRDO"
        ]))

    if flags.Output.doWriteRDO_SGNL:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO_SGNL", ItemList=[
            "CscRawDataContainer#" + flags.Overlay.SigPrefix + "CSCRDO"
        ]))

    return acc


def CscTruthOverlayCfg(flags, name="CscTruthOverlay", **kwargs):
    """Return a ComponentAccumulator for the CSC SDO overlay algorithm"""
    acc = ComponentAccumulator()

    # We do not need background CSC SDOs
    if flags.Overlay.DataOverlay:
        kwargs.setdefault("BkgInputKey", "")
    else:
        kwargs.setdefault("BkgInputKey",
                          flags.Overlay.BkgPrefix + "CSC_SDO")

    kwargs.setdefault("SignalInputKey",
                      flags.Overlay.SigPrefix + "CSC_SDO")
    kwargs.setdefault("OutputKey", "CSC_SDO")

    # Do CSC truth overlay
    CscSimDataOverlay = CompFactory.CscSimDataOverlay
    alg = CscSimDataOverlay(name, **kwargs)
    acc.addEventAlgo(alg)

    # Setup output
    if flags.Output.doWriteRDO:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
            "CscSimDataCollection#CSC_SDO"
        ]))

    if flags.Output.doWriteRDO_SGNL:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO_SGNL", ItemList=[
            "CscSimDataCollection#" + flags.Overlay.SigPrefix + "CSC_SDO"
        ]))

    return acc


def CscOverlayCfg(flags):
    """Configure and return a ComponentAccumulator for CSC overlay"""
    acc = ComponentAccumulator()

    # Add data overlay specifics
    if flags.Overlay.DataOverlay:
        acc.merge(CSCDataOverlayExtraCfg(flags))

    # Add CSC overlay digitization algorithm
    from MuonConfig.CSC_DigitizationConfig import CSC_OverlayDigitizationBasicCfg
    acc.merge(CSC_OverlayDigitizationBasicCfg(flags))
    # Add CSC digit to RDO config
    from MuonConfig.MuonByteStreamCnvTestConfig import CscDigitToCscRDOCfg
    acc.merge(CscDigitToCscRDOCfg(flags))
    # Add CSC overlay algorithm
    acc.merge(CscOverlayAlgCfg(flags))
    # Add CSC truth overlay
    acc.merge(CscTruthOverlayCfg(flags))

    return acc
