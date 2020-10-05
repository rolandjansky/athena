"""ComponentAccumulator confguration for pileup digitization

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from RngComps.RandomServices import dSFMT
from Digitization import PileUpEventType
from Digitization.RunDependentConfigNew import (
    maxNevtsPerXing,
    LumiProfileSvcCfg, NoProfileSvcCfg,
)


def PileUpConfigdSFMT(name):
    """Local wrapper for dSFMT RNG service"""
    return dSFMT(name + " OFFSET 340 123 345")


def StepArrayBMCfg(flags, name="StepArrayBM", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("IntensityPattern", flags.Digitization.PU.BeamIntensityPattern)
    kwargs.setdefault("SignalPattern", flags.Digitization.PU.SignalPatternForSteppingCache)
    acc.addService(CompFactory.StepArrayBM(name, **kwargs))
    return acc


def FixedArrayBMCfg(flags, name="FixedArrayBM", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("IntensityPattern", flags.Digitization.PU.BeamIntensityPattern)
    kwargs.setdefault("T0Offset", flags.Digitization.PU.FixedT0BunchCrossing)
    acc.addService(CompFactory.FixedArrayBM(name, **kwargs))
    return acc


def ArrayBMCfg(flags, name="ArrayBM", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("IntensityPattern", flags.Digitization.PU.BeamIntensityPattern)
    acc.merge(PileUpConfigdSFMT("PileUpCollXingStream"))
    kwargs.setdefault("RandomSvc", acc.getService("AtDSFMTGenSvc"))
    acc.addService(CompFactory.ArrayBM(name, **kwargs))
    return acc


def GenericBackgroundEventSelectorCfg(flags, name="GenericBackgroundEventSelector", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("KeepInputFilesOpen", True)
    kwargs.setdefault("ProcessMetadata", False)
    acc.addService(CompFactory.EventSelectorAthenaPool(name, **kwargs))
    return acc


def LowPtMinBiasEventSelectorCfg(flags, name="LowPtMinBiasEventSelector", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("InputCollections", flags.Digitization.PU.LowPtMinBiasInputCols)
    acc.merge(GenericBackgroundEventSelectorCfg(flags, name, **kwargs))
    return acc


def HighPtMinBiasEventSelectorCfg(flags, name="HighPtMinBiasEventSelector", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("InputCollections", flags.Digitization.PU.HighPtMinBiasInputCols)
    acc.merge(GenericBackgroundEventSelectorCfg(flags, name, **kwargs))
    return acc


def CavernEventSelectorCfg(flags, name="cavernEventSelector", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("InputCollections", flags.Digitization.PU.CavernInputCols)
    acc.merge(GenericBackgroundEventSelectorCfg(flags, name, **kwargs))
    return acc


def BeamGasEventSelectorCfg(flags, name="BeamGasEventSelector", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("InputCollections", flags.Digitization.PU.BeamGasInputCols)
    acc.merge(GenericBackgroundEventSelectorCfg(flags, name, **kwargs))
    return acc


def BeamHaloEventSelectorCfg(flags, name="BeamHaloEventSelector", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("InputCollections", flags.Digitization.PU.BeamHaloInputCols)
    acc.merge(GenericBackgroundEventSelectorCfg(flags, name, **kwargs))
    return acc



def MinBiasCacheCfg(flags, name="MinBiasCache", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("CollPerXing", flags.Digitization.PU.NumberOfLowPtMinBias + flags.Digitization.PU.NumberOfHighPtMinBias)
    kwargs.setdefault("FractionOfCache1Collisions", (flags.Digitization.PU.NumberOfLowPtMinBias/
                                                     (flags.Digitization.PU.NumberOfLowPtMinBias + flags.Digitization.PU.NumberOfHighPtMinBias)))
    # may need to have a separate type in the future
    kwargs.setdefault("PileUpEventType", PileUpEventType.MinimumBias)
    acc.merge(LowPtMinBiasEventSelectorCfg(flags))
    acc.merge(HighPtMinBiasEventSelectorCfg(flags))
    if flags.Digitization.DoXingByXingPileUp or flags.Digitization.PU.SignalPatternForSteppingCache:
        kwargs.setdefault("Cache1ReadDownscaleFactor", 1)
    kwargs.setdefault("Cache1EventSelector", acc.getService("LowPtMinBiasEventSelector"))
    kwargs.setdefault("Cache2ReadDownscaleFactor", 1)
    kwargs.setdefault("Cache2EventSelector", acc.getService("HighPtMinBiasEventSelector"))

    kwargs.setdefault("OccupationFraction", (float(flags.Digitization.PU.BunchSpacing)/
                                             float(flags.Beam.BunchSpacing)))
    
    RndmStreamName = "PileUpCollXingStream"
    acc.merge(PileUpConfigdSFMT(RndmStreamName))
    kwargs.setdefault("RndmGenSvc", acc.getService("AtDSFMTGenSvc"))
    kwargs.setdefault("RndmStreamName", RndmStreamName)

    # FIXME migrated, but SplitBkgStreamsCache does not exist
    acc.setPrivateTools(CompFactory.SplitBkgStreamsCache(name, **kwargs))
    return acc


def LowPtMinBiasCacheCfg(flags, name="LowPtMinBiasCache", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("CollPerXing", flags.Digitization.PU.NumberOfLowPtMinBias)
    # may need to have a separate type in the future
    kwargs.setdefault("PileUpEventType", PileUpEventType.MinimumBias)
    if flags.Digitization.DoXingByXingPileUp or flags.Digitization.PU.SignalPatternForSteppingCache:
        kwargs.setdefault("ReadDownscaleFactor", 1)
    acc.merge(LowPtMinBiasEventSelectorCfg(flags))
    kwargs.setdefault("EventSelector", acc.getService("LowPtMinBiasEventSelector"))

    kwargs.setdefault("OccupationFraction", (float(flags.Digitization.PU.BunchSpacing)/
                                             float(flags.Beam.BunchSpacing)))
    
    RndmStreamName = "PileUpCollXingStream"
    acc.merge(PileUpConfigdSFMT(RndmStreamName))
    kwargs.setdefault("RndmGenSvc", acc.getService("AtDSFMTGenSvc"))
    kwargs.setdefault("RndmStreamName", RndmStreamName)

    # Use BkgStreamsStepCaches when using the StepArrayBM and BkgStreamsCache otherwise
    if flags.Digitization.PU.SignalPatternForSteppingCache:
        # FIXME migrated, but BkgStreamsStepCache does not exist
        tool = CompFactory.BkgStreamsStepCache(name, **kwargs)
    else:
        tool = CompFactory.BkgStreamsCache(name, **kwargs)

    acc.setPrivateTools(tool)
    return acc


def HighPtMinBiasCacheCfg(flags, name="HighPtMinBiasCache", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("CollPerXing", flags.Digitization.PU.NumberOfHighPtMinBias)
    # may need to have a separate type in the future
    kwargs.setdefault("PileUpEventType", PileUpEventType.HighPtMinimumBias)
    kwargs.setdefault("ReadDownscaleFactor", 1)
    acc.merge(HighPtMinBiasEventSelectorCfg(flags))
    kwargs.setdefault("EventSelector", acc.getService("HighPtMinBiasEventSelector"))
    kwargs.setdefault("OccupationFraction", (float(flags.Digitization.PU.BunchSpacing)/
                                             float(flags.Beam.BunchSpacing)))
    
    RndmStreamName = "PileUpCollXingStream"
    acc.merge(PileUpConfigdSFMT(RndmStreamName))
    kwargs.setdefault("RndmGenSvc", acc.getService("AtDSFMTGenSvc"))
    kwargs.setdefault("RndmStreamName", RndmStreamName)

    # Use BkgStreamsStepCaches when using the StepArrayBM and BkgStreamsCache otherwise
    if flags.Digitization.PU.SignalPatternForSteppingCache:
        # FIXME migrated, but BkgStreamsStepCache does not exist
        tool = CompFactory.BkgStreamsStepCache(name, **kwargs)
    else:
        tool = CompFactory.BkgStreamsCache(name, **kwargs)

    acc.setPrivateTools(tool)
    return acc


def CavernCacheCfg(flags, name="CavernCache", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("CollPerXing", flags.Digitization.PU.NumberOfCavern)
    kwargs.setdefault("CollDistribution", "Fixed")
    kwargs.setdefault("PileUpEventType", PileUpEventType.Cavern)
    if flags.Digitization.PU.DoXingByXingPileUp or flags.Digitization.PU.SignalPatternForSteppingCache:
        kwargs.setdefault("ReadDownscaleFactor", 1)
    # Cavern Background Cache Should Ignore Bunch Structure
    OccupationFraction = (float(flags.Digitization.PU.BunchSpacing)/
                          float(flags.Beam.BunchSpacing))
    if flags.Digitization.PU.BeamIntensityPattern:
        kwargs.setdefault("IgnoreBeamInt", flags.Digitization.PU.CavernIgnoresBeamInt)
        if flags.Digitization.PU.CavernIgnoresBeamInt:
            OccupationFraction = 1.0
    kwargs.setdefault("OccupationFraction", OccupationFraction)
    acc.merge(CavernEventSelectorCfg(flags))
    kwargs.setdefault("EventSelector", acc.getService("cavernEventSelector"))
    
    RndmStreamName = "PileUpCollXingStream"
    acc.merge(PileUpConfigdSFMT(RndmStreamName))
    kwargs.setdefault("RndmGenSvc", acc.getService("AtDSFMTGenSvc"))
    kwargs.setdefault("RndmStreamName", RndmStreamName)

    # Use BkgStreamsStepCaches when using the StepArrayBM and BkgStreamsCache otherwise
    if flags.Digitization.PU.SignalPatternForSteppingCache:
        # FIXME migrated, but BkgStreamsStepCache does not exist
        tool = CompFactory.BkgStreamsStepCache(name, **kwargs)
    else:
        tool = CompFactory.BkgStreamsCache(name, **kwargs)
    acc.setPrivateTools(tool)
    return acc


def BeamGasCacheCfg(flags, name="BeamGasCache", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("IgnoreBeamLumi", True)
    kwargs.setdefault("CollPerXing", flags.Digitization.PU.NumberOfBeamGas)
    kwargs.setdefault("PileUpEventType", PileUpEventType.HaloGas)
    kwargs.setdefault("CollDistribution", "Poisson")
    kwargs.setdefault("ReadDownscaleFactor", 1)

    acc.merge(BeamGasEventSelectorCfg(flags))
    kwargs.setdefault("EventSelector", acc.getService("BeamGasEventSelector"))

    RndmStreamName = "PileUpCollXingStream"
    acc.merge(PileUpConfigdSFMT(RndmStreamName))
    kwargs.setdefault("RndmGenSvc", acc.getService("AtDSFMTGenSvc"))
    kwargs.setdefault("RndmStreamName", RndmStreamName)

    # Use BkgStreamsStepCaches when using the StepArrayBM and BkgStreamsCache otherwise
    if flags.Digitization.PU.SignalPatternForSteppingCache:
        # FIXME migrated, but BkgStreamsStepCache does not exist
        tool = CompFactory.BkgStreamsStepCache(name, **kwargs)
    else:
        tool = CompFactory.BkgStreamsCache(name, **kwargs)

    acc.setPrivateTools(tool)
    return acc


def BeamHaloCacheCfg(flags, name="BeamHaloCache", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("IgnoreBeamLumi", True)
    kwargs.setdefault("CollPerXing", flags.Digitization.PU.NumberOfBeamHalo)
    kwargs.setdefault("PileUpEventType", PileUpEventType.HaloGas)
    kwargs.setdefault("CollDistribution", "Poisson")
    kwargs.setdefault("ReadDownscaleFactor",  1)

    acc.merge(BeamHaloEventSelectorCfg(flags))
    kwargs.setdefault("EventSelector", acc.getService("BeamHaloEventSelector"))

    RndmStreamName = "PileUpCollXingStream"
    acc.merge(PileUpConfigdSFMT(RndmStreamName))
    kwargs.setdefault("RndmGenSvc", acc.getService("AtDSFMTGenSvc"))
    kwargs.setdefault("RndmStreamName", RndmStreamName)

    #Use BkgStreamsStepCaches when using the StepArrayBM and BkgStreamsCache otherwise
    if flags.Digitization.PU.SignalPatternForSteppingCache:
        # FIXME migrated, but BkgStreamsStepCache does not exist
        tool = CompFactory.BkgStreamsStepCache(name, **kwargs)
    else:
        tool = CompFactory.BkgStreamsCache(name, **kwargs)

    acc.setPrivateTools(tool)
    return acc


def PileUpEventLoopMgrCfg(flags, name="PileUpEventLoopMgr", **kwargs):
    acc = ComponentAccumulator()
    # SubDet By SubDet (default) or Xing By Xing Pile-up?
    kwargs.setdefault("XingByXing", flags.Digitization.DoXingByXingPileUp)
    # Bunch Structure
    if flags.Digitization.PU.BeamIntensityPattern:
        if flags.Digitization.PU.SignalPatternForSteppingCache:
            # Simulate Bunch Structure with events sliding backwards on a conveyor belt
            acc.merge(StepArrayBMCfg(flags))
            kwargs.setdefault("BeamInt", acc.getService("StepArrayBM"))
        elif flags.Digitization.PU.FixedT0BunchCrossing:
            # Simulate Bunch Structure using a fixed point for the central bunch crossing
            acc.merge(FixedArrayBMCfg(flags))
            kwargs.setdefault("BeamInt", acc.getService("FixedArrayBM"))
        else:
            # Simulate Bunch Structure and allow the central bunch crossing to vary
            acc.merge(ArrayBMCfg(flags))
            kwargs.setdefault("BeamInt", acc.getService("ArrayBM"))

    # define inputs
    assert not flags.Input.SecondaryFiles, ("Found ConfigFlags.Input.SecondaryFiles = %r; "
                                            "double event selection is not supported "
                                            "by PileUpEventLoopMgr" % (not flags.Input.SecondaryFiles))
    acc.merge(PoolReadCfg(flags))
    kwargs.setdefault("OrigSelector", acc.getService("EventSelector"))
    BackgroundCaches = []
    # Note: experimentalDigi not migrated
    if flags.Digitization.PU.LowPtMinBiasInputCols:
        BackgroundCaches += [acc.popToolsAndMerge(LowPtMinBiasCacheCfg(flags))]
    if flags.Digitization.PU.HighPtMinBiasInputCols:
        BackgroundCaches += [acc.popToolsAndMerge(HighPtMinBiasCacheCfg(flags))]
    if flags.Digitization.PU.CavernInputCols:
        BackgroundCaches += [acc.popToolsAndMerge(CavernCacheCfg(flags))]
    if flags.Digitization.PU.BeamGasInputCols:
        BackgroundCaches += [acc.popToolsAndMerge(BeamGasCacheCfg(flags))]
    if flags.Digitization.PU.BeamHaloInputCols:
        BackgroundCaches += [acc.popToolsAndMerge(BeamHaloCacheCfg(flags))]
    kwargs.setdefault("bkgCaches", BackgroundCaches)
    # string output stream
    kwargs.setdefault("OutStreamType", "AthenaOutputStream")
    # xing frequency in ns
    kwargs.setdefault("XingFrequency", flags.Digitization.PU.BunchSpacing)
    # define time range to be studied. t0 at t" ,0, xing" ,0
    kwargs.setdefault("firstXing", flags.Digitization.PU.InitialBunchCrossing)
    kwargs.setdefault("lastXing", flags.Digitization.PU.FinalBunchCrossing)

    if flags.Digitization.PU.RunAndLumiOverrideList:
        kwargs.setdefault("MaxMinBiasCollPerXing", maxNevtsPerXing(flags))
        acc.merge(LumiProfileSvcCfg(flags))
        kwargs.setdefault("BeamLuminosity", acc.getService("LumiProfileSvc"))
    else:
        kwargs.setdefault("MaxMinBiasCollPerXing", flags.Digitization.PU.NumberOfCollisions)
        acc.merge(NoProfileSvcCfg(flags))
        kwargs.setdefault("BeamLuminosity", acc.getService("NoProfileSvc"))

    # for beam spot fix
    kwargs.setdefault("EventInfoName", "Input_EventInfo")

    acc.addService(CompFactory.PileUpEventLoopMgr(name, **kwargs))
    return acc
