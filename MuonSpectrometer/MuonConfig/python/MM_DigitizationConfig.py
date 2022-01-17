"""Define methods to construct configured TGC Digitization tools and algorithms

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import ProductionStep
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
from MuonConfig.MuonByteStreamCnvTestConfig import MM_DigitToRDOCfg
from Digitization.TruthDigitizationOutputConfig import TruthDigitizationOutputCfg
from Digitization.PileUpToolsConfig import PileUpToolsCfg
from Digitization.PileUpMergeSvcConfigNew import PileUpMergeSvcCfg, PileUpXingFolderCfg
from MuonConfig.MuonGeometryConfig import MuonDetectorCondAlgCfg

# The earliest and last bunch crossing times for which interactions will be sent
# to the MMDigitizationTool.
def MM_FirstXing():
    return -200


def MM_LastXing():
    return 200


def MM_RangeCfg(flags, name="MMRange", **kwargs):
    """Return a PileUpXingFolder tool configured for MM"""
    kwargs.setdefault("FirstXing", MM_FirstXing())
    kwargs.setdefault("LastXing", MM_LastXing())
    kwargs.setdefault("CacheRefreshFrequency", 1.0)
    kwargs.setdefault("ItemList", ["MMSimHitCollection#MicromegasSensitiveDetector"])
    return PileUpXingFolderCfg(flags, name, **kwargs)


def MM_DigitizationToolCfg(flags, name="MM_DigitizationTool", **kwargs):
    """Return ComponentAccumulator with configured MM_DigitizationTool"""
    acc = ComponentAccumulator()
    acc.merge(MuonDetectorCondAlgCfg(flags))
    rangetool = acc.popToolsAndMerge(MM_RangeCfg(flags))
    acc.merge(PileUpMergeSvcCfg(flags, Intervals=rangetool))
    if flags.Digitization.DoXingByXingPileUp:
        kwargs.setdefault("FirstXing", MM_FirstXing())
        kwargs.setdefault("LastXing", MM_LastXing())
    kwargs.setdefault("CheckSimHits", True)
    kwargs.setdefault("InputObjectName", "MicromegasSensitiveDetector")
    kwargs.setdefault("OutputObjectName", "MM_DIGITS")
    if flags.Common.ProductionStep == ProductionStep.PileUpPresampling:
        kwargs.setdefault("OutputSDOName", flags.Overlay.BkgPrefix + "MM_SDO")
    else:
        kwargs.setdefault("OutputSDOName", "MM_SDO")
    from RngComps.RandomServices import AthRNGSvcCfg
    kwargs.setdefault("RndmSvc", acc.getPrimaryAndMerge(AthRNGSvcCfg(flags)).name)
    MM_DigitizationTool = CompFactory.MM_DigitizationTool
    acc.setPrivateTools(MM_DigitizationTool(name, **kwargs))
    return acc


def MM_OverlayDigitizationToolCfg(flags, name="MM_OverlayDigitizationTool", **kwargs):
    """Return ComponentAccumulator with MM_DigitizationTool configured for Overlay"""
    acc = ComponentAccumulator()
    kwargs.setdefault("CheckSimHits", True)
    kwargs.setdefault("OnlyUseContainerName", False)
    kwargs.setdefault("OutputObjectName", flags.Overlay.SigPrefix + "MM_DIGITS")
    kwargs.setdefault("OutputSDOName", flags.Overlay.SigPrefix + "MM_SDO")
    from RngComps.RandomServices import AthRNGSvcCfg
    kwargs.setdefault("RndmSvc", acc.getPrimaryAndMerge(AthRNGSvcCfg(flags)).name)
    MM_DigitizationTool = CompFactory.MM_DigitizationTool
    acc.setPrivateTools(MM_DigitizationTool(name, **kwargs))
    return acc


def MM_OutputCfg(flags):
    """Return ComponentAccumulator with Output for MM. Not standalone."""
    acc = ComponentAccumulator()
    if flags.Output.doWriteRDO:
        ItemList = ["Muon::MM_RawDataContainer#*"]
        if flags.Digitization.TruthOutput:
            ItemList += ["MuonSimDataCollection#*"]
            acc.merge(TruthDigitizationOutputCfg(flags))
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList))
    return acc


def MM_DigitizationBasicCfg(flags, **kwargs):
    """Return ComponentAccumulator for MM digitization"""
    acc = MuonGeoModelCfg(flags)
    if "PileUpTools" not in kwargs:
        PileUpTools = acc.popToolsAndMerge(MM_DigitizationToolCfg(flags))
        kwargs["PileUpTools"] = PileUpTools
    acc.merge(PileUpToolsCfg(flags, **kwargs))
    return acc


def MM_OverlayDigitizationBasicCfg(flags, **kwargs):
    """Return ComponentAccumulator with MM Overlay digitization"""
    acc = MuonGeoModelCfg(flags, forceDisableAlignment=not flags.Overlay.DataOverlay)
    if "DigitizationTool" not in kwargs:
        tool = acc.popToolsAndMerge(MM_OverlayDigitizationToolCfg(flags))
        kwargs["DigitizationTool"] = tool

    if flags.Concurrency.NumThreads > 0:
        kwargs.setdefault("Cardinality", flags.Concurrency.NumThreads)

    # Set common overlay extra inputs
    kwargs.setdefault("ExtraInputs", flags.Overlay.ExtraInputs)

    TGCDigitizer = CompFactory.TGCDigitizer
    acc.addEventAlgo(TGCDigitizer(name="MM_OverlayDigitizer", **kwargs))
    return acc


# with output defaults
def MM_DigitizationCfg(flags, **kwargs):
    """Return ComponentAccumulator for MM digitization and Output"""
    acc = MM_DigitizationBasicCfg(flags, **kwargs)
    acc.merge(MM_OutputCfg(flags))
    return acc


def MM_DigitizationDigitToRDOCfg(flags):
    """Return ComponentAccumulator with TGC digitization and Digit to TGCRDO"""
    acc = MM_DigitizationCfg(flags)
    acc.merge(MM_DigitToRDOCfg(flags))
    return acc
