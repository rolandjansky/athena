
"""Define methods to construct configured CSC overlay algorithms

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def CSC_DataOverlayExtraCfg(flags, **kwargs):
    """Return a ComponentAccumulator with CSC data overlay specifics"""
    acc = ComponentAccumulator()

    # We need to convert BS to RDO for data overlay
    from MuonConfig.MuonBytestreamDecodeConfig import CscBytestreamDecodeCfg
    acc.merge(CscBytestreamDecodeCfg(flags))

    return acc


def CSC_OverlayAlgCfg(flags, name="CscOverlay", **kwargs):
    """Return a ComponentAccumulator for CSCOverlay algorithm"""
    from MuonConfig.MuonCalibrationConfig import CscCalibToolCfg
    acc = CscCalibToolCfg(flags)
    kwargs.setdefault("CalibTool", acc.popPrivateTools())

    from MuonConfig.MuonCSC_CnvToolsConfig import MuonCscRDODecoderCfg
    kwargs.setdefault("CscRdoDecoderTool", acc.popToolsAndMerge(MuonCscRDODecoderCfg(flags)))

    kwargs.setdefault("BkgInputKey", f"{flags.Overlay.BkgPrefix}CSCRDO")
    kwargs.setdefault("SignalInputKey", f"{flags.Overlay.SigPrefix}CSCRDO")
    kwargs.setdefault("OutputKey", "CSCRDO")

    kwargs.setdefault("isDataOverlay", flags.Overlay.DataOverlay)

    if not flags.Overlay.DataOverlay:
        from SGComps.SGInputLoaderConfig import SGInputLoaderCfg
        acc.merge(SGInputLoaderCfg(flags, [f'CscRawDataContainer#{kwargs["BkgInputKey"]}']))

    # Do CSC overlay
    acc.addEventAlgo(CompFactory.CscOverlay(name, **kwargs))

    # Setup output
    if flags.Output.doWriteRDO:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
            "CscRawDataContainer#CSCRDO"
        ]))

    if flags.Output.doWriteRDO_SGNL:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO_SGNL", ItemList=[
            f"CscRawDataContainer#{flags.Overlay.SigPrefix}CSCRDO"
        ]))

    return acc


def CSC_TruthOverlayCfg(flags, name="CscTruthOverlay", **kwargs):
    """Return a ComponentAccumulator for the CSC SDO overlay algorithm"""
    acc = ComponentAccumulator()

    # We do not need background CSC SDOs
    if flags.Overlay.DataOverlay:
        kwargs.setdefault("BkgInputKey", "")
    else:
        kwargs.setdefault("BkgInputKey", f"{flags.Overlay.BkgPrefix}CSC_SDO")

    if kwargs["BkgInputKey"]:
        from SGComps.SGInputLoaderConfig import SGInputLoaderCfg
        acc.merge(SGInputLoaderCfg(flags, [f'CscSimDataCollection#{kwargs["BkgInputKey"]}']))

    kwargs.setdefault("SignalInputKey", f"{flags.Overlay.SigPrefix}CSC_SDO")
    kwargs.setdefault("OutputKey", "CSC_SDO")

    # Do CSC truth overlay
    acc.addEventAlgo(CompFactory.CscSimDataOverlay(name, **kwargs))

    # Setup output
    if flags.Output.doWriteRDO:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
            "CscSimDataCollection#CSC_SDO"
        ]))

    if flags.Output.doWriteRDO_SGNL:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO_SGNL", ItemList=[
            f"CscSimDataCollection#{flags.Overlay.SigPrefix}CSC_SDO"
        ]))

    return acc


def CSC_OverlayCfg(flags):
    """Configure and return a ComponentAccumulator for CSC overlay"""
    acc = ComponentAccumulator()

    # Add data overlay specifics
    if flags.Overlay.DataOverlay:
        acc.merge(CSC_DataOverlayExtraCfg(flags))

    # Add CSC overlay digitization algorithm
    from MuonConfig.CSC_DigitizationConfig import CSC_OverlayDigitizationBasicCfg
    acc.merge(CSC_OverlayDigitizationBasicCfg(flags))
    # Add CSC digit to RDO config
    from MuonConfig.MuonByteStreamCnvTestConfig import CscDigitToCscRDOCfg
    acc.merge(CscDigitToCscRDOCfg(flags))
    # Add CSC overlay algorithm
    acc.merge(CSC_OverlayAlgCfg(flags))
    # Add CSC truth overlay
    acc.merge(CSC_TruthOverlayCfg(flags))

    return acc
