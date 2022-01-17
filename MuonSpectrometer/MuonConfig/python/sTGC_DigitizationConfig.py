"""Define methods to construct configured TGC Digitization tools and algorithms

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import ProductionStep
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
from MuonConfig.MuonByteStreamCnvTestConfig import STGC_DigitToRDOCfg
from Digitization.TruthDigitizationOutputConfig import TruthDigitizationOutputCfg
from Digitization.PileUpToolsConfig import PileUpToolsCfg
from Digitization.PileUpMergeSvcConfigNew import PileUpMergeSvcCfg, PileUpXingFolderCfg


# The earliest and last bunch crossing times for which interactions will be sent
# to the sTgcDigitizationTool.
def sTGC_FirstXing():
    return -100


def sTGC_LastXing():
    return 100


def sTGC_RangeCfg(flags, name="sTgcRange", **kwargs):
    """Return a PileUpXingFolder tool configured for sTGC"""
    kwargs.setdefault("FirstXing", sTGC_FirstXing())
    kwargs.setdefault("LastXing", sTGC_LastXing())
    kwargs.setdefault("CacheRefreshFrequency", 1.0)
    kwargs.setdefault("ItemList", ["sTGCSimHitCollection#sTGCSensitiveDetector"])
    return PileUpXingFolderCfg(flags, name, **kwargs)


def sTGC_DigitizationToolCfg(flags, name="sTgcDigitizationTool", **kwargs):
    """Return ComponentAccumulator with configured sTgcDigitizationTool"""
    acc = ComponentAccumulator()
    rangetool = acc.popToolsAndMerge(sTGC_RangeCfg(flags))
    acc.merge(PileUpMergeSvcCfg(flags, Intervals=rangetool))
    if flags.Digitization.DoXingByXingPileUp:
        kwargs.setdefault("FirstXing", sTGC_FirstXing())
        kwargs.setdefault("LastXing", sTGC_LastXing())
    kwargs.setdefault("doToFCorrection", True)
    kwargs.setdefault("doEfficiencyCorrection", False)
    kwargs.setdefault("InputObjectName", "sTGCSensitiveDetector")
    kwargs.setdefault("OutputObjectName", "sTGC_DIGITS")
    from RngComps.RandomServices import AthRNGSvcCfg
    kwargs.setdefault("RndmSvc", acc.getPrimaryAndMerge(AthRNGSvcCfg(flags)).name)
    if flags.Common.ProductionStep == ProductionStep.PileUpPresampling:
        kwargs.setdefault("OutputSDOName", flags.Overlay.BkgPrefix + "sTGC_SDO")
    else:
        kwargs.setdefault("OutputSDOName", "sTGC_SDO")
    sTgcDigitizationTool = CompFactory.sTgcDigitizationTool
    acc.setPrivateTools(sTgcDigitizationTool(name, **kwargs))
    return acc


def sTGC_OverlayDigitizationToolCfg(flags, name="STGC_OverlayDigitizationTool", **kwargs):
    """Return ComponentAccumulator with TgcDigitizationTool configured for Overlay"""
    acc = ComponentAccumulator()
    kwargs.setdefault("doToFCorrection", True)
    kwargs.setdefault("doEfficiencyCorrection", False)
    kwargs.setdefault("OnlyUseContainerName", False)
    kwargs.setdefault("OutputObjectName", flags.Overlay.SigPrefix + "sTGC_DIGITS")
    kwargs.setdefault("OutputSDOName", flags.Overlay.SigPrefix + "sTGC_SDO")
    from RngComps.RandomServices import AthRNGSvcCfg
    kwargs.setdefault("RndmSvc", acc.getPrimaryAndMerge(AthRNGSvcCfg(flags)).name)
    sTgcDigitizationTool = CompFactory.sTgcDigitizationTool
    acc.setPrivateTools(sTgcDigitizationTool(name, **kwargs))
    return acc


def sTGC_OutputCfg(flags):
    """Return ComponentAccumulator with Output for sTGC. Not standalone."""
    acc = ComponentAccumulator()
    if flags.Output.doWriteRDO:
        ItemList = ["Muon::STGC_RawDataContainer#*"]
        if flags.Digitization.TruthOutput:
            ItemList += ["MuonSimDataCollection#*"]
            acc.merge(TruthDigitizationOutputCfg(flags))
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList))
    return acc


def sTGC_DigitizationBasicCfg(flags, **kwargs):
    """Return ComponentAccumulator for sTGC digitization"""
    acc = MuonGeoModelCfg(flags)
    if "PileUpTools" not in kwargs:
        PileUpTools = acc.popToolsAndMerge(sTGC_DigitizationToolCfg(flags))
        kwargs["PileUpTools"] = PileUpTools
    acc.merge(PileUpToolsCfg(flags, **kwargs))
    return acc


def sTGC_OverlayDigitizationBasicCfg(flags, **kwargs):
    """Return ComponentAccumulator with sTGC Overlay digitization"""
    acc = MuonGeoModelCfg(flags, forceDisableAlignment=not flags.Overlay.DataOverlay)
    if "DigitizationTool" not in kwargs:
        tool = acc.popToolsAndMerge(sTGC_OverlayDigitizationToolCfg(flags))
        kwargs["DigitizationTool"] = tool

    if flags.Concurrency.NumThreads > 0:
        kwargs.setdefault("Cardinality", flags.Concurrency.NumThreads)

    # Set common overlay extra inputs
    kwargs.setdefault("ExtraInputs", flags.Overlay.ExtraInputs)

    TGCDigitizer = CompFactory.TGCDigitizer
    acc.addEventAlgo(TGCDigitizer(name="STGC_OverlayDigitizer", **kwargs))
    return acc


# with output defaults
def sTGC_DigitizationCfg(flags, **kwargs):
    """Return ComponentAccumulator for sTGC digitization and Output"""
    acc = sTGC_DigitizationBasicCfg(flags, **kwargs)
    acc.merge(sTGC_OutputCfg(flags))
    return acc


def sTGC_DigitizationDigitToRDOCfg(flags):
    """Return ComponentAccumulator with sTGC digitization and Digit to TGCRDO"""
    acc = sTGC_DigitizationCfg(flags)
    acc.merge(STGC_DigitToRDOCfg(flags))
    return acc
