# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgGetter,CfgMgr,Logging

# Common methods to return default UserAction(Tool)s

# actions to be run at begin/end of run
def getDefaultRunActions():
    from G4AtlasApps.SimFlags import simFlags
    defaultUA=[]
    return defaultUA

# begin of event
def getDefaultEventActions():
    from G4AtlasApps.SimFlags import simFlags
    from AthenaCommon.BeamFlags import jobproperties
    defaultUA=[]
    if not simFlags.ISFRun:
        defaultUA+=['G4UA::G4SimTimerTool']
        defaultUA+=['G4UA::MCTruthSteppingActionTool']
    defaultUA+=['G4UA::G4TrackCounterTool']
    if hasattr(simFlags, 'CavernBG') and simFlags.CavernBG.statusOn and simFlags.CavernBG.get_Value() == 'Read':
        defaultUA+=['G4UA::HitWrapperTool']
    if jobproperties.Beam.beamType() == 'cosmics' and hasattr(simFlags, 'CavernBG') and not simFlags.CavernBG.statusOn:
        defaultUA+=['G4UA::CosmicPerigeeActionTool']
    if hasattr(simFlags, 'StoppedParticleFile') and simFlags.StoppedParticleFile.statusOn:
        defaultUA+=['G4UA::StoppedParticleFilterTool']
    if jobproperties.Beam.beamType() == 'cosmics' and not simFlags.ISFRun:
        defaultUA+=['G4UA::G4CosmicFilterTool']
    if hasattr(simFlags, 'CalibrationRun') and simFlags.CalibrationRun() == 'LAr+Tile':
        defaultUA+=['G4UA::CaloG4::CalibrationDefaultProcessingTool']
    return defaultUA

# stepping
def getDefaultSteppingActions():
    from G4AtlasApps.SimFlags import simFlags
    from AthenaCommon.BeamFlags import jobproperties
    defaultUA=[]
    if not simFlags.ISFRun:
        defaultUA+=['G4UA::MCTruthSteppingActionTool']
    if jobproperties.Beam.beamType() == 'cosmics' and hasattr(simFlags, 'CavernBG') and not simFlags.CavernBG.statusOn:
        defaultUA+=['G4UA::CosmicPerigeeActionTool']
    if hasattr(simFlags, 'CalibrationRun') and simFlags.CalibrationRun() == 'LAr+Tile':
        defaultUA+=['G4UA::CaloG4::CalibrationDefaultProcessingTool']
    if simFlags.PhysicsList == 'QGSP_BERT_HP':
        defaultUA+=['G4UA::PhotonKillerTool']
    if hasattr(simFlags, 'StoppedParticleFile') and simFlags.StoppedParticleFile.statusOn:
        defaultUA+=['G4UA::StoppedParticleActionTool']
    return defaultUA

# tracking
def getDefaultTrackingActions():
    from G4AtlasApps.SimFlags import simFlags
    defaultUA=[]
    if not simFlags.ISFRun:
        defaultUA+=['G4UA::AthenaTrackingActionTool']
    defaultUA+=['G4UA::G4TrackCounterTool']
    return defaultUA

# Stacking Classification
def getDefaultStackingActions():
    defaultUA=[]
    defaultUA+=['G4UA::AthenaStackingActionTool']
    return defaultUA

def getUserActionSvc(name="G4UA::UserActionSvc", **kwargs):
    """
    Get the standard UA svc configurable with all default actions added.
    This function is normally called by the configured factory, not users.
    """

    from G4AtlasApps.SimFlags import simFlags

    optionalActions = simFlags.OptionalUserActionList
    kwargs.setdefault('RunActionTools',
        getDefaultRunActions() + optionalActions.get_Value()['Run'])
    kwargs.setdefault('EventActionTools',
        getDefaultEventActions() + optionalActions.get_Value()['Event'])
    kwargs.setdefault('SteppingActionTools',
        getDefaultSteppingActions() + optionalActions.get_Value()['Step'])
    kwargs.setdefault('TrackingActionTools',
        getDefaultTrackingActions() + optionalActions.get_Value()['Tracking'])
    # no optional actions for stacking
    kwargs.setdefault('StackingActionTools', getDefaultStackingActions())

    # placeholder for more advanced config, if needed
    return CfgMgr.G4UA__UserActionSvc(name, **kwargs)

def getCTBUserActionSvc(name="G4UA::CTBUserActionSvc", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    optionalActions = simFlags.OptionalUserActionList
    run = getDefaultRunActions() + optionalActions.get_Value()['Run']
    event = getDefaultEventActions() + optionalActions.get_Value()['Event']
    tracking = getDefaultTrackingActions() + optionalActions.get_Value()['Tracking']
    stepping = getDefaultSteppingActions() + optionalActions.get_Value()['Step']
    stacking = getDefaultStackingActions()

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
    optionalActions = simFlags.OptionalUserActionList
    run = (getDefaultRunActions() + optionalActions.get_Value()['Run'] +
           PhysicsValidationUserAction)
    event = (getDefaultEventActions() + optionalActions.get_Value()['Event'] +
             TrackProcessorUserAction + PhysicsValidationUserAction)
    tracking = (TrackProcessorUserAction + MCTruthUserAction +
                getDefaultTrackingActions() + optionalActions.get_Value()['Tracking'] +
                PhysicsValidationUserAction)
    stepping = (getDefaultSteppingActions() + optionalActions.get_Value()['Step'] +
                TrackProcessorUserAction + PhysicsValidationUserAction)
    stacking = getDefaultStackingActions()

    kwargs.setdefault('RunActionTools', run)
    kwargs.setdefault('EventActionTools', event)
    kwargs.setdefault('SteppingActionTools', stepping)
    kwargs.setdefault('TrackingActionTools', tracking)
    kwargs.setdefault('StackingActionTools', stacking)

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
