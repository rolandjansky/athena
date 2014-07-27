# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigJetHypo.TrigJetHypoConf import TrigEFHTHypo
from TrigJetHypo.TrigEFHTHypoMonitoring import TrigEFHTHypoValidationMonitoring, TrigEFHTHypoOnlineMonitoring

from AthenaCommon.SystemOfUnits import GeV


class EFHTBase (TrigEFHTHypo):
    __slots__ = []
    def __init__(self, name):
        super( EFHTBase, self ).__init__( name )

        validation = TrigEFHTHypoValidationMonitoring()
        online = TrigEFHTHypoOnlineMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("EFHT_Time")

        self.AthenaMonTools = [ time, validation, online ]
                

class EFHT (EFHTBase):
    __slots__ = []
    def __init__(self, name = "EFHT", HT_cut=350*GeV ):
        super( EFHT, self ).__init__( name )

        self.HTcut = HT_cut
##defaults:
#        self.Etcut = 30*GeV
#        self.etaMincut = 0.
#        self.etaMaxcut = 3.2 


class EFHT_HAD (EFHTBase):
    __slots__ = []
    def __init__(self, name = "EFHT_HAD", HT_cut=350*GeV ):
        super( EFHT_HAD, self ).__init__( name )

        self.HTcut = HT_cut
        self.Etcut = 45*GeV
