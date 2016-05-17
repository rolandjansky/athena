# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## V1 UserActionSvc

class G4AtlasUAStore:

    def __init__(self):
        from AthenaCommon import Logging
        from AthenaCommon.CfgGetter import getService
        self.systemActions=[]
        self.log=Logging.logging.getLogger('G4UAStore')
        from AthenaCommon.AppMgr import ServiceMgr
        ##hack to allow for different named Configurations of the UserActionSvc
        basename='UserActionSvc'
        serviceName=''
        for s in ServiceMgr.getAllChildren():
            if basename in s.getName():
                serviceName = s.getName()
                break
        if ''==serviceName:
            self.log.error('G4AtlasUAStore: Could not find ' + basename + ' instance in ServiceMgr!')
            raise AttributeError('G4AtlasUAStore: Could not find ' + basename + ' instance in ServiceMgr!')
        else:
            self.log.info('G4AtlasUAStore: Found a ' + basename + ' instance called ' + serviceName)
            self.theSvc=getService(serviceName)
        # map roles to attributes
        self.roleMap={'BeginOfRun': 'BeginOfRunActions',
                      'EndOfRun': 'EndOfRunActions',
                      'BeginOfEvent': 'BeginOfEventActions',
                      'EndOfEvent': 'EndOfEventActions',
                      'BeginOfTracking': 'PreTrackingActions',
                      'EndOfTracking': 'PostTrackingActions',
                      'Step': 'SteppingActions',
                      'Classification': 'StackingActionsClassification',
                      'PrepareNewEvent': 'StackingActionsPrepareNewEvent',
                      'NewStage':  'StackingActionsNewStage'}

    def checkAppState(self):
        from AthenaCommon.AppMgr import theApp, AthAppMgr
        if theApp.state() > AthAppMgr.State.CONFIGURED:
            self.log.fatal('Attempt to add a User Action when athena is already initialized. Check you configuration')
            return False
        return True

    # method to add a system action, i.e. an action that is prepended to the list
    # forceoverride bypasses the check on pre-existing system actions
    def addSystemAction(self,actionTool,roles,forceoverride=False):
        if self.checkAppState():
            if isinstance(actionTool, str):
                self.log.info('addSystemAction(): adding system action %s (just the name) with roles %s.'%(actionTool, str(roles)))
            else:
                self.log.info('addSystemAction(): adding system action %s with roles %s.'%(actionTool.name(), str(roles)))
            for role in roles:
                if not role in self.roleMap.keys():
                    self.log.fatal('addSystemAction(): Unknown role '+role+' requested for action ')
                else:
                    if not actionTool in getattr(self.theSvc, self.roleMap[role]):
                        # allowing only one system action per role
                        if (not role in self.systemActions) or forceoverride :
                            setattr(self.theSvc,self.roleMap[role],[actionTool]+getattr(self.theSvc,self.roleMap[role]))
                            self.systemActions+=[role]
                        else:
                            self.log.fatal('addSystemAction(): Requesting a system action for '+role+', which already has one. Check your configuration.')


    def addAction(self,actionTool,roles):
        if self.checkAppState():
            if isinstance(actionTool, str):
                self.log.info('addAction(): adding action %s (just the name) with roles %s.'%(actionTool, str(roles)))
            else:
                self.log.info('addAction(): adding action %s with roles %s.'%(actionTool.name(), str(roles)))
            for role in roles:
                if not role in self.roleMap.keys():
                    self.log.fatal('addAction(): Unknown role '+role+' requested for action ')
                else:
                    if not actionTool in getattr(self.theSvc, self.roleMap[role]):
                        setattr(self.theSvc,self.roleMap[role],getattr(self.theSvc,self.roleMap[role])+[actionTool])


UAStore=G4AtlasUAStore()

