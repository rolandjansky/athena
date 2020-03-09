# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getMdtOverlay(name="MdtOverlay", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags

    if overlayFlags.isOverlayMT():
        kwargs.setdefault("BkgInputKey", overlayFlags.bkgPrefix() + "MDT_DIGITS")
        kwargs.setdefault("SignalInputKey", overlayFlags.sigPrefix() + "MDT_DIGITS")
        kwargs.setdefault("OutputKey", "MDT_DIGITS")
    else:
        kwargs.setdefault("BkgInputKey", overlayFlags.dataStore() + "+MDT_DIGITS")
        kwargs.setdefault("SignalInputKey", overlayFlags.evtStore() + "+MDT_DIGITS")
        kwargs.setdefault("OutputKey", overlayFlags.outputStore() + "+MDT_DIGITS")

    kwargs.setdefault("IntegrationWindow", 20) # in ns

    return CfgMgr.MdtOverlay(name, **kwargs)
