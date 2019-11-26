# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from G4UserActions.G4UserActionsConf import G4UA__AthenaStackingActionTool, G4UA__AthenaTrackingActionTool

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
        if ConfigFlags.Sim.CalibrationRun:
            raise NotImplementedError("Neutron Russian Roulette should not be used in Calibration Runs.")
        kwargs.setdefault('NRRThreshold',  ConfigFlags.Sim.NRRThreshold)
        kwargs.setdefault('NRRWeight',  ConfigFlags.Sim.NRRWeight)
    ## Photon Russian Roulette
    if ConfigFlags.Sim.PRRThreshold and ConfigFlags.Sim.PRRWeight:
        if ConfigFlags.Sim.CalibrationRun:
            raise NotImplementedError("Photon Russian Roulette should not be used in Calibration Runs.")
        kwargs.setdefault('PRRThreshold',  ConfigFlags.Sim.PRRThreshold)
        kwargs.setdefault('PRRWeight',  ConfigFlags.Sim.PRRWeight)
    kwargs.setdefault('IsISFJob', ConfigFlags.Sim.ISF.Run)

    result.setPrivateTools( G4UA__AthenaStackingActionTool(name,**kwargs) )
    return result


def AthenaTrackingActionToolCfg(ConfigFlags, name='G4UA::AthenaTrackingActionTool', **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault('SecondarySavingLevel', 2)
    
    subDetLevel=1
    if "ATLAS" in ConfigFlags.Sim.Layout and \
    (ConfigFlags.Beam.Type == 'cosmics' or ConfigFlags.Sim.CavernBG != 'Signal' ):
        subDetLevel=2

    kwargs.setdefault('SubDetVolumeLevel', subDetLevel)
    result.setPrivateTools( G4UA__AthenaTrackingActionTool(name,**kwargs) )
    return result
