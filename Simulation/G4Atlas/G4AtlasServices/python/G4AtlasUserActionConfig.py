# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgGetter,CfgMgr,Logging

# Common methods to return default UserAction(Tool)s

# provide V1 or V2 action names
def userActionName(aName):
    from G4AtlasApps.SimFlags import simFlags
    if (hasattr(simFlags, 'UseV2UserActions') and simFlags.UseV2UserActions()):
        if aName == 'CalibrationDefaultProcessing': ## This one uses a different naming convention...
            return 'G4UA::CaloG4::CalibrationDefaultProcessingTool'
        return 'G4UA::'+aName+'Tool'
    return aName

# actions to be run at begin of run
def getDefaultBoRActions():
    from G4AtlasApps.SimFlags import simFlags
    defaultUA=[]
    if not ((hasattr(simFlags, 'UseV2UserActions') and simFlags.UseV2UserActions()) or simFlags.ISFRun): #FIXME temp workaround
        defaultUA+=[userActionName('G4SimTimer')]
    if hasattr(simFlags, 'StoppedParticleFile') and simFlags.StoppedParticleFile.statusOn:
        defaultUA+=[userActionName('StoppedParticleAction')]
    return defaultUA

# actions to be run at end of run
def getDefaultEoRActions():
    from G4AtlasApps.SimFlags import simFlags
    from AthenaCommon.BeamFlags import jobproperties
    defaultUA=[]
    #FIXME temp workaround
    if not ((hasattr(simFlags, 'UseV2UserActions') and simFlags.UseV2UserActions()) or simFlags.ISFRun):
        defaultUA+=[userActionName('G4SimTimer')]
        defaultUA+=[userActionName('G4TrackCounter')]
    if hasattr(simFlags, 'StoppedParticleFile') and simFlags.StoppedParticleFile.statusOn:
        defaultUA+=[userActionName('StoppedParticleAction')]
        if not (hasattr(simFlags, 'UseV2UserActions') and simFlags.UseV2UserActions()): #FIXME temp workaround
            defaultUA+=[userActionName('G4CosmicFilter')]
    if jobproperties.Beam.beamType() == 'cosmics' and not ((hasattr(simFlags, 'UseV2UserActions') and simFlags.UseV2UserActions()) or simFlags.ISFRun): #FIXME temp workaround
        defaultUA+=[userActionName('G4CosmicFilter')]
    return defaultUA

# begin of event
def getDefaultBoEActions():
    from G4AtlasApps.SimFlags import simFlags
    from AthenaCommon.BeamFlags import jobproperties
    defaultUA=[]
    if not simFlags.ISFRun:
        defaultUA+=[userActionName('G4SimTimer')]
        defaultUA+=[userActionName('MCTruthSteppingAction')]
    defaultUA+=[userActionName('G4TrackCounter')]
    if jobproperties.Beam.beamType() == 'cosmics' and hasattr(simFlags, 'CavernBG') and not simFlags.CavernBG.statusOn:
        defaultUA+=[userActionName('CosmicPerigeeAction')]
    if hasattr(simFlags, 'StoppedParticleFile') and simFlags.StoppedParticleFile.statusOn:
        defaultUA+=[userActionName('StoppedParticleAction')]
    if hasattr(simFlags, 'CalibrationRun') and simFlags.CalibrationRun() == 'LAr+Tile':
        defaultUA+=[userActionName('CalibrationDefaultProcessing')]
    return defaultUA

# end of event
def getDefaultEoEActions():
    from G4AtlasApps.SimFlags import simFlags
    from AthenaCommon.BeamFlags import jobproperties
    defaultUA=[]
    if not simFlags.ISFRun:
        defaultUA+=[userActionName('G4SimTimer')]
    if hasattr(simFlags, 'CavernBG') and simFlags.CavernBG.statusOn and simFlags.CavernBG.get_Value() == 'Read':
        defaultUA+=[userActionName('HitWrapper')]
    if hasattr(simFlags, 'StoppedParticleFile') and simFlags.StoppedParticleFile.statusOn:
        defaultUA+=[userActionName('StoppedParticleAction')]
        defaultUA+=[userActionName('G4CosmicFilter')]
    if jobproperties.Beam.beamType() == 'cosmics' and not simFlags.ISFRun:
        defaultUA+=[userActionName('G4CosmicFilter')]
    return defaultUA

# stepping
def getDefaultSteppingActions():
    from G4AtlasApps.SimFlags import simFlags
    from AthenaCommon.BeamFlags import jobproperties
    defaultUA=[]
    if not simFlags.ISFRun:
        defaultUA+=[userActionName('MCTruthSteppingAction')]
    if jobproperties.Beam.beamType() == 'cosmics' and hasattr(simFlags, 'CavernBG') and not simFlags.CavernBG.statusOn:
        defaultUA+=[userActionName('CosmicPerigeeAction')]
    if hasattr(simFlags, 'CalibrationRun') and simFlags.CalibrationRun() == 'LAr+Tile':
        defaultUA+=[userActionName('CalibrationDefaultProcessing')]
    if simFlags.PhysicsList == 'QGSP_BERT_HP':
        defaultUA+=[userActionName('PhotonKiller')]
    return defaultUA

# PreUserTracking
def getDefaultBoTActions():
    from G4AtlasApps.SimFlags import simFlags
    defaultUA=[]
    if not simFlags.ISFRun:
        defaultUA+=[userActionName('AthenaTrackingAction')]
    defaultUA+=[userActionName('G4TrackCounter')]
    return defaultUA

# PostUserTracking
def getDefaultEoTActions():
    from G4AtlasApps.SimFlags import simFlags
    defaultUA=[]
    if not simFlags.ISFRun:
        defaultUA+=[userActionName('AthenaTrackingAction')]
    return defaultUA

# Stacking Classification
def getDefaultStackingActions():
    defaultUA=[]
    defaultUA+=[userActionName('AthenaStackingAction')]
    return defaultUA

# Stacking PrepareNewEvent
def getDefaultStaPrepareActions():
    return []

# Stacking NewStage
def getDefaultStaNewStageActions():
    return []

#
# Methods used to retrieve V1 UserActionSvc
#

# this creates the V1 USerActionSvc with the default actions
# additional actions must be added to the service by the
# client packages, through G4AtlasUserActionConfig.UAStore
def getV1UserActionSvc(name="UserActionSvc", **kwargs):
    kwargs.setdefault('BeginOfRunActions',              getDefaultBoRActions())
    kwargs.setdefault('EndOfRunActions',                getDefaultEoRActions())
    kwargs.setdefault('BeginOfEventActions',            getDefaultBoEActions())
    kwargs.setdefault('EndOfEventActions',              getDefaultEoEActions())
    kwargs.setdefault('SteppingActions',                getDefaultSteppingActions())
    kwargs.setdefault('PreTrackingActions',             getDefaultBoTActions())
    kwargs.setdefault('PostTrackingActions',            getDefaultEoTActions())
    kwargs.setdefault('StackingActionsClassification',  getDefaultStackingActions())
    kwargs.setdefault('StackingActionsPrepareNewEvent', getDefaultStaPrepareActions())
    kwargs.setdefault('StackingActionsNewStage',        getDefaultStaNewStageActions())
    # placeholder for more advanced config, if needed
    return CfgMgr.UserActionSvc(name, **kwargs)

def getV1CTBUserActionSvc(name="CTBUserActionSvc", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    bor = getDefaultBoRActions()
    eor = getDefaultEoRActions()
    boe = getDefaultBoEActions()
    eoe = getDefaultEoEActions()
    bot = getDefaultBoTActions()
    eot = getDefaultEoTActions()
    stepping = getDefaultSteppingActions()
    stacking = getDefaultStackingActions()

    if simFlags.SimLayout.get_Value()=='tb_LArH6_2004':
        eoe=['LArHitsH6EventAction']+eoe
        eoe+=['LArGeoH62004EventAction']
        if simFlags.LArTB_H6Step.statusOn:
            if simFlags.LArTB_H6Step.get_Value():
                stepping+=['LArGeoH62004SteppingAction']
                boe+=['RadLenNtuple']
                eoe+=['RadLenNtuple']
                stepping+=['RadLenNtuple']
    kwargs.setdefault('BeginOfRunActions', bor)
    kwargs.setdefault('EndOfRunActions', eor)
    kwargs.setdefault('BeginOfEventActions', boe)
    kwargs.setdefault('EndOfEventActions', eoe)
    kwargs.setdefault('SteppingActions', stepping)
    kwargs.setdefault('PreTrackingActions', bot)
    kwargs.setdefault('PostTrackingActions', eot)
    kwargs.setdefault('StackingActionsClassification', stacking)

    # placeholder for more advanced config, if needed
    return CfgMgr.UserActionSvc(name, **kwargs)


def getV1ISFUserActionSvc(name="ISFUserActionSvc", **kwargs):
    TrackProcessorUserAction = kwargs.pop('TrackProcessorUserAction',[])
    PhysicsValidationUserAction = kwargs.pop('PhysicsValidationUserAction',[])
    MCTruthUserAction = kwargs.pop('MCTruthUserAction',['ISFMCTruthUserAction'])

    from G4AtlasApps.SimFlags import simFlags
    bor = getDefaultBoRActions() + PhysicsValidationUserAction
    eor = getDefaultEoRActions()
    boe = getDefaultBoEActions() + TrackProcessorUserAction + PhysicsValidationUserAction
    eoe = getDefaultEoEActions() + TrackProcessorUserAction + PhysicsValidationUserAction
    bot = TrackProcessorUserAction + MCTruthUserAction + getDefaultBoTActions() + PhysicsValidationUserAction
    eot = TrackProcessorUserAction + MCTruthUserAction + getDefaultEoTActions()
    stepping = getDefaultSteppingActions() + TrackProcessorUserAction + PhysicsValidationUserAction
    stacking = getDefaultStackingActions()

    kwargs.setdefault('BeginOfRunActions', bor)
    kwargs.setdefault('EndOfRunActions', eor)
    kwargs.setdefault('BeginOfEventActions', boe)
    kwargs.setdefault('EndOfEventActions', eoe)
    kwargs.setdefault('SteppingActions', stepping)
    kwargs.setdefault('PreTrackingActions', bot)
    kwargs.setdefault('PostTrackingActions', eot)
    kwargs.setdefault('StackingActionsClassification', stacking)

    return CfgMgr.UserActionSvc(name, **kwargs)

def getV1ISFFullUserActionSvc(name="ISFFullUserActionSvc", **kwargs):
    # this configuration needs ISFMCTruthUserAction,
    # G4OnlyPhysicsValidationUserAction, and FullG4TrackProcessorUserAction
    from ISF_Config.ISF_jobProperties import ISF_Flags
    if ISF_Flags.ValidationMode.get_Value():
        kwargs.setdefault('PhysicsValidationUserAction',['G4OnlyPhysicsValidationUserAction'])
    kwargs.setdefault('TrackProcessorUserAction', ['FullG4TrackProcessorUserAction'])
    return getV1ISFUserActionSvc(name, **kwargs)

def getV1ISFPassBackUserActionSvc(name="ISFPassBackUserActionSvc", **kwargs):
    # this configuration needs ISFMCTruthUserAction,
    # G4OnlyPhysicsValidationUserAction, and PassBackG4TrackProcessorUserAction
    from ISF_Config.ISF_jobProperties import ISF_Flags
    if ISF_Flags.ValidationMode.get_Value():
        kwargs.setdefault('PhysicsValidationUserAction', ['G4OnlyPhysicsValidationUserAction'])
    kwargs.setdefault('TrackProcessorUserAction', ['PassBackG4TrackProcessorUserAction'])
    return getV1ISFUserActionSvc(name, **kwargs)

def getV1ISF_AFIIUserActionSvc(name="ISF_AFIIUserActionSvc", **kwargs):
    # this configuration needs ISFMCTruthUserAction,
    # AFII_G4PhysicsValidationUserAction, and AFII_G4TrackProcessorUserAction
    from ISF_Config.ISF_jobProperties import ISF_Flags
    if ISF_Flags.ValidationMode.get_Value():
        kwargs.setdefault('PhysicsValidationUserAction', ['AFII_G4PhysicsValidationUserAction'])
    kwargs.setdefault('TrackProcessorUserAction', ['AFII_G4TrackProcessorUserAction'])
    return getV1ISFUserActionSvc(name, **kwargs)

def getV1ISFQuasiStableUserActionSvc(name="ISFQuasiStableUserActionSvc", **kwargs):
    # this configuration needs ISFMCTruthUserAction,
    # QuasiStableG4PhysicsValidationUserAction, and FullG4TrackProcessorUserAction
    from ISF_Config.ISF_jobProperties import ISF_Flags
    if ISF_Flags.ValidationMode.get_Value():
        kwargs.setdefault('PhysicsValidationUserAction', ['QuasiStableG4PhysicsValidationUserAction'])
    return getV1ISFFullUserActionSvc(name, **kwargs)


#
# Methods used to retrieve V2 user actions
#

def getV2UserActionSvc(name="G4UA::UserActionSvc", **kwargs):
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

def getV2CTBUserActionSvc(name="G4UA::CTBUserActionSvc", **kwargs):
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


def getV2ISFUserActionSvc(name="G4UA::ISFUserActionSvc", **kwargs):
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

def getV2ISFFullUserActionSvc(name="G4UA::ISFFullUserActionSvc", **kwargs):
    # this configuration needs ISFMCTruthUserAction,
    # G4OnlyPhysicsValidationUserAction, and FullG4TrackProcessorUserAction
    from ISF_Config.ISF_jobProperties import ISF_Flags
    if ISF_Flags.ValidationMode.get_Value():
        kwargs.setdefault('PhysicsValidationUserAction',[])
        # FIXME: this action has not been migrated yet
        #kwargs.setdefault('PhysicsValidationUserAction',['G4OnlyPhysicsValidationUserActionTool'])
    kwargs.setdefault('TrackProcessorUserAction', ['FullG4TrackProcessorUserActionTool'])
    return getV2ISFUserActionSvc(name, **kwargs)

def getV2ISFPassBackUserActionSvc(name="G4UA::ISFPassBackUserActionSvc", **kwargs):
    # this configuration needs ISFMCTruthUserAction,
    # G4OnlyPhysicsValidationUserAction, and PassBackG4TrackProcessorUserAction
    from ISF_Config.ISF_jobProperties import ISF_Flags
    if ISF_Flags.ValidationMode.get_Value():
        kwargs.setdefault('PhysicsValidationUserAction',[])
        # FIXME: this action has not been migrated yet
        #kwargs.setdefault('PhysicsValidationUserAction', ['G4OnlyPhysicsValidationUserActionTool'])
    kwargs.setdefault('TrackProcessorUserAction', ['PassBackG4TrackProcessorUserActionTool'])
    return getV2ISFUserActionSvc(name, **kwargs)

def getV2ISF_AFIIUserActionSvc(name="G4UA::ISF_AFIIUserActionSvc", **kwargs):
    # this configuration needs ISFMCTruthUserAction,
    # AFII_G4PhysicsValidationUserAction, and AFII_G4TrackProcessorUserAction
    from ISF_Config.ISF_jobProperties import ISF_Flags
    if ISF_Flags.ValidationMode.get_Value():
        kwargs.setdefault('PhysicsValidationUserAction',[])
        # FIXME: this action has not been migrated yet
        #kwargs.setdefault('PhysicsValidationUserAction', ['AFII_G4PhysicsValidationUserActionTool'])
    kwargs.setdefault('TrackProcessorUserAction', ['AFII_G4TrackProcessorUserActionTool'])
    return getV2ISFUserActionSvc(name, **kwargs)

def getV2ISFQuasiStableUserActionSvc(name="G4UA::ISFQuasiStableUserActionSvc", **kwargs):
    # this configuration needs ISFMCTruthUserAction,
    # QuasiStableG4PhysicsValidationUserAction, and FullG4TrackProcessorUserAction
    from ISF_Config.ISF_jobProperties import ISF_Flags
    if ISF_Flags.ValidationMode.get_Value():
        kwargs.setdefault('PhysicsValidationUserAction',[])
        # FIXME: this action has not been migrated yet
        #kwargs.setdefault('PhysicsValidationUserAction', ['QuasiStableG4PhysicsValidationUserActionTool'])
    return getV2ISFFullUserActionSvc(name, **kwargs)


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
