# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigJetHypo.TrigEFCentralJetConf import TrigEFCentralJet

from AthenaCommon.SystemOfUnits import GeV


class EFCentralJetBase (TrigEFCentralJet):
    __slots__ = []
    def __init__(self, name):
        super( EFCentralJetBase, self ).__init__( name )

        from TrigCentralJet.TrigCentralJetMonitoring import TrigEFCentralJetValidationMonitoring, TrigEFCentralJetOnlineMonitoring, TrigEFCentralJetCosmicMonitoring
        validation = TrigEFCentralJetValidationMonitoring()
        online = TrigEFCentralJetOnlineMonitoring()
        cosmic = TrigEFCentralJetCosmicMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("EFCentralJet_Time")

        self.AthenaMonTools = [ time, validation, online, cosmic ]
        
        

class EFCentralJet (EFCentralJetBase):
    __slots__ = []
    def __init__(self, name = "EFCentralJet",eta_range=1.1):
        super( EFCentralJet, self ).__init__( name )

        self.eta_range = eta_range

