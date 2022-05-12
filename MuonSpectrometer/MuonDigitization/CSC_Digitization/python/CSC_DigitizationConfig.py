# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

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


def getCscDigitizationToolBase(name, **kwargs):
    if jobproperties.Digitization.doXingByXingPileUp(): # PileUpTool approach
        # This should match the range for the CSC in Simulation/Digitization/share/MuonDigitization.py 
        kwargs.setdefault("FirstXing", CSC_FirstXing() ) 
        kwargs.setdefault("LastXing",  CSC_LastXing() ) 
    from AthenaCommon.DetFlags import DetFlags
    if not DetFlags.pileup.any_on():
        kwargs.setdefault("PileUpMergeSvc", '')
        kwargs.setdefault("OnlyUseContainerName", False)

    kwargs.setdefault("pedestal", 0.0) 	 
    kwargs.setdefault("WindowLowerOffset", -25.0) #-50.0,	 
    kwargs.setdefault("WindowUpperOffset",  25.0) #50.0  	 
    kwargs.setdefault("isPileUp", False)
    kwargs.setdefault("amplification", 0.43e5)
    kwargs.setdefault("NewDigitEDM", True)
    kwargs.setdefault("DriftVelocity", 39)
    kwargs.setdefault("ElectronEnergy", 66) # eV

    return CfgMgr.CscDigitizationTool(name, **kwargs)


def getCscDigitizationTool(name="CscDigitizationTool", **kwargs):
    kwargs.setdefault("InputObjectName", "CSC_Hits")
    kwargs.setdefault("OutputObjectName", "CSC_DIGITS")
    if jobproperties.Digitization.PileUpPresampling and 'LegacyOverlay' not in jobproperties.Digitization.experimentalDigi():
        from OverlayCommonAlgs.OverlayFlags import overlayFlags
        kwargs.setdefault("CSCSimDataCollectionOutputName", overlayFlags.bkgPrefix() + "CSC_SDO")
    else:
        kwargs.setdefault("CSCSimDataCollectionOutputName", "CSC_SDO")
    return getCscDigitizationToolBase(name, **kwargs)


def getCscOverlayDigitizationTool(name="CscOverlayDigitizationTool",**kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    if overlayFlags.isOverlayMT():
        kwargs.setdefault("OnlyUseContainerName", False)
        kwargs.setdefault("OutputObjectName", overlayFlags.sigPrefix() + "CSC_DIGITS")
        if not overlayFlags.isDataOverlay():
            kwargs.setdefault("CSCSimDataCollectionOutputName", overlayFlags.sigPrefix() + "CSC_SDO")
    else:
        kwargs.setdefault("OutputObjectName", overlayFlags.evtStore() +  "+CSC_DIGITS")
        if not overlayFlags.isDataOverlay():
            kwargs.setdefault("CSCSimDataCollectionOutputName", overlayFlags.evtStore() + "+CSC_SDO")
    return getCscDigitizationToolBase(name, **kwargs)


def getCscOverlayDigitBuilder(name="CscOverlayDigitBuilder", **kwargs):
    kwargs.setdefault("DigitizationTool","CscOverlayDigitizationTool")
    # Multi-threading settinggs
    from AthenaCommon.ConcurrencyFlags import jobproperties as concurrencyProps
    is_hive = (concurrencyProps.ConcurrencyFlags.NumThreads() > 0)
    if is_hive:
        kwargs.setdefault('Cardinality', concurrencyProps.ConcurrencyFlags.NumThreads())
        # Set common overlay extra inputs
        kwargs.setdefault("ExtraInputs", [("McEventCollection", "TruthEvent")])

    return CfgMgr.CscDigitBuilder(name,**kwargs)
