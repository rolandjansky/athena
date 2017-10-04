# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr,Logging
from G4AtlasServices import G4AtlasServicesConfig

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


def addFastIDKillerTool(name="G4UA::FastIDKillerTool",system=False):
    G4AtlasServicesConfig.addAction(name,['Run','Step'],system)


def getHIPKillerTool(name="G4UA::HIPKillerTool", **kwargs):
    return CfgMgr.G4UA__HIPKillerTool(name, **kwargs)


def addHIPKillerTool(name="G4UA::HIPKillerTool",system=False):
    G4AtlasServicesConfig.addAction(name,['Step'],system)


def getHIPLArVolumeAcceptTool(name="G4UA::HIPLArVolumeAcceptTool", **kwargs):
    return CfgMgr.G4UA__HIPLArVolumeAcceptTool(name, **kwargs)


def addHIPLArVolumeAcceptTool(name="G4UA::HIPLArVolumeAcceptTool", system=False):
    G4AtlasServicesConfig.addAction(theTool,['Step','Event'],system)


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


def addLooperKillerTool(name="G4UA::LooperKillerTool", system=False):
    G4AtlasServicesConfig.addAction(name,['Step'],system)


def getMomentumConservationTool(name="G4UA::MomentumConservationTool", **kwargs):
    return CfgMgr.G4UA__MomentumConservationTool(name, **kwargs)


def addMomentumConservationTool(name="G4UA::MomentumConservationTool", system=False):
    G4AtlasServicesConfig.addAction(name,['Step','Event'],system)


def getScoringVolumeTrackKillerTool(name="G4UA::ScoringVolumeTrackKillerTool", **kwargs):
    return CfgMgr.G4UA__ScoringVolumeTrackKillerTool(name, **kwargs)


def addScoringVolumeTrackKillerTool(name="G4UA::ScoringVolumeTrackKillerTool", system=False):
    G4AtlasServicesConfig.addAction(theTool,['Step','Event'],system)


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
