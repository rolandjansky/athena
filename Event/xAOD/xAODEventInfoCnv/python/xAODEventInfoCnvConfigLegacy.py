# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr


def getEventInfoOverlay(name="EventInfoOverlay", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags

    kwargs.setdefault("BkgInputKey", overlayFlags.bkgPrefix() + "EventInfo")
    kwargs.setdefault("SignalInputKey", overlayFlags.sigPrefix() + "EventInfo")
    kwargs.setdefault("OutputKey", "EventInfo")

    kwargs.setdefault("DataOverlay", overlayFlags.isDataOverlay())

    return CfgMgr.xAODMaker__EventInfoOverlay(name, **kwargs)
