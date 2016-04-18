# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBphysHypo.TrigBphysHypoConf import TrigL2MultiMuFex
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

class L2MultiMuFex_DiMu (TrigL2MultiMuFex):
    __slots__ = []
    def __init__(self, name = "L2MultiMuFex_DiMu"):
        super( TrigL2MultiMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        # self.TrackPTthr          = 1.4
        # self.TrackAlgoId         = 2
        # self.pTsumTrackPair      = 0.
        self.LowerMassCut      = 1000.
        self.UpperMassCut     = 14000.
        self.doVertexFit         = True
        self.TrigL2VertexFitter = L2VFitTool
        self.TrigVertexingTool = VertexTool
                                

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2MultiMuFexMonitoring import TrigL2MultiMuFexValidationMonitoring
        validation = TrigL2MultiMuFexValidationMonitoring()
        from TrigBphysHypo.TrigL2MultiMuFexMonitoring import TrigL2MultiMuFexOnlineMonitoring
        online = TrigL2MultiMuFexOnlineMonitoring()
        
        self.AthenaMonTools = [ validation, online, time ]

class L2MultiMuFex_Jpsi (TrigL2MultiMuFex):
    __slots__ = []
    def __init__(self, name = "L2MultiMuFex_Jpsi"):
        super( TrigL2MultiMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        # self.TrackPTthr          = 1.4
        # self.TrackAlgoId         = 2
        # self.pTsumTrackPair      = 0.
        self.LowerMassCut      = 2000.
        self.UpperMassCut     = 4800.
        self.doVertexFit         = True
        self.TrigL2VertexFitter = L2VFitTool
        self.TrigVertexingTool = VertexTool
                                

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2MultiMuFexMonitoring import TrigL2MultiMuFexValidationMonitoring
        validation = TrigL2MultiMuFexValidationMonitoring()
        from TrigBphysHypo.TrigL2MultiMuFexMonitoring import TrigL2MultiMuFexOnlineMonitoring
        online = TrigL2MultiMuFexOnlineMonitoring()
        
        self.AthenaMonTools = [ validation, online, time ]

class L2MultiMuFex_Upsi (TrigL2MultiMuFex):
    __slots__ = []
    def __init__(self, name = "L2MultiMuFex_Upsi"):
        super( TrigL2MultiMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        # self.TrackPTthr          = 1.4
        # self.TrackAlgoId         = 2
        # self.pTsumTrackPair      = 0.
        self.LowerMassCut      = 7000.
        self.UpperMassCut     = 13000.
        self.doVertexFit         = True
        self.TrigL2VertexFitter = L2VFitTool
        self.TrigVertexingTool = VertexTool
                                

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2MultiMuFexMonitoring import TrigL2MultiMuFexValidationMonitoring
        validation = TrigL2MultiMuFexValidationMonitoring()
        from TrigBphysHypo.TrigL2MultiMuFexMonitoring import TrigL2MultiMuFexOnlineMonitoring
        online = TrigL2MultiMuFexOnlineMonitoring()
        
        self.AthenaMonTools = [ validation, online, time ]

class L2MultiMuFex_Tau (TrigL2MultiMuFex):
    __slots__ = []
    def __init__(self, name = "L2MultiMuFex_Tau"):
        super( TrigL2MultiMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        # self.TrackPTthr          = 1.4
        # self.TrackAlgoId         = 2
        # self.pTsumTrackPair      = 0.
        self.LowerMassCut      = 0.
        self.UpperMassCut     = 2900.
        self.NMassMuon          =3
        self.CheckNinputTE = False
        self.doVertexFit         = True
        self.TrigL2VertexFitter = L2VFitTool
        self.TrigVertexingTool = VertexTool
                                

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2MultiMuFexMonitoring import TrigL2MultiMuFexValidationMonitoring
        validation = TrigL2MultiMuFexValidationMonitoring()
        from TrigBphysHypo.TrigL2MultiMuFexMonitoring import TrigL2MultiMuFexOnlineMonitoring
        online = TrigL2MultiMuFexOnlineMonitoring()
        
        self.AthenaMonTools = [ validation, online, time ]

class L2MultiMuFex_Tau2 (TrigL2MultiMuFex):
    __slots__ = []
    def __init__(self, name = "L2MultiMuFex_Tau2"):
        super( TrigL2MultiMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        # self.TrackPTthr          = 1.4
        # self.TrackAlgoId         = 2
        # self.pTsumTrackPair      = 0.
        self.LowerMassCut      = 0.
        self.UpperMassCut     = 2900.
        self.NMassMuon          =2
        self.CheckNinputTE = False
        self.doVertexFit         = True
        self.TrigL2VertexFitter = L2VFitTool
        self.TrigVertexingTool = VertexTool
                                

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2MultiMuFexMonitoring import TrigL2MultiMuFexValidationMonitoring
        validation = TrigL2MultiMuFexValidationMonitoring()
        from TrigBphysHypo.TrigL2MultiMuFexMonitoring import TrigL2MultiMuFexOnlineMonitoring
        online = TrigL2MultiMuFexOnlineMonitoring()
        
        self.AthenaMonTools = [ validation, online, time ]

class L2MultiMuFex_Tau_passL2 (TrigL2MultiMuFex):
    __slots__ = []
    def __init__(self, name = "L2MultiMuFex_Tau_passL2"):
        super( TrigL2MultiMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = True

        # L2 Bmumu cuts
        # self.TrackPTthr          = 1.4
        # self.TrackAlgoId         = 2
        # self.pTsumTrackPair      = 0.
        self.LowerMassCut      = 0.
        self.UpperMassCut     = 2900.
        self.NMassMuon          =3
        self.CheckNinputTE = False
        self.doVertexFit         = True
        self.TrigL2VertexFitter = L2VFitTool
        self.TrigVertexingTool = VertexTool
                                

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2MultiMuFexMonitoring import TrigL2MultiMuFexValidationMonitoring
        validation = TrigL2MultiMuFexValidationMonitoring()
        from TrigBphysHypo.TrigL2MultiMuFexMonitoring import TrigL2MultiMuFexOnlineMonitoring_passL2
        online = TrigL2MultiMuFexOnlineMonitoring_passL2()
        
        self.AthenaMonTools = [ validation, online, time ]

