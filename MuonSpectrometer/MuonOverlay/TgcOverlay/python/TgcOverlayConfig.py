# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getTgcOverlay(name="TgcOverlay", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags

    kwargs.setdefault("BkgInputKey", overlayFlags.dataStore() + "+TGC_DIGITS")
    kwargs.setdefault("SignalInputKey", overlayFlags.evtStore() + "+TGC_DIGITS")
    kwargs.setdefault("OutputKey", overlayFlags.outputStore() + "+TGC_DIGITS")

    return CfgMgr.TgcOverlay(name, **kwargs)
