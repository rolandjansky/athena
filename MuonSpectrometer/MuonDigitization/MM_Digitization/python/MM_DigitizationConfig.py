# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# Import MM_Digitization job properties
#
from Digitization.DigitizationFlags import jobproperties
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon import CfgMgr
from AthenaCommon.AppMgr import ToolSvc, ServiceMgr

# The earliest bunch crossing time for which interactions will be sent
# to the MdtDigitizationTool.
def MM_FirstXing():
    return -250

# The latest bunch crossing time for which interactions will be sent
# to the MdtDigitizationTool.
def MM_LastXing():
    return 200

def MM_DigitizationTool(name="MM_DigitizationTool",**kwargs):
    if jobproperties.Digitization.doXingByXingPileUp():
        kwargs.setdefault("FirstXing", MM_FirstXing() ) # this should match the range for the MM in Digitization/share/MuonDigitization.py
        kwargs.setdefault("LastXing",  MM_LastXing() )  # this should match the range for the MM in Digitization/share/MuonDigitization.py
    kwargs.setdefault("CheckSimHits", True)
    kwargs.setdefault("InputObjectName", "MicromegasSensitiveDetector")
    kwargs.setdefault("OutputObjectName", "MM_DIGITS")
    kwargs.setdefault("OutputSDOName", "MM_SDO")
    kwargs.setdefault("SmearingTool","MMCalibSmearingTool")
    if 'NewMerge' in jobproperties.Digitization.experimentalDigi():
        kwargs.setdefault("UseMcEventCollectionHelper",True)
    else:
        kwargs.setdefault("UseMcEventCollectionHelper",False)

    return CfgMgr.MM_DigitizationTool(name,**kwargs)

def getMMRange(name="MMRange", **kwargs):
    # bunch crossing range in ns
    kwargs.setdefault('FirstXing', MM_FirstXing() )
    kwargs.setdefault('LastXing',  MM_LastXing() )
    kwargs.setdefault('CacheRefreshFrequency', 1.0 ) #default 0 no dataproxy reset
    kwargs.setdefault('ItemList', ["MMSimHitCollection#MicromegasSensitiveDetector"] )
    return CfgMgr.PileUpXingFolder(name, **kwargs)


def MM_Response_DigitTool(name="MM_Response_DigitTool",**kwargs):
    kwargs.setdefault("RndmSvc", jobproperties.Digitization.rndmSvc())
    mmRndm = kwargs.setdefault("RndmEngine", "MMResponse")
    jobproperties.Digitization.rndmSeedList.addSeed(mmRndm, 49261510,105132394 )
    return CfgMgr.MM_Response_DigitTool(name,**kwargs)


def MM_OverlayDigitizationTool(name="MM_OverlayDigitizationTool",**kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    if overlayFlags.isOverlayMT():
        kwargs.setdefault("OutputObjectName", overlayFlags.sigPrefix() + "MM_DIGITS")
        if not overlayFlags.isDataOverlay():
            kwargs.setdefault("OutputSDOName", overlayFlags.sigPrefix() + "MM_SDO")
    else:
        kwargs.setdefault("OutputObjectName", overlayFlags.evtStore() +  "+MM_DIGITS")
        if not overlayFlags.isDataOverlay():
            kwargs.setdefault("OutputSDOName", overlayFlags.evtStore() + "+MM_SDO")
    return MM_DigitizationTool(name,**kwargs)

def getMM_OverlayDigitizer(name="MM_OverlayDigitizer", **kwargs):
    kwargs.setdefault("DigitizationTool","MM_OverlayDigitizationTool")
    # Multi-threading settinggs
    from AthenaCommon.ConcurrencyFlags import jobproperties as concurrencyProps
    is_hive = (concurrencyProps.ConcurrencyFlags.NumThreads() > 0)
    if is_hive:
        kwargs.setdefault('Cardinality', concurrencyProps.ConcurrencyFlags.NumThreads())
        # Set common overlay extra inputs
        kwargs.setdefault("ExtraInputs", [("McEventCollection", "TruthEvent")])

    return CfgMgr.MM_Digitizer(name,**kwargs)
