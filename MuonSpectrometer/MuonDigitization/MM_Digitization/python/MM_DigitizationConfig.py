# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#
# Import MM_Digitization job properties
#
from Digitization.DigitizationFlags import jobproperties
from AthenaCommon import CfgMgr

# The earliest bunch crossing time for which interactions will be sent
# to the MMDigitizationTool.
# MM readout window is 8BC. To be safe, do [-7,7]
def MM_FirstXing():
    return -200

# The latest bunch crossing time for which interactions will be sent
# to the MMDigitizationTool.
# MM readout window is 8BC. To be safe, do [-7,7]
def MM_LastXing():
    return 200

def MM_DigitizationTool(name="MM_DigitizationTool",**kwargs):
    if jobproperties.Digitization.doXingByXingPileUp():
        kwargs.setdefault("FirstXing", MM_FirstXing() ) # this should match the range for the MM in Digitization/share/MuonDigitization.py
        kwargs.setdefault("LastXing",  MM_LastXing() )  # this should match the range for the MM in Digitization/share/MuonDigitization.py
    from AthenaCommon.DetFlags import DetFlags
    if not DetFlags.pileup.any_on():
        kwargs.setdefault("MergeSvc", '')
        kwargs.setdefault("OnlyUseContainerName", False)
    kwargs.setdefault("CheckSimHits", True)
    kwargs.setdefault("InputObjectName", "MicromegasSensitiveDetector")
    kwargs.setdefault("OutputObjectName", "MM_DIGITS")
    if jobproperties.Digitization.PileUpPresampling and 'LegacyOverlay' not in jobproperties.Digitization.experimentalDigi():
        from OverlayCommonAlgs.OverlayFlags import overlayFlags
        kwargs.setdefault("OutputSDOName", overlayFlags.bkgPrefix() + "MM_SDO")
    else:
        kwargs.setdefault("OutputSDOName", "MM_SDO")
    kwargs.setdefault("SmearingTool","MMCalibSmearingTool")
    if 'NewMerge' in jobproperties.Digitization.experimentalDigi():
        kwargs.setdefault("UseMcEventCollectionHelper",True)
    else:
        kwargs.setdefault("UseMcEventCollectionHelper",False)
    
    from AthenaCommon.AlgSequence import AthSequencer
    condSequence = AthSequencer("AthCondSeq")
    if not hasattr(condSequence,"MuonDetectorCondAlg"):
        import MuonRecExample.MuonAlignConfig  # noqa: F401 (import side-effects)
    return CfgMgr.MM_DigitizationTool(name,**kwargs)

def getMMRange(name="MMRange", **kwargs):
    # bunch crossing range in ns
    kwargs.setdefault('FirstXing', MM_FirstXing() )
    kwargs.setdefault('LastXing',  MM_LastXing() )
    kwargs.setdefault('CacheRefreshFrequency', 1.0 ) #default 0 no dataproxy reset
    kwargs.setdefault('ItemList', ["MMSimHitCollection#MicromegasSensitiveDetector"] )
    return CfgMgr.PileUpXingFolder(name, **kwargs)


def MM_OverlayDigitizationTool(name="MM_OverlayDigitizationTool",**kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    if overlayFlags.isOverlayMT():
        kwargs.setdefault("OnlyUseContainerName", False)
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
