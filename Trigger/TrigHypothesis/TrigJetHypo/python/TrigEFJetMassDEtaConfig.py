# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigJetHypo.TrigJetHypoConf import TrigEFJetMassDEta
from TrigJetHypo.TrigEFJetMassDEtaMonitoring import TrigEFJetMassDEtaValidationMonitoring, TrigEFJetMassDEtaOnlineMonitoring

from AthenaCommon.SystemOfUnits import GeV


class EFJetMassDEtaBase (TrigEFJetMassDEta):
    __slots__ = []
    def __init__(self, name):
        super( EFJetMassDEtaBase, self ).__init__( name )

        validation = TrigEFJetMassDEtaValidationMonitoring()
        online = TrigEFJetMassDEtaOnlineMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("EFJetMassDEta_Time")

        self.AthenaMonTools = [ time, validation, online ]


class EFJetMassDEta (EFJetMassDEtaBase):
    __slots__ = []
    def __init__(self, name = "EFJetMassDEta", mjj_cut=200*GeV , deta_cut=2.0):
        super( EFJetMassDEta, self ).__init__( name )

        self.MjjCut = mjj_cut
        self.DEtaCut = deta_cut

class EFJetMassDEta2J7 (EFJetMassDEtaBase):
    __slots__ = []
    def __init__(self, name = "EFJetMassDEta2J7", mjj_cut=200*GeV , deta_cut=2.0):
        super( EFJetMassDEta2J7, self ).__init__( name )

        self.MjjCut = mjj_cut
        self.DEtaCut = deta_cut
        self.EtCut = 7*GeV

class EFJetMassDEta2J10 (EFJetMassDEtaBase):
    __slots__ = []
    def __init__(self, name = "EFJetMassDEta2J10", mjj_cut=200*GeV , deta_cut=2.0):
        super( EFJetMassDEta2J10, self ).__init__( name )

        self.MjjCut = mjj_cut
        self.DEtaCut = deta_cut
        self.EtCut = 10*GeV

class EFJetMassDEta2J25 (EFJetMassDEtaBase):
    __slots__ = []
    def __init__(self, name = "EFJetMassDEta2J25", mjj_cut=200*GeV , deta_cut=2.0):
        super( EFJetMassDEta2J25, self ).__init__( name )

        self.MjjCut = mjj_cut
        self.DEtaCut = deta_cut
        self.EtCut = 25*GeV

class EFJetMassDEta2J30 (EFJetMassDEtaBase):
    __slots__ = []
    def __init__(self, name = "EFJetMassDEta2J30", mjj_cut=200*GeV , deta_cut=2.0):
        super( EFJetMassDEta2J30, self ).__init__( name )

        self.MjjCut = mjj_cut
        self.DEtaCut = deta_cut
        self.EtCut = 30*GeV

class EFJetMassDEta2J35 (EFJetMassDEtaBase):
    __slots__ = []
    def __init__(self, name = "EFJetMassDEta2J35", mjj_cut=200*GeV , deta_cut=2.0):
        super( EFJetMassDEta2J35, self ).__init__( name )

        self.MjjCut = mjj_cut
        self.DEtaCut = deta_cut
        self.EtCut = 35*GeV

class EFJetMassDEta2J40 (EFJetMassDEtaBase):
    __slots__ = []
    def __init__(self, name = "EFJetMassDEta2J40", mjj_cut=200*GeV , deta_cut=2.0):
        super( EFJetMassDEta2J40, self ).__init__( name )

        self.MjjCut = mjj_cut
        self.DEtaCut = deta_cut
        self.EtCut = 40*GeV
