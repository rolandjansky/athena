# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBphysHypo.TrigBphysHypoConf import TrigEFDiMuXFex

class TrigEFDiMuXFex_1 (TrigEFDiMuXFex):
    __slots__ = []
    def __init__(self, name = "TrigEFDiMuXFex_1"):
        super( TrigEFDiMuXFex, self ).__init__( name )
        
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # EF DiMuX cuts
        self.LowerMuMuMassCut =  200.
        self.UpperMuMuMassCut = 5000.
        self.LowerKplusMuMuMassCut = 5000.
        self.UpperKplusMuMuMassCut = 5600.
        
        from TrigBphysHypo.TrigEFDiMuXFexMonitoring import TrigEFDiMuXFexValidationMonitoring
        validation = TrigEFDiMuXFexValidationMonitoring()
        
        from TrigBphysHypo.TrigEFDiMuXFexMonitoring import TrigEFDiMuXFexOnlineMonitoring
        online = TrigEFDiMuXFexOnlineMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
                
        self.AthenaMonTools = [ validation, online, time ]



