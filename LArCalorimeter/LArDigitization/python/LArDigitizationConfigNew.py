"""Define functions for LAr Digitization with ComponentAccumulator

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
# utilities
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
# for PileUpTool
from LArGeoAlgsNV.LArGMConfig import LArGMCfg
from LArRecUtils.LArADC2MeVCondAlgConfig import LArADC2MeVCondAlgCfg
from LArRecUtils.LArRecUtilsConfig import LArAutoCorrNoiseCondAlgCfg
from LArBadChannelTool.LArBadChannelConfig import LArBadChannelMaskerCfg, LArBadFebCfg
from LArConfiguration.LArElecCalibDBConfig import LArElecCalibDbCfg
from Digitization.PileUpToolsConfig import PileUpToolsCfg
# for Digitization
from LArROD.LArRawChannelBuilderAlgConfig import LArRawChannelBuilderAlgCfg
from LArROD.LArDigitThinnerConfig import LArDigitThinnerCfg
from Digitization.TruthDigitizationOutputConfig import TruthDigitizationOutputCfg
# for Trigger Tower
from LArCabling.LArCablingConfig import LArFebRodMappingCfg, LArCalibIdMappingCfg
from CaloConditions.CaloConditionsConfig import CaloTriggerTowerCfg

def useLArFloat(flags):
    """Return bool for simplified transient LArHit with float E,time"""
    # temporary, remapping to LArHitFloat does not seeem to work
    # with this scheme... => larger memory usage
    if flags.Digitization.DoXingByXingPileUp or flags.Detector.OverlayLAr:
        return False
    # check for fast chain, running digitisation from hits in memory
    if flags.Sim.DoFullChain:
        return False
    return True


def LArRangeEMCfg(flags, name="LArRangeEM", **kwargs):
    """Return a PileUpXingFoldertool for ElectroMagnetic calorimeter"""
    # bunch crossing range in ns
    kwargs.setdefault("FirstXing", -751)
    kwargs.setdefault("LastXing", 101)
    if useLArFloat(flags):
        kwargs.setdefault("ItemList", ["LArHitFloatContainer#LArHitEMB",
                                       "LArHitFloatContainer#LArHitEMEC"])
    else:
        kwargs.setdefault("ItemList", ["LArHitContainer#LArHitEMB",
                                       "LArHitContainer#LArHitEMEC"])
    PileUpXingFolder = CompFactory.PileUpXingFolder
    return PileUpXingFolder(name, **kwargs)


def LArRangeHECCfg(flags, name="LArRangeHEC", **kwargs):
    """Return a PileUpXingFoldertool for Hadronic End-Cap"""
    # bunch crossing range in ns
    kwargs.setdefault("FirstXing", -676)
    kwargs.setdefault("LastXing", 101)
    if useLArFloat(flags):
        kwargs.setdefault("ItemList", ["LArHitFloatContainer#LArHitHEC"])
    else:
        kwargs.setdefault("ItemList", ["LArHitContainer#LArHitHEC"])
    PileUpXingFolder = CompFactory.PileUpXingFolder
    return PileUpXingFolder(name, **kwargs)


def LArRangeFCALCfg(flags, name="LArRangeFCAL", **kwargs):
    """Return a PileUpXingFoldertool for Forward Calorimeters"""
    # bunch crossing range in ns
    kwargs.setdefault("FirstXing", -551)
    kwargs.setdefault("LastXing", 101)
    if useLArFloat(flags):
        kwargs.setdefault("ItemList", ["LArHitFloatContainer#LArHitFCAL"])
    else:
        kwargs.setdefault("ItemList", ["LArHitContainer#LArHitFCAL"])
    PileUpXingFolder = CompFactory.PileUpXingFolder
    return PileUpXingFolder(name, **kwargs)


def LArPileUpToolCfg(flags, name="LArPileUpTool", **kwargs):
    """Return ComponentAccumulator with configured LArPileUpTool"""
    acc = LArGMCfg(flags)

    #The LArPileupTool needs: Noise, fSampl, Pedestal,Shape ADC2MeV
    # AutoCorrNoise, the list of bad FEBs and the cabling
    acc.merge(LArADC2MeVCondAlgCfg(flags))
    acc.merge(LArBadFebCfg(flags))
    if flags.Overlay.DataOverlay:
        kwargs.setdefault("ShapeKey", "LArShape")
    if not flags.Digitization.DoCaloNoise:
        requiredConditons=["fSampl", "Pedestal", "Shape"]
    else:
        requiredConditons=["Noise", "fSampl", "Pedestal", "Shape"]
    acc.merge(LArElecCalibDbCfg(flags,requiredConditons))

    if not flags.Detector.OverlayLAr:
        acc.merge(LArAutoCorrNoiseCondAlgCfg(flags))
    if "MaskingTool" not in kwargs:
        maskerTool = acc.popToolsAndMerge(LArBadChannelMaskerCfg(flags, ["deadReadout", "deadPhys"], ToolName="LArRCBMasker"))
        kwargs["MaskingTool"] = maskerTool
    # defaults
    kwargs.setdefault("NoiseOnOff", flags.Digitization.DoCaloNoise)
    kwargs.setdefault("DoDigiTruthReconstruction", flags.Digitization.DoDigiTruth)
    if flags.Digitization.DoXingByXingPileUp:
        kwargs.setdefault("FirstXing", -751)
        kwargs.setdefault("LastXing", 101)
    if (not flags.Digitization.HighGainFCal) and (not flags.Detector.OverlayLAr):
        kwargs.setdefault("HighGainThreshFCAL", 0)
    if (not flags.Digitization.HighGainEMECIW) and (not flags.Detector.OverlayLAr):
        kwargs.setdefault("HighGainThreshEMECIW", 0)
    kwargs.setdefault("RndmEvtOverlay", flags.Detector.OverlayLAr)
    if flags.Digitization.PileUpPremixing:
        kwargs.setdefault("DigitContainer", flags.Overlay.BkgPrefix + "LArDigitContainer_MC")
    else:
        kwargs.setdefault("DigitContainer", "LArDigitContainer_MC") # FIXME - should not be hard-coded
    # if doing MC+MC overlay
    if flags.Input.isMC and flags.Detector.OverlayLAr:
          kwargs.setdefault("isMcOverlay", True)
    kwargs.setdefault("Nsamples", flags.LAr.ROD.nSamples)
    kwargs.setdefault("firstSample", flags.LAr.ROD.FirstSample)
    # cosmics digitization
    if flags.Beam.Type == "cosmics":
        kwargs.setdefault("UseTriggerTime", True)
        CosmicTriggerTimeTool = CompFactory.CosmicTriggerTimeTool
        kwargs.setdefault("TriggerTimeToolName", CosmicTriggerTimeTool())
    # pileup configuration "algorithm" way
    if not flags.Digitization.DoXingByXingPileUp:
        if flags.Digitization.Pileup or flags.Detector.OverlayLAr:
            kwargs.setdefault("PileUp", True)
    kwargs.setdefault("useLArFloat", useLArFloat(flags))
    if useLArFloat(flags):
        maps = [
            "LArHitContainer#LArHitEMB->LArHitFloatContainer#LArHitEMB",
            "LArHitContainer#LArHitEMEC->LArHitFloatContainer#LArHitEMEC",
            "LArHitContainer#LArHitHEC->LArHitFloatContainer#LArHitHEC",
            "LArHitContainer#LArHitFCAL->LArHitFloatContainer#LArHitFCAL"
        ]
        AddressRemappingSvc, ProxyProviderSvc = CompFactory.getComps("AddressRemappingSvc", "ProxyProviderSvc",)
        acc.addService(AddressRemappingSvc(TypeKeyOverwriteMaps=maps, ProxyDict="ActiveStoreSvc"))
        acc.addService(ProxyProviderSvc(ProviderNames=["AddressRemappingSvc"]))
        kwargs.setdefault("LArHitContainers", [])
    else:
        kwargs.setdefault("LArHitFloatContainers", [])
    if flags.Detector.OverlayLAr:
        kwargs.setdefault("OnlyUseContainerName", False)
        if flags.Overlay.DataOverlay:
            kwargs.setdefault("InputDigitContainer", flags.Overlay.BkgPrefix + "FREE")
        else:
            kwargs.setdefault("InputDigitContainer", flags.Overlay.BkgPrefix + "LArDigitContainer_MC")
    LArPileUpTool = CompFactory.LArPileUpTool
    acc.setPrivateTools(LArPileUpTool(name, **kwargs))
    return acc


def LArOutputCfg(flags):
    """Return ComponentAccumulator with Output for LAr. Not standalone."""
    acc = ComponentAccumulator()
    if flags.Output.doWriteRDO:
        ItemList = []
        if flags.Digitization.AddCaloDigi:
            ItemList.append("LArDigitContainer#*")
        elif flags.Digitization.PileUpPremixing:
            ItemList.append("LArDigitContainer#" + flags.Overlay.BkgPrefix + "LArDigitContainer_MC")
        else:
            ItemList.append("LArDigitContainer#LArDigitContainer_MC_Thinned")
        if not flags.Digitization.PileUpPremixing:
            ItemList.append("LArRawChannelContainer#LArRawChannels")
        if flags.Detector.SimulateHGTD:
            ItemList.append("LArHitContainer#HGTDDigitContainer_MC")
        if flags.Digitization.TruthOutput:
            ItemList.append("CaloCalibrationHitContainer#*")
            acc.merge(TruthDigitizationOutputCfg(flags))
        acc.merge(OutputStreamCfg(flags, "RDO", ItemList))
    return acc


def LArDigitizationBasicCfg(flags, **kwargs):
    """Return ComponentAccumulator with LAr digitization configuration"""
    acc = ComponentAccumulator()
    if "PileUpTools" not in kwargs:
        PileUpTools = acc.popToolsAndMerge(LArPileUpToolCfg(flags))
        kwargs["PileUpTools"] = PileUpTools
    acc.merge(PileUpToolsCfg(flags, **kwargs))
    acc.merge(LArRawChannelBuilderAlgCfg(flags))
    acc.merge(LArDigitThinnerCfg(flags))
    return acc


def LArDigitizationCfg(flags, **kwargs):
    """Return ComponentAccumulator with LAr digitization and Output"""
    acc = LArDigitizationBasicCfg(flags, **kwargs)
    acc.merge(LArOutputCfg(flags))
    return acc


def LArOverlayDigitizationBasicCfg(flags, name="digitmaker1", **kwargs):
    """Return ComponentAccumulator with LAr Overlay digitization configuration"""
    acc = ComponentAccumulator()

    if flags.Overlay.DataOverlay:
        from LArByteStream.LArRawDataReadingConfig import LArRawDataReadingCfg
        acc.merge(LArRawDataReadingCfg(flags))

    if "LArPileUpTool" not in kwargs:
        kwargs["LArPileUpTool"] = acc.popToolsAndMerge(LArPileUpToolCfg(flags))

    if flags.Concurrency.NumThreads > 0:
        kwargs.setdefault('Cardinality', flags.Concurrency.NumThreads)

    LArDigitMaker = CompFactory.LArDigitMaker
    acc.addEventAlgo(LArDigitMaker(name, **kwargs))

    acc.merge(LArRawChannelBuilderAlgCfg(flags))
    acc.merge(LArDigitThinnerCfg(flags))
    return acc


def LArOverlayCfg(flags, **kwargs):
    """Return ComponentAccumulator with LAr overlay and Output"""
    acc = LArOverlayDigitizationBasicCfg(flags, **kwargs)
    acc.merge(LArOutputCfg(flags))
    return acc


def LArTriggerDigitizationBasicCfg(flags, **kwargs):
    """Return ComponentAccumulator for LAr Trigger Tower"""
    acc = LArDigitizationBasicCfg(flags)
    acc.merge(LArFebRodMappingCfg(flags))
    acc.merge(LArCalibIdMappingCfg(flags))
    acc.merge(CaloTriggerTowerCfg(flags))

    kwargs.setdefault("NoiseOnOff", flags.Digitization.DoCaloNoise)
    if not flags.Digitization.DoXingByXingPileUp:
        if flags.Digitization.Pileup:
            kwargs.setdefault("PileUp", True)
    if flags.Digitization.PileUpPremixing:
        kwargs.setdefault("EmTTL1ContainerName", flags.Overlay.BkgPrefix + "LArTTL1EM")
        kwargs.setdefault("HadTTL1ContainerName", flags.Overlay.BkgPrefix + "LArTTL1HAD")
    LArTTL1Maker = CompFactory.LArTTL1Maker
    acc.addEventAlgo(LArTTL1Maker(**kwargs))
    return acc


def LArTriggerDigitizationCfg(flags, **kwargs):
    """Return ComponentAccumulator for LAr Trigger Tower and Output"""
    acc = LArTriggerDigitizationBasicCfg(flags)
    acc.merge(LArOutputCfg(flags))
    acc.merge(OutputStreamCfg(flags, "RDO", ["LArTTL1Container#*"]))
    return acc


def LArOverlayTriggerDigitizationBasicCfg(flags, **kwargs):
    """Return ComponentAccumulator with LAr Overlay Trigger Tower"""
    acc = LArOverlayDigitizationBasicCfg(flags)
    acc.merge(LArFebRodMappingCfg(flags))
    acc.merge(LArCalibIdMappingCfg(flags))
    acc.merge(CaloTriggerTowerCfg(flags))

    kwargs.setdefault("NoiseOnOff", flags.Digitization.DoCaloNoise)
    kwargs.setdefault("PileUp", True)
    kwargs.setdefault("EmTTL1ContainerName", flags.Overlay.SigPrefix + "LArTTL1EM")
    kwargs.setdefault("HadTTL1ContainerName", flags.Overlay.SigPrefix + "LArTTL1HAD")

    LArTTL1Maker = CompFactory.LArTTL1Maker
    acc.addEventAlgo(LArTTL1Maker(**kwargs))
    return acc
