"""Define methods to construct configured CSC Digitization tools and algorithms

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
from MuonConfig.MuonCalibConfig import CscCoolStrSvcCfg
from CSC_Digitization.CSC_DigitizationConf import CscDigitizationTool, CscDigitBuilder
from PileUpComps.PileUpCompsConf import PileUpXingFolder
from MuonConfig.MuonByteStreamCnvTestConfig import CscDigitToCscRDOCfg, CscOverlayDigitToCscRDOCfg
from MuonConfig.MuonCablingConfig import CSCCablingConfigCfg
from Digitization.TruthDigitizationOutputConfig import TruthDigitizationOutputCfg
from Digitization.PileUpToolsConfig import PileUpToolsCfg


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


def CSC_OutputCfg(flags):
    """Return ComponentAccumulator with Output for CSC. Not standalone."""
    acc = ComponentAccumulator()
    ItemList = ["CscRawDataContainer#*"]
    if flags.Digitization.TruthOutput:
        ItemList += ["MuonSimDataCollection#*", "CscSimDataCollection#CSC_SDO"]
        acc.merge(TruthDigitizationOutputCfg(flags))
    acc.merge(OutputStreamCfg(flags, "RDO", ItemList))
    return acc


def CSC_DigitizationBasicCfg(flags, **kwargs):
    """Return ComponentAccumulator for CSC digitization"""
    acc = MuonGeoModelCfg(flags)
    acc.merge(CscCoolStrSvcCfg(flags))
    if "PileUpTools" not in kwargs:
        PileUpTools = acc.popToolsAndMerge(CSC_DigitizationToolCfg(flags))
        kwargs["PileUpTools"] = PileUpTools
    acc.merge(PileUpToolsCfg(flags, **kwargs))
    return acc


def CSC_OverlayDigitizationBasicCfg(flags, **kwargs):
    """Return ComponentAccumulator with CSC Overlay digitization"""
    acc = MuonGeoModelCfg(flags)
    acc.merge(CscCoolStrSvcCfg(flags))
    if "DigitizationTool" not in kwargs:
        tool = acc.popToolsAndMerge(CSC_OverlayDigitizationToolCfg(flags))
        kwargs["DigitizationTool"] = tool
    acc.addEventAlgo(CscDigitBuilder(**kwargs))
    return acc


# with output defaults
def CSC_DigitizationCfg(flags, **kwargs):
    """Return ComponentAccumulator for CSC digitization and Output"""
    acc = CSC_DigitizationBasicCfg(flags, **kwargs)
    acc.merge(CSC_OutputCfg(flags))
    return acc


def CSC_OverlayDigitizationCfg(flags, **kwargs):
    """Return ComponentAccumulator with CSC Overlay digitization and Output"""
    acc = CSC_OverlayDigitizationBasicCfg(flags, **kwargs)
    acc.merge(CSC_OutputCfg(flags))
    return acc


def CSC_DigitizationDigitToRDOCfg(flags):
    """Return ComponentAccumulator with CSC Digitization and Digit to CSCRDO"""
    acc = CSC_DigitizationCfg(flags)
    acc.merge(CSCCablingConfigCfg(flags))
    acc.merge(CscDigitToCscRDOCfg(flags))
    return acc


def CSC_OverlayDigitizationDigitToRDOCfg(flags):
    """Return ComponentAccumulator with CSC Overlay Digitization and Digit to CSCRDO"""
    acc = CSC_OverlayDigitizationCfg(flags)
    acc.merge(CSCCablingConfigCfg(flags))
    acc.merge(CscOverlayDigitToCscRDOCfg(flags))
    return acc
