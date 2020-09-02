# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

# this is a bit cumbersome, but it seems ike it is a lot easier to separate
# the getter functionality from all the rest (i.e. adding the action).
# This way, e.g., after the getter is called the tool is automatically added
# to the ToolSvc and can be assigned to a ToolHandle by the add function.
# Also, passing arguments to the getter (like "this is a system action") is not straightforward

def AthenaStackingActionToolCfg(ConfigFlags, name='G4UA::AthenaStackingActionTool', **kwargs):

    result = ComponentAccumulator()
    ## Killing neutrinos
    if "ATLAS" in ConfigFlags.Sim.Layout:
        kwargs.setdefault('KillAllNeutrinos',  True)
    ## Neutron Russian Roulette
    #need to check if it exists?
    #if ConfigFlags.hasFlag('Sim.NRRThreshold') and ConfigFlags.hasFlag('Sim.NRRWeight'):
    if ConfigFlags.Sim.NRRThreshold and ConfigFlags.Sim.NRRWeight:
        if ConfigFlags.Sim.CalibrationRun != 'Off':
            raise NotImplementedError("Neutron Russian Roulette should not be used in Calibration Runs.")
        kwargs.setdefault('NRRThreshold',  ConfigFlags.Sim.NRRThreshold)
        kwargs.setdefault('NRRWeight',  ConfigFlags.Sim.NRRWeight)
    ## Photon Russian Roulette
    if ConfigFlags.Sim.PRRThreshold and ConfigFlags.Sim.PRRWeight:
        if ConfigFlags.Sim.CalibrationRun != 'Off':
            raise NotImplementedError("Photon Russian Roulette should not be used in Calibration Runs.")
        kwargs.setdefault('PRRThreshold',  ConfigFlags.Sim.PRRThreshold)
        kwargs.setdefault('PRRWeight',  ConfigFlags.Sim.PRRWeight)
    kwargs.setdefault('IsISFJob', ConfigFlags.Sim.ISFRun)

    result.setPrivateTools( CompFactory.G4UA.AthenaStackingActionTool(name,**kwargs) )
    return result


def AthenaTrackingActionToolCfg(ConfigFlags, name='G4UA::AthenaTrackingActionTool', **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault('SecondarySavingLevel', 2)
    
    subDetLevel=1
    if "ATLAS" in ConfigFlags.Sim.Layout and \
    (ConfigFlags.Beam.Type == 'cosmics' or ConfigFlags.Sim.CavernBG != 'Signal' ):
        subDetLevel=2

    kwargs.setdefault('SubDetVolumeLevel', subDetLevel)
    result.setPrivateTools( CompFactory.G4UA.AthenaTrackingActionTool(name,**kwargs) )
    return result

def LooperKillerToolCfg(ConfigFlags, name='G4UA::LooperKillerTool', **kwargs):
    result = ComponentAccumulator()
    result.setPrivateTools(CompFactory.G4UA.LooperKillerTool(name, **kwargs))
    return result

def G4SimTimerToolCfg(ConfigFlags, name='G4UA::G4SimTimerTool', **kwargs):
    result = ComponentAccumulator()
    result.setPrivateTools(CompFactory.G4UA.G4SimTimerTool(name,**kwargs))
    return result


def G4TrackCounterToolCfg(ConfigFlags, name='G4UA::G4TrackCounterTool', **kwargs):
    result = ComponentAccumulator()
    result.setPrivateTools(CompFactory.G4UA.G4TrackCounterTool(name,**kwargs))
    return result


def StoppedParticleActionToolCfg(ConfigFlags, name="G4UA::StoppedParticleActionTool", **kwargs):
    # Just have to set the stopping condition
    result = ComponentAccumulator()
    # FIXME UserActionConfig not yet migrated
    # example custom configuration 
    # if name in simFlags.UserActionConfig.get_Value().keys():
    #     for prop,value in simFlags.UserActionConfig.get_Value()[name].iteritems():
    #         kwargs.setdefault(prop,value)
    result.setPrivateTools(CompFactory.G4UA.StoppedParticleActionTool(name, **kwargs))
    return result


def HitWrapperToolCfg(ConfigFlags, name="G4UA::HitWrapperTool", **kwargs):
    result = ComponentAccumulator()
    # FIXME UserActionConfig not yet migrated
    # example custom configuration
    # from G4AtlasApps.SimFlags import simFlags
    # if name in simFlags.UserActionConfig.get_Value().keys():
    #     for prop,value in simFlags.UserActionConfig.get_Value()[name].iteritems():
    #         kwargs.setdefault(prop,value)
    result.setPrivateTools(CompFactory.G4UA.HitWrapperTool(name, **kwargs))
    return result
