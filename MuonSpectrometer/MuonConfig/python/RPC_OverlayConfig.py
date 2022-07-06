
"""Define methods to construct configured RPC overlay algorithms

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def RPC_DataOverlayExtraCfg(flags, **kwargs):
    """Return a ComponentAccumulator with RPC data overlay specifics"""
    acc = ComponentAccumulator()

    # We need to convert BS to RDO for data overlay
    from MuonConfig.MuonBytestreamDecodeConfig import RpcBytestreamDecodeCfg
    acc.merge(RpcBytestreamDecodeCfg(flags))

    return acc


def RPC_OverlayAlgCfg(flags, name="RpcOverlay", **kwargs):
    """Return a ComponentAccumulator for RPCOverlay algorithm"""
    acc = ComponentAccumulator()

    kwargs.setdefault("BkgInputKey", f"{flags.Overlay.BkgPrefix}RPC_DIGITS")
    kwargs.setdefault("SignalInputKey", f"{flags.Overlay.SigPrefix}RPC_DIGITS")
    kwargs.setdefault("OutputKey", "RPC_DIGITS")

    # Do RPC overlay
    acc.addEventAlgo(CompFactory.RpcOverlay(name, **kwargs))

    # Setup output
    if flags.Output.doWriteRDO:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
            "RpcPadContainer#RPCPAD"
        ]))

    if flags.Output.doWriteRDO_SGNL:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO_SGNL", ItemList=[
            f"RpcPadContainer#{flags.Overlay.SigPrefix}RPCPAD"
        ]))

    return acc


def RPC_TruthOverlayCfg(flags, name="RpcTruthOverlay", **kwargs):
    """Return a ComponentAccumulator for the RPC SDO overlay algorithm"""
    acc = ComponentAccumulator()

    # We do not need background RPC SDOs
    if flags.Overlay.DataOverlay:
        kwargs.setdefault("BkgInputKey", "")
    else:
        kwargs.setdefault("BkgInputKey", f"{flags.Overlay.BkgPrefix}RPC_SDO")

    if kwargs["BkgInputKey"]:
        from SGComps.SGInputLoaderConfig import SGInputLoaderCfg
        acc.merge(SGInputLoaderCfg(flags, [f'MuonSimDataCollection#{kwargs["BkgInputKey"]}']))

    kwargs.setdefault("SignalInputKey", f"{flags.Overlay.SigPrefix}RPC_SDO")
    kwargs.setdefault("OutputKey", "RPC_SDO")

    # Do RPC truth overlay
    acc.addEventAlgo(CompFactory.MuonSimDataOverlay(name, **kwargs))

    # Setup output
    if flags.Output.doWriteRDO:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
            "MuonSimDataCollection#RPC_SDO"
        ]))

    if flags.Output.doWriteRDO_SGNL:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO_SGNL", ItemList=[
            f"MuonSimDataCollection#{flags.Overlay.SigPrefix}RPC_SDO"
        ]))

    return acc


def RPC_OverlayCfg(flags):
    """Configure and return a ComponentAccumulator for RPC overlay"""
    acc = ComponentAccumulator()

    # Add data overlay specifics
    if flags.Overlay.DataOverlay:
        acc.merge(RPC_DataOverlayExtraCfg(flags))

    # Add RPC RDO to digit config
    from MuonConfig.MuonByteStreamCnvTestConfig import RpcRdoToRpcDigitCfg
    acc.merge(RpcRdoToRpcDigitCfg(flags))
    # Add RPC overlay digitization algorithm
    from MuonConfig.RPC_DigitizationConfig import RPC_OverlayDigitizationBasicCfg
    acc.merge(RPC_OverlayDigitizationBasicCfg(flags))
    # Add RPC overlay algorithm
    acc.merge(RPC_OverlayAlgCfg(flags))
    # Add RPC truth overlay
    acc.merge(RPC_TruthOverlayCfg(flags))
    # Add RPC digit to RDO config
    from MuonConfig.MuonByteStreamCnvTestConfig import RpcDigitToRpcRDOCfg
    acc.merge(RpcDigitToRpcRDOCfg(flags))

    return acc
