"""Define methods to construct configured Pixel Digitization tools and algorithms

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import LHCPeriod, ProductionStep
from Digitization.PileUpMergeSvcConfigNew import PileUpMergeSvcCfg, PileUpXingFolderCfg
from Digitization.PileUpToolsConfig import PileUpToolsCfg
from Digitization.TruthDigitizationOutputConfig import TruthDigitizationOutputCfg
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
from PixelConditionsAlgorithms.PixelConditionsConfig import (
    PixelConfigCondAlgCfg, PixelChargeCalibCondAlgCfg,
    PixelDistortionAlgCfg, PixelRadSimFluenceMapAlgCfg,
    PixelChargeLUTCalibCondAlgCfg
)
from PixelConditionsTools.PixelConditionsSummaryConfig import PixelConditionsSummaryCfg
from PixelGeoModel.PixelGeoModelConfig import PixelReadoutGeometryCfg
from PixelReadoutGeometry.PixelReadoutGeometryConfig import PixelReadoutManagerCfg
from SiLorentzAngleTool.PixelLorentzAngleConfig import PixelLorentzAngleCfg
from SiPropertiesTool.PixelSiPropertiesConfig import PixelSiPropertiesCfg


# The earliest and last bunch crossing times for which interactions will be sent
# to the Pixel Digitization code
def Pixel_FirstXing(flags):
    if flags.Beam.estimatedLuminosity > 0.5e33:
        return -25
    else:
        return -50


def Pixel_LastXing(flags):
    if flags.Beam.estimatedLuminosity > 0.5e33:
        if flags.Beam.BunchSpacing > 50:
            return 75
        else:
            return 25
    else:
        return 100


def RadDamageUtilCfg(flags, name="RadDamageUtil", **kwargs):
    """Return a configured RadDamageUtil"""
    kwargs.setdefault("defaultRamo", 1)
    kwargs.setdefault("betaElectrons", 4.5e-16)
    kwargs.setdefault("betaHoles", 6.0e-16)
    kwargs.setdefault("saveDebugMaps", False)
    RadDamageUtil = CompFactory.RadDamageUtil
    return RadDamageUtil(name, **kwargs)


def EfieldInterpolatorCfg(flags, name="EfieldInterpolator", **kwargs):
    """Return a configured EfieldInterpolator"""
    kwargs.setdefault("initialized", False)
    kwargs.setdefault("useSpline", True)
    kwargs.setdefault("sensorDepth", 200)
    EfieldInterpolator = CompFactory.EfieldInterpolator
    return EfieldInterpolator(name, **kwargs)


def EnergyDepositionToolCfg(flags, name="EnergyDepositionTool", **kwargs):
    """Return a configured EnergyDepositionTool"""
    acc = PixelDistortionAlgCfg(flags)
    kwargs.setdefault("DeltaRayCut", 80.7687)
    kwargs.setdefault("nCols", 5)
    kwargs.setdefault("LoopLimit", 100000)
    kwargs.setdefault("doBichsel", True)
    kwargs.setdefault("doBichselBetaGammaCut", 0.7) # dEdx not quite consistent below this
    kwargs.setdefault("doDeltaRay", False)          # needs validation
    kwargs.setdefault("doPU", True)
    EnergyDepositionTool = CompFactory.EnergyDepositionTool
    acc.setPrivateTools(EnergyDepositionTool(name, **kwargs))
    return acc


def SensorSimPlanarToolCfg(flags, name="SensorSimPlanarTool", **kwargs):
    """Return ComponentAccumulator with configured SensorSimPlanarTool"""
    acc = PixelConfigCondAlgCfg(flags)
    SiTool = acc.popToolsAndMerge(PixelSiPropertiesCfg(flags))
    LorentzTool = acc.popToolsAndMerge(PixelLorentzAngleCfg(flags))
    kwargs.setdefault("SiPropertiesTool", SiTool)
    kwargs.setdefault("LorentzAngleTool", LorentzTool)
    SensorSimPlanarTool = CompFactory.SensorSimPlanarTool
    kwargs.setdefault("doRadDamage", flags.Digitization.DoPixelPlanarRadiationDamage)
    kwargs.setdefault("doRadDamageTemplate", flags.Digitization.DoPixelPlanarRadiationDamageTemplate)
    if flags.Digitization.DoPixelPlanarRadiationDamage:
        acc.merge(PixelRadSimFluenceMapAlgCfg(flags))
    acc.setPrivateTools(SensorSimPlanarTool(name, **kwargs))
    return acc


def SensorSim3DToolCfg(flags, name="SensorSim3DTool", **kwargs):
    """Return ComponentAccumulator with configured SensorSim3DTool"""
    acc = PixelConfigCondAlgCfg(flags)
    SiTool = acc.popToolsAndMerge(PixelSiPropertiesCfg(flags))
    acc.popToolsAndMerge(PixelLorentzAngleCfg(flags))
    kwargs.setdefault("SiPropertiesTool", SiTool)
    SensorSim3DTool = CompFactory.SensorSim3DTool
    kwargs.setdefault("doRadDamage", flags.Digitization.DoPixel3DRadiationDamage)
    kwargs.setdefault("doRadDamageTemplate", flags.Digitization.DoPixel3DRadiationDamageTemplate)
    if flags.Digitization.DoPixel3DRadiationDamage:
        acc.merge(PixelRadSimFluenceMapAlgCfg(flags))
    acc.setPrivateTools(SensorSim3DTool(name, **kwargs))
    return acc


def BarrelFEI4SimToolCfg(flags, name="BarrelFEI4SimTool", **kwargs):
    """Return a FEI4SimTool configured for Barrel"""
    acc = PixelReadoutManagerCfg(flags)
    acc.merge(PixelConfigCondAlgCfg(flags))
    if flags.GeoModel.Run is LHCPeriod.Run3:
        acc.merge(PixelChargeLUTCalibCondAlgCfg(flags))
    else:
        acc.merge(PixelChargeCalibCondAlgCfg(flags))
    kwargs.setdefault("BarrelEC", 0)
    kwargs.setdefault("DoNoise", flags.Digitization.DoInnerDetectorNoise)
    kwargs.setdefault("PixelReadoutManager", acc.getPrimary())
    kwargs.setdefault("PixelConditionsSummaryTool", acc.popToolsAndMerge(PixelConditionsSummaryCfg(flags)))
    FEI4SimTool = CompFactory.FEI4SimTool
    acc.setPrivateTools(FEI4SimTool(name, **kwargs))
    return acc


def DBMFEI4SimToolCfg(flags, name="DBMFEI4SimTool", **kwargs):
    """Return a FEI4SimTool configured for Endcap"""
    acc = PixelReadoutManagerCfg(flags)
    acc.merge(PixelConfigCondAlgCfg(flags))
    if flags.GeoModel.Run is LHCPeriod.Run3:
        acc.merge(PixelChargeLUTCalibCondAlgCfg(flags))
    else:
        acc.merge(PixelChargeCalibCondAlgCfg(flags))
    kwargs.setdefault("BarrelEC", 4)
    kwargs.setdefault("DoNoise", flags.Digitization.DoInnerDetectorNoise)
    kwargs.setdefault("PixelReadoutManager", acc.getPrimary())
    kwargs.setdefault("PixelConditionsSummaryTool", acc.popToolsAndMerge(PixelConditionsSummaryCfg(flags)))
    FEI4SimTool = CompFactory.FEI4SimTool
    acc.setPrivateTools(FEI4SimTool(name, **kwargs))
    return acc


def BarrelFEI3SimToolCfg(flags, name="BarrelFEI3SimTool", **kwargs):
    """Return a FEI3SimTool configured for Barrel"""
    acc = PixelReadoutManagerCfg(flags)
    acc.merge(PixelConfigCondAlgCfg(flags))
    if flags.GeoModel.Run is LHCPeriod.Run3:
        acc.merge(PixelChargeLUTCalibCondAlgCfg(flags))
    else:
        acc.merge(PixelChargeCalibCondAlgCfg(flags))
    kwargs.setdefault("BarrelEC", 0)
    kwargs.setdefault("PixelReadoutManager", acc.getPrimary())
    kwargs.setdefault("PixelConditionsSummaryTool", acc.popToolsAndMerge(PixelConditionsSummaryCfg(flags)))
    FEI3SimTool = CompFactory.FEI3SimTool
    acc.setPrivateTools(FEI3SimTool(name, **kwargs))
    return acc


def EndcapFEI3SimToolCfg(flags, name="EndcapFEI3SimTool", **kwargs):
    """Return a FEI3SimTool configured for Endcap"""
    acc = PixelReadoutManagerCfg(flags)
    acc.merge(PixelConfigCondAlgCfg(flags))
    if flags.GeoModel.Run is LHCPeriod.Run3:
        acc.merge(PixelChargeLUTCalibCondAlgCfg(flags))
    else:
        acc.merge(PixelChargeCalibCondAlgCfg(flags))
    kwargs.setdefault("BarrelEC", 2)
    kwargs.setdefault("PixelReadoutManager", acc.getPrimary())
    kwargs.setdefault("PixelConditionsSummaryTool", acc.popToolsAndMerge(PixelConditionsSummaryCfg(flags)))
    FEI3SimTool = CompFactory.FEI3SimTool
    acc.setPrivateTools(FEI3SimTool(name, **kwargs))
    return acc


def PixelDigitizationBasicToolCfg(flags, name="PixelDigitizationBasicTool", **kwargs):
    """Return ComponentAccumulator with configured PixelDigitizationTool"""
    acc = PixelReadoutGeometryCfg(flags)
    # set up tool handle lists
    chargeTools = []
    feSimTools = []
    chargeTools.append(acc.popToolsAndMerge(SensorSimPlanarToolCfg(flags)))
    chargeTools.append(acc.popToolsAndMerge(SensorSim3DToolCfg(flags)))
    feSimTools.append(acc.popToolsAndMerge(BarrelFEI4SimToolCfg(flags)))
    feSimTools.append(acc.popToolsAndMerge(DBMFEI4SimToolCfg(flags)))
    feSimTools.append(acc.popToolsAndMerge(BarrelFEI3SimToolCfg(flags)))
    feSimTools.append(acc.popToolsAndMerge(EndcapFEI3SimToolCfg(flags)))
    kwargs.setdefault("InputObjectName", "PixelHits")
    kwargs.setdefault("ChargeTools", chargeTools)
    kwargs.setdefault("FrontEndSimTools", feSimTools)
    kwargs.setdefault("EnergyDepositionTool", acc.popToolsAndMerge(EnergyDepositionToolCfg(flags)))
    if flags.Digitization.DoXingByXingPileUp:
        kwargs.setdefault("FirstXing", Pixel_FirstXing(flags))
        kwargs.setdefault("LastXing", Pixel_LastXing(flags))
    from RngComps.RandomServices import AthRNGSvcCfg
    kwargs.setdefault("RndmSvc", acc.getPrimaryAndMerge(AthRNGSvcCfg(flags)).name)

    PixelDigitizationTool = CompFactory.PixelDigitizationTool
    acc.setPrivateTools(PixelDigitizationTool(name, **kwargs))
    return acc


def PixelDigitizationToolCfg(flags, name="PixelDigitizationTool", **kwargs):
    """Return ComponentAccumulator with configured BasicPixelDigitizationTool"""
    acc = ComponentAccumulator()
    if flags.Digitization.PileUp:
        intervals = []
        if not flags.Digitization.DoXingByXingPileUp:
            intervals += [acc.popToolsAndMerge(PixelRangeCfg(flags))]
        kwargs.setdefault("PileUpMergeSvc", acc.getPrimaryAndMerge(PileUpMergeSvcCfg(flags, Intervals=intervals)).name)
    else:
        kwargs.setdefault("PileUpMergeSvc", '')
    kwargs.setdefault("OnlyUseContainerName", flags.Digitization.PileUp)
    kwargs.setdefault("HardScatterSplittingMode", 0)
    if flags.Common.ProductionStep == ProductionStep.PileUpPresampling:
        kwargs.setdefault("RDOCollName", flags.Overlay.BkgPrefix + "PixelRDOs")
        kwargs.setdefault("SDOCollName", flags.Overlay.BkgPrefix + "PixelSDO_Map")
    else:
        kwargs.setdefault("RDOCollName", "PixelRDOs")
        kwargs.setdefault("SDOCollName", "PixelSDO_Map")
    tool = acc.popToolsAndMerge(PixelDigitizationBasicToolCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


def PixelGeantinoTruthDigitizationToolCfg(flags, name="PixelGeantinoTruthDigitizationTool", **kwargs):
    """Return configured PixelDigitizationTool"""
    acc = ComponentAccumulator()
    if flags.Digitization.PileUp:
        intervals = []
        if not flags.Digitization.DoXingByXingPileUp:
            intervals += [acc.popToolsAndMerge(PixelRangeCfg(flags))]
        kwargs.setdefault("PileUpMergeSvc", acc.getPrimaryAndMerge(PileUpMergeSvcCfg(flags, Intervals=intervals)).name)
    else:
        kwargs.setdefault("PileUpMergeSvc", '')
    kwargs.setdefault("OnlyUseContainerName", flags.Digitization.PileUp)
    kwargs.setdefault("ParticleBarcodeVeto", 0)
    from RngComps.RandomServices import AthRNGSvcCfg
    kwargs.setdefault("RndmSvc", acc.getPrimaryAndMerge(AthRNGSvcCfg(flags)).name)
    PixelDigitizationTool = CompFactory.PixelDigitizationTool
    acc.setPrivateTools(PixelDigitizationTool(name, **kwargs))
    return acc


def PixelDigitizationHSToolCfg(flags, name="PixelDigitizationHSTool", **kwargs):
    """Return ComponentAccumulator with PixelDigitizationTool configured for Hard Scatter"""
    acc = ComponentAccumulator()
    rangetool = acc.popToolsAndMerge(PixelRangeCfg(flags))
    kwargs.setdefault("PileUpMergeSvc", acc.getPrimaryAndMerge(PileUpMergeSvcCfg(flags, Intervals=rangetool)).name)
    kwargs.setdefault("HardScatterSplittingMode", 1)
    tool = acc.popToolsAndMerge(PixelDigitizationBasicToolCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


def PixelDigitizationPUToolCfg(flags, name="PixelDigitizationPUTool", **kwargs):
    """Return ComponentAccumulator with PixelDigitizationTool configured for PileUp"""
    acc = ComponentAccumulator()
    rangetool = acc.popToolsAndMerge(PixelRangeCfg(flags))
    kwargs.setdefault("PileUpMergeSvc", acc.getPrimaryAndMerge(PileUpMergeSvcCfg(flags, Intervals=rangetool)).name)
    kwargs.setdefault("HardScatterSplittingMode", 2)
    kwargs.setdefault("RDOCollName", "Pixel_PU_RDOs")
    kwargs.setdefault("SDOCollName", "Pixel_PU_SDO_Map")
    tool = acc.popToolsAndMerge(PixelDigitizationBasicToolCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


def PixelDigitizationSplitNoMergePUToolCfg(flags, name="PixelDigitizationSplitNoMergePUTool", **kwargs):
    """Return ComponentAccumulator with PixelDigitizationTool configured for PileUpPixelHits"""
    acc = ComponentAccumulator()
    rangetool = acc.popToolsAndMerge(PixelRangeCfg(flags))
    kwargs.setdefault("PileUpMergeSvc", acc.getPrimaryAndMerge(PileUpMergeSvcCfg(flags, Intervals=rangetool)).name)
    kwargs.setdefault("HardScatterSplittingMode", 0)
    kwargs.setdefault("InputObjectName", "PileupPixelHits")
    kwargs.setdefault("RDOCollName", "Pixel_PU_RDOs")
    kwargs.setdefault("SDOCollName", "Pixel_PU_SDO_Map")
    tool = acc.popToolsAndMerge(PixelDigitizationBasicToolCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


def PixelOverlayDigitizationToolCfg(flags, name="PixelOverlayDigitizationTool", **kwargs):
    """Return ComponentAccumulator with PixelDigitizationTool configured for overlay"""
    kwargs.setdefault("OnlyUseContainerName", False)
    kwargs.setdefault("RDOCollName", flags.Overlay.SigPrefix + "PixelRDOs")
    kwargs.setdefault("SDOCollName", flags.Overlay.SigPrefix + "PixelSDO_Map")
    kwargs.setdefault("HardScatterSplittingMode", 0)
    return PixelDigitizationBasicToolCfg(flags, name, **kwargs)


def PixelRangeCfg(flags, name="PixelRange", **kwargs):
    """Return a configured PileUpXingFolder tool"""
    kwargs.setdefault("FirstXing", Pixel_FirstXing(flags))
    kwargs.setdefault("LastXing", Pixel_LastXing(flags))
    kwargs.setdefault("CacheRefreshFrequency", 1.0) # default 0 no dataproxy reset
    kwargs.setdefault("ItemList", ["SiHitCollection#PixelHits"])
    return PileUpXingFolderCfg(flags, name, **kwargs)


def PixelOutputCfg(flags):
    """Return ComponentAccumulator with Output for Pixel. Not standalone."""
    acc = ComponentAccumulator()
    if flags.Output.doWriteRDO:
        ItemList = ["PixelRDO_Container#*"]
        if flags.Digitization.TruthOutput:
            ItemList += ["InDetSimDataCollection#*"]
            acc.merge(TruthDigitizationOutputCfg(flags))
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList))
    return acc


def PixelDigitizationBasicCfg(flags, **kwargs):
    """Return ComponentAccumulator for Pixel digitization"""
    acc = ComponentAccumulator()
    if "PileUpTools" not in kwargs:
        PileUpTools = acc.popToolsAndMerge(PixelDigitizationToolCfg(flags))
        kwargs["PileUpTools"] = PileUpTools
    acc.merge(PileUpToolsCfg(flags, **kwargs))
    return acc


def PixelOverlayDigitizationBasicCfg(flags, **kwargs):
    """Return ComponentAccumulator with Pixel Overlay digitization"""
    acc = ComponentAccumulator()
    if "DigitizationTool" not in kwargs:
        tool = acc.popToolsAndMerge(PixelOverlayDigitizationToolCfg(flags))
        kwargs["DigitizationTool"] = tool

    if flags.Concurrency.NumThreads > 0:
        kwargs.setdefault("Cardinality", flags.Concurrency.NumThreads)

    # Set common overlay extra inputs
    kwargs.setdefault("ExtraInputs", flags.Overlay.ExtraInputs)
    PixelDigitization = CompFactory.PixelDigitization
    acc.addEventAlgo(PixelDigitization(name="PixelOverlayDigitization", **kwargs))
    return acc


# with output defaults
def PixelDigitizationCfg(flags, **kwargs):
    """Return ComponentAccumulator for Pixel digitization and Output"""
    acc = PixelDigitizationBasicCfg(flags, **kwargs)
    acc.merge(PixelOutputCfg(flags))
    return acc


# additional specialisations
def PixelDigitizationHSCfg(flags, **kwargs):
    """Return ComponentAccumulator for Hard-Scatter-only Pixel digitization and Output"""
    acc = ComponentAccumulator()
    tool = acc.popToolsAndMerge(PixelDigitizationHSToolCfg(flags))
    kwargs["PileUpTools"] = tool
    acc.merge(PileUpToolsCfg(flags, **kwargs))
    acc.merge(PixelOutputCfg(flags))
    return acc


def PixelDigitizationPUCfg(flags, **kwargs):
    """Return ComponentAccumulator with Pile-up-only Pixel digitization and Output"""
    acc = ComponentAccumulator()
    tool = acc.popToolsAndMerge(PixelDigitizationPUToolCfg(flags))
    kwargs["PileUpTools"] = tool
    acc.merge(PileUpToolsCfg(flags, **kwargs))
    acc.merge(PixelOutputCfg(flags))
    return acc
