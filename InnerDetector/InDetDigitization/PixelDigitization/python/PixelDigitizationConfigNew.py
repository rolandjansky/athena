"""Define methods to construct configured Pixel Digitization tools and algorithms

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from PileUpComps.PileUpCompsConf import PileUpXingFolder
from PixelCabling.PixelCablingConfigNew import PixelCablingSvcCfg
from PixelDigitization.PixelDigitizationConf import (
    PixelDigitizationTool, PixelDigitization, ChargeCollProbSvc,
    EnergyDepositionTool, SensorSimPlanarTool, SensorSim3DTool,
    RD53SimTool, FEI4SimTool, FEI3SimTool,
)
from SiPropertiesTool.PixelSiPropertiesConfig import PixelSiPropertiesCfg
from SiLorentzAngleTool.PixelLorentzAngleConfig import PixelLorentzAngleCfg
from PixelConditionsTools.PixelConditionsSummaryConfig import PixelConditionsSummaryCfg
from PixelConditionsAlgorithms.PixelConditionsConfig import PixelChargeCalibCondAlgCfg, PixelOfflineCalibCondAlgCfg
from PixelGeoModel.PixelGeoModelConfig import PixelGeometryCfg
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg

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

def ChargeCollProbSvcCfg(name="ChargeCollProbSvc", **kwargs):
    """Return a Charge Collection Prob service"""
    return ChargeCollProbSvc(name, **kwargs)

def EnergyDepositionToolCfg(flags, name="EnergyDepositionTool", **kwargs):
    """Return a configured EnergyDepositionTool"""
    kwargs.setdefault("DeltaRayCut", 117.)
    kwargs.setdefault("nCols", 5)
    kwargs.setdefault("LoopLimit", 100000)
    kwargs.setdefault("doBichsel", True)
    kwargs.setdefault("doBichselBetaGammaCut", 0.7) # dEdx not quite consistent below this
    kwargs.setdefault("doDeltaRay", False)          # needs validation
    kwargs.setdefault("doPU", True)
    return EnergyDepositionTool(name, **kwargs)

def SensorSimPlanarToolCfg(flags, name="SensorSimPlanarTool", **kwargs):
    """Return ComponentAccumulator with configured SensorSimPlanarTool"""
    acc = PixelSiPropertiesCfg(flags)
    SiTool = acc.popPrivateTools()
    LorentzTool = acc.popToolsAndMerge(PixelLorentzAngleCfg(flags))
    kwargs.setdefault("SiPropertiesTool", SiTool)
    kwargs.setdefault("LorentzAngleTool", LorentzTool)
    acc.setPrivateTools(SensorSimPlanarTool(name, **kwargs))
    return acc

def SensorSim3DToolCfg(flags, name="SensorSim3DTool", **kwargs):
    """Return ComponentAccumulator with configured SensorSim3DTool"""
    acc = PixelSiPropertiesCfg(flags)
    SiTool = acc.popPrivateTools()
    acc.popToolsAndMerge(PixelLorentzAngleCfg(flags))
    kwargs.setdefault("SiPropertiesTool", SiTool)
    acc.setPrivateTools(SensorSim3DTool(name, **kwargs))
    return acc

def BarrelRD53SimToolCfg(flags, name="BarrelRD53SimTool", **kwargs):
    """Return a RD53SimTool configured for Barrel"""
    kwargs.setdefault("BarrelEC", 0)
    return RD53SimTool(name, **kwargs)

def EndcapRD53SimToolCfg(flags, name="EndcapRD53SimTool", **kwargs):
    """Return a RD53SimTool configured for Endcap"""
    kwargs.setdefault("BarrelEC", 2)
    return RD53SimTool(name, **kwargs)

def BarrelFEI4SimToolCfg(flags, name="BarrelFEI4SimTool", **kwargs):
    """Return a FEI4SimTool configured for Barrel"""    
    kwargs.setdefault("BarrelEC", 0)
    return FEI4SimTool(name, **kwargs)

def DBMFEI4SimToolCfg(flags, name="DBMFEI4SimTool", **kwargs):
    """Return a FEI4SimTool configured for Endcap"""
    kwargs.setdefault("BarrelEC", 4)
    return FEI4SimTool(name, **kwargs)

def BarrelFEI3SimToolCfg(flags, name="BarrelFEI3SimTool", **kwargs):
    """Return a FEI3SimTool configured for Barrel"""
    kwargs.setdefault("BarrelEC", 0)
    return FEI3SimTool(name, **kwargs)

def EndcapFEI3SimToolCfg(flags, name="EndcapFEI3SimTool", **kwargs):
    """Return a FEI3SimTool configured for Endcap"""
    kwargs.setdefault("BarrelEC", 2)
    return FEI3SimTool(name, **kwargs)

def PixelDigitizationBasicToolCfg(flags, name="PixelDigitizationBasicTool", **kwargs):
    """Return ComponentAccumulator with configured PixelDigitizationTool"""
    acc = PixelGeometryCfg(flags)
    acc.popToolsAndMerge(PixelConditionsSummaryCfg(flags))
    acc.merge(PixelChargeCalibCondAlgCfg(flags))
    acc.popToolsAndMerge(PixelSiPropertiesCfg(flags))
    acc.popToolsAndMerge(PixelLorentzAngleCfg(flags))
    acc.merge(PixelCablingSvcCfg(flags))
    acc.merge(PixelOfflineCalibCondAlgCfg(flags))
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
    acc.setPrivateTools(PixelDigitizationTool(name, **kwargs))
    return acc

def PixelDigitizationToolCfg(flags, name="PixelDigitizationTool", **kwargs):
    """Return ComponentAccumulator with configured BasicPixelDigitizationTool"""
    kwargs.setdefault("HardScatterSplittingMode", 0)
    return PixelDigitizationBasicToolCfg(flags, name, **kwargs)

def PixelGeantinoTruthDigitizationToolCfg(flags, name="PixelGeantinoTruthDigitizationTool", **kwargs):
    """Return configured PixelDigitizationTool"""
    kwargs.setdefault("ParticleBarcodeVeto", 0)
    return PixelDigitizationTool(name, **kwargs)

def PixelDigitizationHSToolCfg(flags, name="PixelDigitizationToolHS", **kwargs):
    """Return ComponentAccumulator with PixelDigitizationTool configured for Hard Scatter"""
    kwargs.setdefault("HardScatterSplittingMode", 1)
    return PixelDigitizationBasicToolCfg(flags, name, **kwargs)

def PixelDigitizationPUToolCfg(flags, name="PixelDigitizationToolPU", **kwargs):
    """Return ComponentAccumulator with PixelDigitizationTool configured for PileUp"""
    kwargs.setdefault("HardScatterSplittingMode", 2)
    kwargs.setdefault("RDOCollName", "Pixel_PU_RDOs")
    kwargs.setdefault("SDOCollName", "Pixel_PU_SDO_Map")
    return PixelDigitizationBasicToolCfg(flags, name, **kwargs)

def PixelDigitizationSplitNoMergePUToolCfg(flags, name="PixelDigitizationToolSplitNoMergePU", **kwargs):
    """Return ComponentAccumulator with PixelDigitizationTool configured for PileUpPixelHits"""
    kwargs.setdefault("HardScatterSplittingMode", 0)
    kwargs.setdefault("InputObjectName", "PileupPixelHits")
    kwargs.setdefault("RDOCollName", "Pixel_PU_RDOs")
    kwargs.setdefault("SDOCollName", "Pixel_PU_SDO_Map")
    return PixelDigitizationBasicToolCfg(flags, name, **kwargs)

def PixelDigitizationOverlayToolCfg(flags, name="PixelDigitizationOverlayTool", **kwargs):
    """Return ComponentAccumulator with PixelDigitizationTool configured for overlay"""
    kwargs.setdefault("OnlyUseContainerName", False)
    kwargs.setdefault("RDOCollName", "StoreGateSvc+" + flags.Overlay.SigPrefix + "PixelRDOs")
    kwargs.setdefault("SDOCollName", "StoreGateSvc+" + flags.Overlay.SigPrefix + "PixelSDO_Map")
    kwargs.setdefault("HardScatterSplittingMode", 0)
    return PixelDigitizationBasicToolCfg(flags, name, **kwargs)

def PixelRangeCfg(flags, name="PixelRange", **kwargs):
    """Return a configured PileUpXingFolder tool"""
    kwargs.setdefault("FirstXing", Pixel_FirstXing(flags))
    kwargs.setdefault("LastXing", Pixel_LastXing(flags))
    kwargs.setdefault("CacheRefreshFrequency", 1.0) # default 0 no dataproxy reset
    kwargs.setdefault("ItemList", ["SiHitCollection#PixelHits"])
    return PileUpXingFolder(name, **kwargs)


def PixelDigitizationBasicCfg(toolCfg, flags, name, **kwargs):
    """Return ComponentAccumulator with basic toolCfg configured Pixel digitization"""
    acc = ComponentAccumulator()
    if "DigitizationTool" not in kwargs:
        tool = acc.popToolsAndMerge(toolCfg(flags))
        kwargs["DigitizationTool"] = tool
    acc.addEventAlgo(PixelDigitization(name, **kwargs))
    return acc

def PixelDigitizationOutputCfg(toolCfg, flags, name, **kwargs):
    """Return ComponentAccumulator with toolCfg configured Pixel Digitization algorithm and OutputStream"""
    acc = PixelDigitizationBasicCfg(toolCfg, flags, name, **kwargs)
    acc.merge(OutputStreamCfg(flags, "RDO", ["InDetSimDataCollection#*", "PixelRDO_Container#*"]))
    return acc


def PixelDigitizationCfg(flags, name="PixelDigitization", **kwargs):
    """Return ComponentAccumulator with standard Pixel Digitization and Output"""
    return PixelDigitizationOutputCfg(PixelDigitizationToolCfg, flags, name, **kwargs)

def PixelDigitizationHSCfg(flags, name="PixelDigitizationHS", **kwargs):
    """Return ComponentAccumulator with Hard Scatter-only Pixel Digitization and Output"""
    return PixelDigitizationOutputCfg(PixelDigitizationHSToolCfg, flags, name, **kwargs)

def PixelDigitizationPUCfg(flags, name="PixelDigitizationPU", **kwargs):
    """Return ComponentAccumulator with Pile-up-only Pixel Digitization and Output"""
    return PixelDigitizationOutputCfg(PixelDigitizationPUToolCfg, flags, name, **kwargs)

def PixelDigitizationOverlayCfg(flags, name="PixelDigitizationOverlay", **kwargs):
    """Return ComponentAccumulator with Overlay Pixel Digitization and Output"""
    return PixelDigitizationOutputCfg(PixelDigitizationOverlayToolCfg, flags, name, **kwargs)

