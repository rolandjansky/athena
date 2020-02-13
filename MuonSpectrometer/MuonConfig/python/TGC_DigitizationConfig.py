"""Define methods to construct configured TGC Digitization tools and algorithms

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
TgcDigitizationTool, TGCDigitizer=CompFactory.getComps("TgcDigitizationTool","TGCDigitizer",)
PileUpXingFolder=CompFactory.PileUpXingFolder
from MuonConfig.MuonByteStreamCnvTestConfig import TgcDigitToTgcRDOCfg, TgcOverlayDigitToTgcRDOCfg
from MuonConfig.MuonCablingConfig import TGCCablingConfigCfg
from Digitization.TruthDigitizationOutputConfig import TruthDigitizationOutputCfg
from Digitization.PileUpToolsConfig import PileUpToolsCfg


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


def TGC_OutputCfg(flags):
    """Return ComponentAccumulator with Output for TGC. Not standalone."""
    acc = ComponentAccumulator()
    ItemList = ["TgcRdoContainer#*"]
    if flags.Digitization.TruthOutput:
        ItemList += ["MuonSimDataCollection#*"]
        acc.merge(TruthDigitizationOutputCfg(flags))
    acc.merge(OutputStreamCfg(flags, "RDO", ItemList))
    return acc


def TGC_DigitizationBasicCfg(flags, **kwargs):
    """Return ComponentAccumulator for TGC digitization"""
    acc = MuonGeoModelCfg(flags)
    if "PileUpTools" not in kwargs:
        PileUpTools = acc.popToolsAndMerge(TGC_DigitizationToolCfg(flags))
        kwargs["PileUpTools"] = PileUpTools
    acc.merge(PileUpToolsCfg(flags, **kwargs))
    return acc


def TGC_OverlayDigitizationBasicCfg(flags, **kwargs):
    """Return ComponentAccumulator with TGC Overlay digitization"""
    acc = MuonGeoModelCfg(flags)
    if "DigitizationTool" not in kwargs:
        tool = acc.popToolsAndMerge(TGC_OverlayDigitizationToolCfg(flags))
        kwargs["DigitizationTool"] = tool
    acc.addEventAlgo(TGCDigitizer(**kwargs))
    return acc


# with output defaults
def TGC_DigitizationCfg(flags, **kwargs):
    """Return ComponentAccumulator for TGC digitization and Output"""
    acc = TGC_DigitizationBasicCfg(flags, **kwargs)
    acc.merge(TGC_OutputCfg(flags))
    return acc


def TGC_OverlayDigitizationCfg(flags, **kwargs):
    """Return ComponentAccumulator with TGC Overlay digitization and Output"""
    acc = TGC_OverlayDigitizationBasicCfg(flags, **kwargs)
    acc.merge(TGC_OutputCfg(flags))
    return acc


def TGC_DigitizationDigitToRDOCfg(flags):
    """Return ComponentAccumulator with TGC digitization and Digit to TGCRDO"""
    acc = TGC_DigitizationCfg(flags)
    acc.merge(TGCCablingConfigCfg(flags))
    acc.merge(TgcDigitToTgcRDOCfg(flags))
    return acc


def TGC_OverlayDigitizationDigitToRDOCfg(flags):
    """Return ComponentAccumulator with TGC Overlay digitization and Digit to TGCRDO"""
    acc = TGC_OverlayDigitizationCfg(flags)
    acc.merge(TGCCablingConfigCfg(flags))
    acc.merge(TgcOverlayDigitToTgcRDOCfg(flags))
    return acc

