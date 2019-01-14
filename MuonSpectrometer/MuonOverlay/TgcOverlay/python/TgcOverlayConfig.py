# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getTgcOverlay(name="TgcOverlay", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    kwargs.setdefault("MainInputDigitKey",overlayFlags.dataStore()+"+TGC_DIGITS")
    kwargs.setdefault("OverlayInputDigitKey",overlayFlags.evtStore()+"+TGC_DIGITS")
    kwargs.setdefault("OutputDigitKey",overlayFlags.outputStore()+"+TGC_DIGITS")
    kwargs.setdefault("MCStore",overlayFlags.evtStore())
    kwargs.setdefault("DataStore", overlayFlags.dataStore())
    kwargs.setdefault("CopySDO",not overlayFlags.isDataOverlay())
    return CfgMgr.TgcOverlay(name, **kwargs)
