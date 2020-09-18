# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# Import sTGC_Digitization job properties
#
from Digitization.DigitizationFlags import jobproperties
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon import CfgMgr
from AthenaCommon.AppMgr import ToolSvc, ServiceMgr

# The earliest bunch crossing time for which interactions will be sent 
# to the sTGCDigitizationTool. 
def sTGC_FirstXing(): 
    return -375 
 
# The latest bunch crossing time for which interactions will be sent 
# to the sTGCDigitizationTool. 
def sTGC_LastXing(): 
    return 175 

def sTgcDigitizationTool(name="sTgcDigitizationTool",**kwargs):
    if jobproperties.Digitization.doXingByXingPileUp():
        kwargs.setdefault("FirstXing", sTGC_FirstXing() )  # this should match the range for the sTGC in Digitization/share/MuonDigitization.py
        kwargs.setdefault("LastXing",  sTGC_LastXing() )  # this should match the range for the sTGC in Digitization/share/MuonDigitization.py   
    kwargs.setdefault("InputObjectName", "sTGCSensitiveDetector")
    kwargs.setdefault("OutputObjectName", "sTGC_DIGITS")
    kwargs.setdefault("OutputSDOName", "sTGC_SDO")
    kwargs.setdefault("doToFCorrection", True)
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
    if overlayFlags.isOverlayMT():
        kwargs.setdefault("OutputObjectName", overlayFlags.sigPrefix() + "sTGC_DIGITS")
        if not overlayFlags.isDataOverlay():
            kwargs.setdefault("OutputSDOName", overlayFlags.sigPrefix() + "sTGC_SDO")
    else:
        kwargs.setdefault("OutputObjectName", overlayFlags.evtStore() +  "+sTGC_DIGITS")
        if not overlayFlags.isDataOverlay():
            kwargs.setdefault("OutputSDOName", overlayFlags.evtStore() + "+sTGC_SDO")
    return sTgcDigitizationTool(name,**kwargs)

def getSTGC_OverlayDigitizer(name="STGC_OverlayDigitizer", **kwargs):
    kwargs.setdefault("DigitizationTool","STGC_OverlayDigitizationTool")
    # Multi-threading settinggs
    from AthenaCommon.ConcurrencyFlags import jobproperties as concurrencyProps
    is_hive = (concurrencyProps.ConcurrencyFlags.NumThreads() > 0)
    if is_hive:
        kwargs.setdefault('Cardinality', concurrencyProps.ConcurrencyFlags.NumThreads())
        # Set common overlay extra inputs
        kwargs.setdefault("ExtraInputs", [("McEventCollection", "TruthEvent")])

    return CfgMgr.sTGC_Digitizer(name,**kwargs)
