# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#
# Import sTGC_Digitization job properties
#
from Digitization.DigitizationFlags import jobproperties
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon import CfgMgr
from AthenaCommon.AppMgr import ToolSvc, ServiceMgr

# The earliest bunch crossing time for which interactions will be sent 
# to the sTGCDigitizationTool. BCID = -4
def sTGC_FirstXing(): 
    return -100
 
# The latest bunch crossing time for which interactions will be sent 
# to the sTGCDigitizationTool. BCID = 3
def sTGC_LastXing(): 
    return 100

def sTgcDigitizationTool(name="sTgcDigitizationTool",**kwargs):
    kwargs.setdefault("RndmSvc", jobproperties.Digitization.rndmSvc() )   
    # set rndm seeds
    sTgcRndm = kwargs.setdefault("RndmEngine","sTGC_Digitization")
    jobproperties.Digitization.rndmSeedList.addSeed(sTgcRndm, 49261510, 105132394 )
    if jobproperties.Digitization.doXingByXingPileUp():
        kwargs.setdefault("FirstXing", sTGC_FirstXing() )  # this should match the range for the sTGC in Digitization/share/MuonDigitization.py
        kwargs.setdefault("LastXing",  sTGC_LastXing() )  # this should match the range for the sTGC in Digitization/share/MuonDigitization.py   
    kwargs.setdefault("InputObjectName", "sTGCSensitiveDetector")
    kwargs.setdefault("OutputObjectName", "sTGC_DIGITS")
    kwargs.setdefault("OutputSDOName", "sTGC_SDO")
    kwargs.setdefault("doToFCorrection", True)
    kwargs.setdefault("doEfficiencyCorrection", False)
    kwargs.setdefault("SmearingTool","STgcCalibSmearingTool")
    if 'NewMerge' in jobproperties.Digitization.experimentalDigi():
        kwargs.setdefault("UseMcEventCollectionHelper",True)
    else:
        kwargs.setdefault("UseMcEventCollectionHelper",False)
     
    return CfgMgr.sTgcDigitizationTool(name,**kwargs)

def getSTGCRange(name="sTgcRange", **kwargs): 
    # bunch crossing range in ns 
    kwargs.setdefault('FirstXing', sTGC_FirstXing() ) 
    kwargs.setdefault('LastXing',  sTGC_LastXing() ) 
    kwargs.setdefault('CacheRefreshFrequency', 1.0 ) #default 0 no dataproxy reset 
    kwargs.setdefault('ItemList', ["sTGCSimHitCollection#sTGCSensitiveDetector"] ) 
    return CfgMgr.PileUpXingFolder(name, **kwargs)

def STGC_OverlayDigitizationTool(name="STGC_OverlayDigitizationTool",**kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    kwargs.setdefault("EvtStore", overlayFlags.evtStore())
    return sTgcDigitizationTool(name,**kwargs)
