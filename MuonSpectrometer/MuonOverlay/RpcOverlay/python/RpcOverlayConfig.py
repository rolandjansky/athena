# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getRpcOverlay(name="RpcOverlay", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    kwargs.setdefault("MainInputDigitKey",overlayFlags.dataStore()+"+RPC_DIGITS")
    kwargs.setdefault("OverlayInputDigitKey",overlayFlags.evtStore()+"+RPC_DIGITS")
    kwargs.setdefault("OutputDigitKey",overlayFlags.outputStore()+"+RPC_DIGITS")
    kwargs.setdefault("MCStore",overlayFlags.evtStore())
    kwargs.setdefault("DataStore", overlayFlags.dataStore())
    kwargs.setdefault("CopySDO",not overlayFlags.isDataOverlay())
    return CfgMgr.RpcOverlay(name, **kwargs)
