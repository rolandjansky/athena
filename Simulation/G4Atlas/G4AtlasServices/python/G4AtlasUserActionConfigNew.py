# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from MCTruthBase.MCTruthBaseConfigNew import MCTruthSteppingActionToolCfg

from G4AtlasServices.G4AtlasServicesConf import G4UA__UserActionSvc

from G4UserActions.G4UserActionsConfigNew import AthenaStackingActionToolCfg#, AthenaTrackingActionToolCfg


# New function for all user action types
def getDefaultActions(ConfigFlags):
    result = ComponentAccumulator()

    actions = []

    # System stacking action
    #actions += ['G4UA::AthenaStackingActionTool']
    actions += [result.popToolsAndMerge( AthenaStackingActionToolCfg(ConfigFlags)  )]

    # Some truth handling actions (and timing)
    if not ConfigFlags.Sim.ISFRun:
        actions += [
                    #result.popToolsAndMerge( AthenaTrackingActionToolCfg(ConfigFlags) ),
                    result.popToolsAndMerge( MCTruthSteppingActionToolCfg(ConfigFlags) )
                    #'G4UA::G4SimTimerTool']
                    ]
    # Track counter
    #actions += ['G4UA::G4TrackCounterTool']

    # Cosmic Perigee action
    if ConfigFlags.Beam.Type == 'cosmics' and ConfigFlags.Sim.CavernBG:
        actions += ['G4UA::CosmicPerigeeActionTool']
    # Cosmic filter
    if ConfigFlags.Beam.Type == 'cosmics' and not ConfigFlags.Sim.ISFRun:
        actions += ['G4UA::G4CosmicFilterTool']
    if ConfigFlags.Sim.StoppedParticleFile:
        actions += ['G4UA::StoppedParticleFilterTool',
                    'G4UA::StoppedParticleActionTool']
    # Hit wrapper action
    if ConfigFlags.Sim.CavernBG == 'Read':
        actions += ['G4UA::HitWrapperTool']
    # Photon killer
    if ConfigFlags.Sim.PhysicsList == 'QGSP_BERT_HP':
        actions += ['G4UA::PhotonKillerTool']
    # Calo calibration default processing
    if ConfigFlags.Sim.CalibrationRun == 'LAr+Tile':
        actions+=['G4UA::CaloG4::CalibrationDefaultProcessingTool']

    return actions

def UserActionSvcCfg(ConfigFlags, name="G4UA::UserActionSvc", **kwargs):
    """
    Get the standard UA svc configurable with all default actions added.
    This function is normally called by the configured factory, not users.
    """
    result = ComponentAccumulator()

    #how to convert this flag?
    from G4AtlasApps.SimFlags import simFlags
    optActions = simFlags.OptionalUserActionList.get_Value()
    # new user action tools
    kwargs.setdefault('UserActionTools',
                      getDefaultActions(ConfigFlags) + optActions['General'])

    # placeholder for more advanced config, if needed
    result.addService ( G4UA__UserActionSvc(name, **kwargs) )

    return result
    