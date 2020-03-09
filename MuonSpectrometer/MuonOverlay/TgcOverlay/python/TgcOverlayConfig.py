# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getTgcOverlay(name="TgcOverlay", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags

    if overlayFlags.isOverlayMT():
        kwargs.setdefault("BkgInputKey", overlayFlags.bkgPrefix() + "TGC_DIGITS")
        kwargs.setdefault("SignalInputKey", overlayFlags.sigPrefix() + "TGC_DIGITS")
        kwargs.setdefault("OutputKey", "TGC_DIGITS")
    else:
        kwargs.setdefault("BkgInputKey", overlayFlags.dataStore() + "+TGC_DIGITS")
        kwargs.setdefault("SignalInputKey", overlayFlags.evtStore() + "+TGC_DIGITS")
        kwargs.setdefault("OutputKey", overlayFlags.outputStore() + "+TGC_DIGITS")

    return CfgMgr.TgcOverlay(name, **kwargs)
