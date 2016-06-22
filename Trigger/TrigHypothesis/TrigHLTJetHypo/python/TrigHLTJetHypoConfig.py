# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigHLTJetHypo.TrigHLTJetHypoConf import TrigHLTJetHypo

class TrigHLTJetHypoMon (TrigHLTJetHypo):
    __slots__ = []
    def __init__(self, name, **kwargs):
        super( TrigHLTJetHypoMon, self ).__init__( name, **kwargs )
        
        from TrigHLTJetHypo.TrigHLTJetHypoMonitoring import TrigHLTJetHypoValidationMonitoring, TrigHLTJetHypoOnlineMonitoring, TrigHLTJetHypoCosmicMonitoring
        validation = TrigHLTJetHypoValidationMonitoring()
        online = TrigHLTJetHypoOnlineMonitoring()
        cosmic = TrigHLTJetHypoCosmicMonitoring()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("HLTJetHypo_Time")
        
        self.AthenaMonTools = [ time, validation, online, cosmic ]
