# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBphysHypo.TrigBphysHypoConf import TrigL2JpsieeFex
from TrigVertexFitter.TrigVertexFitterConf import TrigL2VertexFitter
from TrigVertexFitter.TrigVertexFitterConf import TrigVertexFitter
from TrigVertexFitter.TrigVertexFitterConf import TrigVertexingTool

L2VFitTool = TrigL2VertexFitter()
VFitTool = TrigVertexFitter()
VertexTool = TrigVertexingTool()

from AthenaCommon.AppMgr import ToolSvc

ToolSvc += L2VFitTool
ToolSvc += VFitTool
ToolSvc += VertexTool


# basic cut
class L2JpsieeFex_1 (TrigL2JpsieeFex):
    __slots__ = []
    def __init__(self, name = "L2JpsieeFex_1"):
        super( TrigL2JpsieeFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Jpsiee cuts
#        self.TrackAlgoId          = 2
        self.dEtaTrackPair        = 1.2
        self.dz0TrackPair         = 1.2
        self.pTsumTrackPair       = 4.
        self.JpsiMasslow          = 1500.
        self.JpsiMasshigh         = 3200.
        self.doVertexFit          = True
        self.TrigL2VertexFitter   = L2VFitTool
        self.TrigVertexFitter     = VFitTool
        self.TrigVertexingTool    = VertexTool
        self.dEtaClusRoI          = 1.5
        self.dPhiClusRoI          = 1.5
        self.TrackPTthr           = 1000.
        self.CaloTrackdEoverPLow  = 0.5
        self.CaloTrackdEoverPHigh = 2.5
        self.CaloTrackdETA        = 0.1
        self.CaloTrackdPHI        = 0.1
        self.TRTcut               = 0.1

        from TrigBphysHypo.TrigL2JpsieeFexMonitoring import TrigL2JpsieeFexValidationMonitoring
        validation = TrigL2JpsieeFexValidationMonitoring()

        from TrigBphysHypo.TrigL2JpsieeFexMonitoring import TrigL2JpsieeFexOnlineMonitoring
        online = TrigL2JpsieeFexOnlineMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
                
        self.AthenaMonTools = [ validation, online, time ]


# basic cut
class L2JpsieeFex_FS (TrigL2JpsieeFex):
    __slots__ = []
    def __init__(self, name = "L2JpsieeFex_FS"):
        super( TrigL2JpsieeFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Jpsiee cuts
#        self.TrackAlgoId          = 2
        self.dEtaTrackPair        = 1.2
        self.dz0TrackPair         = 1.2
        self.pTsumTrackPair       = 4.
        self.JpsiMasslow          = 1500.
        self.JpsiMasshigh         = 3200.
        self.doVertexFit          = True
        self.TrigL2VertexFitter   = L2VFitTool
        self.TrigVertexFitter     = VFitTool
        self.TrigVertexingTool    = VertexTool
        self.dEtaClusRoI          = 1.5
        self.dPhiClusRoI          = 1.5
        self.TrackPTthr           = 1000.
        self.CaloTrackdEoverPLow  = 0.5
        self.CaloTrackdEoverPHigh = 2.5
        self.CaloTrackdETA        = 0.1
        self.CaloTrackdPHI        = 0.1
        self.TRTcut               = 0.1

        from TrigBphysHypo.TrigL2JpsieeFexMonitoring import TrigL2JpsieeFexValidationMonitoring
        validation = TrigL2JpsieeFexValidationMonitoring()
        
        from TrigBphysHypo.TrigL2JpsieeFexMonitoring import TrigL2JpsieeFexOnlineMonitoring
        online = TrigL2JpsieeFexOnlineMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
                
        self.AthenaMonTools = [ validation, online, time ]


