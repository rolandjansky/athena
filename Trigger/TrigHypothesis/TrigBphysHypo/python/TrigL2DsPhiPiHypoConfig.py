# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBphysHypo.TrigBphysHypoConf import TrigL2DsPhiPiHypo

####
#### !!!!!!!!!!!!!!!!!!!!!!!!!!
# we have to do something with this, where was this defined before?
#from AthenaCommon.SystemOfUnits import GeV

# basic cut
class L2DsPhiPiHypo_1 (TrigL2DsPhiPiHypo):
    __slots__ = []
    def __init__(self, name = "L2DsPhiPiHypo_1"):
        super( TrigL2DsPhiPiHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 DsPhiPi cuts
        self.EtaBins             = [0.,1.0,1.5]
        self.PhiMasslow_cut      = [1005.,1003.,1002.]
        self.PhiMasshigh_cut     = [1035.,1037.,1037.]
        self.DsMasslow_cut       = [1887.,1870.,1863.]
        self.DsMasshigh_cut      = [2047.,2062.,2071.]
        self.useVertexFit        = False

        from TrigBphysHypo.TrigL2DsPhiPiHypoMonitoring import TrigL2DsPhiPiHypoValidationMonitoring
        validation = TrigL2DsPhiPiHypoValidationMonitoring()
        
        from TrigBphysHypo.TrigL2DsPhiPiHypoMonitoring import TrigL2DsPhiPiHypoOnlineMonitoring
        online = TrigL2DsPhiPiHypoOnlineMonitoring()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
                
        self.AthenaMonTools = [ validation, online, time ]


# full scan
class L2DsPhiPiHypo_FS (TrigL2DsPhiPiHypo):
    __slots__ = []
    def __init__(self, name = "L2DsPhiPiHypo_FS"):
        super( TrigL2DsPhiPiHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 DsPhiPi cuts
        self.EtaBins             = [0.,1.0,1.5]
        self.PhiMasslow_cut      = [1005.,1003.,1002.]
        self.PhiMasshigh_cut     = [1035.,1037.,1037.]
        self.DsMasslow_cut       = [1887.,1870.,1863.]
        self.DsMasshigh_cut      = [2047.,2062.,2071.]
        self.useVertexFit        = False

 
        from TrigBphysHypo.TrigL2DsPhiPiHypoMonitoring import TrigL2DsPhiPiHypoValidationMonitoring
        validation = TrigL2DsPhiPiHypoValidationMonitoring()
        
        from TrigBphysHypo.TrigL2DsPhiPiHypoMonitoring import TrigL2DsPhiPiHypoOnlineMonitoring
        online = TrigL2DsPhiPiHypoOnlineMonitoring()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
                
        self.AthenaMonTools = [ validation, online, time ]
