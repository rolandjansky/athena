# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigSteerMonitor.TrigSteerMonitorConf import *


class TrigErrorMonConfigValidation(TrigErrorMon):
    """ HLT Error Code monitoring """
    __slots__ =  [ ]
    def __init__(self,name="TrigErrorMonValidation"):
        super(TrigErrorMonConfigValidation, self).__init__(name)
        self.LBNHistoryDepth=0

    def target(self):
        return [ "Validation" ]

class TrigRoIMoniConfigValidation(TrigRoIMoni):
    """ RoI monitoring """
    __slots__ =  [ ]
    def __init__(self,name="TrigRoIMoniValidation"):
        super(TrigRoIMoniConfigValidation, self).__init__(name)
        self.LBNHistoryDepth=0
        self.ThreshMultiMax=18

    def target(self):
        return [ "Validation" ]

class TrigSignatureMoniConfigValidation(TrigSignatureMoni):
    """ Signature monitoring """
    __slots__ =  [ ]
    def __init__(self,name="TrigSignatureMoniValidation"):
        super(TrigSignatureMoniConfigValidation, self).__init__(name)
        self.LBNHistoryDepth=0

    def target(self):
        return [ "Validation" ]

class TrigTEMoniConfig(TrigTEMoni):
    """ TriggerElement monitoring """
    __slots__ =  [ ]
    def __init__(self,name="TrigTEMoni"):
        super(TrigTEMoniConfig, self).__init__(name)
        self.LBNHistoryDepth=0

    def target(self):
        return [ "Validation" ]


class TrigChainMoniConfig(TrigChainMoni):
    """ Chains monitoring """
    __slots__ =  [ ]
    def __init__(self,name="TrigChainMoni"):
        super(TrigChainMoniConfig, self).__init__(name)
        self.LBNHistoryDepth=0

    def target(self):
        return [ "Online", "Validation" ]

class TrigErrorMonitor(TrigErrorMon):
    """ HLT Error Code monitoring """
    __slots__ =  [ ]
    def __init__(self,name="TrigErrorMonitor"):
        super(TrigErrorMonitor, self).__init__(name)
        self.LBNHistoryGroup=10
        self.expertMode=False
        
    def target(self):
        return [ "Online", "Validation" ]

class TrigErrorExpertMonitor(TrigErrorMon):
    """ HLT Error Code monitoring for experts (all errorcodes) """
    __slots__ =  [ ]
    def __init__(self,name="TrigErrorExpertMonitor"):
        super(TrigErrorExpertMonitor, self).__init__(name)
        self.LBNHistoryGroup=10
        self.expertMode=True
        
    def target(self):
        return [ "Online", "Validation" ]    

class TrigRoIMoniConfigOnline(TrigRoIMoni):
    """ RoI monitoring """
    __slots__ =  [ ]
    def __init__(self,name="TrigRoIMoniOnline"):
        super(TrigRoIMoniConfigOnline, self).__init__(name)
        self.LBNHistoryDepth=2
        self.ThreshMultiMax=30

    def target(self):
        return [ "Online" ]

class TrigSignatureMoniConfigOnline(TrigSignatureMoni):
    """ Signature monitoring """
    __slots__ =  [ ]
    def __init__(self,name="TrigSignatureMoniOnline"):
        super(TrigSignatureMoniConfigOnline, self).__init__(name)
        self.LBNHistoryDepth=5

    def target(self):
        return [ "Online" ]

class TrigRateMoniConfig20s(TrigRateMoni):
    """ Rates monitor for online use only """
    def __init__(self,name="TrigRate20s"):
        super(TrigRateMoniConfig20s, self).__init__(name)
        self.IntervalDuration = 20
        self.NumberOfIntervals = 3 
        self.doChains=True
        self.doStreams=True
        self.StreamSets = [
            'recording_physics_prompt:Main',
            'recording_physics_delayed:BphysDelayed,ExoDelayed',
            'recording_physics_other:'
            ]
        
    def target(self):
        return [ "Online" ]    


class TrigOpMonitor(TrigOpMoni):
    """ Trigger operation monitor """
    def __init__(self,name="TrigOpMonitor"):
        super(TrigOpMonitor, self).__init__(name)
        
    def target(self):
        return [ "Online", "Validation" ]

class TrigMemMonitor(TrigMemMoni):
    """ Memory monitor """
    def __init__(self,name="TrigMemMonitor"):
        super(TrigMemMonitor, self).__init__(name)

        from AthenaCommon.AppMgr import ServiceMgr as svcMgr,theApp
        if not hasattr(svcMgr.AuditorSvc,"TrigMemAuditor"):
            from TrigSteerMonitor.TrigSteerMonitorConf import TrigMemAuditor
            svcMgr.AuditorSvc += TrigMemAuditor()            
            theApp.AuditAlgorithms = True
        
    def target(self):
        return [ "Online" ]

class TrigROBMoniConfig(TrigROBMoni):
    """ ROB request monitor for online use """
    def __init__(self,name="TrigROBMoni"):
        super(TrigROBMoniConfig, self).__init__(name)
        
    def target(self):
        return [ "OnlineDetail" ]

class TrigCorMonitor(TrigCorMoni):
    """ Trigger L1 and HLT correlation monitor """
    def __init__(self,name="TrigCorMonitor"):
        super(TrigCorMonitor, self).__init__(name)
        
    def target(self):
        return [ "Online", "Validation" ]
    
TrigSteerMonitorToolList = [  TrigRateMoniConfig20s(),   # leave first(!) so it gets finalized first (ATDSUPPORT-223)
                              TrigErrorMonitor(),
                              TrigErrorExpertMonitor(),
                              TrigRoIMoniConfigValidation(), TrigRoIMoniConfigOnline(),
                              TrigSignatureMoniConfigValidation(), TrigSignatureMoniConfigOnline(),
                              TrigTEMoniConfig(), 
                              TrigChainMoniConfig(),                                                            
                              TrigOpMonitor(),
                              TrigMemMonitor(),
                              TrigROBMoniConfig(),
                              TrigCorMonitor() ]


