# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigT2IDTau.TrigT2IDTauConf import  T2IDTau
from AthenaCommon.SystemOfUnits import GeV


##################### basic class ##############################
class T2IDTau_Tau (T2IDTau):
    __slots__ = []
    def __init__(self, name = "T2IDTau_Tau"):
        super(  T2IDTau_Tau, self ).__init__( name )

        self.dRCore                = 0.1
        self.dRIso                 = 0.3
        self.ptMin                 = 1.5*GeV 
        self.ptSlowMax             = 7.5*GeV
        self.secondStep            = False   #default value
        self.TrigTrackingAlgoLabel = ""
            
        # monitoring part. To switch off do in topOption TriggerFlags.enableMonitoring = []
        from TrigT2IDTau.T2IDTauMonitoring import T2IDTauValidationMonitoring, T2IDTauOnlineMonitoring 
        validation = T2IDTauValidationMonitoring()        
        online     = T2IDTauOnlineMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        self.AthenaMonTools = [ time, validation, online ]


## provide a configurable base class
class T2IDTau_Tau_custom (T2IDTau_Tau):
    __slots__ = []
    def __init__(self, name = "T2IDTau_Tau_dZ0", dRCore = 0.1, dRIso = 0.3, ptMin = 1.5*GeV, dZ0Max = -1.0):
        T2IDTau_Tau.__init__(self,name)
        self.dRCore = dRCore
        self.dRIso  = dRIso
        self.ptMin  = ptMin
        self.dZ0Max = dZ0Max
        
        
## instance for 2012
class T2IDTau_Tau_1GeV_dZ02_dR0103 (T2IDTau_Tau_custom):
    def __init__(self, name = "T2IDTau_Tau_1GeV_dZ02_dR0103"):
        T2IDTau_Tau_custom.__init__(self, name, dRCore = 0.1, dRIso = 0.3, ptMin = 1.0*GeV, dZ0Max = 2.0)



## instances for 2 step tracking Tests
class  T2IDTau_Tau_core(T2IDTau_Tau_1GeV_dZ02_dR0103):
    def __init__(self, name="T2IDTau_Tau_core"):
        T2IDTau_Tau_1GeV_dZ02_dR0103.__init__(self,name)

class  T2IDTau_Tau_iso(T2IDTau_Tau_1GeV_dZ02_dR0103):
    def __init__(self, name="T2IDTau_Tau_iso"):
        T2IDTau_Tau_1GeV_dZ02_dR0103.__init__(self,name)
        self.secondStep = True



