# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getSTGC_Overlay(name="STGC_Overlay", **kwargs):
    kwargs.setdefault("mainInputSTGC_Name", "sTGC_DIGITS")
    kwargs.setdefault("overlayInputSTGC_Name", "sTGC_DIGITS")
    kwargs.setdefault("IntegrationWindow", 50)
    kwargs.setdefault("DigitizationTool", "STGC_OverlayDigitizationTool")
    kwargs.setdefault("ConvertRDOToDigitTool", "STGC_RdoToDigitOverlay")
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    kwargs.setdefault("MCStore",overlayFlags.evtStore())
    kwargs.setdefault("DataStore", overlayFlags.dataStore())
    if overlayFlags.doSignal():
        kwargs.setdefault("CopyObject", True)
    return CfgMgr.STGC_Overlay(name, **kwargs)
