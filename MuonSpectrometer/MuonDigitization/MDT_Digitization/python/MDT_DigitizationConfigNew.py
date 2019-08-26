"""Define methods to construct configured MDT Digitization tools and algorithms

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
from MDT_Digitization.MDT_DigitizationConf import (
    MdtDigitizationTool, MDT_Response_DigiTool, RT_Relation_DB_DigiTool, MDT_Digitizer
)
from PileUpComps.PileUpCompsConf import PileUpXingFolder
from MuonByteStreamCnvTest.MuonByteStreamCnvTestConfigNew import MdtDigitToMdtRDOCfg, MdtOverlayDigitToMdtRDOCfg
from MuonConfig.MuonCablingConfig import MDTCablingConfigCfg

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


def MDT_DigitizerBasicCfg(toolCfg, flags, name, **kwargs):
    """Return ComponentAccumulator with toolCfg configured MDT_Digitizer algorithm"""
    acc = MuonGeoModelCfg(flags)
    tool = acc.popToolsAndMerge(toolCfg(flags))
    kwargs.setdefault("DigitizationTool", tool)
    acc.addEventAlgo(MDT_Digitizer(name, **kwargs))
    return acc


def MDT_DigitizerOutputCfg(toolCfg, flags, name, **kwargs):
    """Return ComponentAccumulator with toolCfg configured MDT Digitizer algorithm and OutputStream"""
    acc = MDT_DigitizerBasicCfg(toolCfg, flags, name, **kwargs)
    acc.merge(OutputStreamCfg(flags, "RDO", ["MuonSimDataCollection#*", "MdtCsmContainer#*"]))
    return acc


def MDT_DigitizerCfg(flags, name="MDT_Digitizer", **kwargs):
    """Return ComponentAccumulator with configured MDT_Digitizer algorithm and Output"""
    return MDT_DigitizerOutputCfg(MDT_DigitizationToolCfg, flags, name, **kwargs)


def MDT_DigitizerOverlayCfg(flags, name="MDT_OverlayDigitizer", **kwargs):
    """Return ComponentAccumulator with Overlay configured MDT_Digitizer algorithm and Output"""
    return MDT_DigitizerOutputCfg(MDT_OverlayDigitizationToolCfg, flags, name, **kwargs)


def MDT_DigitizerDigitToRDOCfg(flags):
    """Return ComponentAccumulator with MDT Digitization and Digit to MDTCSM RDO"""
    acc = MDT_DigitizerCfg(flags)
    acc.merge(MDTCablingConfigCfg(flags))
    acc.merge(MdtDigitToMdtRDOCfg(flags))
    return acc


def MDT_DigitizerOverlayDigitToRDOCfg(flags):
    """Return ComponentAccumulator with MDT Digitization and Digit to MDTCSM RDO for Overlay"""
    acc = MDT_DigitizerOverlayCfg(flags)
    acc.merge(MDTCablingConfigCfg(flags))
    acc.merge(MdtOverlayDigitToMdtRDOCfg(flags))
    return acc
