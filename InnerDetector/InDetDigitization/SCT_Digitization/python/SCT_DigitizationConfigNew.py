"""Define methods to construct configured SCT Digitization tools and algorithms

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaCommon import Logging
from SCT_Digitization.SCT_DigitizationConf import (
    SCT_RandomDisabledCellGenerator,
    SCT_Amp,
    SCT_DetailedSurfaceChargesGenerator,
    SCT_SurfaceChargesGenerator,
    SCT_FrontEnd,
    SCT_DigitizationTool,
    SCT_Digitization,
)
from PileUpComps.PileUpCompsConf import PileUpXingFolder
from SCT_ConditionsTools.SCT_ConditionsToolsConf import SCT_RadDamageSummaryTool
from SCT_GeoModel.SCT_GeoModelConfig import SCT_GeometryCfg
from SCT_ConditionsTools.SCT_DCSConditionsConfig import SCT_DCSConditionsCfg
from SCT_ConditionsTools.SCT_SiliconConditionsConfig import SCT_SiliconConditionsCfg
from SCT_ConditionsTools.SCT_ReadCalibChipDataConfig import SCT_ReadCalibChipDataCfg
from SiPropertiesTool.SCT_SiPropertiesConfig import SCT_SiPropertiesCfg
from SiLorentzAngleTool.SCT_LorentzAngleConfig import SCT_LorentzAngleCfg
from StoreGate.StoreGateConf import StoreGateSvc

# The earliest and last bunch crossing times for which interactions will be sent
# to the SCT Digitization code
def SCT_FirstXing():
    return -50

def SCT_LastXing():
    return 25

def SCT_DigitizationCommonCfg(flags, name="SCT_DigitizationToolCommon", **kwargs):
    """Return ComponentAccumulator with common SCT digitization tool config"""
    acc = SCT_GeometryCfg(flags)
    if not flags.Digitization.DoInnerDetectorNoise:
        kwargs.setdefault("OnlyHitElements", True)
    kwargs.setdefault("InputObjectName", "SCT_Hits")
    kwargs.setdefault("EnableHits", True)
    kwargs.setdefault("BarrelOnly", False)
    # Set FixedTime for cosmics for use in SurfaceChargesGenerator
    if flags.Beam.Type == "cosmics":
        kwargs.setdefault("CosmicsRun", True)
        kwargs.setdefault("FixedTime", 10)
    if flags.Digitization.DoXingByXingPileUp:
        kwargs.setdefault("FirstXing", SCT_FirstXing())
        kwargs.setdefault("LastXing", SCT_LastXing() )
    tool = SCT_DigitizationTool(name, **kwargs)
    # attach ToolHandles
    frontAcc = SCT_FrontEndCfg(flags)
    tool.FrontEnd = frontAcc.popPrivateTools()
    surfAcc = SCT_SurfaceChargesGeneratorCfg(flags)
    tool.SurfaceChargesGenerator = surfAcc.popPrivateTools()
    tool.RandomDisabledCellGenerator = SCT_RandomDisabledCellGeneratorCfg(flags)
    acc.mergeAll([frontAcc, surfAcc])
    acc.setPrivateTools(tool)
    return acc

def SCT_DigitizationToolCfg(flags, name="SCT_DigitizationTool", **kwargs):
    """Return a ComponentAccumulator with configured SCT digitization tool"""
    if flags.Digitization.PileUpPremixingForMT:
        kwargs.setdefault("OutputObjectName", flags.Overlay.BkgPrefix + "SCT_RDOs")
        kwargs.setdefault("OutputSDOName", flags.Overlay.BkgPrefix + "SCT_SDO_Map")
    else:
        kwargs.setdefault("OutputObjectName", "SCT_RDOs")
        kwargs.setdefault("OutputSDOName", "SCT_SDO_Map")
    kwargs.setdefault("HardScatterSplittingMode", 0)
    return SCT_DigitizationCommonCfg(flags, name, **kwargs)

def SCT_DigitizationToolHSCfg(flags, name="SCT_DigitizationToolHS", **kwargs):
    """Return a ComponentAccumulator with hard scatter configured SCT digitization tool"""
    kwargs.setdefault("OutputObjectName", "SCT_RDOs")
    kwargs.setdefault("OutputSDOName", "SCT_SDO_Map")
    kwargs.setdefault("HardScatterSplittingMode", 1)
    return SCT_DigitizationCommonCfg(flags, name, **kwargs)

def SCT_DigitizationToolPUCfg(flags, name="SCT_DigitizationToolPU",**kwargs):
    """Return a ComponentAccumulator with pileup configured SCT digitization tool"""
    kwargs.setdefault("OutputObjectName", "SCT_PU_RDOs")
    kwargs.setdefault("OutputSDOName", "SCT_PU_SDO_Map")
    kwargs.setdefault("HardScatterSplittingMode", 2)
    return SCT_DigitizationCommonCfg(flags, name, **kwargs)

def SCT_DigitizationToolOverlayCfg(flags, name="SCT_OverlayDigitizationTool",**kwargs):
    """Return a ComponentAccumulator with overlay configured SCT digitization tool"""
    acc = ComponentAccumulator()
    if flags.Overlay.Legacy.MT:
        kwargs.setdefault("InputSingleHitsName", "SCT_Hits")
        kwargs.setdefault("OutputObjectName", "StoreGateSvc+" + flags.Overlay.SigPrefix + "SCT_RDOs")
        kwargs.setdefault("OutputSDOName", "StoreGateSvc+" + flags.Overlay.SigPrefix + "SCT_SDO_Map")
    else:
        acc.addService(StoreGateSvc(flags.Overlay.Legacy.EventStore))
        kwargs.setdefault("OutputObjectName", flags.Overlay.Legacy.EventStore + "+SCT_RDOs")
        kwargs.setdefault("OutputSDOName", flags.Overlay.Legacy.EventStore + "+SCT_SDO_Map")
    kwargs.setdefault("HardScatterSplittingMode", 0)
    acc.merge(SCT_DigitizationCommonCfg(flags, name, **kwargs))
    return acc

def SCT_DigitizationToolSplitNoMergePUCfg(flags, name="SCT_DigitizationToolSplitNoMergePU",**kwargs):
    """Return a ComponentAccumulator with merged pileup configured SCT digitization tool"""
    kwargs.setdefault("InputObjectName", "PileupSCT_Hits")
    kwargs.setdefault("HardScatterSplittingMode", 0)
    kwargs.setdefault("OutputObjectName", "SCT_PU_RDOs")
    kwargs.setdefault("OutputSDOName", "SCT_PU_SDO_Map")
    kwargs.setdefault("OnlyHitElements", True)
    kwargs.setdefault("FrontEnd", "PileupSCT_FrontEnd")
    return SCT_DigitizationCommonCfg(flags, name, **kwargs)

def SCT_DigitizationToolGeantinoTruthCfg(flags, name="SCT_GeantinoTruthDigitizationTool", **kwargs):
    """Return Geantino truth configured digitization tool"""
    kwargs.setdefault("ParticleBarcodeVeto", 0)
    return SCT_DigitizationToolCfg(flags, name, **kwargs)

def SCT_RandomDisabledCellGeneratorCfg(flags, name="SCT_RandomDisabledCellGenerator", **kwargs):
    """Return configured random cell disabling tool"""
    kwargs.setdefault("TotalBadChannels", 0.01)
    return SCT_RandomDisabledCellGenerator(name, **kwargs)

def SCT_AmpCfg(flags, name="SCT_Amp", **kwargs):
    """Return configured amplifier and shaper tool"""
    kwargs.setdefault("CrossFactor2sides", 0.1)
    kwargs.setdefault("CrossFactorBack", 0.07)
    kwargs.setdefault("PeakTime", 21)
    kwargs.setdefault("deltaT", 1.0)
    kwargs.setdefault("Tmin", -25.0)
    kwargs.setdefault("Tmax", 150.0)
    return SCT_Amp(name, **kwargs)

def SCT_SurfaceChargesGeneratorCfg(flags, name="SCT_SurfaceChargesGenerator", **kwargs):
    """Return a ComponentAccumulator with configured surface charges tool"""
    acc = ComponentAccumulator()
    kwargs.setdefault("FixedTime", -999)
    kwargs.setdefault("SubtractTime", -999)
    kwargs.setdefault("SurfaceDriftTime", 10)
    kwargs.setdefault("NumberOfCharges", 1)
    kwargs.setdefault("SmallStepLength", 5)
    kwargs.setdefault("DepletionVoltage", 70)
    kwargs.setdefault("BiasVoltage", 150)
    kwargs.setdefault("isOverlay", flags.Detector.Overlay)
    # kwargs.setdefault("doTrapping", True) # ATL-INDET-INT-2016-019
    # experimental SCT_DetailedSurfaceChargesGenerator config dropped here
    tool = SCT_SurfaceChargesGenerator(name, **kwargs)
    tool.RadDamageSummaryTool = SCT_RadDamageSummaryTool()
    DCSCondAcc = SCT_DCSConditionsCfg(flags)
    DCSCondTool = DCSCondAcc.popPrivateTools()
    SiliCondAcc = SCT_SiliconConditionsCfg(flags, DCSConditionsTool=DCSCondTool)
    SiliCondTool = SiliCondAcc.popPrivateTools()
    SiliPropsAcc = SCT_SiPropertiesCfg(flags, SiConditionsTool=SiliCondTool)
    LorentzAcc = SCT_LorentzAngleCfg(flags)
    tool.SiConditionsTool = SiliCondTool
    tool.SiPropertiesTool = SiliPropsAcc.popPrivateTools()
    tool.LorentzAngleTool = LorentzAcc.popPrivateTools()
    acc.mergeAll([DCSCondAcc, SiliCondAcc, SiliPropsAcc, LorentzAcc])
    acc.setPrivateTools(tool)
    return acc

def SCT_FrontEndCfg(flags, name="SCT_FrontEnd", **kwargs):
    """Return a ComponentAccumulator with configured front-end electronics tool"""
    # Setup noise treament in SCT_FrontEnd
    # To set the mean noise values for the different module types
    # Default values set at 0 degrees, plus/minus ~5 enc per plus/minus degree
    kwargs.setdefault("NoiseBarrel", 1500.0)
    kwargs.setdefault("NoiseBarrel3", 1541.0)
    kwargs.setdefault("NoiseInners", 1090.0)
    kwargs.setdefault("NoiseMiddles", 1557.0)
    kwargs.setdefault("NoiseShortMiddles", 940.0)
    kwargs.setdefault("NoiseOuters", 1618.0)
    kwargs.setdefault("NOBarrel", 1.5e-5)
    kwargs.setdefault("NOBarrel3", 2.1e-5)
    kwargs.setdefault("NOInners", 5.0e-9)
    kwargs.setdefault("NOMiddles", 2.7e-5)
    kwargs.setdefault("NOShortMiddles", 2.0e-9)
    kwargs.setdefault("NOOuters", 3.5e-5)
    if not flags.Digitization.DoInnerDetectorNoise:
        Logging.logging.getLogger("SCT_FrontEndCfg")
        log.info("SCT_Digitization:::: Turned off Noise in SCT_FrontEnd")
        kwargs.setdefault("NoiseOn", False)
        kwargs.setdefault("AnalogueNoiseOn", False)
    else:
        kwargs.setdefault("NoiseOn", True)
        kwargs.setdefault("AnalogueNoiseOn", True)
    # In overlay MC, only analogue noise is on. Noise hits are not added.
    if flags.Detector.Overlay and flags.Input.isMC:
        kwargs["NoiseOn"] = False
        kwargs["AnalogueNoiseOn"] = True
    # Use Calibration data from Conditions DB, still for testing purposes only
    kwargs.setdefault("UseCalibData", True)
    # Setup the ReadCalibChip folders and Svc
    acc = SCT_ReadCalibChipDataCfg(flags)
    kwargs.setdefault("SCT_ReadCalibChipDataTool", acc.popPrivateTools())
    # DataCompressionMode: 1 is level mode x1x (default), 2 is edge mode 01x, 3 is expanded any hit xxx
    if flags.Digitization.PileUpPremixing:
        kwargs.setdefault("DataCompressionMode", 3)
    elif flags.Detector.Overlay and flags.Input.isMC:
        kwargs.setdefault("DataCompressionMode", 2)
    elif flags.Beam.BunchSpacing <= 50:
        kwargs.setdefault("DataCompressionMode", 1)
    else:
        kwargs.setdefault("DataCompressionMode", 3)
    # DataReadOutMode: 0 is condensed mode and 1 is expanded mode
    if flags.Detector.Overlay and flags.Input.isMC:
        kwargs.setdefault("DataReadOutMode", 0)
    else:
        kwargs.setdefault("DataReadOutMode", 1)
    acc.setPrivateTools(SCT_FrontEnd(name, **kwargs))
    return acc

def SCT_FrontEndPileupCfg(flags, name="PileupSCT_FrontEnd", **kwargs):
    """Return a ComponentAccumulator with pileup-configured front-end electronics tool"""
    kwargs.setdefault("NoiseBarrel", 0.0)
    kwargs.setdefault("NoiseBarrel3", 0.0)
    kwargs.setdefault("NoiseInners", 0.0)
    kwargs.setdefault("NoiseMiddles", 0.0)
    kwargs.setdefault("NoiseShortMiddles", 0.0)
    kwargs.setdefault("NoiseOuters", 0.0)
    kwargs.setdefault("NOBarrel", 0.0)
    kwargs.setdefault("NOBarrel3", 0.0)
    kwargs.setdefault("NOInners", 0.0)
    kwargs.setdefault("NOMiddles", 0.0)
    kwargs.setdefault("NOShortMiddles", 0.0)
    kwargs.setdefault("NOOuters", 0.0)
    kwargs.setdefault("NoiseOn", False)
    return SCT_FrontEndCfg(flags, name, **kwargs)

def SCT_RangeCfg(flags, name="SiliconRange", **kwargs):
    """Return an SCT configured PileUpXingFolder tool"""
    kwargs.setdefault("FirstXing", SCT_FirstXing())
    kwargs.setdefault("LastXing", SCT_LastXing())
    kwargs.setdefault("CacheRefreshFrequency", 1.0) # default 0 no dataproxy reset
    kwargs.setdefault("ItemList", ["SiHitCollection#SCT_Hits"] )
    return PileUpXingFolder(name, **kwargs)

def SCT_DigitizationCfg(toolCfg, flags, name="SCT_Digitization", **kwargs):
    """Return a ComponentAccumulator with toolCfg type SCT digitization"""
    acc = ComponentAccumulator()
    if "DigitizationTool" not in kwargs:
        toolAcc = toolCfg(flags)
        kwargs["DigitizationTool"] = toolAcc.popPrivateTools()
        acc.merge(toolAcc)
    alg = SCT_Digitization(name, **kwargs)
    acc.addEventAlgo(alg)
    return acc

def SCT_DigitizationHSCfg(flags, name="SCT_DigitizationHS", **kwargs):
    """Return a ComponentAccumulator with Hard Scatter SCT digitization"""
    return SCT_DigitizationCfg(SCT_DigitizationToolHSCfg, flags, name, **kwargs)

def SCT_DigitizationPUCfg(flags, name="SCT_DigitizationPU", **kwargs):
    """Return a ComponentAccumulator with PileUp SCT digitization"""
    return SCT_DigitizationCfg(SCT_DigitizationToolPUCfg, flags, name, **kwargs)

def SCT_DigitizationOverlayCfg(flags, name="SCT_OverlayDigitization", **kwargs):
    """Return a ComponentAccumulator with Overlay SCT digitization"""
    return SCT_DigitizationCfg(SCT_DigitizationToolOverlayCfg, flags, name, **kwargs)

