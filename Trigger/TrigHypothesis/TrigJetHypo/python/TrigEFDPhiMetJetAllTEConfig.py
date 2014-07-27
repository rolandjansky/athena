# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigJetHypo.TrigJetHypoConf import TrigEFDPhiMetJetAllTE
from TrigJetHypo.TrigEFDPhiMetJetAllTEMonitoring import TrigEFDPhiMetJetAllTEValidationMonitoring, TrigEFDPhiMetJetAllTEOnlineMonitoring 

from AthenaCommon.SystemOfUnits import GeV

class TrigEFDPhiMetJetAllTE(TrigEFDPhiMetJetAllTE):
    __slots__ = []
    def __init__(self, name = "TrigEFDPhiMetJetAllTE"):
        super(TrigEFDPhiMetJetAllTE, self).__init__( name )
        validation = TrigEFDPhiMetJetAllTEValidationMonitoring()
        online = TrigEFDPhiMetJetAllTEOnlineMonitoring()
        self.AthenaMonTools = [ validation, online]

class EFDPhiMetJet_2J20(TrigEFDPhiMetJetAllTE):
    __slots__ = []
    def __init__(self, name = "EFDPhiMetJet_2J20", dPhiCut = 0.3):
        super( EFDPhiMetJet_2J20 , self ).__init__( name )
        self.MaxDPhiJets = 2
        self.MinJetEt    = 20*GeV
        self.MinDPhiCut  = dPhiCut

class EFDPhiMetJet_2J30(TrigEFDPhiMetJetAllTE):
    __slots__ = []
    def __init__(self, name = "EFDPhiMetJet_2J30", dPhiCut = 0.3):
        super( EFDPhiMetJet_2J30 , self ).__init__( name )
        self.MaxDPhiJets = 2
        self.MinJetEt    = 30*GeV
        self.MinDPhiCut  = dPhiCut

class EFDPhiMetJet_2J45(TrigEFDPhiMetJetAllTE):
    __slots__ = []
    def __init__(self, name = "EFDPhiMetJet_2J45", dPhiCut = 0.3):
        super( EFDPhiMetJet_2J45 , self ).__init__( name )
        self.MaxDPhiJets = 2
        self.MinJetEt    = 45*GeV
        self.MinDPhiCut  = dPhiCut

class EFDPhiMetJet_2J10(TrigEFDPhiMetJetAllTE):
    __slots__ = []
    def __init__(self, name = "EFDPhiMetJet_2J10", dPhiCut = 0.3):
        super( EFDPhiMetJet_2J10 , self ).__init__( name )
        self.MaxDPhiJets = 2
        self.MinJetEt    = 10*GeV
        self.MinDPhiCut  = dPhiCut

class EFDPhiMetJet_2J15(TrigEFDPhiMetJetAllTE):
    __slots__ = []
    def __init__(self, name = "EFDPhiMetJet_2J15", dPhiCut = 0.3):
        super( EFDPhiMetJet_2J15 , self ).__init__( name )
        self.MaxDPhiJets = 2
        self.MinJetEt    = 15*GeV
        self.MinDPhiCut  = dPhiCut

class EFDPhiMetJet_100J10(TrigEFDPhiMetJetAllTE):
    __slots__ = []
    def __init__(self, name = "EFDPhiMetJet_100J10", dPhiCut = 0.7):
        super( EFDPhiMetJet_100J10 , self ).__init__( name )
        self.MaxDPhiJets = 100
        self.MinJetEt    = 10*GeV
        self.MinDPhiCut  = dPhiCut

        
class EFDPhiMetJet_Generic(TrigEFDPhiMetJetAllTE):
    __slots__ = []
    def __init__(self, name = "EFDPhiMetJet_Generic", dPhiCut = 0.3, maxDPhiJets = 2, minJetEt = 20*GeV):
        super( EFDPhiMetJet_Generic , self ).__init__( name )
        self.MaxDPhiJets = maxDPhiJets
        self.MinJetEt    = minJetEt
        self.MinDPhiCut  = dPhiCut
        
