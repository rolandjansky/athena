# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigHLTJetHypo.TrigHLTJetHypoConf import TrigEFHLTHTHypo
from TrigHLTJetHypo.TrigEFHLTHTHypoMonitoring import (
    TrigEFHLTHTHypoValidationMonitoring,
    TrigEFHLTHTHypoOnlineMonitoring)

from AthenaCommon.SystemOfUnits import GeV


class EFHLTHTBase (TrigEFHLTHTHypo):
    __slots__ = []
    def __init__(self, name):
        super( EFHLTHTBase, self ).__init__( name )

        validation = TrigEFHLTHTHypoValidationMonitoring()
        online = TrigEFHLTHTHypoOnlineMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("EFHT_Time")

        self.AthenaMonTools = [ time, validation, online ]
                

class EFHLTHT (EFHLTHTBase):
    __slots__ = []
    def __init__(self,
                 name = "EFHT",
                 HT_cut=350*GeV,
                 eta_min=0.,
                 eta_max=3.2,
                 Et_cut=30*GeV,
                 doMonitoring=False):
        
        super( EFHLTHT, self ).__init__( name )

        self.HTcut = HT_cut  # cut on Sum(Et))
        self.etaMincut = eta_min  # eta cuts on  jets contributing to the ET Sum
        self.etaMaxcut = eta_max
        self.Etcut = Et_cut  # cut on jets contributing to the ET Sum
        self.doMonitoring = doMonitoring
        
##defaults:
#        self.Etcut = 30*GeV
#        self.etaMincut = 0.
#        self.etaMaxcut = 3.2 


class EFHLTHT_HAD (EFHLTHTBase):
    __slots__ = []
    def __init__(self, name = "EFHT_HAD", HT_cut=350*GeV ):
        super( EFHLTHT_HAD, self ).__init__( name )

        self.HTcut = HT_cut
        self.Etcut = 45*GeV
