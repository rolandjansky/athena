"""Define methods to construct configured SCT Digitization tools and algorithms

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""
from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType, ProductionStep
from Digitization.PileUpMergeSvcConfigNew import PileUpMergeSvcCfg, PileUpXingFolderCfg
from Digitization.PileUpToolsConfig import PileUpToolsCfg
from Digitization.TruthDigitizationOutputConfig import TruthDigitizationOutputCfg
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
from SCT_ConditionsTools.ITkStripConditionsToolsConfig import ITkStripSiliconConditionsCfg
#from SCT_ConditionsTools.ITkStripConditionsToolsConfig import ItkStripReadCalibChipDataCfg
from SiLorentzAngleTool.ITkStripLorentzAngleConfig import ITkStripLorentzAngleCfg
from SiPropertiesTool.ITkStripSiPropertiesConfig import ITkStripSiPropertiesCfg
from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripReadoutGeometryCfg

import AthenaCommon.SystemOfUnits as Units

# The earliest and last bunch crossing times for which interactions will be sent
# to the SCT Digitization code
def ITkStripFirstXing():
    return -50


def ITkStripLastXing():
    return 25


def ITkStripDigitizationCommonCfg(flags, name="ITkStripDigitizationToolCommon", **kwargs):
    """Return ComponentAccumulator with common ITkStrip digitization tool config"""
    acc = ITkStripReadoutGeometryCfg(flags)
    if not flags.Digitization.DoInnerDetectorNoise:
        kwargs.setdefault("OnlyHitElements", True)
    kwargs.setdefault("InputObjectName", "ITkStripHits")
    kwargs.setdefault("EnableHits", True)
    kwargs.setdefault("BarrelOnly", False)
    # Set FixedTime for cosmics for use in SurfaceChargesGenerator
    if flags.Beam.Type is BeamType.Cosmics:
        kwargs.setdefault("CosmicsRun", True)
        kwargs.setdefault("FixedTime", 10)
    if flags.Digitization.DoXingByXingPileUp:
        kwargs.setdefault("FirstXing", ITkStripFirstXing())
        kwargs.setdefault("LastXing", ITkStripLastXing() )
    
    ITkStripDigitizationTool = CompFactory.ITk.StripDigitizationTool
    tool = ITkStripDigitizationTool(name, **kwargs)
    # attach ToolHandles
    tool.FrontEnd = acc.popToolsAndMerge(ITkStripFrontEndCfg(flags))
    tool.SurfaceChargesGenerator = acc.popToolsAndMerge(ITkStripSurfaceChargesGeneratorCfg(flags))
    tool.RandomDisabledCellGenerator = ITkStripRandomDisabledCellGeneratorCfg(flags)
    acc.setPrivateTools(tool)
    return acc


def ITkStripDigitizationToolCfg(flags, name="ITkStripDigitizationTool", **kwargs):
    """Return ComponentAccumulator with configured ITkStrip digitization tool"""
    acc = ComponentAccumulator()
    if flags.Digitization.PileUp:
        intervals = []
        if not flags.Digitization.DoXingByXingPileUp:
            intervals += [acc.popToolsAndMerge(ITkStripRangeCfg(flags))]
        kwargs.setdefault("MergeSvc", acc.getPrimaryAndMerge(PileUpMergeSvcCfg(flags, Intervals=intervals)).name)
    else:
        kwargs.setdefault("MergeSvc", '')
    kwargs.setdefault("OnlyUseContainerName", flags.Digitization.PileUp)
    if flags.Common.ProductionStep == ProductionStep.PileUpPresampling:
        kwargs.setdefault("OutputObjectName", flags.Overlay.BkgPrefix + "ITkStripRDOs")
        kwargs.setdefault("OutputSDOName", flags.Overlay.BkgPrefix + "ITkStripSDO_Map")
    else:
        kwargs.setdefault("OutputObjectName", "ITkStripRDOs")
        kwargs.setdefault("OutputSDOName", "ITkStripSDO_Map")
    kwargs.setdefault("HardScatterSplittingMode", 0)
    tool = acc.popToolsAndMerge(ITkStripDigitizationCommonCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


def ITkStripDigitizationHSToolCfg(flags, name="ITkStripDigitizationHSTool", **kwargs):
    """Return ComponentAccumulator with hard scatter configured SCT digitization tool"""
    acc = ComponentAccumulator()
    rangetool = acc.popToolsAndMerge(ITkStripRangeCfg(flags))
    kwargs.setdefault("MergeSvc", acc.getPrimaryAndMerge(PileUpMergeSvcCfg(flags, Intervals=rangetool)).name)
    kwargs.setdefault("OutputObjectName", "ITkStripRDOs")
    kwargs.setdefault("OutputSDOName", "ITkStripSDO_Map")
    kwargs.setdefault("HardScatterSplittingMode", 1)
    tool = acc.popToolsAndMerge(ITkStripDigitizationCommonCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


def ITkStripDigitizationPUToolCfg(flags, name="ITkStripDigitizationPUTool",**kwargs):
    """Return ComponentAccumulator with pileup configured SCT digitization tool"""
    acc = ComponentAccumulator()
    rangetool = acc.popToolsAndMerge(ITkStripRangeCfg(flags))
    kwargs.setdefault("MergeSvc", acc.getPrimaryAndMerge(PileUpMergeSvcCfg(flags, Intervals=rangetool)).name)
    kwargs.setdefault("OutputObjectName", "ITkStrip_PU_RDOs")
    kwargs.setdefault("OutputSDOName", "ITkStrip_PU_SDO_Map")
    kwargs.setdefault("HardScatterSplittingMode", 2)
    tool = acc.popToolsAndMerge(ITkStripDigitizationCommonCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


def ITkStripOverlayDigitizationToolCfg(flags, name="ITkStripOverlayDigitizationTool",**kwargs):
    """Return ComponentAccumulator with overlay configured SCT digitization tool"""
    acc = ComponentAccumulator()
    kwargs.setdefault("OnlyUseContainerName", False)
    kwargs.setdefault("OutputObjectName", flags.Overlay.SigPrefix + "ITkStripRDOs")
    kwargs.setdefault("OutputSDOName", flags.Overlay.SigPrefix + "ITkStripSDO_Map")
    kwargs.setdefault("HardScatterSplittingMode", 0)
    tool = acc.popToolsAndMerge(ITkStripDigitizationCommonCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


def ITkStripDigitizationToolSplitNoMergePUCfg(flags, name="ITkStripDigitizationToolSplitNoMergePU",**kwargs):
    """Return ComponentAccumulator with merged pileup configured SCT digitization tool"""
    acc = ComponentAccumulator()
    rangetool = acc.popToolsAndMerge(ITkStripRangeCfg(flags))
    kwargs.setdefault("MergeSvc", acc.getPrimaryAndMerge(PileUpMergeSvcCfg(flags, Intervals=rangetool)).name)
    kwargs.setdefault("InputObjectName", "PileupITkStripHits")
    kwargs.setdefault("HardScatterSplittingMode", 0)
    kwargs.setdefault("OutputObjectName", "ITkStrip_PU_RDOs")
    kwargs.setdefault("OutputSDOName", "ITkStrip_PU_SDO_Map")
    kwargs.setdefault("OnlyHitElements", True)
    kwargs.setdefault("FrontEnd", "PileupITkStripFrontEnd")
    tool = acc.popToolsAndMerge(ITkStripDigitizationCommonCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


def ITkStripDigitizationToolGeantinoTruthCfg(flags, name="ITkStripGeantinoTruthDigitizationTool", **kwargs):
    """Return Geantino truth configured digitization tool"""
    kwargs.setdefault("ParticleBarcodeVeto", 0)
    return ITkStripDigitizationToolCfg(flags, name, **kwargs)


def ITkStripRandomDisabledCellGeneratorCfg(flags, name="ITkStripRandomDisabledCellGenerator", **kwargs):
    """Return configured random cell disabling tool"""
    kwargs.setdefault("TotalBadChannels", 0.01)
    ITkStripRandomDisabledCellGenerator = CompFactory.SCT_RandomDisabledCellGenerator
    return ITkStripRandomDisabledCellGenerator(name, **kwargs)


def ITkStripAmpCfg(flags, name="ITkStripAmp", **kwargs):
    """Return configured amplifier and shaper tool"""
    acc = ComponentAccumulator()
    kwargs.setdefault("CrossFactor2sides", 0.1)
    kwargs.setdefault("CrossFactorBack", 0.07)
    kwargs.setdefault("PeakTime", 21)
    kwargs.setdefault("deltaT", 1.0)
    kwargs.setdefault("Tmin", -25.0)
    kwargs.setdefault("Tmax", 150.0)
    acc.setPrivateTools(CompFactory.SCT_Amp(name, **kwargs))
    return acc


def ITkStripSurfaceChargesGeneratorCfg(flags, name="ITkStripSurfaceChargesGenerator", **kwargs):
    """Return ComponentAccumulator with configured surface charges tool"""
    acc = ComponentAccumulator()
    kwargs.setdefault("FixedTime", -999)
    kwargs.setdefault("SubtractTime", -999)
    kwargs.setdefault("SurfaceDriftTime", 10*Units.ns)
    kwargs.setdefault("NumberOfCharges", 1)
    kwargs.setdefault("SmallStepLength", 5*Units.micrometer)
    kwargs.setdefault("DepletionVoltage", 70)
    kwargs.setdefault("BiasVoltage", 150)
    kwargs.setdefault("isOverlay", flags.Common.ProductionStep == ProductionStep.Overlay)
    # kwargs.setdefault("doTrapping", True) # ATL-INDET-INT-2016-019
    # experimental ITkStripDetailedSurfaceChargesGenerator config dropped here
    tool = CompFactory.ITk.StripSurfaceChargesGenerator(name, **kwargs)
    tool.RadDamageSummaryTool = CompFactory.SCT_RadDamageSummaryTool(name="ITkStripRadDamageSummaryTool")
    tool.SiConditionsTool = acc.popToolsAndMerge(ITkStripSiliconConditionsCfg(flags))
    tool.SiPropertiesTool = acc.popToolsAndMerge(ITkStripSiPropertiesCfg(flags, SiConditionsTool=tool.SiConditionsTool))
    tool.LorentzAngleTool = acc.popToolsAndMerge(ITkStripLorentzAngleCfg(flags, SiConditionsTool=tool.SiConditionsTool))
    acc.setPrivateTools(tool)
    return acc


def ITkStripFrontEndCfg(flags, name="ITkStripFrontEnd", **kwargs):
    """Return ComponentAccumulator with configured front-end electronics tool"""
    # Setup noise treament in ITkStripFrontEnd
    # To set the mean noise values for the different module types
    # These are SCT values, kept for reference
    #kwargs.setdefault("NoiseBarrel", 1500.0)
    #kwargs.setdefault("NoiseBarrel3", 1541.0)
    #kwargs.setdefault("NoiseInners", 1090.0)
    #kwargs.setdefault("NoiseMiddles", 1557.0)
    #kwargs.setdefault("NoiseShortMiddles", 940.0)
    #kwargs.setdefault("NoiseOuters", 1618.0)
    #kwargs.setdefault("NOBarrel", 1.5e-5)
    #kwargs.setdefault("NOBarrel3", 2.1e-5)
    #kwargs.setdefault("NOInners", 5.0e-9)
    #kwargs.setdefault("NOMiddles", 2.7e-5)
    #kwargs.setdefault("NOShortMiddles", 2.0e-9)
    #kwargs.setdefault("NOOuters", 3.5e-5)
    kwargs.setdefault("DetectorManager","ITkStrip")
    #Always turn off noise for ITkStrip, since implementation is SCT specific
    #ITkStrip noise treatment would be done in dedicated FrontEnd
    #if not flags.Digitization.DoInnerDetectorNoise:
    log = logging.getLogger("ITkStripFrontEndCfg")
    log.info("ITkStripDigitization:::: Turned off Noise in ITkStripFrontEnd")
    log.info("ITkStripDigitization:::: Overriding Digitization.DoInnerDetectorNoise flag")
    kwargs.setdefault("NoiseOn", False)
    kwargs.setdefault("AnalogueNoiseOn", False)
    #else:
    #    kwargs.setdefault("NoiseOn", True)
    #    kwargs.setdefault("AnalogueNoiseOn", True)
    # In overlay MC, only analogue noise is on (off for data). Noise hits are not added.
    if flags.Common.ProductionStep == ProductionStep.Overlay:
        kwargs["NoiseOn"] = False
        kwargs["AnalogueNoiseOn"] = flags.Input.isMC
    # Use Calibration data from Conditions DB, still for testing purposes only
    #This doesn't work yet for ITk - need specific version
    kwargs.setdefault("UseCalibData", False)
    # Setup the ReadCalibChip folders and Svc
    #acc = SCT_ReadCalibChipDataCfg(flags)
    #kwargs.setdefault("SCT_ReadCalibChipDataTool", acc.popPrivateTools())
    acc = ComponentAccumulator()
    
    # DataCompressionMode: 1 is level mode X1X (default), 2 is edge mode 01X, 3 is any hit mode (1XX|X1X|XX1)
    #if flags.Common.ProductionStep == ProductionStep.PileUpPresampling:
    #    kwargs.setdefault("DataCompressionMode", 3)
    #elif flags.Common.ProductionStep == ProductionStep.Overlay and flags.Input.isMC:
    #    kwargs.setdefault("DataCompressionMode", 2)
    #elif flags.Beam.BunchSpacing <= 50:
    #    kwargs.setdefault("DataCompressionMode", 1)
    #else:
    #    kwargs.setdefault("DataCompressionMode", 3)
    # DataReadOutMode: 0 is condensed mode and 1 is expanded mode
    #if flags.Common.ProductionStep == ProductionStep.Overlay and flags.Input.isMC:
    #    kwargs.setdefault("DataReadOutMode", 0)
    #else:
    #    Only condensed mode works for ITkStrip at the moment...
    #    kwargs.setdefault("DataReadOutMode", 0)
    
    #Block above kept for future reference
    #For ITkStrip initially we will just take the following for consistency
    #with previous configuration in 21.9
    kwargs.setdefault("DataReadOutMode", 0)
    kwargs.setdefault("DataCompressionMode",2)

    kwargs.setdefault("SCT_Amp", acc.popToolsAndMerge(ITkStripAmpCfg(flags)))

    acc.setPrivateTools(CompFactory.SCT_FrontEnd(name, **kwargs))
    return acc


def ITkStripFrontEndPileupCfg(flags, name="PileupITkStripFrontEnd", **kwargs):
    """Return ComponentAccumulator with pileup-configured front-end electronics tool"""
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
    return ITkStripFrontEndCfg(flags, name, **kwargs)


def ITkStripRangeCfg(flags, name="SiliconRange", **kwargs):
    """Return an SCT configured PileUpXingFolder tool"""
    kwargs.setdefault("FirstXing", ITkStripFirstXing())
    kwargs.setdefault("LastXing", ITkStripLastXing())
    kwargs.setdefault("CacheRefreshFrequency", 1.0) # default 0 no dataproxy reset
    kwargs.setdefault("ItemList", ["SiHitCollection#ITkStripHits"] )
    return PileUpXingFolderCfg(flags, name, **kwargs)


def ITkStripOutputCfg(flags):
    """Return ComponentAccumulator with Output for SCT. Not standalone."""
    acc = ComponentAccumulator()
    if flags.Output.doWriteRDO:
        ItemList = ["SCT_RDO_Container#*"]
        if flags.Digitization.TruthOutput:
            ItemList += ["InDetSimDataCollection#*"]
            acc.merge(TruthDigitizationOutputCfg(flags))
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList))
    return acc


def ITkStripDigitizationBasicCfg(flags, **kwargs):
    """Return ComponentAccumulator for SCT digitization"""
    acc = ComponentAccumulator()
    if "PileUpTools" not in kwargs:
        PileUpTools = acc.popToolsAndMerge(ITkStripDigitizationToolCfg(flags))
        kwargs["PileUpTools"] = PileUpTools
    acc.merge(PileUpToolsCfg(flags, **kwargs))
    return acc


def ITkStripOverlayDigitizationBasicCfg(flags, **kwargs):
    """Return ComponentAccumulator with SCT Overlay digitization"""
    acc = ComponentAccumulator()
    if "DigitizationTool" not in kwargs:
        tool = acc.popToolsAndMerge(ITkStripOverlayDigitizationToolCfg(flags))
        kwargs["DigitizationTool"] = tool

    if flags.Concurrency.NumThreads > 0:
        kwargs.setdefault('Cardinality', flags.Concurrency.NumThreads)

    # Set common overlay extra inputs
    kwargs.setdefault("ExtraInputs", flags.Overlay.ExtraInputs)

    ITkStripDigitization = CompFactory.ITkStripDigitization
    acc.addEventAlgo(ITkStripDigitization(name="ITkStripOverlayDigitization", **kwargs))
    return acc


# with output defaults
def ITkStripDigitizationCfg(flags, **kwargs):
    """Return ComponentAccumulator for SCT digitization and Output"""
    acc = ITkStripDigitizationBasicCfg(flags, **kwargs)
    acc.merge(ITkStripOutputCfg(flags))
    return acc


def ITkStripOverlayDigitizationCfg(flags, **kwargs):
    """Return ComponentAccumulator with SCT Overlay digitization and Output"""
    acc = ITkStripOverlayDigitizationBasicCfg(flags, **kwargs)
    acc.merge(ITkStripOutputCfg(flags))
    return acc


# additional specialisations
def ITkStripDigitizationHSCfg(flags, name="ITkStripDigitizationHS", **kwargs):
    """Return ComponentAccumulator for Hard-Scatter-only SCT digitization and Output"""
    acc = ITkStripDigitizationHSToolCfg(flags)
    kwargs["PileUpTools"] = acc.popPrivateTools()
    acc = ITkStripDigitizationBasicCfg(flags, name=name, **kwargs)
    acc.merge(ITkStripOutputCfg(flags))
    return acc


def ITkStripDigitizationPUCfg(flags, name="ITkStripDigitizationPU", **kwargs):
    """Return ComponentAccumulator with Pile-up-only SCT digitization and Output"""
    acc = ITkStripDigitizationPUToolCfg(flags)
    kwargs["PileUpTools"] = acc.popPrivateTools()
    acc = ITkStripDigitizationBasicCfg(flags, name=name, **kwargs)
    acc.merge(ITkStripOutputCfg(flags))
    return acc
