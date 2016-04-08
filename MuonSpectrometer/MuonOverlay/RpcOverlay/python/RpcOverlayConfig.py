# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getRpcOverlay(name="RpcOverlay", **kwargs):
    kwargs.setdefault("mainInputRPC_Name", "RPC_DIGITS")
    kwargs.setdefault("overlayInputRPC_Name", "RPC_DIGITS")
    kwargs.setdefault("DigitizationTool", "Rpc_OverlayDigitizationTool")
    kwargs.setdefault("ConvertRDOToDigitTool", "RpcRdoToRpcDigitOverlay")
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    kwargs.setdefault("MCStore",overlayFlags.evtStore())
    kwargs.setdefault("DataStore", overlayFlags.dataStore())
    if overlayFlags.doSignal():
        kwargs.setdefault("CopyObject", True)
    return CfgMgr.RpcOverlay(name, **kwargs)
