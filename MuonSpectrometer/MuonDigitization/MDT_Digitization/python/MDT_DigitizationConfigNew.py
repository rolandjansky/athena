"""Define methods to construct configured MDT Digitization tools and algorithms

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from StoreGate.StoreGateConf import StoreGateSvc
from MDT_Digitization.MDT_DigitizationConf import (
    MdtDigitizationTool, MDT_Response_DigiTool, RT_Relation_DB_DigiTool, MDT_Digitizer
)
from PileUpComps.PileUpCompsConf import PileUpXingFolder

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
    """Return a ComponentAccumulator with configured MdtDigitizationTool"""
    acc = ComponentAccumulator()
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
    acc.setPrivateTools(MdtDigitizationTool(name,**kwargs))
    return acc

def MDT_DigitizerCfg(flags, name="MDT_Digitizer", **kwargs):
    """Return a ComponentAccumulator with configured MDT_Digitizer algorithm"""
    acc = MDT_DigitizationToolCfg(flags)
    kwargs.setdefault("DigitizationTool", acc.popPrivateTools())
    acc.addEventAlgo(MDT_Digitizer(name,**kwargs))
    return acc

def MDT_OverlayDigitizationToolCfg(flags, name="MDT_OverlayDigitizationTool",**kwargs):
    """Return a ComponentAccumulator with MdtDigitizationTool configured for Overlay"""
    acc = ComponentAccumulator()
    acc.addService(StoreGateSvc(flags.Overlay.Legacy.EventStore))
    kwargs.setdefault("OutputObjectName", flags.Overlay.Legacy.EventStore + "+MDT_DIGITS")
    kwargs.setdefault("GetT0FromBD", flags.Detector.Overlay)
    if not flags.Detector.Overlay:
        kwargs.setdefault("OutputSDOName", flags.Overlay.Legacy.EventStore + "+MDT_SDO")
    acc.setPrivateTools(MdtDigitizationTool(name,**kwargs))
    return acc

def MDT_OverlayDigitizerCfg(flags, name="MDT_OverlayDigitizer", **kwargs):
    """Return a ComponentAccumulator with MDT_Digitizer algorithm configured for Overlay"""
    acc = MDT_OverlayDigitizationToolCfg(flags)
    kwargs.setdefault("DigitizationTool", acc.popPrivateTools())
    acc.addEventAlgo(MDT_Digitizer(name,**kwargs))
    return acc

