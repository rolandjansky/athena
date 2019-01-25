"""Define methods to construct configured BCM Digitization tools and algs

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaCommon import CfgMgr
from RngComps.RandomServices import RNG, AthEngines
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

# The earliest and last bunch crossing times for which interactions will be sent
# to the BCM Digitization code.
def BCM_FirstXing():
    return -25

def BCM_LastXing():
    return 0

def BCM_DigitizationToolCfg(configFlags, name="BCM_DigitizationTool", **kwargs):
    """Return tuple (ComponentAccumulator, tool) configured for BCM Digitization"""
    acc = ComponentAccumulator()
    # Configure the random engine
    Engine = configFlags.Random.Engine
    acc.merge(RNG(Engine))
    # Build the argument dict
    kwargs.setdefault("RndmSvc", "AthRNGSvc")
    kwargs.setdefault("HitCollName", "BCMHits")
    if configFlags.Digitization.DoInnerDetectorNoise:
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
    if configFlags.Digitization.DoXingByXingPileUp:
        kwargs.setdefault("FirstXing", BCM_FirstXing())
        kwargs.setdefault("LastXing",  BCM_LastXing())
    # Make the tool
    tool = CfgMgr.BCM_DigitizationTool(name, **kwargs)
    return (acc, tool)

def BCM_DigitizationCfg(configFlags, name="BCM_OverlayDigitization", **kwargs):
    """Return a ComponentAccumulator with BCM Digitization algorithm"""
    acc, tool = BCM_DigitizationToolCfg(configFlags, **kwargs)
    kwargs.setdefault("DigitizationTool", tool)
    alg = CfgMgr.BCM_Digitization(name, **kwargs)
    acc.addEventAlgo(alg)
    return acc

def BCM_OverlayDigitizationToolCfg(configFlags, name="BCM_OverlayDigitizationTool", **kwargs):
    """Return tuple (ComponentAccumulator, tool) configured for BCM Overlay Digitization"""
    kwargs.setdefault("EvtStore", configFlags.Overlay.EventStore)
    return BCM_DigitizationToolCfg(configFlags, name, **kwargs)

def BCM_OverlayDigitization(configFlags, name="BCM_OverlayDigitization", **kwargs):
    """Return a ComponentAccumulator with BCM Overlay Digitization algorithm"""
    acc, tool = BCM_OverlayDigitizationToolCfg(configFlags, **kwargs)
    kwargs.setdefault("DigitizationTool", tool)
    alg = CfgMgr.BCM_Digitization(name, **kwargs)
    acc.addEventAlgo(alg)
    return acc

def getBCM_Range(name="BCM_Range" , **kwargs):
    """Return a configured PileUpXingFolder tool"""
    # This is the time of the xing in ns 
    kwargs.setdefault("FirstXing", BCM_FirstXing())
    kwargs.setdefault("LastXing",  BCM_LastXing())
    # Default 0 no dataproxy reset
    kwargs.setdefault("CacheRefreshFrequency", 1.0)
    kwargs.setdefault("ItemList", ["SiHitCollection#BCMHits"])
    return CfgMgr.PileUpXingFolder(name, **kwargs)

