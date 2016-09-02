# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgGetter,CfgMgr,Logging

# Common methods to return default UserAction(Tool)s

# actions to be run at begin of run
def getDefaultBoRActions():
    from G4AtlasApps.SimFlags import simFlags
    defaultUA=[]
    #if not  simFlags.ISFRun:
    #    defaultUA+=['G4UA::G4SimTimerTool']
    if hasattr(simFlags, 'StoppedParticleFile') and simFlags.StoppedParticleFile.statusOn:
        defaultUA+=['G4UA::StoppedParticleActionTool']
    return defaultUA

# actions to be run at end of run
def getDefaultEoRActions():
    from G4AtlasApps.SimFlags import simFlags
    from AthenaCommon.BeamFlags import jobproperties
    defaultUA=[]
    if hasattr(simFlags, 'StoppedParticleFile') and simFlags.StoppedParticleFile.statusOn:
        defaultUA+=['G4UA::StoppedParticleActionTool']
    return defaultUA

# begin of event
def getDefaultBoEActions():
    from G4AtlasApps.SimFlags import simFlags
    from AthenaCommon.BeamFlags import jobproperties
    defaultUA=[]
    if not simFlags.ISFRun:
        defaultUA+=['G4UA::G4SimTimerTool']
        defaultUA+=['G4UA::MCTruthSteppingActionTool']
    defaultUA+=['G4UA::G4TrackCounterTool']

    if jobproperties.Beam.beamType() == 'cosmics' and hasattr(simFlags, 'CavernBG') and not simFlags.CavernBG.statusOn:
        defaultUA+=['G4UA::CosmicPerigeeActionTool']
    if hasattr(simFlags, 'StoppedParticleFile') and simFlags.StoppedParticleFile.statusOn:
        defaultUA+=['G4UA::StoppedParticleActionTool']
    if hasattr(simFlags, 'CalibrationRun') and simFlags.CalibrationRun() == 'LAr+Tile':
        defaultUA+=['G4UA::CaloG4::CalibrationDefaultProcessingTool']
    return defaultUA

# end of event
def getDefaultEoEActions():
    from G4AtlasApps.SimFlags import simFlags
    from AthenaCommon.BeamFlags import jobproperties
    defaultUA=[]
    if not simFlags.ISFRun:
        defaultUA+=['G4UA::G4SimTimerTool']
    if hasattr(simFlags, 'CavernBG') and simFlags.CavernBG.statusOn and simFlags.CavernBG.get_Value() == 'Read':
        defaultUA+=['G4UA::HitWrapperTool']
    if hasattr(simFlags,'StoppedParticleFile') and simFlags.StoppedParticleFile.statusOn:
        defaultUA+=['G4UA::StoppedParticleActionTool']
        defaultUA+=['G4UA::G4CosmicFilterTool']
    if jobproperties.Beam.beamType() == 'cosmics' and not simFlags.ISFRun:
        defaultUA+=['G4UA::G4CosmicFilterTool']
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
    return defaultUA

# PreUserTracking
def getDefaultBoTActions():
    from G4AtlasApps.SimFlags import simFlags
    defaultUA=[]
    if not simFlags.ISFRun:
        defaultUA+=['G4UA::AthenaTrackingActionTool']
    defaultUA+=['G4UA::G4TrackCounterTool']
    return defaultUA

# PostUserTracking
def getDefaultEoTActions():
    from G4AtlasApps.SimFlags import simFlags
    defaultUA=[]
    if not simFlags.ISFRun:
        defaultUA+=['G4UA::AthenaTrackingActionTool']
    return defaultUA

# Stacking Classification
def getDefaultStackingActions():
    defaultUA=[]
    defaultUA+=['G4UA::AthenaStackingActionTool']
    return defaultUA

# Stacking PrepareNewEvent
def getDefaultStaPrepareActions():
    return []

# Stacking NewStage
def getDefaultStaNewStageActions():
    return []


def getUserActionSvc(name="G4UA::UserActionSvc", **kwargs):
    """
    Get the standard UA svc configurable with all default actions added.
    This function is normally called by the configured factory, not users.
    """

    from G4AtlasApps.SimFlags import simFlags

    kwargs.setdefault('BeginRunActionTools', getDefaultBoRActions()+simFlags.OptionalUserActionList.get_Value()['BeginOfRun'])
    kwargs.setdefault('EndRunActionTools', getDefaultEoRActions()+simFlags.OptionalUserActionList.get_Value()['EndOfRun'])
    kwargs.setdefault('BeginEventActionTools', getDefaultBoEActions()+simFlags.OptionalUserActionList.get_Value()['BeginOfEvent'])
    kwargs.setdefault('EndEventActionTools', getDefaultEoEActions()+simFlags.OptionalUserActionList.get_Value()['EndOfEvent'])
    kwargs.setdefault('SteppingActionTools', getDefaultSteppingActions()+simFlags.OptionalUserActionList.get_Value()['Step'])
    kwargs.setdefault('PreTrackingActionTools', getDefaultBoTActions()+simFlags.OptionalUserActionList.get_Value()['PreTracking'])
    kwargs.setdefault('PostTrackingActionTools', getDefaultEoTActions()+simFlags.OptionalUserActionList.get_Value()['PostTracking'])
    # no optional actions for stacking
    kwargs.setdefault('StackingActionTools', getDefaultStackingActions())

    # placeholder for more advanced config, if needed
    return CfgMgr.G4UA__UserActionSvc(name, **kwargs)

def getCTBUserActionSvc(name="G4UA::CTBUserActionSvc", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    bor = getDefaultBoRActions()+simFlags.OptionalUserActionList.get_Value()['BeginOfRun']
    eor = getDefaultEoRActions()+simFlags.OptionalUserActionList.get_Value()['EndOfRun']
    boe = getDefaultBoEActions()+simFlags.OptionalUserActionList.get_Value()['BeginOfEvent']
    eoe = getDefaultEoEActions()+simFlags.OptionalUserActionList.get_Value()['EndOfEvent']
    bot = getDefaultBoTActions()+simFlags.OptionalUserActionList.get_Value()['PreTracking']
    eot = getDefaultEoTActions()+simFlags.OptionalUserActionList.get_Value()['PostTracking']
    stepping = getDefaultSteppingActions()+simFlags.OptionalUserActionList.get_Value()['Step']
    stacking = getDefaultStackingActions()

    # FIXME: ADS these actions are not yet migrated to Hive
    #if simFlags.SimLayout.get_Value()=='tb_LArH6_2004':
    #    eoe=['LArHitsH6EventAction']+eoe
    #    eoe+=['LArGeoH62004EventAction']
    #    if simFlags.LArTB_H6Step.statusOn:
    #        if simFlags.LArTB_H6Step.get_Value():
    #            stepping+=['LArGeoH62004SteppingAction']
    #            boe+=['RadLenNtuple']
    #            eoe+=['RadLenNtuple']
    #            stepping+=['RadLenNtuple']

    kwargs.setdefault('BeginRunActionTools', bor)
    kwargs.setdefault('EndRunActionTools', eor)
    kwargs.setdefault('BeginEventActionTools', boe)
    kwargs.setdefault('EndEventActionTools', eoe)
    kwargs.setdefault('SteppingActionTools', stepping)
    kwargs.setdefault('PreTrackingActionTools', bot)
    kwargs.setdefault('PostTrackingActionTools', eot)
    kwargs.setdefault('StackingActionTools', stacking)

    # placeholder for more advanced config, if needed
    return CfgMgr.G4UA__UserActionSvc(name, **kwargs)


def getISFUserActionSvc(name="G4UA::ISFUserActionSvc", **kwargs):
    TrackProcessorUserAction = kwargs.pop('TrackProcessorUserAction',[])
    PhysicsValidationUserAction = kwargs.pop('PhysicsValidationUserAction',[])
    MCTruthUserAction = kwargs.pop('MCTruthUserAction',['ISFMCTruthUserActionTool'])

    from G4AtlasApps.SimFlags import simFlags
    bor = getDefaultBoRActions()+simFlags.OptionalUserActionList.get_Value()['BeginOfRun'] + PhysicsValidationUserAction
    eor = getDefaultEoRActions()+simFlags.OptionalUserActionList.get_Value()['EndOfRun']
    boe = getDefaultBoEActions()+simFlags.OptionalUserActionList.get_Value()['BeginOfEvent'] + TrackProcessorUserAction + PhysicsValidationUserAction
    eoe = getDefaultEoEActions()+simFlags.OptionalUserActionList.get_Value()['EndOfEvent'] + TrackProcessorUserAction + PhysicsValidationUserAction
    bot = TrackProcessorUserAction + MCTruthUserAction + getDefaultBoTActions()+simFlags.OptionalUserActionList.get_Value()['PreTracking'] + PhysicsValidationUserAction
    eot = TrackProcessorUserAction + MCTruthUserAction + getDefaultEoTActions()+simFlags.OptionalUserActionList.get_Value()['PostTracking']
    stepping = getDefaultSteppingActions()+simFlags.OptionalUserActionList.get_Value()['Step'] + TrackProcessorUserAction + PhysicsValidationUserAction
    stacking = getDefaultStackingActions()

    kwargs.setdefault('BeginRunActionTools', bor)
    kwargs.setdefault('EndRunActionTools', eor)
    kwargs.setdefault('BeginEventActionTools', boe)
    kwargs.setdefault('EndEventActionTools', eoe)
    kwargs.setdefault('SteppingActionTools', stepping)
    kwargs.setdefault('PreTrackingActionTools', bot)
    kwargs.setdefault('PostTrackingActionTools', eot)
    kwargs.setdefault('StackingActionTools', stacking)

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

    roleMap={'BeginOfRun': 'BeginRunActionTools',
             'EndOfRun': 'EndRunActionTools',
             'BeginOfEvent': 'BeginEventActionTools',
             'EndOfEvent': 'EndEventActionTools',
             'BeginOfTracking': 'PreTrackingActionTools',
             'EndOfTracking': 'PostTrackingActionTools',
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
