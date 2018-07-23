# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration


def BaseCscDigitToCscRDOTool(name,**kwargs):
    kwargs.setdefault("NumSamples", 4)
    kwargs.setdefault("Latency", 0)
    from Digitization.DigitizationFlags import digitizationFlags
    kwargs.setdefault("addNoise", digitizationFlags.doMuonNoise())
    kwargs.setdefault("RndmSvc",  digitizationFlags.rndmSvc.get_Value())
    kwargs.setdefault("cscCalibTool", "CscCalibTool")
    from AthenaCommon import CfgMgr
    return CfgMgr.CscDigitToCscRDOTool(name,**kwargs)

def CscDigitToCscRDOTool(name,**kwargs):
    from Digitization.DigitizationFlags import digitizationFlags
    digitizationFlags.rndmSeedList.addSeed("CscDigitToCscRDOTool", 49261510, 105132394 )
    return BaseCscDigitToCscRDOTool(name,**kwargs)

def CscDigitToCscRDOTool2(name,**kwargs):
    # consider a separate random number stream
    #from Digitization.DigitizationFlags import digitizationFlags
    #digitizationFlags.rndmSeedList.addSeed("CscDigitToCscRDOTool2", 49261510, 105132394 )
    #kwargs.setdefault("RndmEngine","CscDigitToCscRDOTool2")
    kwargs.setdefault("NumSamples", 2)
    kwargs.setdefault("addNoise", False)
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    kwargs.setdefault("InputObjectName",overlayFlags.evtStore()+"+CSC_DIGITS")
    kwargs.setdefault("OutputObjectName",overlayFlags.evtStore()+"+CSCRDO")
    return BaseCscDigitToCscRDOTool(name,**kwargs)

def CscDigitToCscRDOTool4(name,**kwargs):
    # consider a separate random number stream
    #from Digitization.DigitizationFlags import digitizationFlags
    #digitizationFlags.rndmSeedList.addSeed("CscDigitToCscRDOTool4", 49261510, 105132394 )
    #kwargs.setdefault("RndmEngine","CscDigitToCscRDOTool4")
    kwargs.setdefault("NumSamples", 4)
    kwargs.setdefault("addNoise", False)
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    kwargs.setdefault("InputObjectName",overlayFlags.evtStore()+"+CSC_DIGITS")
    kwargs.setdefault("OutputObjectName",overlayFlags.evtStore()+"+CSCRDO")
    return BaseCscDigitToCscRDOTool(name,**kwargs)

def CscDigitToCscRDO(name,**kwargs):
    kwargs.setdefault("CscDigitToRDOTool", "CscDigitToCscRDOTool")
    from AthenaCommon import CfgMgr
    return CfgMgr.CscDigitToCscRDO(name,**kwargs)
