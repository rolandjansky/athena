# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

def getCscOverlay(name="CscOverlay", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    if overlayFlags.isOverlayMT():
        kwargs.setdefault("BkgInputKey", overlayFlags.bkgPrefix() + "CSCRDO")
        kwargs.setdefault("SignalInputKey", overlayFlags.sigPrefix() + "CSCRDO")
        kwargs.setdefault("OutputKey", "CSCRDO")
    else:
        kwargs.setdefault("BkgInputKey", overlayFlags.dataStore()+"+CSCRDO")
        kwargs.setdefault("SignalInputKey", overlayFlags.evtStore()+"+CSCRDO")
        kwargs.setdefault("OutputKey", "StoreGateSvc+CSCRDO")
    kwargs.setdefault("isDataOverlay", overlayFlags.isDataOverlay())
    from AthenaCommon import CfgMgr
    return CfgMgr.CscOverlay(name,**kwargs)
