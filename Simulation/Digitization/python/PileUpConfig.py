# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
from AthenaCommon.CfgGetter import getService, getPrivateTool

def getStepArrayBM(name="StepArrayBM", **kwargs):
    from Digitization.DigitizationFlags import digitizationFlags
    kwargs.setdefault('IntensityPattern', digitizationFlags.BeamIntensityPattern.get_Value())
    kwargs.setdefault('SignalPattern', digitizationFlags.SignalPatternForSteppingCache.get_Value())

    from PileUpComps.PileUpCompsConf import StepArrayBM
    return StepArrayBM(name, **kwargs);


def getFixedArrayBM(name="FixedArrayBM", **kwargs):
    from Digitization.DigitizationFlags import digitizationFlags
    kwargs.setdefault('IntensityPattern', digitizationFlags.BeamIntensityPattern.get_Value())
    kwargs.setdefault('T0Offset', digitizationFlags.FixedT0BunchCrossing.get_Value() )

    from PileUpComps.PileUpCompsConf import FixedArrayBM
    return FixedArrayBM(name, **kwargs);


def getArrayBM(name="ArrayBM", **kwargs):
    from Digitization.DigitizationFlags import digitizationFlags
    kwargs.setdefault('IntensityPattern', digitizationFlags.BeamIntensityPattern.get_Value())
    kwargs.setdefault('RandomSvc', digitizationFlags.rndmSvc() )
    digitizationFlags.rndmSeedList.addSeed( "BEAMINT", 678, 91011 )

    from PileUpComps.PileUpCompsConf import ArrayBM
    return ArrayBM(name, **kwargs);

def getGenericBackgroundEventSelector(name="GenericBackgroundEventSelector", **kwargs):
    kwargs.setdefault('KeepInputFilesOpen', True)
    kwargs.setdefault('ProcessMetadata', False)

    from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import EventSelectorAthenaPool
    return EventSelectorAthenaPool(name, **kwargs);

def getLowPtMinBiasEventSelector(name="LowPtMinBiasEventSelector", **kwargs):
    from Digitization.DigitizationFlags import digitizationFlags
    kwargs.setdefault('InputCollections', digitizationFlags.LowPtMinBiasInputCols())

    return getGenericBackgroundEventSelector(name, **kwargs);


def getHighPtMinBiasEventSelector(name="HighPtMinBiasEventSelector", **kwargs):
    from Digitization.DigitizationFlags import digitizationFlags
    kwargs.setdefault('InputCollections', digitizationFlags.HighPtMinBiasInputCols())

    return getGenericBackgroundEventSelector(name, **kwargs);


def getcavernEventSelector(name="cavernEventSelector", **kwargs):
    from Digitization.DigitizationFlags import digitizationFlags
    kwargs.setdefault('InputCollections', digitizationFlags.cavernInputCols())

    return getGenericBackgroundEventSelector(name, **kwargs);


def getBeamGasEventSelector(name="BeamGasEventSelector", **kwargs):
    from Digitization.DigitizationFlags import digitizationFlags
    kwargs.setdefault('InputCollections', digitizationFlags.beamGasInputCols())

    return getGenericBackgroundEventSelector(name, **kwargs);


def getBeamHaloEventSelector(name="BeamHaloEventSelector", **kwargs):
    from Digitization.DigitizationFlags import digitizationFlags
    kwargs.setdefault('InputCollections', digitizationFlags.beamHaloInputCols())

    return getGenericBackgroundEventSelector(name, **kwargs);


def getMinBiasCache(name="MinBiasCache", **kwargs):
    from Digitization.DigitizationFlags import digitizationFlags
    from AthenaCommon.BeamFlags import jobproperties
    kwargs.setdefault('CollPerXing', (digitizationFlags.numberOfLowPtMinBias() + digitizationFlags.numberOfHighPtMinBias()) )
    kwargs.setdefault('FractionOfCache1Collisions', (digitizationFlags.numberOfLowPtMinBias()/(digitizationFlags.numberOfLowPtMinBias() + digitizationFlags.numberOfHighPtMinBias())))
    from Digitization import PileUpEventType
    kwargs.setdefault('PileUpEventType', PileUpEventType.MinimumBias) # may need to have a separate type in the future
    if (digitizationFlags.doXingByXingPileUp() or digitizationFlags.SignalPatternForSteppingCache.statusOn):
        kwargs.setdefault('Cache1ReadDownscaleFactor', 1)
    kwargs.setdefault('Cache1EventSelector', getService('LowPtMinBiasEventSelector') )
    kwargs.setdefault('Cache2ReadDownscaleFactor', 1)
    kwargs.setdefault('Cache2EventSelector', getService('HighPtMinBiasEventSelector') )
    kwargs.setdefault('OccupationFraction', float(digitizationFlags.bunchSpacing.get_Value())/float(jobproperties.Beam.bunchSpacing.get_Value()))

    kwargs.setdefault('RndmGenSvc', digitizationFlags.rndmSvc())
    kwargs.setdefault('RndmStreamName', "PileUpCollXingStream")
    return CfgMgr.SplitBkgStreamsCache(name, **kwargs)


def getLowPtMinBiasCache(name="LowPtMinBiasCache", **kwargs):
    from Digitization.DigitizationFlags import digitizationFlags
    from AthenaCommon.BeamFlags import jobproperties
    kwargs.setdefault('CollPerXing', digitizationFlags.numberOfLowPtMinBias() )
    from Digitization import PileUpEventType
    kwargs.setdefault('PileUpEventType', PileUpEventType.MinimumBias ) # may need to have a separate type in the future
    if (digitizationFlags.doXingByXingPileUp() or digitizationFlags.SignalPatternForSteppingCache.statusOn):
        kwargs.setdefault('ReadDownscaleFactor', 1 )
    kwargs.setdefault('EventSelector', getService('LowPtMinBiasEventSelector') )
    kwargs.setdefault('OccupationFraction', float(digitizationFlags.bunchSpacing.get_Value())/float(jobproperties.Beam.bunchSpacing.get_Value()) )

    kwargs.setdefault('RndmGenSvc', digitizationFlags.rndmSvc() )
    kwargs.setdefault('RndmStreamName', "PileUpCollXingStream" )
    #Use BkgStreamsStepCaches when using the StepArrayBM and BkgStreamsCache otherwise
    if digitizationFlags.SignalPatternForSteppingCache.statusOn:
        return CfgMgr.BkgStreamsStepCache(name, **kwargs)
    else:
        return CfgMgr.BkgStreamsCache(name, **kwargs)


def getHighPtMinBiasCache(name="HighPtMinBiasCache", **kwargs):
    from Digitization.DigitizationFlags import digitizationFlags
    from AthenaCommon.BeamFlags import jobproperties
    kwargs.setdefault('CollPerXing', digitizationFlags.numberOfHighPtMinBias() )
    from Digitization import PileUpEventType
    kwargs.setdefault('PileUpEventType', PileUpEventType.HighPtMinimumBias ) # may need to have a separate type in the future
    kwargs.setdefault('ReadDownscaleFactor', 1 )
    kwargs.setdefault('EventSelector', getService('HighPtMinBiasEventSelector') )
    kwargs.setdefault('OccupationFraction', float(digitizationFlags.bunchSpacing.get_Value())/float(jobproperties.Beam.bunchSpacing.get_Value()) )

    kwargs.setdefault('RndmGenSvc', digitizationFlags.rndmSvc() )
    kwargs.setdefault('RndmStreamName', "PileUpCollXingStream" )
    #Use BkgStreamsStepCaches when using the StepArrayBM and BkgStreamsCache otherwise
    if digitizationFlags.SignalPatternForSteppingCache.statusOn:
        return CfgMgr.BkgStreamsStepCache(name, **kwargs)
    else:
        return CfgMgr.BkgStreamsCache(name, **kwargs)


def getCavernCache(name="CavernCache", **kwargs):
    from Digitization.DigitizationFlags import digitizationFlags
    from AthenaCommon.BeamFlags import jobproperties
    kwargs.setdefault('CollPerXing', digitizationFlags.numberOfCavern() )
    kwargs.setdefault('CollDistribution', "Fixed" )
    from Digitization import PileUpEventType
    kwargs.setdefault('PileUpEventType', PileUpEventType.Cavern )
    if (digitizationFlags.doXingByXingPileUp() or digitizationFlags.SignalPatternForSteppingCache.statusOn):
        kwargs.setdefault('ReadDownscaleFactor', 1 )
    # Cavern Background Cache Should Ignore Bunch Structure
    OccupationFraction =  float(digitizationFlags.bunchSpacing.get_Value())/float(jobproperties.Beam.bunchSpacing.get_Value())
    if digitizationFlags.BeamIntensityPattern.statusOn :
        kwargs.setdefault('IgnoreBeamInt', digitizationFlags.cavernIgnoresBeamInt.get_Value() )
        if digitizationFlags.cavernIgnoresBeamInt.get_Value():
            OccupationFraction = 1.0
    kwargs.setdefault('OccupationFraction', OccupationFraction )
    kwargs.setdefault('EventSelector', getService('cavernEventSelector') )

    kwargs.setdefault('RndmGenSvc', digitizationFlags.rndmSvc() )
    kwargs.setdefault('RndmStreamName', "PileUpCollXingStream" )
    #Use BkgStreamsStepCaches when using the StepArrayBM and BkgStreamsCache otherwise
    if digitizationFlags.SignalPatternForSteppingCache.statusOn:
        return CfgMgr.BkgStreamsStepCache(name, **kwargs)
    else:
        return CfgMgr.BkgStreamsCache(name, **kwargs)


def getBeamGasCache(name="BeamGasCache", **kwargs):
    from Digitization.DigitizationFlags import digitizationFlags
    from AthenaCommon.BeamFlags import jobproperties
    kwargs.setdefault('IgnoreBeamLumi', True )
    kwargs.setdefault('CollPerXing', digitizationFlags.numberOfBeamGas() )
    from Digitization import PileUpEventType
    kwargs.setdefault('PileUpEventType', PileUpEventType.HaloGas )
    kwargs.setdefault('CollDistribution', "Poisson" )
    kwargs.setdefault('ReadDownscaleFactor',  1 )
    kwargs.setdefault('EventSelector', "BeamGasEventSelector" )

    kwargs.setdefault('RndmGenSvc', digitizationFlags.rndmSvc() )
    kwargs.setdefault('RndmStreamName', "PileUpCollXingStream" )
    #Use BkgStreamsStepCaches when using the StepArrayBM and BkgStreamsCache otherwise
    if digitizationFlags.SignalPatternForSteppingCache.statusOn:
        return CfgMgr.BkgStreamsStepCache(name, **kwargs)
    else:
        return CfgMgr.BkgStreamsCache(name, **kwargs)


def getBeamHaloCache(name="BeamHaloCache", **kwargs):
    from Digitization.DigitizationFlags import digitizationFlags
    from AthenaCommon.BeamFlags import jobproperties
    kwargs.setdefault('IgnoreBeamLumi', True )
    kwargs.setdefault('CollPerXing', digitizationFlags.numberOfBeamHalo() )
    from Digitization import PileUpEventType
    kwargs.setdefault('PileUpEventType', PileUpEventType.HaloGas )
    kwargs.setdefault('CollDistribution', "Poisson" )
    kwargs.setdefault('ReadDownscaleFactor',  1 )
    kwargs.setdefault('EventSelector', "BeamHaloEventSelector" )

    kwargs.setdefault('RndmGenSvc', digitizationFlags.rndmSvc() )
    kwargs.setdefault('RndmStreamName', "PileUpCollXingStream" )
    #Use BkgStreamsStepCaches when using the StepArrayBM and BkgStreamsCache otherwise
    if digitizationFlags.SignalPatternForSteppingCache.statusOn:
        return CfgMgr.BkgStreamsStepCache(name, **kwargs)
    else:
        return CfgMgr.BkgStreamsCache(name, **kwargs)

def getPileUpEventLoopMgr(name="PileUpEventLoopMgr", **kwargs):
    from Digitization.DigitizationFlags import digitizationFlags
    #configure random generator
    digitizationFlags.rndmSeedList.addSeed( "PileUpCollXingStream", 123, 345 )
    # SubDet By SubDet (default) or Xing By Xing Pile-up?
    kwargs.setdefault('XingByXing', digitizationFlags.doXingByXingPileUp() )
    # Bunch Structure
    if digitizationFlags.BeamIntensityPattern.statusOn :
        if digitizationFlags.SignalPatternForSteppingCache.statusOn :
            # Simulate Bunch Structure with events sliding backwards on a conveyor belt
            kwargs.setdefault('BeamInt', getService("StepArrayBM") )
        elif digitizationFlags.FixedT0BunchCrossing.statusOn :
            # Simulate Bunch Structure using a fixed point for the central bunch crossing
            kwargs.setdefault('BeamInt', getService("FixedArrayBM") )
        else:
            # Simulate Bunch Structure and allow the central bunch crossing to vary
            kwargs.setdefault('BeamInt', getService("ArrayBM") )

    #define inputs
    kwargs.setdefault('OrigSelector', "EventSelector" )
    BackgroundCaches = []
    if 'SplitCacheForMinbias' in digitizationFlags.experimentalDigi() and digitizationFlags.doLowPtMinBias() and digitizationFlags.doHighPtMinBias():
        BackgroundCaches += [ getPrivateTool("MinBiasCache", checkType=True) ]
    else:
        if digitizationFlags.doLowPtMinBias():
            BackgroundCaches += [ getPrivateTool("LowPtMinBiasCache", checkType=True) ]

        if digitizationFlags.doHighPtMinBias():
            BackgroundCaches += [ getPrivateTool("HighPtMinBiasCache", checkType=True) ]

    if digitizationFlags.doCavern():
        BackgroundCaches += [ getPrivateTool("CavernCache", checkType=True) ]

    if digitizationFlags.doBeamGas():
        BackgroundCaches += [ getPrivateTool("BeamGasCache", checkType=True) ]

    if digitizationFlags.doBeamHalo():
        BackgroundCaches += [ getPrivateTool("BeamHaloCache", checkType=True) ]

    kwargs.setdefault('bkgCaches', BackgroundCaches )
    #define output stream
    kwargs.setdefault('OutStreamType', "AthenaOutputStream" )

    #xing frequency in ns
    kwargs.setdefault('XingFrequency', digitizationFlags.bunchSpacing() )

    #define time range to be studied. t0 at t' ,0, xing' ,0
    kwargs.setdefault('firstXing', digitizationFlags.initialBunchCrossing() )
    kwargs.setdefault('lastXing', digitizationFlags.finalBunchCrossing() )

    from AthenaCommon.AppMgr import ServiceMgr
    if digitizationFlags.RunAndLumiOverrideList.statusOn:
        if not(digitizationFlags.RunAndLumiOverrideList.is_locked()):
            raise RuntimeError( 'You cannot configure the PileUpEventLoopMgr with an unlocked JobProperty.' )
        kwargs.setdefault('MaxMinBiasCollPerXing', digitizationFlags.RunAndLumiOverrideList.getMaxNevtsPerXing())
        lumiProfileSvc = getService("LumiProfileSvc")
        kwargs.setdefault('BeamLuminosity', lumiProfileSvc)
        ServiceMgr += lumiProfileSvc
    else:
        kwargs.setdefault('MaxMinBiasCollPerXing', digitizationFlags.numberOfCollisions())
        noProfileSvc = getService("NoProfileSvc")
        kwargs.setdefault('BeamLuminosity', noProfileSvc)
        ServiceMgr += noProfileSvc

    return CfgMgr.PileUpEventLoopMgr(name, **kwargs)
