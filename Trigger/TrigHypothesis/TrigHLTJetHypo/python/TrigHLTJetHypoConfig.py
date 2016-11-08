# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# from TrigHLTJetHypo.TrigHLTJetHypoConf import TrigHLTJetHypo
import  TrigHLTJetHypoConf

# modules to decide whether to add monitoring plots
# this is chain dependent.
from JetCleanMonitoring import JetChainsToKeepMonitoring
from TriggerMenu.menu.CleanMonitoring import KeepMonitoring
# from TriggerMenu.menu.CleanMonitoring import DisableMonitoringButValAndTime

from TrigHLTJetHypo.TrigHLTJetHypo2Monitoring import (
    TrigHLTJetHypo2ValidationMonitoring,
    TrigHLTJetHypo2OnlineMonitoring,
    TrigHLTJetHypo2CosmicMonitoring)

from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig


class TrigHLTJetHypo2 (TrigHLTJetHypoConf.TrigHLTJetHypo2):
    __slots__ = []
    def __init__(self, name, **kwargs):
        super( TrigHLTJetHypo2, self ).__init__( name, **kwargs )

        if  KeepMonitoring(self.chain_name,
                           JetChainsToKeepMonitoring,
                           strictComparison=True):
            validation = TrigHLTJetHypo2ValidationMonitoring()
            online = TrigHLTJetHypo2OnlineMonitoring()
            cosmic = TrigHLTJetHypo2CosmicMonitoring()
            
            time = TrigTimeHistToolConfig("HLTJetHypo2_Time")
            
            self.AthenaMonTools = [ time, validation, online, cosmic ]
