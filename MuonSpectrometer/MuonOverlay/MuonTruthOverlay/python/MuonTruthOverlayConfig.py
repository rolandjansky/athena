# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getCscTruthOverlay(name="CscTruthOverlay", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags

    if overlayFlags.isDataOverlay():
        kwargs.setdefault("BkgInputKey", "")
    else:
        kwargs.setdefault("BkgInputKey", overlayFlags.dataStore() + "+CSC_SDO");
    kwargs.setdefault("SignalInputKey", overlayFlags.evtStore() + "+CSC_SDO");
    kwargs.setdefault("OutputKey", overlayFlags.outputStore() + "+CSC_SDO");

    return CfgMgr.CscSimDataOverlay(name, **kwargs)


def getMdtTruthOverlay(name="MdtTruthOverlay", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags

    if overlayFlags.isDataOverlay():
        kwargs.setdefault("BkgInputKey", "")
    else:
        kwargs.setdefault("BkgInputKey", overlayFlags.dataStore() + "+MDT_SDO");
    kwargs.setdefault("SignalInputKey", overlayFlags.evtStore() + "+MDT_SDO");
    kwargs.setdefault("OutputKey", overlayFlags.outputStore() + "+MDT_SDO");
    
    kwargs.setdefault("OutputLevel", 1)

    return CfgMgr.MuonSimDataOverlay(name, **kwargs)


def getRpcTruthOverlay(name="RpcTruthOverlay", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags

    if overlayFlags.isDataOverlay():
        kwargs.setdefault("BkgInputKey", "")
    else:
        kwargs.setdefault("BkgInputKey", overlayFlags.dataStore() + "+RPC_SDO");
    kwargs.setdefault("SignalInputKey", overlayFlags.evtStore() + "+RPC_SDO");
    kwargs.setdefault("OutputKey", overlayFlags.outputStore() + "+RPC_SDO");

    return CfgMgr.MuonSimDataOverlay(name, **kwargs)


def getTgcTruthOverlay(name="TgcTruthOverlay", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags

    if overlayFlags.isDataOverlay():
        kwargs.setdefault("BkgInputKey", "")
    else:
        kwargs.setdefault("BkgInputKey", overlayFlags.dataStore() + "+TGC_SDO");
    kwargs.setdefault("SignalInputKey", overlayFlags.evtStore() + "+TGC_SDO");
    kwargs.setdefault("OutputKey", overlayFlags.outputStore() + "+TGC_SDO");

    return CfgMgr.MuonSimDataOverlay(name, **kwargs)
