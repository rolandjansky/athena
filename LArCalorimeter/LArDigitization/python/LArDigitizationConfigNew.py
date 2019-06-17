"""Define functions for LAr Digitization with ComponentAccumulator

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from PileUpComps.PileUpCompsConf import PileUpXingFolder
from LArGeoAlgsNV.LArGMConfig import LArGMCfg
from LArBadChannelTool.LArBadChannelConfig import LArBadChannelMaskerCfg
from LArDetDescr.LArDetDescrConfig import LArDetDescrCfg
from LArRecUtils.LArRecUtilsConfig import LArAutoCorrNoiseCondAlgCfg
from LArRecUtils.LArRecUtilsConfig import LArADC2MeVCondAlgCfg
from LArRawConditions.LArRawConditionsConfig import LArRawConditionsMCCfg
from LArDigitization.LArDigitizationConf import LArPileUpTool, LArDigitMaker
from CommissionUtils.CommissionUtilsConf import CosmicTriggerTimeTool
from SGComps.SGCompsConf import AddressRemappingSvc, ProxyProviderSvc
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg

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

def LArItemList():
    """Return list of item names needed for LAr output"""
    ItemList = [
        "LArRawChannelContainer#*",
        "LArDigitContainer#*",
        "LArDigitContainer#*LArDigitContainer_MC",
        "LArDigitContainer#LArDigitContainer_MC_Thinned",
        "LArHitContainer#HGTDDigitContainer_MC",
        "LArTTL1Container#*",
    ]
    return ItemList

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
    return PileUpXingFolder(name, **kwargs)

def LArPileUpToolCfg(flags, name="LArPileUpTool", **kwargs):
    """Return a ComponentAccumulator with configured LArPileUpTool"""
    acc = LArGMCfg(flags)
    # the LAr and Calo detector description package
    if not flags.Detector.OverlayLAr:
        acc.merge(LArDetDescrCfg(flags))
        # This is conditionally merged in LArDetDescrCfg
        acc.merge(LArRawConditionsMCCfg(flags))
    # other merges
    acc.merge(LArADC2MeVCondAlgCfg(flags))
    if not flags.Detector.OverlayLAr:
        acc.merge(LArAutoCorrNoiseCondAlgCfg(flags))
    if "MaskingTool" not in kwargs:
        maskerTool = acc.popToolsAndMerge(LArBadChannelMaskerCfg(flags, ["deadReadout", "deadPhys"]))
        kwargs["MaskingTool"] = maskerTool
    # defaults
    kwargs.setdefault("NoiseOnOff", flags.Digitization.DoCaloNoise)
    kwargs.setdefault("DoDigiTruthReconstruction", flags.Digitization.DoDigiTruth)
    if flags.Digitization.DoXingByXingPileUp:
        kwargs.setdefault("FirstXing", -751)
        kwargs.setdefault("LastXing", 101)
    if flags.Digitization.HighGainFCal and not flags.Detector.OverlayLAr:
        kwargs.setdefault("HighGainThreshFCAL", 0)
    if flags.Digitization.HighGainEMECIW and not flags.Detector.OverlayLAr:
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
    if flags.Detector.OverlayLAr:
        kwargs.setdefault("RandomDigitContainer", "LArDigitContainer_MC")
    # cosmics digitization
    if flags.Beam.Type == "cosmics":
        kwargs.setdefault("UseTriggerTime", True)
        kwargs.setdefault("TriggerTimeToolName", CosmicTriggerTimeTool())
    # pileup configuration "algorithm" way
    if not flags.Digitization.DoXingByXingPileUp :
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
        acc.addService(AddressRemappingSvc(TypeKeyOverwriteMaps=maps, ProxyDict="ActiveStoreSvc"))
        acc.addService(ProxyProviderSvc(ProviderNames=["AddressRemappingSvc"]))
    acc.setPrivateTools(LArPileUpTool(name, **kwargs))
    return acc

def LArDigitMakerCfg(flags, name="LArDigitMaker", **kwargs):
    """Return a ComponentAccumulator with configured LArDigitMaker algorithm"""
    acc = ComponentAccumulator()
    if "LArPileUpTool" not in kwargs:
        tool = acc.popToolsAndMerge(LArPileUpToolCfg(flags))
        kwargs["LArPileUpTool"] = tool
    acc.addEventAlgo(LArDigitMaker(name, **kwargs))
    acc.merge(OutputStreamCfg(flags, "RDO", LArItemList()))
    return acc

