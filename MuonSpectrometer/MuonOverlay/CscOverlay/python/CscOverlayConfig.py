
def getCscOverlay(name="CscOverlay", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    kwargs.setdefault("InputDataRDOKey", overlayFlags.dataStore()+"+CSCRDO")
    kwargs.setdefault("InputOverlayRDOKey", overlayFlags.evtStore()+"+CSCRDO")
    kwargs.setdefault("OutputContainerKey", "StoreGateSvc+CSCRDO")
    kwargs.setdefault("DigitizationTool", "Csc_OverlayDigitizationTool")
    kwargs.setdefault("MakeRDOTool2", "CscDigitToCscRDOTool2")
    kwargs.setdefault("MakeRDOTool4", "CscDigitToCscRDOTool4")
    from Digitization.DigitizationFlags import digitizationFlags
    kwargs.setdefault("RndmSvc", digitizationFlags.rndmSvc.get_Value())
    digitizationFlags.rndmSeedList.addSeed("CscOverlay", 12261551, 245536301 )
    kwargs.setdefault("MCStore", overlayFlags.evtStore())
    kwargs.setdefault("DataStore", overlayFlags.dataStore())

    from AthenaCommon import CfgMgr
    return CfgMgr.CscOverlay(name,**kwargs)
