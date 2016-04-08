# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getTgcOverlay(name="TgcOverlay", **kwargs):
    kwargs.setdefault("mainInputTGC_Name", "TGC_DIGITS")
    kwargs.setdefault("overlayInputTGC_Name", "TGC_DIGITS")
    kwargs.setdefault("DigitizationTool", "Tgc_OverlayDigitizationTool")
    kwargs.setdefault("ConvertRDOToDigitTool", "TgcRdoToTgcDigitOverlay")
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    kwargs.setdefault("MCStore",overlayFlags.evtStore())
    kwargs.setdefault("DataStore", overlayFlags.dataStore())
    if overlayFlags.doSignal():
        kwargs.setdefault("CopyObject", True)
    return CfgMgr.TgcOverlay(name, **kwargs)

