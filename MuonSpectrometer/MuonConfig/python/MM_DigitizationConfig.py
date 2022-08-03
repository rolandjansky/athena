"""Define methods to construct configured MM Digitization tools and algorithms

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import ProductionStep
from Digitization.PileUpMergeSvcConfigNew import PileUpMergeSvcCfg, PileUpXingFolderCfg
from Digitization.PileUpToolsConfig import PileUpToolsCfg
from Digitization.TruthDigitizationOutputConfig import TruthDigitizationOutputCfg
from MagFieldServices.MagFieldServicesConfig import AtlasFieldCacheCondAlgCfg
from MuonConfig.MuonByteStreamCnvTestConfig import MM_DigitToRDOCfg
from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg

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
    if 'MMSimHitCollection#MicromegasSensitiveDetector' in flags.Input.TypedCollections:
        kwargs.setdefault("ItemList", ["MMSimHitCollection#MicromegasSensitiveDetector"])
    else:
        kwargs.setdefault("ItemList", ["MMSimHitCollection#MM_Hits"])
    return PileUpXingFolderCfg(flags, name, **kwargs)


def MM_DigitizationToolCfg(flags, name="MM_DigitizationTool", **kwargs):
    """Return ComponentAccumulator with configured MM_DigitizationTool"""
    from MuonConfig.MuonCalibrationConfig import NSWCalibToolCfg, MMCalibSmearingToolCfg
    result = ComponentAccumulator()
    kwargs.setdefault("CalibrationTool", result.popToolsAndMerge(NSWCalibToolCfg(flags)))
    kwargs.setdefault("SmearingTool", result.popToolsAndMerge(MMCalibSmearingToolCfg(flags)))
    result.merge(AtlasFieldCacheCondAlgCfg(flags))
    if flags.Digitization.PileUp:
        intervals = []
        if flags.Digitization.DoXingByXingPileUp:
            kwargs.setdefault("FirstXing", MM_FirstXing())
            kwargs.setdefault("LastXing", MM_LastXing())
        else:
            intervals += [result.popToolsAndMerge(MM_RangeCfg(flags))]
        kwargs.setdefault("MergeSvc", result.getPrimaryAndMerge(PileUpMergeSvcCfg(flags, Intervals=intervals)).name)
    else:
        kwargs.setdefault("MergeSvc", '')
    kwargs.setdefault("OnlyUseContainerName", flags.Digitization.PileUp)
    kwargs.setdefault("CheckSimHits", True)
    if 'MMSimHitCollection#MicromegasSensitiveDetector' in flags.Input.TypedCollections:
        kwargs.setdefault("InputObjectName", "MicromegasSensitiveDetector")
    else:
        kwargs.setdefault("InputObjectName", "MM_Hits")
    kwargs.setdefault("OutputObjectName", "MM_DIGITS")
    if flags.Common.ProductionStep == ProductionStep.PileUpPresampling:
        kwargs.setdefault("OutputSDOName", flags.Overlay.BkgPrefix + "MM_SDO")
    else:
        kwargs.setdefault("OutputSDOName", "MM_SDO")
    from RngComps.RandomServices import AthRNGSvcCfg
    kwargs.setdefault("RndmSvc", result.getPrimaryAndMerge(AthRNGSvcCfg(flags)).name)
    result.setPrivateTools(CompFactory.MM_DigitizationTool(name, **kwargs))
    return result


def MM_OverlayDigitizationToolCfg(flags, name="MM_OverlayDigitizationTool", **kwargs):
    """Return ComponentAccumulator with MM_DigitizationTool configured for Overlay"""
    acc = AtlasFieldCacheCondAlgCfg(flags)
    kwargs.setdefault("CheckSimHits", True)
    kwargs.setdefault("OnlyUseContainerName", False)
    if 'MMSimHitCollection#MicromegasSensitiveDetector' in flags.Input.SecondaryTypedCollections:
        kwargs.setdefault("InputObjectName", "MicromegasSensitiveDetector")
    else:
        kwargs.setdefault("InputObjectName", "MM_Hits")
    kwargs.setdefault("OutputObjectName", flags.Overlay.SigPrefix + "MM_DIGITS")
    kwargs.setdefault("OutputSDOName", flags.Overlay.SigPrefix + "MM_SDO")
    from RngComps.RandomServices import AthRNGSvcCfg
    kwargs.setdefault("RndmSvc", acc.getPrimaryAndMerge(AthRNGSvcCfg(flags)).name)
    acc.setPrivateTools(CompFactory.MM_DigitizationTool(name, **kwargs))
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
    if flags.Common.ProductionStep != ProductionStep.FastChain:
        from SGComps.SGInputLoaderConfig import SGInputLoaderCfg
        if 'MMSimHitCollection#MicromegasSensitiveDetector' in flags.Input.SecondaryTypedCollections:
            acc.merge(SGInputLoaderCfg(flags, ["MMSimHitCollection#MicromegasSensitiveDetector"]))
        else:
            acc.merge(SGInputLoaderCfg(flags, ["MMSimHitCollection#MM_Hits"]))

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
