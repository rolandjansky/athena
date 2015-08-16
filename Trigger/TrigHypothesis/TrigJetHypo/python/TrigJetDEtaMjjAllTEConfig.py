# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigJetHypo.TrigJetHypoConf import TrigL2DEtaMjjAllTE

class L2JetDEtaMjjAllTEBase (TrigL2DEtaMjjAllTE):
    __slots__ = []
    def __init__(self, name):
        super( L2JetDEtaMjjAllTEBase, self ).__init__(name)
        
        from TrigJetHypo.TrigJetDEtaMjjAllTEMonitoring import TrigL2JetDEtaMjjAllTEValidationMonitoring, TrigL2JetDEtaMjjAllTEOnlineMonitoring
        validation = TrigL2JetDEtaMjjAllTEValidationMonitoring()
        online = TrigL2JetDEtaMjjAllTEOnlineMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TrigL2JetDEtaMjjAllTE_Time")

        self.AthenaMonTools = [ validation, online, time ]

class L2JetDEtaMjjAllTE (L2JetDEtaMjjAllTEBase):
    __slots__ = []
    def __init__(self, name = "L2JetDEtaMjjAllTE"):
        super( L2JetDEtaMjjAllTE, self ).__init__( name )

class L2JetDEtaMjjAllTE2J15 (L2JetDEtaMjjAllTEBase):
    __slots__ = []
    def __init__(self, name = "L2JetDEtaMjjAllTE2J15", deta_cut=2.5, mjj_cut=0.):
        super( L2JetDEtaMjjAllTE2J15, self ).__init__( name )
        self.EtCut = 15000
        self.MjjCut = mjj_cut
        self.dEtaCut = deta_cut
        #self.jetInputKey = 

