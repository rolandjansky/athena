"""Define methods to construct configured Pixel Digitization tools and algorithms

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from PixelCabling.PixelCablingConfigNew import PixelCablingSvcCfg
from PixelConditionsAlgorithms.PixelConditionsConfig import (
    PixelCablingCondAlgCfg, PixelChargeCalibCondAlgCfg, PixelConfigCondAlgCfg, 
    PixelDCSCondHVAlgCfg, PixelDCSCondStateAlgCfg, PixelDCSCondStatusAlgCfg, 
    PixelDCSCondTempAlgCfg, PixelDistortionAlgCfg, 
    PixelHitDiscCnfgAlgCfg, PixelOfflineCalibCondAlgCfg, PixelReadoutSpeedAlgCfg
# NEW FOR RUN3    PixelDeadMapCondAlgCfg, PixelChargeLUTCalibCondAlgCfg
)

from Digitization.PileUpToolsConfig import PileUpToolsCfg
from SiPropertiesTool.PixelSiPropertiesConfig import PixelSiPropertiesCfg
from SiLorentzAngleTool.PixelLorentzAngleConfig import PixelLorentzAngleCfg
from PixelConditionsTools.PixelConditionsSummaryConfig import PixelConditionsSummaryCfg

from PixelGeoModel.PixelGeoModelConfig import PixelGeometryCfg
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
from Digitization.TruthDigitizationOutputConfig import TruthDigitizationOutputCfg
from Digitization.PileUpMergeSvcConfigNew import PileUpMergeSvcCfg, PileUpXingFolderCfg


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
    kwargs.setdefault("DeltaRayCut", 117.)
    kwargs.setdefault("nCols", 5)
    kwargs.setdefault("LoopLimit", 100000)
    kwargs.setdefault("doBichsel", True)
    kwargs.setdefault("doBichselBetaGammaCut", 0.7) # dEdx not quite consistent below this
    kwargs.setdefault("doDeltaRay", False)          # needs validation
    kwargs.setdefault("doPU", True)
    EnergyDepositionTool = CompFactory.EnergyDepositionTool
    return EnergyDepositionTool(name, **kwargs)


def SensorSimPlanarToolCfg(flags, name="SensorSimPlanarTool", **kwargs):
    """Return ComponentAccumulator with configured SensorSimPlanarTool"""
    acc = PixelSiPropertiesCfg(flags)
    SiTool = acc.popPrivateTools()
    LorentzTool = acc.popToolsAndMerge(PixelLorentzAngleCfg(flags))
    kwargs.setdefault("SiPropertiesTool", SiTool)
    kwargs.setdefault("LorentzAngleTool", LorentzTool)
    SensorSimPlanarTool = CompFactory.SensorSimPlanarTool
    acc.setPrivateTools(SensorSimPlanarTool(name, **kwargs))
    return acc


def SensorSim3DToolCfg(flags, name="SensorSim3DTool", **kwargs):
    """Return ComponentAccumulator with configured SensorSim3DTool"""
    acc = PixelSiPropertiesCfg(flags)
    SiTool = acc.popPrivateTools()
    acc.popToolsAndMerge(PixelLorentzAngleCfg(flags))
    kwargs.setdefault("SiPropertiesTool", SiTool)
    SensorSim3DTool = CompFactory.SensorSim3DTool
    acc.setPrivateTools(SensorSim3DTool(name, **kwargs))
    return acc


def BarrelRD53SimToolCfg(flags, name="BarrelRD53SimTool", **kwargs):
    """Return a RD53SimTool configured for Barrel"""
    kwargs.setdefault("BarrelEC", 0)
    kwargs.setdefault("DoNoise", flags.Digitization.DoInnerDetectorNoise)
    RD53SimTool = CompFactory.RD53SimTool
    return RD53SimTool(name, **kwargs)


def EndcapRD53SimToolCfg(flags, name="EndcapRD53SimTool", **kwargs):
    """Return a RD53SimTool configured for Endcap"""
    kwargs.setdefault("BarrelEC", 2)
    kwargs.setdefault("DoNoise", flags.Digitization.DoInnerDetectorNoise)
    RD53SimTool = CompFactory.RD53SimTool
    return RD53SimTool(name, **kwargs)


def BarrelFEI4SimToolCfg(flags, name="BarrelFEI4SimTool", **kwargs):
    """Return a FEI4SimTool configured for Barrel"""
    kwargs.setdefault("BarrelEC", 0)
    kwargs.setdefault("DoNoise", flags.Digitization.DoInnerDetectorNoise)
    FEI4SimTool = CompFactory.FEI4SimTool
    return FEI4SimTool(name, **kwargs)


def DBMFEI4SimToolCfg(flags, name="DBMFEI4SimTool", **kwargs):
    """Return a FEI4SimTool configured for Endcap"""
    kwargs.setdefault("BarrelEC", 4)
    kwargs.setdefault("DoNoise", flags.Digitization.DoInnerDetectorNoise)
    FEI4SimTool = CompFactory.FEI4SimTool
    return FEI4SimTool(name, **kwargs)


def BarrelFEI3SimToolCfg(flags, name="BarrelFEI3SimTool", **kwargs):
    """Return a FEI3SimTool configured for Barrel"""
    kwargs.setdefault("BarrelEC", 0)
    FEI3SimTool = CompFactory.FEI3SimTool
    return FEI3SimTool(name, **kwargs)


def EndcapFEI3SimToolCfg(flags, name="EndcapFEI3SimTool", **kwargs):
    """Return a FEI3SimTool configured for Endcap"""
    kwargs.setdefault("BarrelEC", 2)
    FEI3SimTool = CompFactory.FEI3SimTool
    return FEI3SimTool(name, **kwargs)


def PixelDigitizationBasicToolCfg(flags, name="PixelDigitizationBasicTool", **kwargs):
    """Return ComponentAccumulator with configured PixelDigitizationTool"""
    acc = PixelGeometryCfg(flags)

    # module parameters
    acc.merge(PixelConfigCondAlgCfg(flags,
                                    UseCalibConditions=True,
                                    UseDeadmapConditions=True,
                                    UseDCSStateConditions=False,
                                    UseDCSStatusConditions=False,
                                    UseDCSHVConditions=True,
                                    UseDCSTemperatureConditions=True,
                                    UseTDAQConditions=False))
    # charge calibration
    acc.merge(PixelChargeCalibCondAlgCfg(flags))
# NEW FOR RUN3    acc.merge(PixelChargeLUTCalibCondAlgCfg(flags))
    # DCS setup
    acc.merge(PixelDCSCondHVAlgCfg(flags))
    acc.merge(PixelDCSCondTempAlgCfg(flags))
    # cabling setup
    acc.merge(PixelHitDiscCnfgAlgCfg(flags))
    acc.merge(PixelReadoutSpeedAlgCfg(flags))
    acc.merge(PixelCablingCondAlgCfg(flags))
    # deadmap
    acc.merge(PixelDCSCondStateAlgCfg(flags))
    acc.merge(PixelDCSCondStatusAlgCfg(flags))
# NEW FOR RUN3    acc.merge(PixelDeadMapCondAlgCfg(flags))
    # offline calibration
    acc.merge(PixelDistortionAlgCfg(flags))
    acc.merge(PixelOfflineCalibCondAlgCfg(flags))

    acc.popToolsAndMerge(PixelConditionsSummaryCfg(flags))
    acc.popToolsAndMerge(PixelSiPropertiesCfg(flags))
    acc.popToolsAndMerge(PixelLorentzAngleCfg(flags))
    acc.merge(PixelCablingSvcCfg(flags))

    # set up tool handle lists
    chargeTools = []
    feSimTools = []
    chargeTools.append(acc.popToolsAndMerge(SensorSimPlanarToolCfg(flags)))
    if flags.GeoModel.Run == "RUN4":
        feSimTools.append(BarrelRD53SimToolCfg(flags))
        feSimTools.append(EndcapRD53SimToolCfg(flags))
    else:
        chargeTools.append(acc.popToolsAndMerge(SensorSim3DToolCfg(flags)))
        feSimTools.append(BarrelFEI4SimToolCfg(flags))
        feSimTools.append(DBMFEI4SimToolCfg(flags))
        feSimTools.append(BarrelFEI3SimToolCfg(flags))
        feSimTools.append(EndcapFEI3SimToolCfg(flags))
    kwargs.setdefault("InputObjectName", "PixelHits")
    kwargs.setdefault("ChargeTools", chargeTools)
    kwargs.setdefault("FrontEndSimTools", feSimTools)
    kwargs.setdefault("EnergyDepositionTool", EnergyDepositionToolCfg(flags))
    if flags.Digitization.DoXingByXingPileUp:
        kwargs.setdefault("FirstXing", Pixel_FirstXing(flags))
        kwargs.setdefault("LastXing", Pixel_LastXing(flags))

    PixelDigitizationTool = CompFactory.PixelDigitizationTool
    acc.setPrivateTools(PixelDigitizationTool(name, **kwargs))
    return acc


def PixelDigitizationToolCfg(flags, name="PixelDigitizationTool", **kwargs):
    """Return ComponentAccumulator with configured BasicPixelDigitizationTool"""
    acc = ComponentAccumulator()
    rangetool = acc.popToolsAndMerge(PixelRangeCfg(flags))
    acc.merge(PileUpMergeSvcCfg(flags, Intervals=rangetool))
    kwargs.setdefault("HardScatterSplittingMode", 0)
    tool = acc.popToolsAndMerge(PixelDigitizationBasicToolCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


def PixelGeantinoTruthDigitizationToolCfg(flags, name="PixelGeantinoTruthDigitizationTool", **kwargs):
    """Return configured PixelDigitizationTool"""
    acc = ComponentAccumulator()
    rangetool = acc.popToolsAndMerge(PixelRangeCfg(flags))
    acc.merge(PileUpMergeSvcCfg(flags, Intervals=rangetool))
    kwargs.setdefault("ParticleBarcodeVeto", 0)
    PixelDigitizationTool = CompFactory.PixelDigitizationTool
    acc.setPrivateTools(PixelDigitizationTool(name, **kwargs))
    return acc


def PixelDigitizationHSToolCfg(flags, name="PixelDigitizationHSTool", **kwargs):
    """Return ComponentAccumulator with PixelDigitizationTool configured for Hard Scatter"""
    acc = ComponentAccumulator()
    rangetool = acc.popToolsAndMerge(PixelRangeCfg(flags))
    acc.merge(PileUpMergeSvcCfg(flags, Intervals=rangetool))
    kwargs.setdefault("HardScatterSplittingMode", 1)
    tool = acc.popToolsAndMerge(PixelDigitizationBasicToolCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


def PixelDigitizationPUToolCfg(flags, name="PixelDigitizationPUTool", **kwargs):
    """Return ComponentAccumulator with PixelDigitizationTool configured for PileUp"""
    acc = ComponentAccumulator()
    rangetool = acc.popToolsAndMerge(PixelRangeCfg(flags))
    acc.merge(PileUpMergeSvcCfg(flags, Intervals=rangetool))
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
    acc.merge(PileUpMergeSvcCfg(flags, Intervals=rangetool))
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


def PixelOverlayDigitizationCfg(flags, **kwargs):
    """Return ComponentAccumulator with Pixel Overlay digitization and Output"""
    acc = PixelOverlayDigitizationBasicCfg(flags, **kwargs)
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
