# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigJetHypo.TrigJetHypoConf import TrigL2DijetAllTE
#from TrigEgammaHypo.TrigL2DielectronMassHypoMonitoring import *

class L2DijetAllTEBase (TrigL2DijetAllTE):
    __slots__ = []
    def __init__(self, name):
        super( L2DijetAllTEBase, self ).__init__(name)
        
        # AcceptAll flag: if true take events regardless of cuts
        #self.AcceptAll=False

        #from TrigJetHypo.TrigJetHypoMonitoring import TrigL2DijetAllTEValidationMonitoring, TrigL2DijetAllTEOnlineMonitoring, TrigL2DijetAllTECosmicMonitoring
        from TrigJetHypo.TrigDijetAllTEMonitoring import TrigL2DijetAllTEValidationMonitoring, TrigL2DijetAllTEOnlineMonitoring
        validation = TrigL2DijetAllTEValidationMonitoring()
        online = TrigL2DijetAllTEOnlineMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TrigL2DijetAllTE_Time")

        self.AthenaMonTools = [ validation, online, time ]

class L2DijetAllTE (L2DijetAllTEBase):
    __slots__ = []
    def __init__(self, name = "L2DijetAllTE"):
        super( L2DijetAllTE, self ).__init__( name )

