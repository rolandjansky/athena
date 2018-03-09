# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from Digitization.DigitizationFlags import jobproperties
from AthenaCommon import CfgMgr

# The earliest bunch crossing time for which interactions will be sent
# to the TgcDigitizationTool.
def TGC_FirstXing():
    return -50

# The latest bunch crossing time for which interactions will be sent
# to the TgcDigitizationTool.
def TGC_LastXing():
    return 75

def TgcDigitizationTool(name="TgcDigitizationTool", **kwargs):
    kwargs.setdefault("RndmSvc", jobproperties.Digitization.rndmSvc())
    tgcRndm = kwargs.setdefault("RndmEngine", "TGC_Digitization")
    # set rndm seeds
    jobproperties.Digitization.rndmSeedList.addSeed(tgcRndm, 49261510, 105132394) 
    if jobproperties.Digitization.doXingByXingPileUp(): # PileUpTool approach
        # This should match the range for the TGC in Simulation/Digitization/share/MuonDigitization.py 
        kwargs.setdefault("FirstXing", TGC_FirstXing() ) 
        kwargs.setdefault("LastXing",  TGC_LastXing() )

    # Pile-up premixing - do not include pile-up truth
    if jobproperties.Digitization.PileUpPremixing():
        kwargs.setdefault("IncludePileUpTruth", False)

    return CfgMgr.TgcDigitizationTool(name, **kwargs)

def getTgcRange(name="TgcRange", **kwargs):
    kwargs.setdefault('FirstXing', TGC_FirstXing() )
    kwargs.setdefault('LastXing',  TGC_LastXing() )
    kwargs.setdefault('CacheRefreshFrequency', 1.0 ) #default 0 no dataproxy reset
    kwargs.setdefault('ItemList', ["TGCSimHitCollection#TGC_Hits"] )
    return CfgMgr.PileUpXingFolder(name, **kwargs)

def Tgc_OverlayDigitizationTool(name="Tgc_OverlayDigitizationTool", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    kwargs.setdefault("EvtStore", overlayFlags.evtStore())
    return TgcDigitizationTool(name,**kwargs)
