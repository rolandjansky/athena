
def getCscOverlay(name="CscOverlay", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    kwargs.setdefault("BkgInputKey", overlayFlags.dataStore()+"+CSCRDO")
    kwargs.setdefault("SignalInputKey", overlayFlags.evtStore()+"+CSCRDO")
    kwargs.setdefault("OutputKey", "StoreGateSvc+CSCRDO")

    from AthenaCommon import CfgMgr
    return CfgMgr.CscOverlay(name,**kwargs)
