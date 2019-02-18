
def getCscOverlay(name="CscOverlay", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    kwargs.setdefault("BkgInputKey", overlayFlags.dataStore()+"+CSCRDO")
    kwargs.setdefault("SignalInputKey", overlayFlags.evtStore()+"+CSCRDO")
    kwargs.setdefault("OutputKey", "StoreGateSvc+CSCRDO")
    kwargs.setdefault("MCStore", overlayFlags.evtStore())
    kwargs.setdefault("DataStore", overlayFlags.dataStore())

    from AthenaCommon import CfgMgr
    return CfgMgr.CscOverlay(name,**kwargs)


def getCscOverlayDigitToRDO(name="CscOverlayDigitToRDO", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    kwargs.setdefault("InputBkgRDOKey", overlayFlags.dataStore()+"+CSCRDO")
    kwargs.setdefault("MakeRDOTool2", "CscDigitToCscRDOTool2")
    kwargs.setdefault("MakeRDOTool4", "CscDigitToCscRDOTool4")

    from AthenaCommon import CfgMgr
    return CfgMgr.CscOverlayDigitToRDO(name,**kwargs)
