# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getSTGC_Overlay(name="STGC_Overlay", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    
    if overlayFlags.isOverlayMT():
        kwargs.setdefault("BkgInputKey", overlayFlags.bkgPrefix() + "sTGC_DIGITS")
        kwargs.setdefault("SignalInputKey", overlayFlags.sigPrefix() + "sTGC_DIGITS")
        kwargs.setdefault("OutputKey", "sTGC_DIGITS")
    else:
        kwargs.setdefault("BkgInputKey", overlayFlags.dataStore() + "+sTGC_DIGITS")
        kwargs.setdefault("SignalInputKey", overlayFlags.evtStore() + "+sTGC_DIGITS")
        kwargs.setdefault("OutputKey", overlayFlags.outputStore() + "+sTGC_DIGITS")
    
    kwargs.setdefault("IntegrationWindow", 30) # in ns

    return CfgMgr.STGC_Overlay(name, **kwargs)
