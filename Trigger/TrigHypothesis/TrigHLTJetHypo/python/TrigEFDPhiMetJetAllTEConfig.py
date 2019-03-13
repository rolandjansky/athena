# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from TrigHLTJetHypo.TrigHLTJetHypoConf import TrigEFDPhiMetJetAllTE
from TrigHLTJetHypo.TrigEFDPhiMetJetAllTEMonitoring import (
    TrigEFDPhiMetJetAllTEValidationMonitoring,
    TrigEFDPhiMetJetAllTEOnlineMonitoring)

from AthenaCommon.SystemOfUnits import GeV

class EFDPhiMetJet_Generic(TrigEFDPhiMetJetAllTE):
    __slots__ = []
    def __init__(self,
                 name = "EFDPhiMetJet_Generic",
                 dPhiCut = 0.3,
                 maxDPhiJets = 2,
                 minJetEt = 20*GeV):
        
        super( EFDPhiMetJet_Generic , self ).__init__( name )
        self.MaxDPhiJets = maxDPhiJets
        self.MinJetEt    = minJetEt
        self.MinDPhiCut  = dPhiCut
        
