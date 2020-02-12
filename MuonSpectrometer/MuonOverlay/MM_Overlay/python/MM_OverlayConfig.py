# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getMM_Overlay(name="MM_Overlay", **kwargs):
    kwargs.setdefault("mainInputMM_Name", "MM_DIGITS")
    kwargs.setdefault("overlayInputMM_Name", "MM_DIGITS")
    kwargs.setdefault("IntegrationWindow", 100)
    kwargs.setdefault("DigitizationTool", "MM_OverlayDigitizationTool")
    kwargs.setdefault("ConvertRDOToDigitTool", "MM_RdoToDigitOverlay")
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    kwargs.setdefault("MCStore",overlayFlags.evtStore())
    kwargs.setdefault("DataStore", overlayFlags.dataStore())
    if overlayFlags.doSignal():
        kwargs.setdefault("CopyObject", True)
    return CfgMgr.MM_Overlay(name, **kwargs)

