# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#from TrigL2CosmicMuonHypo.TrigL2CosmicMuonHypoConf import CombinedIDHalfTrackMuonHypo
from TrigL2CosmicMuonHypo.TrigL2CosmicMuonHypoConf import CosmicTrtHypo
from TrigL2CosmicMuonHypo.TrigL2CosmicMuonHypoConf import CombinedIDHalfTracksHypo
from TrigL2CosmicMuonHypo.TrigL2CosmicMuonHypoConf import CombinedIDMuonHypo
from TrigL2CosmicMuonHypo.TrigL2CosmicMuonHypoConf import CosmicMuonHypo
from TrigL2CosmicMuonHypo.TrigL2CosmicMuonHypoMonitoring import *

from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
time = TrigTimeHistToolConfig("Time")

# i need to write this hypo but leave this in commented out for when i do - jamie
#class CombinedIDHalfTrackMuonHypo_Cosmic(CombinedIDHalfTrackMuonHypo):
#    __slots__ = []
#    def __init__(self, name = "CombinedIDHalfTrackMuonHypo_Cosmic"):
#        CombinedIDHalfTrackMuonHypo.__init__(self, name)
#        self.MonDir = '/EXMON/CombinedIDHalfTrackMuonHypo'
#        self.algId = 0
#        self.OutputLevel = 2


class CosmicTrtHypo_Cosmic(CosmicTrtHypo):
    __slots__ = []
    def __init__(self, name = "CosmicTrtHypo_Cosmic"):
        from __main__ import ToolSvc
        CosmicTrtHypo.__init__(self, name)
        self.OutputLevel = 3
        self.NTrthitsCut = 10
        self.NTrtSegs = 1
        self.TrtSegD0Cut = 100000.0
        self.AthenaMonTools = [
            time,
           CosmicTrtHypoCosmicMonitoring(),
           #TrigTimeHistToolConfig('Time')
           ]
        
class CombinedIDHalfTracksHypo_Cosmic(CombinedIDHalfTracksHypo):
    __slots__ = []
    def __init__(self, name = "CombinedIDHalfTracksHypo_Cosmic"):
        CombinedIDHalfTracksHypo.__init__(self, name)
        self.OutputLevel = 3
        self.CombIDNtracks = 1 
        self.CombID_DeltaPhi0Cut = 0.1
        self.CombID_DeltaEtaCut = 0.1
        self.AthenaMonTools = [
            CombinedIDHTHypoCosmicMonitoring(),
            TrigTimeHistToolConfig('Time')
            ]
        
        
class CombinedIDMuonHypo_Cosmic(CombinedIDMuonHypo):
    __slots__ = []
    def __init__(self, name = "CombinedIDMuonHypo_Cosmic"):
        CombinedIDMuonHypo.__init__(self, name)
        self.OutputLevel = 3
        self.CombIDMuNtracks = 1 
        self.CombIDMu_DeltaPhi0Cut = 0.1
        self.CombIDMu_DeltaEtaCut = 0.1
        self.CombIDMu_DeltaThetaCut = 0.1
        self.AthenaMonTools = [
            CombinedIDMuonHypoCosmicMonitoring(),
            TrigTimeHistToolConfig('Time')
            ]
        

class CosmicMuonHypo_Cosmic(CosmicMuonHypo):
    __slots__ = []
    def __init__(self, name = "CosmicMuonHypo_Cosmic"):
        CosmicMuonHypo.__init__(self, name)
        # self.OutputLevel = 3
        self.NMuons = 1
        self.NRpcHits = 2
        self.NMdtStations = 1
        self.UpDownDeltaEta = -1.0
        self.UpDownDeltaPhi = -1.0 
        self.UpDownTiming = 0.0
        self.AthenaMonTools = [
            CosmicMuonHypoCosmicMonitoring(),
            TrigTimeHistToolConfig('Time')
            ]

class CosmicMuonHypo_SelectID(CosmicMuonHypo):
    __slots__ = []
    def __init__(self, name = "CosmicMuonHypo_Cosmic"):
        CosmicMuonHypo.__init__(self, name)
        # self.OutputLevel = 3
        self.NMuons = 1
        self.NRpcHits = 2
        self.NMdtStations = 1
        self.MuonDCA_XY = 100
        self.UpDownDeltaEta = -1.0
        self.UpDownDeltaPhi = -1.0 
        self.UpDownTiming = 0.0
        self.AthenaMonTools = [
            CosmicMuonHypoCosmicMonitoring('CosmicMuonHypo_SelectID'),
            TrigTimeHistToolConfig('Time')
            ]


        

