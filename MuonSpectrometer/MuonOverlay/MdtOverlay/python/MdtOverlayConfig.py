# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getMdtOverlay(name="MdtOverlay", **kwargs):
    kwargs.setdefault("mainInputMDT_Name", "MDT_DIGITS")
    kwargs.setdefault("overlayInputMDT_Name", "MDT_DIGITS")
    kwargs.setdefault("IntegrationWindow", 20)
    kwargs.setdefault("DigitizationTool", "Mdt_OverlayDigitizationTool")
    kwargs.setdefault("ConvertRDOToDigitTool", "MdtRdoToMdtDigitOverlay")
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    kwargs.setdefault("MCStore",overlayFlags.evtStore())
    kwargs.setdefault("DataStore", overlayFlags.dataStore())
    if overlayFlags.doSignal():
        kwargs.setdefault("CopyObject", True)
    return CfgMgr.MdtOverlay(name, **kwargs)
