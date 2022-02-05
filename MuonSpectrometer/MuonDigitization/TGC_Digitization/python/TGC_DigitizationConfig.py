# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

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

def setupTgcDigitASDposCondAlg():
    from AthenaCommon.AlgSequence import AthSequencer
    condSequence = AthSequencer("AthCondSeq")
    if not hasattr(condSequence, "TgcDigitASDposCondAlg"):
        from IOVDbSvc.CondDB import conddb
        conddb.addFolder("TGC_OFL", "/TGC/DIGIT/ASDPOS", className='CondAttrListCollection')
        condSequence += CfgMgr.TgcDigitASDposCondAlg("TgcDigitASDposCondAlg")

def TgcDigitizationTool(name="TgcDigitizationTool", **kwargs):
    if jobproperties.Digitization.doXingByXingPileUp(): # PileUpTool approach
        # This should match the range for the TGC in Simulation/Digitization/share/MuonDigitization.py 
        kwargs.setdefault("FirstXing", TGC_FirstXing() ) 
        kwargs.setdefault("LastXing",  TGC_LastXing() )

    kwargs.setdefault("OutputObjectName", "TGC_DIGITS")
    if jobproperties.Digitization.PileUpPresampling and 'LegacyOverlay' not in jobproperties.Digitization.experimentalDigi():
        from OverlayCommonAlgs.OverlayFlags import overlayFlags
        kwargs.setdefault("OutputSDOName", overlayFlags.bkgPrefix() + "TGC_SDO")
    else:
        kwargs.setdefault("OutputSDOName", "TGC_SDO")

    from Digitization.DigitizationFlags import digitizationFlags
    if digitizationFlags.UseUpdatedTGCConditions():
        setupTgcDigitASDposCondAlg()
        kwargs.setdefault("TGCDigitASDposKey", "TGCDigitASDposData")

    return CfgMgr.TgcDigitizationTool(name, **kwargs)

def getTgcRange(name="TgcRange", **kwargs):
    kwargs.setdefault('FirstXing', TGC_FirstXing() )
    kwargs.setdefault('LastXing',  TGC_LastXing() )
    kwargs.setdefault('CacheRefreshFrequency', 1.0 ) #default 0 no dataproxy reset
    kwargs.setdefault('ItemList', ["TGCSimHitCollection#TGC_Hits"] )
    return CfgMgr.PileUpXingFolder(name, **kwargs)

def Tgc_OverlayDigitizationTool(name="Tgc_OverlayDigitizationTool", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    if overlayFlags.isOverlayMT():
        kwargs.setdefault("OnlyUseContainerName", False)
        kwargs.setdefault("OutputObjectName",overlayFlags.sigPrefix()+"TGC_DIGITS")
        if not overlayFlags.isDataOverlay():
            kwargs.setdefault("OutputSDOName",overlayFlags.sigPrefix()+"TGC_SDO")
    else:
        kwargs.setdefault("OutputObjectName",overlayFlags.evtStore()+"+TGC_DIGITS")
        if not overlayFlags.isDataOverlay():
            kwargs.setdefault("OutputSDOName",overlayFlags.evtStore()+"+TGC_SDO")

    from Digitization.DigitizationFlags import digitizationFlags
    if digitizationFlags.UseUpdatedTGCConditions():
        setupTgcDigitASDposCondAlg()
        kwargs.setdefault("TGCDigitASDposKey", "TGCDigitASDposData")    

    return TgcDigitizationTool(name,**kwargs)

def getTGC_OverlayDigitizer(name="TGC_OverlayDigitizer", **kwargs):
    kwargs.setdefault("DigitizationTool","Tgc_OverlayDigitizationTool")
    # Multi-threading settinggs
    from AthenaCommon.ConcurrencyFlags import jobproperties as concurrencyProps
    is_hive = (concurrencyProps.ConcurrencyFlags.NumThreads() > 0)
    if is_hive:
        kwargs.setdefault('Cardinality', concurrencyProps.ConcurrencyFlags.NumThreads())
        # Set common overlay extra inputs
        kwargs.setdefault("ExtraInputs", [("McEventCollection", "TruthEvent")])

    return CfgMgr.TGCDigitizer(name,**kwargs)
