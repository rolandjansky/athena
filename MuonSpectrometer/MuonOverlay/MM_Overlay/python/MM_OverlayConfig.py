# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getMM_Overlay(name="MM_Overlay", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags

    if overlayFlags.isOverlayMT():
        kwargs.setdefault("BkgInputKey", overlayFlags.bkgPrefix() + "MM_DIGITS")
        kwargs.setdefault("SignalInputKey", overlayFlags.sigPrefix() + "MM_DIGITS")
        kwargs.setdefault("OutputKey", "MM_DIGITS")
    else:
        kwargs.setdefault("BkgInputKey", overlayFlags.dataStore() + "+MM_DIGITS")
        kwargs.setdefault("SignalInputKey", overlayFlags.evtStore() + "+MM_DIGITS")
        kwargs.setdefault("OutputKey", overlayFlags.outputStore() + "+MM_DIGITS")

    kwargs.setdefault("IntegrationWindow", 30) # in ns

    return CfgMgr.MM_Overlay(name, **kwargs)
