# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBphysHypo.TrigBphysHypoConf import TrigEFDiMuXHypo


# basic cut
class TrigEFDiMuXHypo_1 (TrigEFDiMuXHypo):
    __slots__ = []
    def __init__(self, name = "TrigEFDiMuXHypo_1"):
        super( TrigEFDiMuXHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # EF DiMuX cuts
        self.LowerBplusMassCut = 5100.
        self.UpperBplusMassCut = 5500.
        
#        from TrigBphysHypo.TrigEFDiMuXHypoMonitoring import TrigEFDiMuXHypoValidationMonitoring
#        validation = TrigEFDiMuXHypoValidationMonitoring()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        self.AthenaMonTools = [ time ]


# basic cut
