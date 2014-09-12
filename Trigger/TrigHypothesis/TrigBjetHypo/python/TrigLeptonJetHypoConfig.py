# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBjetHypo.TrigBjetHypoConf import TrigLeptonJetHypo

         

class L2LeptonJetHypo (TrigLeptonJetHypo):
    __slots__ = []
    def __init__(self, name = "L2LeptonJetHypo"):
        super( L2LeptonJetHypo, self ).__init__( name )

        self.AcceptAll    = False
        self.deltaRCut    = 0.4
        self.Instance     = "L2"
        self.LVL2Refining = True

        from TrigBjetHypo.TrigLeptonJetHypoMonitoring import TrigL2LeptonJetHypoValidationMonitoring, TrigL2LeptonJetHypoOnlineMonitoring
        validation = TrigL2LeptonJetHypoValidationMonitoring()        
        online     = TrigL2LeptonJetHypoOnlineMonitoring()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TimeHistogramForTrigLeptonJetHypo")
        time.TimerHistLimits = [0,0.2]

        self.AthenaMonTools = [ time, validation, online ]



class EFLeptonJetHypo (TrigLeptonJetHypo):
    __slots__ = []
    def __init__(self, name = "EFLeptonJetHypo"):
        super( EFLeptonJetHypo, self ).__init__( name )

        self.AcceptAll    = False
        self.deltaRCut    = 0.4
        self.Instance     = "EF"
        self.LVL2Refining = True

        from TrigBjetHypo.TrigLeptonJetHypoMonitoring import TrigEFLeptonJetHypoValidationMonitoring, TrigEFLeptonJetHypoOnlineMonitoring
        validation = TrigEFLeptonJetHypoValidationMonitoring()        
        online     = TrigEFLeptonJetHypoOnlineMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TimeHistogramForTrigLeptonJetHypo")
        time.TimerHistLimits = [0,0.2]

        self.AthenaMonTools = [ time, validation, online ]
