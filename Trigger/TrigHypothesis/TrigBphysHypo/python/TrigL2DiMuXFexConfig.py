# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBphysHypo.TrigBphysHypoConf import TrigL2DiMuXFex

class TrigL2DiMuXFex_1 (TrigL2DiMuXFex):
    __slots__ = []
    def __init__(self, name = "TrigL2DiMuXFex_1"):
        super( TrigL2DiMuXFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # mass cuts
        self.LowerMuMuMassCut =  200.
        self.UpperMuMuMassCut = 5000.
        self.LowerKplusMuMuMassCut = 5000.
        self.UpperKplusMuMuMassCut = 5600.
        # Vertexing
        self.doVertexFit        = True
 
        from TrigBphysHypo.TrigL2DiMuXFexMonitoring import TrigL2DiMuXFexValidationMonitoring_RoI
        validation = TrigL2DiMuXFexValidationMonitoring_RoI()
        
        from TrigBphysHypo.TrigL2DiMuXFexMonitoring import TrigL2DiMuXFexOnlineMonitoring_RoI
        online = TrigL2DiMuXFexOnlineMonitoring_RoI()
        
        self.AthenaMonTools = [ validation, online ]
