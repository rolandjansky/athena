"""Define methods to construct configured TGC Digitization tools and algorithms

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
from TGC_Digitization.TGC_DigitizationConf import TgcDigitizationTool, TGCDigitizer
from PileUpComps.PileUpCompsConf import PileUpXingFolder
from MuonConfig.MuonByteStreamCnvTestConfig import TgcDigitToTgcRDOCfg, TgcOverlayDigitToTgcRDOCfg
from MuonConfig.MuonCablingConfig import TGCCablingConfigCfg

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
    """Return ComponentAccumulator with configured TgcDigitizationTool"""
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


def TGC_OverlayDigitizationToolCfg(flags, name="TGC_OverlayDigitizationTool", **kwargs):
    """Return ComponentAccumulator with TgcDigitizationTool configured for Overlay"""
    acc = ComponentAccumulator()
    kwargs.setdefault("OnlyUseContainerName", False)
    kwargs.setdefault("OutputObjectName", "StoreGateSvc+" + flags.Overlay.SigPrefix + "TGC_DIGITS")
    if not flags.Overlay.DataOverlay:
        kwargs.setdefault("OutputSDOName", "StoreGateSvc+" + flags.Overlay.SigPrefix + "TGC_SDO")
    acc.setPrivateTools(TgcDigitizationTool(name, **kwargs))
    return acc


def TGC_DigitizerBasicCfg(toolCfg, flags, name, **kwargs):
    """Return ComponentAccumulator with toolCfg configured TGCDigitizer algorithm"""
    acc = MuonGeoModelCfg(flags)
    tool = acc.popToolsAndMerge(toolCfg(flags))
    kwargs.setdefault("DigitizationTool", tool)
    acc.addEventAlgo(TGCDigitizer(name,**kwargs))
    return acc


def TGC_DigitizerOutputCfg(toolCfg, flags, name, **kwargs):
    """Return ComponentAccumulator with toolCfg configured TGC Digitizer algorithm and OutputStream"""
    acc = TGC_DigitizerBasicCfg(toolCfg, flags, name, **kwargs)
    acc.merge(OutputStreamCfg(flags, "RDO", ["MuonSimDataCollection#*", "TgcRdoContainer#*"]))
    return acc


def TGC_DigitizerCfg(flags, name="TGC_Digitizer", **kwargs):
    """Return ComponentAccumulator with configured TGC_Digitizer algorithm and Output"""
    return TGC_DigitizerOutputCfg(TGC_DigitizationToolCfg, flags, name, **kwargs)


def TGC_DigitizerOverlayCfg(flags, name="TGC_OverlayDigitizer", **kwargs):
    """Return ComponentAccumulator with Overlay configured TGC_Digitizer algorithm and Output"""
    return TGC_DigitizerOutputCfg(TGC_OverlayDigitizationToolCfg, flags, name, **kwargs)


def TGC_DigitizerDigitToRDOCfg(flags):
    """Return ComponentAccumulator with TGC Digitization and Digit to TGCRDO"""
    acc = TGC_DigitizerCfg(flags)
    acc.merge(TGCCablingConfigCfg(flags))
    acc.merge(TgcDigitToTgcRDOCfg(flags))
    return acc


def TGC_DigitizerOverlayDigitToRDOCfg(flags):
    """Return ComponentAccumulator with TGC Digitization and Digit to TGCRDO for Overlay"""
    acc = TGC_DigitizerOverlayCfg(flags)
    acc.merge(TGCCablingConfigCfg(flags))
    acc.merge(TgcOverlayDigitToTgcRDOCfg(flags))
    return acc

