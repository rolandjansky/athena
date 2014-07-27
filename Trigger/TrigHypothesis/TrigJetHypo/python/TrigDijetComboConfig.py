# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigJetHypo.TrigJetHypoConf import TrigL2DijetCombo
#from TrigEgammaHypo.TrigL2DielectronMassHypoMonitoring import *

class L2DijetComboBase (TrigL2DijetCombo):
    __slots__ = []
    def __init__(self, name):
        super( L2DijetComboBase, self ).__init__(name)
        
        # AcceptAll flag: if true take events regardless of cuts
        #self.AcceptAll=False

        #from TrigJetHypo.TrigJetHypoMonitoring import TrigL2DijetComboValidationMonitoring, TrigL2DijetComboOnlineMonitoring, TrigL2DijetComboCosmicMonitoring
        from TrigJetHypo.TrigDijetComboMonitoring import TrigL2DijetComboValidationMonitoring, TrigL2DijetComboOnlineMonitoring
        validation = TrigL2DijetComboValidationMonitoring()
        online = TrigL2DijetComboOnlineMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TrigL2DijetCombo_Time")

        self.AthenaMonTools = [ validation, online, time]

class L2DijetCombo (L2DijetComboBase):
    __slots__ = []
    def __init__(self, name = "L2DijetCombo"):
        super( L2DijetCombo, self ).__init__( name )

