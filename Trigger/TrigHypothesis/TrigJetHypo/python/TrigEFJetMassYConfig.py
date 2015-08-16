# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#from TrigJetHypo.TrigJetHypoConf import TrigEFJetMctHypo
from TrigJetHypo.TrigJetHypoConf import TrigEFJetMassY
from TrigJetHypo.TrigEFJetMassYMonitoring import TrigEFJetMassYValidationMonitoring, TrigEFJetMassYOnlineMonitoring

from AthenaCommon.SystemOfUnits import GeV


class EFJetMassYBase (TrigEFJetMassY):
    __slots__ = []
    def __init__(self, name):
        super( EFJetMassYBase, self ).__init__( name )

        validation = TrigEFJetMassYValidationMonitoring()
        online = TrigEFJetMassYOnlineMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("EFJetMassY_Time")

        self.AthenaMonTools = [ time, validation, online ]


class EFJetMassY (EFJetMassYBase):
    __slots__ = []
    def __init__(self, name = "EFJetMassY", mjj_cut=2000*GeV , ystar_cut=1.7):
        super( EFJetMassY, self ).__init__( name )

        self.Masscut = mjj_cut
        self.Ystarcut = ystar_cut


