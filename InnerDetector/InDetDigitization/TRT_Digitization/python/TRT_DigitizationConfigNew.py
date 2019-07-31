"""Define methods to construct configured TRT Digitization tools and algorithms

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from TRT_Digitization.TRT_DigitizationConf import TRTDigitizationTool, TRTDigitization
from TRT_GeoModel.TRT_GeoModelConfig import TRT_GeometryCfg
from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
from TRT_PAI_Process.TRT_PAI_ProcessConfigNew import TRT_PAI_Process_XeToolCfg
from TRT_PAI_Process.TRT_PAI_ProcessConfigNew import TRT_PAI_Process_ArToolCfg
from TRT_PAI_Process.TRT_PAI_ProcessConfigNew import TRT_PAI_Process_KrToolCfg
from PileUpComps.PileUpCompsConf import PileUpXingFolder
from PartPropSvc.PartPropSvcConf import PartPropSvc
from IOVDbSvc.IOVDbSvcConfig import addFolders
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg

# The earliest and last bunch crossing times for which interactions will be sent
# to the TRT Digitization code
def TRT_FirstXing():
    return -50

def TRT_LastXing():
    return 50

def TRT_ItemList():
    """Return list of item names needed for TRT output"""
    return ["InDetSimDataCollection#*", "TRT_RDO_Container#*"]

def TRT_RangeCfg(flags, name="TRTRange", **kwargs):
    """Return an TRT configured PileUpXingFolder tool"""
    kwargs.setdefault("FirstXing", TRT_FirstXing())
    kwargs.setdefault("LastXing", TRT_LastXing())
    kwargs.setdefault("CacheRefreshFrequency", 1.0) #default 0 no dataproxy reset
    kwargs.setdefault("ItemList", ["TRTUncompressedHitCollection#TRTUncompressedHits"])
    return PileUpXingFolder(name, **kwargs)

def TRT_DigitizationBasicToolCfg(flags, name="TRT_DigitizationBasicTool", **kwargs):
    """Return a ComponentAccumulator with common TRT digitization tool config"""
    acc = TRT_GeometryCfg(flags)
    acc.merge(MagneticFieldSvcCfg(flags))
    # included options
    acc.addService(PartPropSvc(InputFile="PDGTABLE.MeV=PDG"))
    if flags.Detector.Overlay and flags.Input.isMC:
        acc.merge(addFolders("/TRT/Cond/DigVers", "TRT_OFL"))
    # default arguments
    kwargs.setdefault("PAI_Tool_Xe", TRT_PAI_Process_XeToolCfg(flags))
    kwargs.setdefault("PAI_Tool_Ar", TRT_PAI_Process_ArToolCfg(flags))
    kwargs.setdefault("PAI_Tool_Kr", TRT_PAI_Process_KrToolCfg(flags))
    if not flags.Digitization.DoInnerDetectorNoise:
        kwargs.setdefault("Override_noiseInSimhits", 0)
        kwargs.setdefault("Override_noiseInUnhitStraws", 0)
    kwargs.setdefault("Override_TrtRangeCutProperty", 0.05) # todo flags.InnerDetector.TRTRangeCut)
    if flags.Beam.Type == "cosmics":
        kwargs.setdefault("PrintDigSettings", True)
        kwargs.setdefault("Override_cosmicFlag", 0)
        kwargs.setdefault("Override_doCosmicTimingPit", 1)
        kwargs.setdefault("Override_jitterTimeOffset", 0.)
        kwargs.setdefault("Override_timeCorrection", 0)
    if flags.Digitization.DoXingByXingPileUp:
        kwargs.setdefault("FirstXing", TRT_FirstXing())
        kwargs.setdefault("LastXing", TRT_LastXing())
    tool = TRTDigitizationTool(name, **kwargs)
    acc.setPrivateTools(tool)
    return acc

def TRT_DigitizationToolCfg(flags, name="TRT_DigitizationTool", **kwargs):
    """Return a ComponentAccumulator with configured TRT digitization tool"""
    if flags.Digitization.PileUpPremixing:
        kwargs.setdefault("OutputObjectName", flags.Overlay.BkgPrefix + "TRT_RDOs")
        kwargs.setdefault("OutputSDOName", flags.Overlay.BkgPrefix + "TRT_SDO_Map")
    else:
        kwargs.setdefault("OutputObjectName", "TRT_RDOs")
        kwargs.setdefault("OutputSDOName", "TRT_SDO_Map")
    kwargs.setdefault("HardScatterSplittingMode", 0)
    return TRT_DigitizationBasicToolCfg(flags, name, **kwargs)

def TRT_DigitizationGeantinoTruthToolCfg(flags, name="TRT_GeantinoTruthDigitizationTool", **kwargs):
    """Return a ComponentAccumulator with Geantino configured TRT digitization tool"""
    kwargs.setdefault("ParticleBarcodeVeto", 0)
    return TRT_DigitizationToolCfg(flags, name, **kwargs)

def TRT_DigitizationHSToolCfg(flags, name="TRT_DigitizationToolHS", **kwargs):
    """Return a ComponentAccumulator with Hard Scatter configured TRT digitization tool"""
    kwargs.setdefault("OutputObjectName", "TRT_RDOs")
    kwargs.setdefault("OutputSDOName", "TRT_SDO_Map")
    kwargs.setdefault("HardScatterSplittingMode", 1)
    return TRT_DigitizationBasicToolCfg(flags, name, **kwargs)

def TRT_DigitizationPUToolCfg(flags, name="TRT_DigitizationToolPU", **kwargs):
    """Return a ComponentAccumulator with Pile Up configured TRT digitization tool"""
    kwargs.setdefault("OutputObjectName", "TRT_PU_RDOs")
    kwargs.setdefault("OutputSDOName", "TRT_PU_SDO_Map")
    kwargs.setdefault("HardScatterSplittingMode", 2)
    return TRT_DigitizationBasicToolCfg(flags, name, **kwargs)

def TRT_DigitizationSplitNoMergePUToolCfg(flags, name="TRT_DigitizationToolSplitNoMergePU", **kwargs):
    """Return a ComponentAccumulator with PileUpPixelHits configured TRT digitization tool"""
    kwargs.setdefault("HardScatterSplittingMode", 0)
    kwargs.setdefault("DataObjectName", "PileupTRTUncompressedHits")
    kwargs.setdefault("OutputObjectName", "TRT_PU_RDOs")
    kwargs.setdefault("OutputSDOName", "TRT_PU_SDO_Map")
    kwargs.setdefault("Override_noiseInSimhits", 0)
    kwargs.setdefault("Override_noiseInUnhitStraws", 0)
    return TRT_DigitizationBasicToolCfg(flags, name, **kwargs)

def TRT_DigitizationOverlayToolCfg(flags, name="TRT_OverlayDigitizationTool", **kwargs):
    """Return a ComponentAccumulator with configured Overlay TRT digitization tool"""
    acc = ComponentAccumulator()
    kwargs.setdefault("OnlyUseContainerName", False)
    kwargs.setdefault("OutputObjectName", "StoreGateSvc+" + flags.Overlay.SigPrefix + "TRT_RDOs")
    kwargs.setdefault("OutputSDOName", "StoreGateSvc+" + flags.Overlay.SigPrefix + "TRT_SDO_Map")
    kwargs.setdefault("HardScatterSplittingMode", 0)
    kwargs.setdefault("Override_getT0FromData", 0)
    kwargs.setdefault("Override_noiseInSimhits", 0)
    kwargs.setdefault("Override_noiseInUnhitStraws", 0)
    kwargs.setdefault("Override_isOverlay", 1)
    tool = acc.popToolsAndMerge(TRT_DigitizationBasicToolCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc

def TRT_DigitizationBasicCfg(toolCfg, flags, name="TRT_DigitizationBasic", **kwargs):
    """Return a ComponentAccumulator with basic toolCfg type TRT digitization"""
    acc = ComponentAccumulator()
    if "DigitizationTool" not in kwargs:
        tool = acc.popToolsAndMerge(toolCfg(flags))
        kwargs["DigitizationTool"] = tool
    acc.addEventAlgo(TRTDigitization(name, **kwargs))
    return acc

def TRT_DigitizationOutputCfg(toolCfg, flags, name="TRT_Digitization", **kwargs):
    """Return a ComponentAccumulator with toolCfg type TRT digitization and Output"""
    acc = TRT_DigitizationBasicCfg(toolCfg, flags, name, **kwargs)
    acc.merge(OutputStreamCfg(flags, "RDO", TRT_ItemList()))
    return acc

def TRT_DigitizationCfg(flags, name="TRT_Digitization", **kwargs):
    """Return a ComponentAccumulator configured for standard TRT digitization"""
    return TRT_DigitizationOutputCfg(TRT_DigitizationToolCfg, flags, name, **kwargs)

def TRT_DigitizationHSCfg(flags, name="TRT_DigitizationHS", **kwargs):
    """Return a ComponentAccumulator configured for Hard Scatter-only TRT digitization"""
    return TRT_DigitizationOutputCfg(TRT_DigitizationHSToolCfg, flags, name, **kwargs)

def TRT_DigitizationPUCfg(flags, name="TRT_DigitizationPU", **kwargs):
    """Return a ComponentAccumulator configured for Pile-up-only TRT digitization"""
    return TRT_DigitizationOutputCfg(TRT_DigitizationPUToolCfg, flags, name, **kwargs)

def TRT_DigitizationOverlayCfg(flags, name="TRT_OverlayDigitization", **kwargs):
    """Return a ComponentAccumulator configured for Overlay TRT digitization"""
    return TRT_DigitizationBasicCfg(TRT_DigitizationOverlayToolCfg, flags, name, **kwargs)

