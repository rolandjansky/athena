"""Define methods to construct configured TGC Digitization tools and algorithms

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from StoreGate.StoreGateConf import StoreGateSvc
from TGC_Digitization.TGC_DigitizationConf import TgcDigitizationTool, TGCDigitizer
from PileUpComps.PileUpCompsConf import PileUpXingFolder

# The earliest and last bunch crossing times for which interactions will be sent
# to the TgcDigitizationTool.
def TGC_FirstXing():
    return -50

def TGC_LastXing():
    return 75

def TGC_RangeToolCfg(flags, name="TGC_Range", **kwargs):
    """Return a PileUpXingFolder tool configured for TGC"""
    kwargs.setdefault("FirstXing", TGC_FirstXing())
    kwargs.setdefault("LastXing", TGC_LastXing())
    kwargs.setdefault("CacheRefreshFrequency", 1.0)
    kwargs.setdefault("ItemList", ["TGCSimHitCollection#TGC_Hits"])
    return PileUpXingFolder(name, **kwargs)

def TGC_DigitizationToolCfg(flags, name="TGC_DigitizationTool", **kwargs):
    """Return a ComponentAccumulator with configured TgcDigitizationTool"""
    acc = ComponentAccumulator()
    if flags.Digitization.DoXingByXingPileUp:
        kwargs.setdefault("FirstXing", TGC_FirstXing()) 
        kwargs.setdefault("LastXing", TGC_LastXing())
    kwargs.setdefault("OutputObjectName", "TGC_DIGITS")
    if flags.Digitization.PileUpPremixing:
        kwargs.setdefault("OutputSDOName", flags.Overlay.BkgPrefix + "TGC_SDO")
    else:
        kwargs.setdefault("OutputSDOName", "TGC_SDO")
    acc.setPrivateTools(TgcDigitizationTool(name, **kwargs))
    return acc

def TGC_DigitizerCfg(flags, name="TGC_Digitizer", **kwargs):
    """Return a ComponentAccumulator with configured TGCDigitizer algorithm"""
    acc = TGC_DigitizationToolCfg(flags)
    kwargs.setdefault("DigitizationTool", acc.popPrivateTools())
    acc.addEventAlgo(TGCDigitizer(name,**kwargs))
    return acc

def TGC_OverlayDigitizationToolCfg(flags, name="TGC_OverlayDigitizationTool", **kwargs):
    """Return a ComponentAccumulator with TgcDigitizationTool configured for Overlay"""
    acc = ComponentAccumulator()
    acc.addService(StoreGateSvc(flags.Overlay.Legacy.EventStore))
    kwargs.setdefault("OutputObjectName", flags.Overlay.Legacy.EventStore + "+TGC_DIGITS")
    if not flags.Detector.Overlay:
        kwargs.setdefault("OutputSDOName", flags.Overlay.Legacy.EventStore + "+TGC_SDO")
    kwargs.setdefault("EvtStore", flags.Overlay.Legacy.EventStore)
    acc.setPrivateTools(TgcDigitizationTool(name,**kwargs))
    return acc

def TGC_OverlayDigitizerCfg(flags, name="TGC_OverlayDigitizer", **kwargs):
    """Return a ComponentAccumulator with TGCDigitizer algorithm configured for Overlay"""
    acc = TGC_OverlayDigitizationToolCfg(flags)
    kwargs.setdefault("DigitizationTool", acc.popPrivateTools())
    acc.addEventAlgo(TGCDigitizer(name,**kwargs))
    return acc

