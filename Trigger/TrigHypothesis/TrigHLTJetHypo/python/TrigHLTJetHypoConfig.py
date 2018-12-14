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

from GaudiKernel.Constants import (VERBOSE,
                                   DEBUG,
                                   INFO,
                                   WARNING,
                                   ERROR,
                                   FATAL,)
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


class TrigHLTJetHypo_DijetMassDEta (
        TrigHLTJetHypoConf.TrigHLTJetHypo_DijetMassDEta):
    __slots__ = []
    def __init__(self, name, **kwargs):
        super( TrigHLTJetHypo_DijetMassDEta, self ).__init__( name, **kwargs )

        if  KeepMonitoring(self.chain_name,
                           JetChainsToKeepMonitoring,
                           strictComparison=True):
            validation = TrigHLTJetHypo2ValidationMonitoring()
            online = TrigHLTJetHypo2OnlineMonitoring()
            cosmic = TrigHLTJetHypo2CosmicMonitoring()
            
            time = TrigTimeHistToolConfig("HLTJetHypo_DijetDEta_Time")
            
            self.AthenaMonTools = [ time, validation, online, cosmic ]


class TrigHLTJetHypo_DijetMassDEtaDPhi (
        TrigHLTJetHypoConf.TrigHLTJetHypo_DijetMassDEtaDPhi):
    __slots__ = []
    def __init__(self, name, **kwargs):
        super( TrigHLTJetHypo_DijetMassDEtaDPhi, self ).__init__( name, **kwargs )

        if  KeepMonitoring(self.chain_name,
                           JetChainsToKeepMonitoring,
                           strictComparison=True):
            validation = TrigHLTJetHypo2ValidationMonitoring()
            online = TrigHLTJetHypo2OnlineMonitoring()
            cosmic = TrigHLTJetHypo2CosmicMonitoring()
            
            time = TrigTimeHistToolConfig("HLTJetHypo_DijetDEtaDPhi_Time")
            
            self.AthenaMonTools = [ time, validation, online, cosmic ]


class TrigHLTJetHypo_EtaEt (TrigHLTJetHypoConf.TrigHLTJetHypo_EtaEt):
    __slots__ = []
    def __init__(self, name, **kwargs):
        super( TrigHLTJetHypo_EtaEt, self ).__init__( name, **kwargs )

        if  KeepMonitoring(self.chain_name,
                           JetChainsToKeepMonitoring,
                           strictComparison=True):
            validation = TrigHLTJetHypo2ValidationMonitoring()
            online = TrigHLTJetHypo2OnlineMonitoring()
            cosmic = TrigHLTJetHypo2CosmicMonitoring()
            
            time = TrigTimeHistToolConfig("HLTJetHypo_EtaEt_Time")
            
            self.AthenaMonTools = [ time, validation, online, cosmic ]


class TrigHLTJetHypo_SMC (TrigHLTJetHypoConf.TrigHLTJetHypo_SMC):
    __slots__ = []
    def __init__(self, name, **kwargs):
        super( TrigHLTJetHypo_SMC, self ).__init__( name, **kwargs )

        if  KeepMonitoring(self.chain_name,
                           JetChainsToKeepMonitoring,
                           strictComparison=True):
            validation = TrigHLTJetHypo2ValidationMonitoring()
            online = TrigHLTJetHypo2OnlineMonitoring()
            cosmic = TrigHLTJetHypo2CosmicMonitoring()
            
            time = TrigTimeHistToolConfig("HLTJetHypo_SMC_Time")
            
            self.AthenaMonTools = [ time, validation, online, cosmic ]


class TrigHLTJetHypo_HT (TrigHLTJetHypoConf.TrigHLTJetHypo_HT):
    __slots__ = []
    def __init__(self, name, **kwargs):
        super( TrigHLTJetHypo_HT, self ).__init__( name, **kwargs )

        if  KeepMonitoring(self.chain_name,
                           JetChainsToKeepMonitoring,
                           strictComparison=True):
            validation = TrigHLTJetHypo2ValidationMonitoring()
            online = TrigHLTJetHypo2OnlineMonitoring()
            cosmic = TrigHLTJetHypo2CosmicMonitoring()
            
            time = TrigTimeHistToolConfig("HLTJetHypo_HT_Time")
            
            self.AthenaMonTools = [ time, validation, online, cosmic ]


class TrigHLTJetHypo_TLA (TrigHLTJetHypoConf.TrigHLTJetHypo_TLA):
    __slots__ = []
    def __init__(self, name, **kwargs):
        super( TrigHLTJetHypo_TLA, self ).__init__( name, **kwargs )

        if  KeepMonitoring(self.chain_name,
                           JetChainsToKeepMonitoring,
                           strictComparison=True):
            validation = TrigHLTJetHypo2ValidationMonitoring()
            online = TrigHLTJetHypo2OnlineMonitoring()
            cosmic = TrigHLTJetHypo2CosmicMonitoring()
            
            time = TrigTimeHistToolConfig("HLTJetHypo_TLA_Time")
            
            self.AthenaMonTools = [ time, validation, online, cosmic ]




class TrigHLTJetHypo_Dijet (TrigHLTJetHypoConf.TrigHLTJetHypo_Dijet):
    __slots__ = []
    def __init__(self, name, **kwargs):
        super( TrigHLTJetHypo_Dijet, self ).__init__( name, **kwargs )

        
        if  KeepMonitoring(self.chain_name,
                           JetChainsToKeepMonitoring,
                           strictComparison=True):
            validation = TrigHLTJetHypo2ValidationMonitoring()
            online = TrigHLTJetHypo2OnlineMonitoring()
            cosmic = TrigHLTJetHypo2CosmicMonitoring()
            
            time = TrigTimeHistToolConfig("HLTJetHypo_Dijet_Time")
            
            self.AthenaMonTools = [ time, validation, online, cosmic ]
