# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigJetHypo.TrigJetHypoConf import TrigEFJetDphiHypo
from TrigJetHypo.TrigEFJetDphiHypoMonitoring import TrigEFJetDphiHypoValidationMonitoring, TrigEFJetDphiHypoOnlineMonitoring

from AthenaCommon.SystemOfUnits import GeV


class EFJetDphiBase (TrigEFJetDphiHypo):
    __slots__ = []
    def __init__(self, name):
        super( EFJetDphiBase, self ).__init__( name )

        validation = TrigEFJetDphiHypoValidationMonitoring()
        online = TrigEFJetDphiHypoOnlineMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("EFJetDphi_Time")

        self.AthenaMonTools = [ time, validation, online ]
        

        


class EFJetDphi04 (EFJetDphiBase):
    __slots__ = []
    def __init__(self, name = "EFJetDphi04", dphi_cut=0.4 ):
        super( EFJetDphi04, self ).__init__( name )

        self.DphiCut = dphi_cut

class EFJetDphi04_had (EFJetDphiBase):
    __slots__ = []
    def __init__(self, name = "EFJetDphi04_had", dphi_cut=0.4 ):
        super( EFJetDphi04_had, self ).__init__( name )

        self.DphiCut = dphi_cut
        self.Etcut1 = 45*GeV
        self.Etcut2 = 45*GeV
