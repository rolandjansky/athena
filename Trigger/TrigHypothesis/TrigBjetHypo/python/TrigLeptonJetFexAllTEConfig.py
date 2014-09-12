# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBjetHypo.TrigBjetHypoConf import TrigLeptonJetFexAllTE

from AthenaCommon.SystemOfUnits import GeV



class EFLeptonJetFexAllTE (TrigLeptonJetFexAllTE):
    __slots__ = []
    def __init__(self, name = "EFLeptonJetFexAllTE"):
        super( EFLeptonJetFexAllTE, self ).__init__( name )
       
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TimeHistogramForTrigEFLeptonJetFexAllTE")
        time.TimerHistLimits = [0,0.2]

        self.AthenaMonTools = [ time ]



class EFLeptonJetFexAllTE_CloseBy (EFLeptonJetFexAllTE):
    __slots__ = []
    def __init__(self, name = "EFLeptonJetFexAllTE_CloseBy"):
        super( EFLeptonJetFexAllTE_CloseBy, self ).__init__( name )

        self.WorkingMode = 1
        self.DeltaRCut   = 0.4
        self.EtThreshold = 10*GeV

        from TrigBjetHypo.TrigLeptonJetFexAllTEMonitoring import TrigEFLeptonJetFexAllTEValidationMonitoring_CloseBy, TrigEFLeptonJetFexAllTEOnlineMonitoring_CloseBy
        validation = TrigEFLeptonJetFexAllTEValidationMonitoring_CloseBy()
        online     = TrigEFLeptonJetFexAllTEOnlineMonitoring_CloseBy()

        self.AthenaMonTools += [ validation, online ]



class EFLeptonJetFexAllTE_FarOff (EFLeptonJetFexAllTE):
    __slots__ = []
    def __init__(self, name = "EFLeptonJetFexAllTE_FarOff"):
        super( EFLeptonJetFexAllTE_FarOff, self ).__init__( name )

        self.WorkingMode = 2
        self.DeltaRCut   = 0.2
        self.EtThreshold = 30*GeV

        from TrigBjetHypo.TrigLeptonJetFexAllTEMonitoring import TrigEFLeptonJetFexAllTEValidationMonitoring_FarOff, TrigEFLeptonJetFexAllTEOnlineMonitoring_FarOff
        validation = TrigEFLeptonJetFexAllTEValidationMonitoring_FarOff()
        online     = TrigEFLeptonJetFexAllTEOnlineMonitoring_FarOff()

        self.AthenaMonTools += [ validation, online ]


