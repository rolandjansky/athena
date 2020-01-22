"""Define methods to construct configured MDT Digitization tools and algorithms

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
from MDT_Digitization.MDT_DigitizationConf import (
    MdtDigitizationTool, MDT_Response_DigiTool, RT_Relation_DB_DigiTool, MDT_Digitizer
)
PileUpXingFolder=CompFactory.PileUpXingFolder
from MuonConfig.MuonByteStreamCnvTestConfig import MdtDigitToMdtRDOCfg, MdtOverlayDigitToMdtRDOCfg
from MuonConfig.MuonCablingConfig import MDTCablingConfigCfg
from Digitization.TruthDigitizationOutputConfig import TruthDigitizationOutputCfg
from Digitization.PileUpToolsConfig import PileUpToolsCfg


# The earliest and last bunch crossing times for which interactions will be sent
# to the MdtDigitizationTool.
def MDT_FirstXing():
    return -800


def MDT_LastXing():
    # was 800 for large time window
    return 150


def MDT_RangeToolCfg(flags, name="MDT_Range", **kwargs):
    """Return a PileUpXingFolder tool configured for MDT"""
    kwargs.setdefault("FirstXing", MDT_FirstXing())
    kwargs.setdefault("LastXing",  MDT_LastXing())
    kwargs.setdefault("CacheRefreshFrequency", 1.0)
    kwargs.setdefault("ItemList", ["MDTSimHitCollection#MDT_Hits"])
    return PileUpXingFolder(name, **kwargs)


def RT_Relation_DB_DigiToolCfg(flags, name="RT_Relation_DB_DigiTool", **kwargs):
    """Return an RT_Relation_DB_DigiTool"""
    return RT_Relation_DB_DigiTool(name, **kwargs)


def MDT_Response_DigiToolCfg(flags, name="MDT_Response_DigiTool",**kwargs):
    """Return a configured MDT_Response_DigiTool"""
    QballConfig = (flags.Digitization.SpecialConfiguration.get("MDT_QballConfig") == "True")
    kwargs.setdefault("DoQballGamma", QballConfig)
    return MDT_Response_DigiTool(name, **kwargs)


def MDT_DigitizationToolCfg(flags, name="MDT_DigitizationTool", **kwargs):
    """Return ComponentAccumulator with configured MdtDigitizationTool"""
    from MuonConfig.MuonCondAlgConfig import MdtCondDbAlgCfg # MT-safe conditions access
    acc = MdtCondDbAlgCfg(flags)
    kwargs.setdefault("MaskedStations", [])
    kwargs.setdefault("UseDeadChamberSvc", True)
    kwargs.setdefault("DiscardEarlyHits", True)
    kwargs.setdefault("UseTof", flags.Beam.Type != "cosmics")
    # "RT_Relation_DB_DigiTool" in jobproperties.Digitization.experimentalDigi() not migrated
    kwargs.setdefault("DigitizationTool", MDT_Response_DigiToolCfg(flags))
    QballConfig = (flags.Digitization.SpecialConfiguration.get("MDT_QballConfig") == "True")
    kwargs.setdefault("DoQballCharge", QballConfig)
    if flags.Digitization.DoXingByXingPileUp:
        kwargs.setdefault("FirstXing", MDT_FirstXing())
        kwargs.setdefault("LastXing", MDT_LastXing())
    kwargs.setdefault("OutputObjectName", "MDT_DIGITS")
    if flags.Digitization.PileUpPremixing:
        kwargs.setdefault("OutputSDOName", flags.Overlay.BkgPrefix + "MDT_SDO")
    else:
        kwargs.setdefault("OutputSDOName", "MDT_SDO")
    acc.setPrivateTools(MdtDigitizationTool(name, **kwargs))
    return acc


def MDT_OverlayDigitizationToolCfg(flags, name="MDT_OverlayDigitizationTool",**kwargs):
    """Return ComponentAccumulator with MdtDigitizationTool configured for Overlay"""
    acc = ComponentAccumulator()
    kwargs.setdefault("OnlyUseContainerName", False)
    kwargs.setdefault("OutputObjectName", "StoreGateSvc+" + flags.Overlay.SigPrefix + "MDT_DIGITS")
    kwargs.setdefault("GetT0FromBD", flags.Detector.Overlay)
    if not flags.Overlay.DataOverlay:
        kwargs.setdefault("OutputSDOName", "StoreGateSvc+" + flags.Overlay.SigPrefix + "MDT_SDO")
    return acc


def MDT_OutputCfg(flags):
    """Return ComponentAccumulator with Output for MDT. Not standalone."""
    acc = ComponentAccumulator()
    ItemList = ["MdtCsmContainer#*"]
    if flags.Digitization.TruthOutput:
        ItemList += ["MuonSimDataCollection#*"]
        acc.merge(TruthDigitizationOutputCfg(flags))
    acc.merge(OutputStreamCfg(flags, "RDO", ItemList))
    return acc


def MDT_DigitizationBasicCfg(flags, **kwargs):
    """Return ComponentAccumulator for MDT digitization"""
    acc = MuonGeoModelCfg(flags)
    if "PileUpTools" not in kwargs:
        PileUpTools = acc.popToolsAndMerge(MDT_DigitizationToolCfg(flags))
        kwargs["PileUpTools"] = PileUpTools
    acc.merge(PileUpToolsCfg(flags, **kwargs))
    return acc


def MDT_OverlayDigitizationBasicCfg(flags, **kwargs):
    """Return ComponentAccumulator with MDT Overlay digitization"""
    acc = MuonGeoModelCfg(flags)
    if "DigitizationTool" not in kwargs:
        tool = acc.popToolsAndMerge(MDT_OverlayDigitizationToolCfg(flags))
        kwargs["DigitizationTool"] = tool
    acc.addEventAlgo(MDT_Digitizer(**kwargs))
    return acc


# with output defaults
def MDT_DigitizationCfg(flags, **kwargs):
    """Return ComponentAccumulator for MDT digitization and Output"""
    acc = MDT_DigitizationBasicCfg(flags, **kwargs)
    acc.merge(MDT_OutputCfg(flags))
    return acc


def MDT_OverlayDigitizationCfg(flags, **kwargs):
    """Return ComponentAccumulator with MDT Overlay digitization and Output"""
    acc = MDT_OverlayDigitizationBasicCfg(flags, **kwargs)
    acc.merge(MDT_OutputCfg(flags))
    return acc


def MDT_DigitizationDigitToRDOCfg(flags):
    """Return ComponentAccumulator with MDT digitization and Digit to MDTCSM RDO"""
    acc = MDT_DigitizationCfg(flags)
    acc.merge(MDTCablingConfigCfg(flags))
    acc.merge(MdtDigitToMdtRDOCfg(flags))
    return acc


def MDT_OverlayDigitizationDigitToRDOCfg(flags):
    """Return ComponentAccumulator with MDT Overlay digitization and Digit to MDTCSM RDO"""
    acc = MDT_OverlayDigitizationCfg(flags)
    acc.merge(MDTCablingConfigCfg(flags))
    acc.merge(MdtOverlayDigitToMdtRDOCfg(flags))
    return acc
