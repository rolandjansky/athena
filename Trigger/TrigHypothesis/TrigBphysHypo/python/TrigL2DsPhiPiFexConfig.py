# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBphysHypo.TrigBphysHypoConf import TrigL2DsPhiPiFex
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
class L2DsPhiPiFex_1 (TrigL2DsPhiPiFex):
    __slots__ = []
    def __init__(self, name = "L2DsPhiPiFex_1"):
        super( TrigL2DsPhiPiFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 DsPhiPi cuts
        self.TrackPTthr          = 1400
#        self.TrackAlgoId         = 2
        self.dEtaTrackPair       = 0.2
        self.dPhiTrackPair       = 0.2
        self.dz0TrackPair        = 3.
        self.pTsumTrackPair      = 0.
        self.EtaBins             = [0.,1.0,1.5]
        self.PhiMasslow_cut      = [990.,990.,990.]
        self.PhiMasshigh_cut     = [1050.,1050.,1050.]
        self.DsMasslow_cut       = [1863.,1863.,1863.]
        self.DsMasshigh_cut      = [2071.,2071.,2071.]
        self.NumberDsToKeep = 10
        self.PhiChi2Prob_cut     = -1.
        self.DsChi2Prob_cut      = 0.01  #-1.
        self.doVertexFit         = True
        self.TrigL2VertexFitter = L2VFitTool
        self.TrigVertexFitter = VFitTool
        self.TrigVertexingTool = VertexTool

        from TrigBphysHypo.TrigL2DsPhiPiFexMonitoring import TrigL2DsPhiPiFexValidationMonitoring_RoI
        validation = TrigL2DsPhiPiFexValidationMonitoring_RoI()

        from TrigBphysHypo.TrigL2DsPhiPiFexMonitoring import TrigL2DsPhiPiFexOnlineMonitoring_RoI
        online = TrigL2DsPhiPiFexOnlineMonitoring_RoI()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        self.AthenaMonTools = [ validation, online, time ]


# basic cut
class L2DsPhiPiFex_FS (TrigL2DsPhiPiFex):
    __slots__ = []
    def __init__(self, name = "L2DsPhiPiFex_FS"):
        super( TrigL2DsPhiPiFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 DsPhiPi cuts
        self.TrackPTthr          = 1400
#        self.TrackAlgoId         = 2
        self.dEtaTrackPair       = 0.2
        self.dPhiTrackPair       = 0.2
        self.dz0TrackPair        = 3.
        self.pTsumTrackPair      = 0.
        self.EtaBins             = [0.,1.0,1.5]
        self.PhiMasslow_cut      = [990.,990.,990.]
        self.PhiMasshigh_cut     = [1050.,1050.,1050.]
        self.DsMasslow_cut       = [1863.,1863.,1863.]
        self.DsMasshigh_cut      = [2071.,2071.,2071.]
        self.NumberDsToKeep = 10
        self.PhiChi2Prob_cut     = -1.
        self.DsChi2Prob_cut      = 0.01 #-1.
        self.doVertexFit         = True
        self.TrigL2VertexFitter = L2VFitTool
        self.TrigVertexFitter = VFitTool
        self.TrigVertexingTool = VertexTool

        from TrigBphysHypo.TrigL2DsPhiPiFexMonitoring import TrigL2DsPhiPiFexValidationMonitoring_FS
        validation = TrigL2DsPhiPiFexValidationMonitoring_FS()

        from TrigBphysHypo.TrigL2DsPhiPiFexMonitoring import TrigL2DsPhiPiFexOnlineMonitoring_FS
        online = TrigL2DsPhiPiFexOnlineMonitoring_FS()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        self.AthenaMonTools = [ validation, online, time ]


