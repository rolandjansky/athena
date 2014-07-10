# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


def CscDigitToCscRDOTool(name,**kwargs):
    
    from Digitization.DigitizationFlags import digitizationFlags
    
    kwargs.setdefault("NumSamples", 4)
    kwargs.setdefault("Latency", 0)
    kwargs.setdefault("addNoise", digitizationFlags.doMuonNoise())
    kwargs.setdefault("RndmSvc",  digitizationFlags.rndmSvc.get_Value())
    kwargs.setdefault("cscCalibTool", "CscCalibTool")

    digitizationFlags.rndmSeedList.addSeed("CscDigitToCscRDOTool", 49261510, 105132394 )
    
    from AthenaCommon import CfgMgr
    return CfgMgr.CscDigitToCscRDOTool(name,**kwargs)

def CscDigitToCscRDO(name,**kwargs):

    kwargs.setdefault("CscDigitToRDOTool", "CscDigitToCscRDOTool")
    
    from AthenaCommon import CfgMgr
    return CfgMgr.CscDigitToCscRDO(name,**kwargs)
