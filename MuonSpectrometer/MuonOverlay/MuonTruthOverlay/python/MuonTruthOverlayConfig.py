# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getCscTruthOverlay(name="CscTruthOverlay", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags

    if overlayFlags.isDataOverlay():
        kwargs.setdefault("BkgInputKey", "")
    else:
        if overlayFlags.isOverlayMT():
            kwargs.setdefault("BkgInputKey", overlayFlags.bkgPrefix() + "CSC_SDO")
        else:
            kwargs.setdefault("BkgInputKey", overlayFlags.dataStore() + "+CSC_SDO")

    if overlayFlags.isOverlayMT():
        kwargs.setdefault("SignalInputKey", overlayFlags.sigPrefix() + "CSC_SDO")
        kwargs.setdefault("OutputKey", "CSC_SDO")
    else:
        kwargs.setdefault("SignalInputKey", overlayFlags.evtStore() + "+CSC_SDO")
        kwargs.setdefault("OutputKey", overlayFlags.outputStore() + "+CSC_SDO")

    return CfgMgr.CscSimDataOverlay(name, **kwargs)


def getMdtTruthOverlay(name="MdtTruthOverlay", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags

    if overlayFlags.isDataOverlay():
        kwargs.setdefault("BkgInputKey", "")
    else:
        if overlayFlags.isOverlayMT():
            kwargs.setdefault("BkgInputKey", overlayFlags.bkgPrefix() + "MDT_SDO")
        else:
            kwargs.setdefault("BkgInputKey", overlayFlags.dataStore() + "+MDT_SDO")

    if overlayFlags.isOverlayMT():
        kwargs.setdefault("SignalInputKey", overlayFlags.sigPrefix() + "MDT_SDO")
        kwargs.setdefault("OutputKey", "MDT_SDO")
    else:
        kwargs.setdefault("SignalInputKey", overlayFlags.evtStore() + "+MDT_SDO")
        kwargs.setdefault("OutputKey", overlayFlags.outputStore() + "+MDT_SDO")

    return CfgMgr.MuonSimDataOverlay(name, **kwargs)


def getRpcTruthOverlay(name="RpcTruthOverlay", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags

    if overlayFlags.isDataOverlay():
        kwargs.setdefault("BkgInputKey", "")
    else:
        if overlayFlags.isOverlayMT():
            kwargs.setdefault("BkgInputKey", overlayFlags.bkgPrefix() + "RPC_SDO")
        else:
            kwargs.setdefault("BkgInputKey", overlayFlags.dataStore() + "+RPC_SDO")

    if overlayFlags.isOverlayMT():
        kwargs.setdefault("SignalInputKey", overlayFlags.sigPrefix() + "RPC_SDO")
        kwargs.setdefault("OutputKey", "RPC_SDO")
    else:
        kwargs.setdefault("SignalInputKey", overlayFlags.evtStore() + "+RPC_SDO")
        kwargs.setdefault("OutputKey", overlayFlags.outputStore() + "+RPC_SDO")

    return CfgMgr.MuonSimDataOverlay(name, **kwargs)


def getTgcTruthOverlay(name="TgcTruthOverlay", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags

    if overlayFlags.isDataOverlay():
        kwargs.setdefault("BkgInputKey", "")
    else:
        if overlayFlags.isOverlayMT():
            kwargs.setdefault("BkgInputKey", overlayFlags.bkgPrefix() + "TGC_SDO")
        else:
            kwargs.setdefault("BkgInputKey", overlayFlags.dataStore() + "+TGC_SDO")

    if overlayFlags.isOverlayMT():
        kwargs.setdefault("SignalInputKey", overlayFlags.sigPrefix() + "TGC_SDO")
        kwargs.setdefault("OutputKey", "TGC_SDO")
    else:
        kwargs.setdefault("SignalInputKey", overlayFlags.evtStore() + "+TGC_SDO")
        kwargs.setdefault("OutputKey", overlayFlags.outputStore() + "+TGC_SDO")

    return CfgMgr.MuonSimDataOverlay(name, **kwargs)


def getSTGC_TruthOverlay(name="STGC_TruthOverlay", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags

    if overlayFlags.isDataOverlay():
        kwargs.setdefault("BkgInputKey", "")
    else:
        if overlayFlags.isOverlayMT():
            kwargs.setdefault("BkgInputKey", overlayFlags.bkgPrefix() + "sTGC_SDO")
        else:
            kwargs.setdefault("BkgInputKey", overlayFlags.dataStore() + "+sTGC_SDO")

    if overlayFlags.isOverlayMT():
        kwargs.setdefault("SignalInputKey", overlayFlags.sigPrefix() + "sTGC_SDO")
        kwargs.setdefault("OutputKey", "sTGC_SDO")
    else:
        kwargs.setdefault("SignalInputKey", overlayFlags.evtStore() + "+sTGC_SDO")
        kwargs.setdefault("OutputKey", overlayFlags.outputStore() + "+sTGC_SDO")

    return CfgMgr.MuonSimDataOverlay(name, **kwargs)


def getMM_TruthOverlay(name="MM_TruthOverlay", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags

    if overlayFlags.isDataOverlay():
        kwargs.setdefault("BkgInputKey", "")
    else:
        if overlayFlags.isOverlayMT():
            kwargs.setdefault("BkgInputKey", overlayFlags.bkgPrefix() + "MM_SDO")
        else:
            kwargs.setdefault("BkgInputKey", overlayFlags.dataStore() + "+MM_SDO")

    if overlayFlags.isOverlayMT():
        kwargs.setdefault("SignalInputKey", overlayFlags.sigPrefix() + "MM_SDO")
        kwargs.setdefault("OutputKey", "MM_SDO")
    else:
        kwargs.setdefault("SignalInputKey", overlayFlags.evtStore() + "+MM_SDO")
        kwargs.setdefault("OutputKey", overlayFlags.outputStore() + "+MM_SDO")

    return CfgMgr.MuonSimDataOverlay(name, **kwargs)
