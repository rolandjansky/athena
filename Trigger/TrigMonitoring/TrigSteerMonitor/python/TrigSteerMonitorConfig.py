# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigSteerMonitor.TrigSteerMonitorConf import *


class TrigChainMoniConfigValidation(TrigChainMoni):
    """ Chains monitoring """
    __slots__ =  [ ]
    def __init__(self,name="TrigChainMoniValidation"):
        super(TrigChainMoniConfigValidation, self).__init__(name)
        self.LBNHistoryDepth=0

    def target(self):
        return [ "Validation" ]

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

class TrigTEMoniConfigValidation(TrigTEMoni):
    """ TriggerElement monitoring """
    __slots__ =  [ ]
    def __init__(self,name="TrigTEMoniValidation"):
        super(TrigTEMoniConfigValidation, self).__init__(name)
        self.LBNHistoryDepth=0

    def target(self):
        return [ "Validation" ]


# online

class TrigChainMoniConfigOnline(TrigChainMoni):
    """ Chains monitoring """
    __slots__ =  [ ]
    def __init__(self,name="TrigChainMoniOnline"):
        super(TrigChainMoniConfigOnline, self).__init__(name)
        self.LBNHistoryDepth=0

    def target(self):
        return [ "Online", "Cosmic" ]

class TrigErrorMonitor(TrigErrorMon):
    """ HLT Error Code monitoring """
    __slots__ =  [ ]
    def __init__(self,name="TrigErrorMonitor"):
        super(TrigErrorMonitor, self).__init__(name)
        self.LBNHistoryGroup=10
        self.expertMode=False
        
    def target(self):
        return [ "Online", "Cosmic", "Validation" ]

class TrigErrorExpertMonitor(TrigErrorMon):
    """ HLT Error Code monitoring for experts (all errorcodes) """
    __slots__ =  [ ]
    def __init__(self,name="TrigErrorExpertMonitor"):
        super(TrigErrorExpertMonitor, self).__init__(name)
        self.LBNHistoryGroup=10
        self.expertMode=True
        
    def target(self):
        return [ "Online", "Cosmic", "Validation" ]    

class TrigRoIMoniConfigOnline(TrigRoIMoni):
    """ RoI monitoring """
    __slots__ =  [ ]
    def __init__(self,name="TrigRoIMoniOnline"):
        super(TrigRoIMoniConfigOnline, self).__init__(name)
        self.LBNHistoryDepth=2
        self.ThreshMultiMax=30



    def target(self):
        return [ "Online", "Cosmic" ]

class TrigSignatureMoniConfigOnline(TrigSignatureMoni):
    """ Signature monitoring """
    __slots__ =  [ ]
    def __init__(self,name="TrigSignatureMoniOnline"):
        super(TrigSignatureMoniConfigOnline, self).__init__(name)
        self.LBNHistoryDepth=5

    def target(self):
        return [ "Online", "Cosmic" ]

class TrigTEMoniConfigOnline(TrigTEMoni):
    """ TriggerElement monitoring """
    __slots__ =  [ ]
    def __init__(self,name="TrigTEMoniOnline"):
        super(TrigTEMoniConfigOnline, self).__init__(name)
        self.LBNHistoryDepth=0

    def target(self):
        return [ "Online", "Cosmic" ]

class TrigRateMoniConfig10s(TrigRateMoni):
    """ Rates monitor for online use only """
    def __init__(self,name="TrigRate10s"):
        super(TrigRateMoniConfig10s, self).__init__(name)
        self.IntervalDuration = 10
        self.NumberOfIntervals = 6 
        self.doChains=True
        self.doStreams=True
        self.StreamSets = [
            'recording_physics_prompt:JetTauEtmiss,Muons,Egamma,MinBias',
            'recording_physics_delayed:Bphysics,HadDelayed,JetCalibDelayed',
            'recording_physics_other:'
            ]
        
    def target(self):
        return [ "Online", "Cosmic" ]    


class TrigRateMoniConfig20s(TrigRateMoni):
    """ Rates monitor for online use only """
    def __init__(self,name="TrigRate20s"):
        super(TrigRateMoniConfig20s, self).__init__(name)
        self.IntervalDuration = 20
        self.NumberOfIntervals = 3 
        self.doChains=True
        self.doStreams=True
        self.StreamSets = [
            'recording_physics_prompt:JetTauEtmiss,Muons,Egamma,MinBias',
            'recording_physics_delayed:Bphysics,HadDelayed,JetCalibDelayed',
            'recording_physics_other:'
            ]
        
    def target(self):
        return [ "Online", "Cosmic" ]    



# class TrigRateMoniConfig5min(TrigRateMoni):
#     """ Rates monitor for online use only """
#     def __init__(self,name="TrigRate5min"):
#         super(TrigRateMoniConfig5min, self).__init__(name)
#         self.IntervalDuration = 600
#         self.NumberOfIntervals = 2 
        
#     def target(self):
#         return [ "Online", "Cosmic" ]

class TrigOpMonitor(TrigOpMoni):
    """ Trigger operation monitor """
    def __init__(self,name="TrigOpMonitor"):
        super(TrigOpMonitor, self).__init__(name)
        
    def target(self):
        return [ "Online", "Cosmic" ]

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
        return [ "OnlineDetail" ]

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
        return [ "Online", "Cosmic" ]
    
TrigSteerMonitorToolList = [  TrigChainMoniConfigValidation(),
                              TrigErrorMonitor(),
                              TrigErrorExpertMonitor(),
                              TrigRoIMoniConfigValidation(),
                              TrigSignatureMoniConfigValidation(),
                              TrigTEMoniConfigValidation(), 
                              TrigChainMoniConfigOnline(),
                              TrigRoIMoniConfigOnline(),
                              TrigSignatureMoniConfigOnline(),
                              TrigTEMoniConfigOnline(),
                              TrigOpMonitor(),
                              TrigROBMoniConfig(),
                              TrigCorMonitor() ]


TrigSteerMonitorToolList_L2 = [  TrigRateMoniConfig20s() ]
TrigSteerMonitorToolList_EF = [  TrigRateMoniConfig20s() ]


##TrigSignatureMoniConfigOnline(), TrigTEMoniConfigOnline(), TrigRoIMoniConfigOnline(), TrigErrorMonConfigOnline() ]
##TrigSteerMonitorToolList = [ TrigChainMoniConfigValidation(),  TrigRoIMoniConfigValidation(), TrigErrorMonConfigValidation(),TrigChainMoniConfigOnline(),  TrigRoIMoniConfigOnline(), TrigErrorMonConfigOnline() ]


# etc
