# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigHLTJetHypo.TrigHLTJetHypoConf import TrigEFHLTJetMassDEta

from TrigHLTJetHypo.TrigEFHLTJetMassDEtaMonitoring import (
    TrigEFHLTJetMassDEtaValidationMonitoring,
    TrigEFHLTJetMassDEtaOnlineMonitoring)

from AthenaCommon.SystemOfUnits import GeV


class EFHLTJetMassDEtaBase (TrigEFHLTJetMassDEta):
    __slots__ = []
    def __init__(self, name):
        super( EFHLTJetMassDEtaBase, self ).__init__( name )

        validation = TrigEFHLTJetMassDEtaValidationMonitoring()
        online = TrigEFHLTJetMassDEtaOnlineMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("EFHLTJetMassDEta_Time")

        self.AthenaMonTools = [ time, validation, online ]


class EFHLTJetMassDEta (EFHLTJetMassDEtaBase):
    __slots__ = []
    def __init__(self, name = "EFHLTJetMassDEta", mjj_cut=200*GeV , deta_cut=2.0):
        super( EFHLTJetMassDEta, self ).__init__( name )

        self.MjjCut = mjj_cut
        self.DEtaCut = deta_cut

class EFHLTJetMassDEta2J7 (EFHLTJetMassDEtaBase):
    __slots__ = []
    def __init__(self, name = "EFJetMassDEta2J7", mjj_cut=200*GeV , deta_cut=2.0):
        super( EFHLTJetMassDEta2J7, self ).__init__( name )

        self.MjjCut = mjj_cut
        self.DEtaCut = deta_cut
        self.EtCut = 7*GeV

class EFHLTJetMassDEta2J10 (EFHLTJetMassDEtaBase):
    __slots__ = []
    def __init__(self, name = "EFHLTJetMassDEta2J10", mjj_cut=200*GeV , deta_cut=2.0):
        super( EFHLTJetMassDEta2J10, self ).__init__( name )

        self.MjjCut = mjj_cut
        self.DEtaCut = deta_cut
        self.EtCut = 10*GeV

class EFHLTJetMassDEta2J25 (EFHLTJetMassDEtaBase):
    __slots__ = []
    def __init__(self, name = "EFHLTJetMassDEta2J25", mjj_cut=200*GeV , deta_cut=2.0):
        super( EFHLTJetMassDEta2J25, self ).__init__( name )

        self.MjjCut = mjj_cut
        self.DEtaCut = deta_cut
        self.EtCut = 25*GeV

class EFHLTJetMassDEta2J30 (EFHLTJetMassDEtaBase):
    __slots__ = []
    def __init__(self, name = "EFHLTJetMassDEta2J30", mjj_cut=200*GeV , deta_cut=2.0):
        super( EFHLTJetMassDEta2J30, self ).__init__( name )

        self.MjjCut = mjj_cut
        self.DEtaCut = deta_cut
        self.EtCut = 30*GeV

class EFHLTJetMassDEta2J35 (EFHLTJetMassDEtaBase):
    __slots__ = []
    def __init__(self, name = "EFHLTJetMassDEta2J35", mjj_cut=200*GeV , deta_cut=2.0):
        super( EFHLTJetMassDEta2J35, self ).__init__( name )

        self.MjjCut = mjj_cut
        self.DEtaCut = deta_cut
        self.EtCut = 35*GeV

class EFHLTJetMassDEta2J40 (EFHLTJetMassDEtaBase):
    __slots__ = []
    def __init__(self, name = "EFHLTJetMassDEta2J40", mjj_cut=200*GeV , deta_cut=2.0):
        super( EFHLTJetMassDEta2J40, self ).__init__( name )

        self.MjjCut = mjj_cut
        self.DEtaCut = deta_cut
        self.EtCut = 40*GeV
