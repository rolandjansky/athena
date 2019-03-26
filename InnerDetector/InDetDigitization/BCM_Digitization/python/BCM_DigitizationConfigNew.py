"""Define methods to construct configured BCM Digitization tools and algs

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from RngComps.RandomServices import RNG, AthEngines
from PileUpComps.PileUpCompsConf import PileUpXingFolder
from BCM_Digitization.BCM_DigitizationConf import BCM_DigitizationTool, BCM_Digitization

# The earliest and last bunch crossing times for which interactions will be sent
# to the BCM Digitization code.
def BCM_FirstXing():
    return -25

def BCM_LastXing():
    return 0

def BCM_RangeCfg(flags, name="BCM_Range" , **kwargs):
    """Return a BCM configured PileUpXingFolder tool"""
    kwargs.setdefault("FirstXing", BCM_FirstXing())
    kwargs.setdefault("LastXing",  BCM_LastXing())
    # Default 0 no dataproxy reset
    kwargs.setdefault("CacheRefreshFrequency", 1.0)
    kwargs.setdefault("ItemList", ["SiHitCollection#BCMHits"])
    return PileUpXingFolder(name, **kwargs)

def BCM_DigitizationToolCfg(flags, name="BCM_DigitizationTool", **kwargs):
    """Return a ComponentAccumulator with configured BCM_DigitizationTool"""
    # take initial ComponentAccumulator from RNG
    acc = RNG(flags.Random.Engine)
    kwargs.setdefault("RndmSvc", "AthRNGSvc")
    kwargs.setdefault("HitCollName", "BCMHits")
    kwargs.setdefault("OutputRDOKey", "BCM_RDOs")
    kwargs.setdefault("OutputSDOKey", "BCM_SDO_Map")
    if flags.Digitization.DoInnerDetectorNoise:
        kwargs.setdefault("ModNoise", [90.82] * 8)
    else:
        kwargs.setdefault("ModNoise", [0.0] * 8)
    kwargs.setdefault("ModSignal", [450.0] * 8)
    kwargs.setdefault("NinoThr", [330.0] * 8)
    # BCM with diamond
    kwargs.setdefault("MIPDeposit", 0.33)
    # Alternative BCM with graphite
    # kwargs.setdefault("MIPDeposit", 0.25)
    kwargs.setdefault("EffDistanceParam", 4.0)
    kwargs.setdefault("EffSharpnessParam", 0.11)
    kwargs.setdefault("TimeDelay", 9.0)
    if flags.Digitization.DoXingByXingPileUp:
        kwargs.setdefault("FirstXing", BCM_FirstXing())
        kwargs.setdefault("LastXing",  BCM_LastXing())
    acc.setPrivateTools(BCM_DigitizationTool(name, **kwargs))
    return acc

def BCM_DigitizationCfg(flags, name="BCM_OverlayDigitization", **kwargs):
    """Return a ComponentAccumulator with configured BCM_Digitization algorithm"""
    acc = BCM_DigitizationToolCfg(flags, **kwargs)
    kwargs.setdefault("DigitizationTool", acc.popPrivateTools())
    acc.addEventAlgo(BCM_Digitization(name, **kwargs))
    return acc

def BCM_OverlayDigitizationToolCfg(flags, name="BCM_OverlayDigitizationTool", **kwargs):
    """Return ComponentAccumulator with BCM_DigitizationTool configured for Overlay"""
    kwargs.setdefault("EvtStore", flags.Overlay.Legacy.EventStore)
    return BCM_DigitizationToolCfg(flags, name, **kwargs)

def BCM_OverlayDigitizationCfg(flags, name="BCM_OverlayDigitization", **kwargs):
    """Return a ComponentAccumulator with BCM_Digitization algorithm configured for Overlay"""
    acc = BCM_OverlayDigitizationToolCfg(flags, **kwargs)
    kwargs.setdefault("DigitizationTool", acc.popPrivateTools())
    acc.addEventAlgo(BCM_Digitization(name, **kwargs))
    return acc

