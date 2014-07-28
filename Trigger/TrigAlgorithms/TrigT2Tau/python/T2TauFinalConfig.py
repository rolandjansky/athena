# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigT2Tau.TrigT2TauConf import  T2TauEtaPhiPtTool, T2TauEnergyTool, T2TauEtFlowTool
from TrigT2Tau.TrigT2TauConf import  T2TauMatchingTool, T2TauFinal, T2TauTrkRadiusTool
from AthenaCommon.SystemOfUnits import GeV

class T2TauMatchingToolConfig (T2TauMatchingTool):
   __slots__ = []
   def __init__ (self, name="T2TauMatchingTool",  toolPtCut=1.*GeV, toolConeCut=0.3): 
      super(T2TauMatchingToolConfig, self).__init__(name)
      self.TrackPTCut        = toolPtCut
      self.TrackMatchConeCut = toolConeCut
      
      
class T2TauTrkRadiusToolConfig (T2TauTrkRadiusTool):
   __slots__ = []
   def __init__ (self, name="T2TauTrkRadiusTool",  toolPtCut=1.*GeV, toolConeCut=0.3, toolDZ0Max = -1.0):
      super(T2TauTrkRadiusToolConfig, self).__init__(name )
      self.TrackPTCut        = toolPtCut
      self.TrackMatchConeCut = toolConeCut
      self.TrackdZ0Max       = toolDZ0Max


##################### basic class ##############################
class T2TauFinal_custom (T2TauFinal):
    __slots__ = []
    def __init__(self, name = "T2TauFinal_custom", ptCut=1.*GeV, coneCut=0.3, dZ0Max=-1.0):
        super(  T2TauFinal_custom, self ).__init__( name )
        self.T2TauTools= [ T2TauEtaPhiPtTool(), 
                           T2TauEnergyTool(), 
                           T2TauEtFlowTool(), 
                           T2TauMatchingToolConfig ( toolPtCut=ptCut, toolConeCut=coneCut ),
                           T2TauTrkRadiusToolConfig( toolPtCut=ptCut, toolConeCut=coneCut, toolDZ0Max=dZ0Max )]

        # monitoring part. To switch off do in topOption TriggerFlags.enableMonitoring = []
        from TrigT2Tau.T2TauFinalMonitoring import T2TauFinalValidationMonitoring, T2TauFinalOnlineMonitoring 
        validation = T2TauFinalValidationMonitoring()        
        online     = T2TauFinalOnlineMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        self.AthenaMonTools = [ time, validation, online ]

##################### instances to be imported by the menu ##############################
class T2TauFinal_Tau (T2TauFinal_custom):
   __slots__ = []
   def __init__(self, name = "T2TauFinal_Tau"):
      T2TauFinal_custom.__init__(self, name, ptCut=1.5*GeV, coneCut=0.3, dZ0Max=-1.0)


class T2TauFinal_Tau_dR03_1GeV_dZ02 (T2TauFinal_custom):
   __slots__ = []
   def __init__(self, name = "T2TauFinal_Tau_dR03_1GeV_dZ02"):
      T2TauFinal_custom.__init__(self, name, ptCut=1.0*GeV, coneCut=0.3, dZ0Max=2.0)
