# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr,Logging

# this is a bit cumbersome, but it seems ike it is a lot easier to separate
# the getter functionality from all the rest (i.e. adding the action).
# This way, e.g., after the getter is called the tool is automatically added
# to the ToolSvc and can be assigned to a ToolHandle by the add function.
# Also, passing arguments to the getter (like "this is a system action") is not straightforward

def getHitWrapperTool(name="G4UA::HitWrapperTool", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    # example custom configuration
    if name in simFlags.UserActionConfig.get_Value().keys():
        for prop,value in simFlags.UserActionConfig.get_Value()[name].iteritems():
            kwargs.setdefault(prop,value)
    return CfgMgr.G4UA__HitWrapperTool(name, **kwargs)


def getFastIDKillerTool(name="G4UA::FastIDKillerTool", **kwargs):
    return CfgMgr.G4UA__FastIDKillerTool(name, **kwargs)


def getFastMBKillerTool(name="G4UA::FastMBKillerTool", **kwargs):
    kwargs.setdefault('Z',3600)
    kwargs.setdefault('R',14)
    return getFastIDKillerTool(name, **kwargs)


def getHIPKillerTool(name="G4UA::HIPKillerTool", **kwargs):
    return CfgMgr.G4UA__HIPKillerTool(name, **kwargs)


def getHIPLArVolumeAcceptTool(name="G4UA::HIPLArVolumeAcceptTool", **kwargs):
    return CfgMgr.G4UA__HIPLArVolumeAcceptTool(name, **kwargs)


def getLooperKillerTool(name="G4UA::LooperKillerTool", **kwargs):
    return CfgMgr.G4UA__LooperKillerTool(name, **kwargs)


def getLooperKillerEventOverlayTool(name="G4UA::LooperKillerEventOverlayTool", **kwargs):
    kwargs.setdefault("MaxSteps",1000000)
    kwargs.setdefault("PrintSteps",10)
    kwargs.setdefault("VerboseLevel",1)
    kwargs.setdefault("AbortEvent",False)
    kwargs.setdefault("SetError",True)
    return getLooperKillerTool(name, **kwargs)


def getMonopoleLooperKillerTool(name="G4UA::MonopoleLooperKillerTool", **kwargs):
    kwargs.setdefault("MaxSteps",2000000)
    kwargs.setdefault("PrintSteps",2)
    kwargs.setdefault("VerboseLevel",0)
    return getLooperKillerTool(name, **kwargs)


def getMomentumConservationTool(name="G4UA::MomentumConservationTool", **kwargs):
    return CfgMgr.G4UA__MomentumConservationTool(name, **kwargs)


def getScoringVolumeTrackKillerTool(name="G4UA::ScoringVolumeTrackKillerTool", **kwargs):
    return CfgMgr.G4UA__ScoringVolumeTrackKillerTool(name, **kwargs)


def getScoringPlaneTool(name="G4UA::ScoringPlaneTool", **kwargs):
    from AthenaCommon.ConcurrencyFlags import jobproperties as concurrencyProps
    if concurrencyProps.ConcurrencyFlags.NumThreads() >1:
        log=Logging.logging.getLogger(name)
        log.fatal(' Attempt to run '+name+' with more than one thread, which is not supported')
        #from AthenaCommon.AppMgr import theApp
        #theApp.exit(1)
        return False
    from G4AtlasApps.SimFlags import simFlags
    # example custom configuration
    if name in simFlags.UserActionConfig.get_Value().keys():
        for prop,value in simFlags.UserActionConfig.get_Value()[name].iteritems():
            kwargs.setdefault(prop,value)
    return CfgMgr.G4UA__ScoringPlaneTool(name, **kwargs)


def getFluxRecorderTool(name="G4UA::FluxRecorderTool", **kwargs):
    from AthenaCommon.ConcurrencyFlags import jobproperties as concurrencyProps
    if concurrencyProps.ConcurrencyFlags.NumThreads() >1:
        log=Logging.logging.getLogger(name)
        log.fatal(' Attempt to run '+name+' with more than one thread, which is not supported')
        #from AthenaCommon.AppMgr import theApp
        #theApp.exit(1)
        return False
    return CfgMgr.G4UA__FluxRecorderTool(name, **kwargs)

def getRadiationMapsMakerTool(name="G4UA::RadiationMapsMakerTool", **kwargs):
    return CfgMgr.G4UA__RadiationMapsMakerTool(name, **kwargs)

def getStoppedParticleActionTool(name="G4UA::StoppedParticleActionTool", **kwargs):
    # Just have to set the stopping condition
    from G4AtlasApps.SimFlags import simFlags
    # example custom configuration
    if name in simFlags.UserActionConfig.get_Value().keys():
        for prop,value in simFlags.UserActionConfig.get_Value()[name].iteritems():
            kwargs.setdefault(prop,value)
    return CfgMgr.G4UA__StoppedParticleActionTool(name, **kwargs)

def getRadLengthActionTool(name="G4UA::RadLengthActionTool", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    # example custom configuration
    if name in simFlags.UserActionConfig.get_Value().keys():
        for prop,value in simFlags.UserActionConfig.get_Value()[name].iteritems():
            kwargs.setdefault(prop,value)
    return CfgMgr.G4UA__RadLengthActionTool(name, **kwargs)

def getLooperThresholdSetTool(name="G4UA::LooperThresholdSetTool", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    if name in simFlags.UserActionConfig.get_Value().keys():
        for prop,value in simFlags.UserActionConfig.get_Value()[name].iteritems():
            kwargs.setdefault(prop,value)
    return CfgMgr.G4UA__LooperThresholdSetTool(name, **kwargs)
