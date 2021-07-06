# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration


def BaseCscDigitToCscRDOTool(name,**kwargs):
    kwargs.setdefault("NumSamples", 4)
    kwargs.setdefault("Latency", 0)
    from Digitization.DigitizationFlags import digitizationFlags
    kwargs.setdefault("addNoise", digitizationFlags.doMuonNoise())
    kwargs.setdefault("cscCalibTool", "CscCalibTool")
    from AthenaCommon import CfgMgr
    return CfgMgr.CscDigitToCscRDOTool(name,**kwargs)

def CscDigitToCscRDOTool(name,**kwargs):
    from Digitization.DigitizationFlags import digitizationFlags
    if digitizationFlags.PileUpPresampling and 'LegacyOverlay' not in digitizationFlags.experimentalDigi():
        from OverlayCommonAlgs.OverlayFlags import overlayFlags
        kwargs.setdefault("OutputObjectName", overlayFlags.bkgPrefix() + "CSCRDO")
    else:
        kwargs.setdefault("OutputObjectName", "CSCRDO")
    return BaseCscDigitToCscRDOTool(name,**kwargs)

def CscDigitToCscRDOTool2(name,**kwargs):
    kwargs.setdefault("NumSamples", 2)
    kwargs.setdefault("addNoise", False)
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    if overlayFlags.isOverlayMT():
        kwargs.setdefault("InputObjectName", overlayFlags.sigPrefix() + "CSC_DIGITS")
        kwargs.setdefault("OutputObjectName", overlayFlags.sigPrefix() + "CSCRDO")
    else:
        kwargs.setdefault("InputObjectName", overlayFlags.evtStore()+"+CSC_DIGITS")
        kwargs.setdefault("OutputObjectName", overlayFlags.evtStore()+"+CSCRDO")
    return BaseCscDigitToCscRDOTool(name,**kwargs)

def CscDigitToCscRDOTool4(name,**kwargs):
    kwargs.setdefault("NumSamples", 4)
    kwargs.setdefault("addNoise", False)
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    if overlayFlags.isOverlayMT():
        kwargs.setdefault("InputObjectName", overlayFlags.sigPrefix() + "CSC_DIGITS")
        kwargs.setdefault("OutputObjectName", overlayFlags.sigPrefix() + "CSCRDO")
    else:
        kwargs.setdefault("InputObjectName", overlayFlags.evtStore()+"+CSC_DIGITS")
        kwargs.setdefault("OutputObjectName", overlayFlags.evtStore()+"+CSCRDO")
    return BaseCscDigitToCscRDOTool(name,**kwargs)

def CscDigitToCscRDO(name,**kwargs):
    kwargs.setdefault("CscDigitToRDOTool", "CscDigitToCscRDOTool")
    from AthenaCommon import CfgMgr
    return CfgMgr.CscDigitToCscRDO(name,**kwargs)

def CscDigitToCscRDO4(name="CscDigitToCscRDO",**kwargs):
    kwargs.setdefault("CscDigitToRDOTool", "CscDigitToCscRDOTool4")
    from AthenaCommon import CfgMgr
    return CfgMgr.CscDigitToCscRDO(name,**kwargs)
