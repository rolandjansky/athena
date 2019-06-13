"""Define methods to construct configured TGC Digitization tools and algorithms

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from StoreGate.StoreGateConf import StoreGateSvc
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
from TGC_Digitization.TGC_DigitizationConf import TgcDigitizationTool, TGCDigitizer
from PileUpComps.PileUpCompsConf import PileUpXingFolder

# The earliest and last bunch crossing times for which interactions will be sent
# to the TgcDigitizationTool.
def TGC_FirstXing():
    return -50

def TGC_LastXing():
    return 75

def TGC_ItemList():
    """Return list of item names needed for TGC output"""
    return ["MuonSimDataCollection#*", "TgcRdoContainer#*"]

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
    acc = MuonGeoModelCfg(flags)
    tool = acc.popToolsAndMerge(TGC_DigitizationToolCfg(flags))
    kwargs.setdefault("DigitizationTool", tool)
    acc.addEventAlgo(TGCDigitizer(name,**kwargs))
    acc.merge(OutputStreamCfg(flags, "RDO", TGC_ItemList()))
    return acc

def TGC_OverlayDigitizationToolCfg(flags, name="TGC_OverlayDigitizationTool", **kwargs):
    """Return a ComponentAccumulator with TgcDigitizationTool configured for Overlay"""
    acc = ComponentAccumulator()
    kwargs.setdefault("OnlyUseContainerName", False)
    kwargs.setdefault("OutputObjectName", "StoreGateSvc+" + flags.Overlay.SigPrefix + "TGC_DIGITS")
    if not flags.Overlay.DataOverlay:
        kwargs.setdefault("OutputSDOName", "StoreGateSvc+" + flags.Overlay.SigPrefix + "TGC_SDO")
    acc.setPrivateTools(TgcDigitizationTool(name, **kwargs))
    return acc

def TGC_OverlayDigitizerCfg(flags, name="TGC_OverlayDigitizer", **kwargs):
    """Return a ComponentAccumulator with TGCDigitizer algorithm configured for Overlay"""
    acc = MuonGeoModelCfg(flags)
    tool = acc.popToolsAndMerge(TGC_OverlayDigitizationToolCfg(flags))
    kwargs.setdefault("DigitizationTool", tool)
    acc.addEventAlgo(TGCDigitizer(name, **kwargs))
    return acc

