"""Define methods to construct configured CSC Digitization tools and algorithms

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
from MuonConfig.MuonCalibConfig import CscCoolStrSvcCfg
from CSC_Digitization.CSC_DigitizationConf import (
    CscDigitizationTool, CscDigitBuilder,
)
from PileUpComps.PileUpCompsConf import PileUpXingFolder

# The earliest and last bunch crossing times for which interactions will be sent
# to the CscDigitizationTool.
def CSC_FirstXing():
    return -375

def CSC_LastXing():
    return 175

def CSC_RangeToolCfg(flags, name="CSC_Range", **kwargs):
    """Return a PileUpXingFolder tool configured for CSC"""
    kwargs.setdefault("FirstXing", CSC_FirstXing())
    kwargs.setdefault("LastXing",  CSC_LastXing())
    kwargs.setdefault("CacheRefreshFrequency", 1.0)
    kwargs.setdefault("ItemList", ["CSCSimHitCollection#CSC_Hits"])
    return PileUpXingFolder(name, **kwargs)

def CSC_DigitizationToolCfg(flags, name="CSC_DigitizationTool", **kwargs):
    """Return a ComponentAccumulator with configured CscDigitizationTool"""
    acc = ComponentAccumulator()
    if flags.Digitization.DoXingByXingPileUp:
        kwargs.setdefault("FirstXing", CSC_FirstXing())
        kwargs.setdefault("LastXing",  CSC_LastXing())
    kwargs.setdefault("InputObjectName", "CSC_Hits")
    kwargs.setdefault("OutputObjectName", "CSC_DIGITS")
    if flags.Digitization.PileUpPremixing:
        kwargs.setdefault("CSCSimDataCollectionOutputName", flags.Overlay.BkgPrefix + "CSC_SDO")
    else:
        kwargs.setdefault("CSCSimDataCollectionOutputName", "CSC_SDO")
    kwargs.setdefault("pedestal", 0.0)
    kwargs.setdefault("WindowLowerOffset", -25.0)
    kwargs.setdefault("WindowUpperOffset",  25.0)
    kwargs.setdefault("isPileUp", False)
    kwargs.setdefault("amplification", 0.43e5)
    kwargs.setdefault("NewDigitEDM", True)
    kwargs.setdefault("DriftVelocity", 39)
    kwargs.setdefault("ElectronEnergy", 66) # eV
    acc.setPrivateTools(CscDigitizationTool(name, **kwargs))
    return acc

def CSC_OverlayDigitizationToolCfg(flags, name="CSC_OverlayDigitizationTool",**kwargs):
    """Return a ComponentAccumulator with CscDigitizationTool configured for Overlay"""
    acc = ComponentAccumulator()
    kwargs.setdefault("OnlyUseContainerName", False)
    kwargs.setdefault("OutputObjectName", "StoreGateSvc+" + flags.Overlay.SigPrefix + "CSC_DIGITS")
    if not flags.Overlay.DataOverlay:
        kwargs.setdefault("CSCSimDataCollectionOutputName", "StoreGateSvc+" + flags.Overlay.SigPrefix + "CSC_SDO")
    acc.setPrivateTools(CscDigitizationTool(name, **kwargs))
    return acc


def CSC_DigitBuilderBasicCfg(toolCfg, flags, name, **kwargs):
    """Return a ComponentAccumulator with toolCfg configured CscDigitBuilder algorithm"""
    acc = MuonGeoModelCfg(flags)
    acc.merge(CscCoolStrSvcCfg(flags))
    tool = acc.popToolsAndMerge(toolCfg(flags))
    kwargs.setdefault("DigitizationTool", tool)
    acc.addEventAlgo(CscDigitBuilder(name, **kwargs))
    return acc

def CSC_DigitBuilderOutputCfg(toolCfg, flags, name, **kwargs):
    """Return ComponentAccumulator with toolCfg configured CscDigitBuilder algorithm and OutputStream"""
    acc = CSC_DigitBuilderBasicCfg(toolCfg, flags, name, **kwargs)
    acc.merge(OutputStreamCfg(flags, "RDO", ["MuonSimDataCollection#*", "CscSimDataCollection#CSC_SDO", "CscRawDataContainer#*"]))
    return acc


def CSC_DigitBuilderCfg(flags, name="CSC_DigitBuilder", **kwargs):
    """Return ComponentAccumulator with configured CSC_Digitizer algorithm and Output"""
    return CSC_DigitBuilderOutputCfg(CSC_DigitizationToolCfg, flags, name, **kwargs)

def CSC_DigitBuilderOverlayCfg(flags, name="CSC_OverlayDigitBuilder", **kwargs):
    """Return DigitBuilderOutputCfg with Overlay configured CSC_Digitizer algorithm and Output"""
    return CSC_DigitBuilderBasicCfg(CSC_OverlayDigitizationToolCfg, flags, name, **kwargs)

