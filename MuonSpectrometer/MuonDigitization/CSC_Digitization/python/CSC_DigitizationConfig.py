# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from Digitization.DigitizationFlags import jobproperties
from AthenaCommon import CfgMgr

#from AthenaCommon.AppMgr import ToolSvc
#include ("CscCalibTools/CscCalibTool_jobOptions.py")


# The earliest bunch crossing time for which interactions will be sent
# to the CscDigitizationTool.
def CSC_FirstXing():
    return -375

# The latest bunch crossing time for which interactions will be sent
# to the CscDigitizationTool.
def CSC_LastXing():
    return 175

def getCscRange(name="CscRange", **kwargs):
    # bunch crossing range in ns
    kwargs.setdefault('FirstXing', CSC_FirstXing() )
    kwargs.setdefault('LastXing',  CSC_LastXing() )
    kwargs.setdefault('CacheRefreshFrequency', 1.0 ) #default 0 no dataproxy reset
    kwargs.setdefault('ItemList', ["CSCSimHitCollection#CSC_Hits"] )
    return CfgMgr.PileUpXingFolder(name, **kwargs)


def CscDigitizationTool(name="CscDigitizationTool", **kwargs):
    if jobproperties.Digitization.doXingByXingPileUp(): # PileUpTool approach
        # This should match the range for the CSC in Simulation/Digitization/share/MuonDigitization.py 
        kwargs.setdefault("FirstXing", CSC_FirstXing() ) 
        kwargs.setdefault("LastXing",  CSC_LastXing() ) 

    kwargs.setdefault("InputObjectName", "CSC_Hits") 	 
    kwargs.setdefault("OutputObjectName", "CSC_DIGITS") 	 
    kwargs.setdefault("pedestal", 0.0) 	 
    kwargs.setdefault("WindowLowerOffset", -25.0) #-50.0,	 
    kwargs.setdefault("WindowUpperOffset",  25.0) #50.0  	 
    kwargs.setdefault("isPileUp", False)
    kwargs.setdefault("amplification", 0.43e5)
    kwargs.setdefault("NewDigitEDM", True)
    kwargs.setdefault("DriftVelocity", 39)
    kwargs.setdefault("ElectronEnergy", 66) # eV

    return CfgMgr.CscDigitizationTool(name, **kwargs)

def Csc_OverlayDigitizationTool(name="Csc_OverlayDigitizationTool",**kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    kwargs.setdefault("EvtStore", overlayFlags.evtStore())
    kwargs.setdefault("OutputObjectName",overlayFlags.evtStore()+"+CSC_DIGITS")
    if not overlayFlags.isDataOverlay():
        kwargs.setdefault("CSCSimDataCollectionOutputName",overlayFlags.evtStore()+"+CSC_SDO")
    return CscDigitizationTool(name,**kwargs)
