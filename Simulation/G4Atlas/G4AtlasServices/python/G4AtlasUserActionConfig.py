# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

# Common methods to return default UserAction(Tool)s

def flag_on(name):
    from G4AtlasApps.SimFlags import simFlags
    return hasattr(simFlags, name) and getattr(simFlags, name).statusOn

def flag_off(name):
    from G4AtlasApps.SimFlags import simFlags
    return hasattr(simFlags, name) and not getattr(simFlags, name).statusOn

# New function for all user action types
def getDefaultActions():
    from G4AtlasApps.SimFlags import simFlags
    from AthenaCommon.BeamFlags import beamFlags

    actions = []

    # System stacking action
    actions += ['G4UA::AthenaStackingActionTool']

    # Some truth handling actions (and timing)
    if not simFlags.ISFRun:
        actions += ['G4UA::AthenaTrackingActionTool',
                    'G4UA::MCTruthSteppingActionTool',
                    'G4UA::G4SimTimerTool']
    # Track counter
    actions += ['G4UA::G4TrackCounterTool']
    # Cosmic Perigee action
    if beamFlags.beamType() == 'cosmics' and flag_off('CavernBG'):
        actions += ['G4UA::CosmicPerigeeActionTool']
    # Cosmic filter
    if beamFlags.beamType() == 'cosmics' and not simFlags.ISFRun:
        actions += ['G4UA::G4CosmicFilterTool']
    if flag_on('StoppedParticleFile'):
        actions += ['G4UA::StoppedParticleFilterTool',
                    'G4UA::StoppedParticleActionTool']
    # Hit wrapper action
    if flag_on('CavernBG') and simFlags.CavernBG.get_Value() == 'Read':
        actions += ['G4UA::HitWrapperTool']
    # Photon killer
    if simFlags.PhysicsList == 'QGSP_BERT_HP':
        actions += ['G4UA::PhotonKillerTool']
    # Calo calibration default processing
    if simFlags.CalibrationRun == 'LAr+Tile':
        actions+=['G4UA::CaloG4::CalibrationDefaultProcessingTool']

    return actions

def getUserActionSvc(name="G4UA::UserActionSvc", **kwargs):
    """
    Get the standard UA svc configurable with all default actions added.
    This function is normally called by the configured factory, not users.
    """

    from G4AtlasApps.SimFlags import simFlags

    optActions = simFlags.OptionalUserActionList.get_Value()
    # new user action tools
    kwargs.setdefault('UserActionTools',
                      getDefaultActions() + optActions['General'])

    # placeholder for more advanced config, if needed
    return CfgMgr.G4UA__UserActionSvc(name, **kwargs)

def getCTBUserActionSvc(name="G4UA::CTBUserActionSvc", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    optActions = simFlags.OptionalUserActionList.get_Value()
    generalActions = getDefaultActions() + optActions['General']

    # FIXME: ADS these actions are not yet migrated to Hive
    #if simFlags.SimLayout.get_Value()=='tb_LArH6_2004':
    #    eoe=['LArHitsH6EventAction']+eoe
    #    eoe+=['LArGeoH62004EventAction']
    #    if simFlags.LArTB_H6Step.statusOn:
    #        if simFlags.LArTB_H6Step.get_Value():
    #            stepping+=['LArGeoH62004SteppingAction']
    #            event+=['RadLenNtuple']
    #            eoe+=['RadLenNtuple']
    #            stepping+=['RadLenNtuple']

    # New user action tools
    kwargs.setdefault('UserActionTools', generalActions)

    # placeholder for more advanced config, if needed
    return CfgMgr.G4UA__UserActionSvc(name, **kwargs)

def getISFUserActionSvc(name="G4UA::ISFUserActionSvc", **kwargs):
    TrackProcessorUserAction = kwargs.pop('TrackProcessorUserAction',[])
    PhysicsValidationUserAction = []
    from ISF_Config.ISF_jobProperties import ISF_Flags
    if ISF_Flags.ValidationMode.get_Value():
        PhysicsValidationUserAction = ['ISFG4PhysicsValidationUserActionTool']
    MCTruthUserAction = kwargs.pop('MCTruthUserAction',['ISFMCTruthUserActionTool'])

    from G4AtlasApps.SimFlags import simFlags
    optActions = simFlags.OptionalUserActionList.get_Value()
    generalActions = (
        TrackProcessorUserAction + MCTruthUserAction +
        getDefaultActions() + optActions['General'] +
        PhysicsValidationUserAction
    )

    # New user action tools
    kwargs.setdefault('UserActionTools', generalActions)

    return CfgMgr.G4UA__UserActionSvc(name, **kwargs)

def getISFFullUserActionSvc(name="G4UA::ISFFullUserActionSvc", **kwargs):
    # this configuration needs ISFMCTruthUserAction
    # and FullG4TrackProcessorUserAction
    kwargs.setdefault('TrackProcessorUserAction', ['FullG4TrackProcessorUserActionTool'])
    return getISFUserActionSvc(name, **kwargs)

def getISFPassBackUserActionSvc(name="G4UA::ISFPassBackUserActionSvc", **kwargs):
    # this configuration needs ISFMCTruthUserAction and
    # PassBackG4TrackProcessorUserAction
    kwargs.setdefault('TrackProcessorUserAction', ['PassBackG4TrackProcessorUserActionTool'])
    return getISFUserActionSvc(name, **kwargs)

def getISF_AFIIUserActionSvc(name="G4UA::ISF_AFIIUserActionSvc", **kwargs):
    # this configuration needs ISFMCTruthUserAction and
    # AFII_G4TrackProcessorUserAction
    kwargs.setdefault('TrackProcessorUserAction', ['AFII_G4TrackProcessorUserActionTool'])
    return getISFUserActionSvc(name, **kwargs)
