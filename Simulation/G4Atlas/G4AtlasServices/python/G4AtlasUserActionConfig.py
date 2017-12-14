# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgGetter,CfgMgr,Logging

# Common methods to return default UserAction(Tool)s

# actions to be run at begin/end of run
def getDefaultRunActions():
    defaultUA=[]
    return defaultUA

# begin of event
def getDefaultEventActions():
    from G4AtlasApps.SimFlags import simFlags
    defaultUA=[]
    if hasattr(simFlags, 'CalibrationRun') and simFlags.CalibrationRun() == 'LAr+Tile':
        defaultUA+=['G4UA::CaloG4::CalibrationDefaultProcessingTool']
    return defaultUA

# stepping
def getDefaultSteppingActions():
    from G4AtlasApps.SimFlags import simFlags
    defaultUA=[]
    if hasattr(simFlags, 'CalibrationRun') and simFlags.CalibrationRun() == 'LAr+Tile':
        defaultUA+=['G4UA::CaloG4::CalibrationDefaultProcessingTool']
    return defaultUA

# tracking
def getDefaultTrackingActions():
    defaultUA=[]
    return defaultUA

# Stacking Classification
def getDefaultStackingActions():
    defaultUA=[]
    return defaultUA

def flag_on(name):
    from G4AtlasApps.SimFlags import simFlags
    return hasattr(simFlags, name) and getattr(simFlags, name).statusOn

def flag_off(name):
    from G4AtlasApps.SimFlags import simFlags
    return hasattr(simFlags, name) and not getattr(simFlags, name).statusOn

# New function for all user action types
def getDefaultActions():
    from G4AtlasApps.SimFlags import simFlags
    from AthenaCommon.BeamFlags import jobproperties as beamProps

    actions = []

    # System stacking action
    actions += ['G4UA::AthenaStackingActionTool']

    # Some truth handling actions (and timing)
    if not simFlags.ISFRun:
        actions += ['G4UA::AthenaTrackingActionTool',
                    'G4UA::MCTruthSteppingActionTool',
                    'G4UA::G4SimTimerTool',
                    ]
    # Track counter
    actions += ['G4UA::G4TrackCounterTool']
    # Cosmic Perigee action
    if beamProps.Beam.beamType() == 'cosmics' and flag_off('CavernBG'):
        actions += ['G4UA::CosmicPerigeeActionTool']
    # Stopped particle action
    if flag_on('StoppedParticleFile'):
        actions += ['G4UA::StoppedParticleActionTool']
    # Hit wrapper action
    if flag_on('CavernBG') and simFlags.CavernBG.get_Value() == 'Read':
        actions += ['G4UA::HitWrapperTool']
    # Cosmic filter
    if flag_on('StoppedParticleFile') or (
       beamProps.Beam.beamType() == 'cosmics' and not simFlags.ISFRun):
        actions += ['G4UA::G4CosmicFilterTool']
    # Photon killer
    if simFlags.PhysicsList == 'QGSP_BERT_HP':
        actions += ['G4UA::PhotonKillerTool']

    return actions

def getUserActionSvc(name="G4UA::UserActionSvc", **kwargs):
    """
    Get the standard UA svc configurable with all default actions added.
    This function is normally called by the configured factory, not users.
    """

    from G4AtlasApps.SimFlags import simFlags

    optActions = simFlags.OptionalUserActionList.get_Value()
    kwargs.setdefault('RunActionTools',
                      getDefaultRunActions() + optActions['Run'])
    kwargs.setdefault('EventActionTools',
                      getDefaultEventActions() + optActions['Event'])
    kwargs.setdefault('SteppingActionTools',
                      getDefaultSteppingActions() + optActions['Step'])
    kwargs.setdefault('TrackingActionTools',
                      getDefaultTrackingActions() + optActions['Tracking'])
    # no optional actions for stacking
    kwargs.setdefault('StackingActionTools', getDefaultStackingActions())

    # new user action tools
    kwargs.setdefault('UserActionTools',
                      getDefaultActions() + optActions['General'])

    # placeholder for more advanced config, if needed
    return CfgMgr.G4UA__UserActionSvc(name, **kwargs)

def getCTBUserActionSvc(name="G4UA::CTBUserActionSvc", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    optActions = simFlags.OptionalUserActionList.get_Value()
    run = getDefaultRunActions() + optActions['Run']
    event = getDefaultEventActions() + optActions['Event']
    tracking = getDefaultTrackingActions() + optActions['Tracking']
    stepping = getDefaultSteppingActions() + optActions['Step']
    stacking = getDefaultStackingActions()
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

    kwargs.setdefault('RunActionTools', run)
    kwargs.setdefault('EventActionTools', event)
    kwargs.setdefault('SteppingActionTools', stepping)
    kwargs.setdefault('TrackingActionTools', tracking)
    kwargs.setdefault('StackingActionTools', stacking)

    # New user action tools
    kwargs.setdefault('UserActionTools', generalActions)

    # placeholder for more advanced config, if needed
    return CfgMgr.G4UA__UserActionSvc(name, **kwargs)

def getISFUserActionSvc(name="G4UA::ISFUserActionSvc", **kwargs):
    TrackProcessorUserAction = kwargs.pop('TrackProcessorUserAction',[])
    PhysicsValidationUserAction = kwargs.pop('PhysicsValidationUserAction',[])
    MCTruthUserAction = kwargs.pop('MCTruthUserAction',['ISFMCTruthUserActionTool'])

    from G4AtlasApps.SimFlags import simFlags
    optActions = simFlags.OptionalUserActionList.get_Value()
    run = (
        getDefaultRunActions() + optActions['Run'] +
        PhysicsValidationUserAction)
    event = (
        getDefaultEventActions() + optActions['Event'] +
        TrackProcessorUserAction + PhysicsValidationUserAction)
    tracking = (
        TrackProcessorUserAction + MCTruthUserAction +
        getDefaultTrackingActions() + optActions['Tracking'] +
        PhysicsValidationUserAction)
    stepping = (
        getDefaultSteppingActions() + optActions['Step'] +
        TrackProcessorUserAction + PhysicsValidationUserAction)
    stacking = getDefaultStackingActions()
    generalActions = getDefaultActions() + optActions['General']

    kwargs.setdefault('RunActionTools', run)
    kwargs.setdefault('EventActionTools', event)
    kwargs.setdefault('SteppingActionTools', stepping)
    kwargs.setdefault('TrackingActionTools', tracking)
    kwargs.setdefault('StackingActionTools', stacking)

    # New user action tools
    kwargs.setdefault('UserActionTools', generalActions)

    return CfgMgr.G4UA__UserActionSvc(name, **kwargs)

def getISFFullUserActionSvc(name="G4UA::ISFFullUserActionSvc", **kwargs):
    # this configuration needs ISFMCTruthUserAction,
    # G4OnlyPhysicsValidationUserAction, and FullG4TrackProcessorUserAction
    from ISF_Config.ISF_jobProperties import ISF_Flags
    if ISF_Flags.ValidationMode.get_Value():
        kwargs.setdefault('PhysicsValidationUserAction',['G4OnlyPhysicsValidationUserActionTool'])
    kwargs.setdefault('TrackProcessorUserAction', ['FullG4TrackProcessorUserActionTool'])
    return getISFUserActionSvc(name, **kwargs)

def getISFPassBackUserActionSvc(name="G4UA::ISFPassBackUserActionSvc", **kwargs):
    # this configuration needs ISFMCTruthUserAction,
    # G4OnlyPhysicsValidationUserAction, and PassBackG4TrackProcessorUserAction
    from ISF_Config.ISF_jobProperties import ISF_Flags
    if ISF_Flags.ValidationMode.get_Value():
        kwargs.setdefault('PhysicsValidationUserAction',['G4OnlyPhysicsValidationUserActionTool'])
    kwargs.setdefault('TrackProcessorUserAction', ['PassBackG4TrackProcessorUserActionTool'])
    return getISFUserActionSvc(name, **kwargs)

def getISF_AFIIUserActionSvc(name="G4UA::ISF_AFIIUserActionSvc", **kwargs):
    # this configuration needs ISFMCTruthUserAction,
    # AFII_G4PhysicsValidationUserAction, and AFII_G4TrackProcessorUserAction
    from ISF_Config.ISF_jobProperties import ISF_Flags
    if ISF_Flags.ValidationMode.get_Value():
        kwargs.setdefault('PhysicsValidationUserAction',['AFII_G4PhysicsValidationUserActionTool'])
    kwargs.setdefault('TrackProcessorUserAction', ['AFII_G4TrackProcessorUserActionTool'])
    return getISFUserActionSvc(name, **kwargs)

def getISFQuasiStableUserActionSvc(name="G4UA::ISFQuasiStableUserActionSvc", **kwargs):
    # this configuration needs ISFMCTruthUserAction,
    # QuasiStableG4PhysicsValidationUserAction, and FullG4TrackProcessorUserAction
    from ISF_Config.ISF_jobProperties import ISF_Flags
    if ISF_Flags.ValidationMode.get_Value():
        kwargs.setdefault('PhysicsValidationUserAction',['QuasiStableG4PhysicsValidationUserActionTool'])
    return getISFFullUserActionSvc(name, **kwargs)


def addAction(actionTool, roles, systemAction=False):
    """
    Add an action tool to the list for a requested role.

    This should only be used in very special cases, by experts. Normally, you
    should use the getter for one of the specialized UASvc versions listed
    above to add an action. If systemAction=True, the action is prepended to
    the list. Otherwise, it is appended.
    """

    from AthenaCommon.AppMgr import theApp, AthAppMgr, ServiceMgr

    roleMap={'Run': 'RunActionTools',
             'Event': 'EventActionTools',
             'Tracking': 'TrackingActionTools',
             'Step': 'SteppingActionTools',
             'Stack': 'StackingActionTools'}

    ##hack to allow for different named Configurations of the UserActionSvc
    basename='UserActionSvc'
    serviceName=''
    for s in ServiceMgr.getAllChildren():
        if basename in s.getName():
            serviceName = s.getName()
            break
    if ''==serviceName:
        Logging.logging.getLogger('G4UASvc').error('addAction: Could not find ' + basename + ' instance in ServiceMgr!')
        raise AttributeError('addAction: Could not find ' + basename + ' instance in ServiceMgr!')
    else:
        Logging.logging.getLogger('G4UASvc').info('addAction: Found a ' + basename + ' instance called ' + serviceName)
    from AthenaCommon.CfgGetter import getService
    theSvc=getService(serviceName)

    if theApp.state() > AthAppMgr.State.CONFIGURED:
        Logging.logging.getLogger('G4UASvc').fatal('Attempt to add a User Action when athena is ' +
                                                   'already initialized. Check your configuration')
        return

    for role in roles:
        roleAttr = roleMap[role]
        if not role in roleMap.keys():
            Logging.logging.getLogger('G4UASvc').fatal('Unkown role '+role+' requested for action ')
        else:
            currentActionList = getattr(theSvc, roleAttr)
            if not actionTool in currentActionList:
                # Add to beginning of list if system, otherwise the end
                if systemAction:
                    setattr(theSvc, roleAttr, [actionTool] + currentActionList)
                else:
                    setattr(theSvc, roleAttr, currentActionList + [actionTool])
