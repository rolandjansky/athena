
"""Define methods to construct configured RPC overlay algorithms

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def RPCDataOverlayExtraCfg(flags, **kwargs):
    """Return a ComponentAccumulator with RPC data overlay specifics"""
    acc = ComponentAccumulator()

    # We need to convert BS to RDO for data overlay
    from MuonConfig.MuonBytestreamDecodeConfig import RpcBytestreamDecodeCfg
    acc.merge(RpcBytestreamDecodeCfg(flags))

    return acc


def RpcOverlayAlgCfg(flags, name="RpcOverlay", **kwargs):
    """Return a ComponentAccumulator for RPCOverlay algorithm"""
    acc = ComponentAccumulator()

    kwargs.setdefault("BkgInputKey", flags.Overlay.BkgPrefix + "RPC_DIGITS")
    kwargs.setdefault("SignalInputKey", flags.Overlay.SigPrefix + "RPC_DIGITS")
    kwargs.setdefault("OutputKey", "RPC_DIGITS")

    # Do RPC overlay
    RpcOverlay = CompFactory.RpcOverlay
    alg = RpcOverlay(name, **kwargs)
    acc.addEventAlgo(alg)

    # Setup output
    if flags.Output.doWriteRDO:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
            "RpcPadContainer#RPCPAD"
        ]))

    if flags.Output.doWriteRDO_SGNL:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO_SGNL", ItemList=[
            "RpcPadContainer#" + flags.Overlay.SigPrefix + "RPCPAD"
        ]))

    return acc


def RpcTruthOverlayCfg(flags, name="RpcTruthOverlay", **kwargs):
    """Return a ComponentAccumulator for the RPC SDO overlay algorithm"""
    acc = ComponentAccumulator()

    # We do not need background RPC SDOs
    if flags.Overlay.DataOverlay:
        kwargs.setdefault("BkgInputKey", "")
    else:
        kwargs.setdefault("BkgInputKey",
                          flags.Overlay.BkgPrefix + "RPC_SDO")

    kwargs.setdefault("SignalInputKey",
                      flags.Overlay.SigPrefix + "RPC_SDO")
    kwargs.setdefault("OutputKey", "RPC_SDO")

    # Do RPC truth overlay
    MuonSimDataOverlay = CompFactory.MuonSimDataOverlay
    alg = MuonSimDataOverlay(name, **kwargs)
    acc.addEventAlgo(alg)

    # Setup output
    if flags.Output.doWriteRDO:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList=[
            "MuonSimDataCollection#RPC_SDO"
        ]))

    if flags.Output.doWriteRDO_SGNL:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, "RDO_SGNL", ItemList=[
            "MuonSimDataCollection#" + flags.Overlay.SigPrefix + "RPC_SDO"
        ]))

    return acc


def RpcOverlayCfg(flags):
    """Configure and return a ComponentAccumulator for RPC overlay"""
    acc = ComponentAccumulator()

    # Add data overlay specifics
    if flags.Overlay.DataOverlay:
        acc.merge(RPCDataOverlayExtraCfg(flags))

    # Add RPC RDO to digit config
    from MuonConfig.MuonByteStreamCnvTestConfig import RpcRdoToRpcDigitCfg
    acc.merge(RpcRdoToRpcDigitCfg(flags))
    # Add RPC overlay digitization algorithm
    from MuonConfig.RPC_DigitizationConfig import RPC_OverlayDigitizationBasicCfg
    acc.merge(RPC_OverlayDigitizationBasicCfg(flags))
    # Add RPC overlay algorithm
    acc.merge(RpcOverlayAlgCfg(flags))
    # Add RPC truth overlay
    acc.merge(RpcTruthOverlayCfg(flags))
    # Add RPC digit to RDO config
    from MuonConfig.MuonByteStreamCnvTestConfig import RpcDigitToRpcRDOCfg
    acc.merge(RpcDigitToRpcRDOCfg(flags))

    return acc
