# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBphysHypo.TrigBphysHypoConf import TrigL2BMuMuFex
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

class L2BMuMuFex_1 (TrigL2BMuMuFex):
    __slots__ = []
    def __init__(self, name = "L2BMuMuFex_1"):
        super( TrigL2BMuMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        self.AcceptSameMuon = True

        # L2 Bmumu cuts
        # self.TrackPTthr       = 1.4
        # self.TrackAlgoId      = 2
        # self.pTsumTrackPair   = 0.
        self.LowerMassCut       = 100.
        self.UpperMassCut       = 13000.
        self.ApplyUpperMassCut  = True
        self.doVertexFit        = True
        self.TrigL2VertexFitter = L2VFitTool
        self.TrigVertexingTool  = VertexTool

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexValidationMonitoring
        validation = TrigL2BMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexOnlineMonitoring
        online = TrigL2BMuMuFexOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class L2BMuMuFex_Jpsi (TrigL2BMuMuFex):
    __slots__ = []
    def __init__(self, name = "L2BMuMuFex_Jpsi"):
        super( TrigL2BMuMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        self.AcceptSameMuon = True

        # L2 Bmumu cuts
        # self.TrackPTthr       = 1.4
        # self.TrackAlgoId      = 2
        # self.pTsumTrackPair   = 0.
        self.LowerMassCut       = 2000.
        self.UpperMassCut       = 4800.
        self.ApplyUpperMassCut  = True
        self.doVertexFit        = True
        self.TrigL2VertexFitter = L2VFitTool
        self.TrigVertexingTool  = VertexTool

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexValidationMonitoring
        validation = TrigL2BMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexOnlineMonitoring
        online = TrigL2BMuMuFexOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class L2BMuMuFex_Jpsi_noSameMu (L2BMuMuFex_Jpsi):
    __slots__ = []
    def __init__(self, name = "L2BMuMuFex_Jpsi_noSameMu"):
        L2BMuMuFex_Jpsi.__init__( self, name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptSameMuon = False
        
class L2BMuMuFex_Upsi (TrigL2BMuMuFex):
    __slots__ = []
    def __init__(self, name = "L2BMuMuFex_Upsi"):
        super( TrigL2BMuMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        self.AcceptSameMuon = True

        # L2 Bmumu cuts
        # self.TrackPTthr       = 1.4
        # self.TrackAlgoId      = 2
        # self.pTsumTrackPair   = 0.
        self.LowerMassCut       = 7000.
        self.UpperMassCut       = 13000.
        self.ApplyUpperMassCut  = True
        self.doVertexFit        = True
        self.TrigL2VertexFitter = L2VFitTool
        self.TrigVertexingTool  = VertexTool

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexValidationMonitoring
        validation = TrigL2BMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexOnlineMonitoring
        online = TrigL2BMuMuFexOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class L2BMuMuFex_B (TrigL2BMuMuFex):
    __slots__ = []
    def __init__(self, name = "L2BMuMuFex_B"):
        super( TrigL2BMuMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        self.AcceptSameMuon = True

        # L2 Bmumu cuts
        # self.TrackPTthr       = 1.4
        # self.TrackAlgoId      = 2
        # self.pTsumTrackPair   = 0.
        self.LowerMassCut       = 4000.
        self.UpperMassCut       = 9000.
        self.ApplyUpperMassCut  = True
        self.doVertexFit        = True
        self.TrigL2VertexFitter = L2VFitTool
        self.TrigVertexingTool  = VertexTool

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexValidationMonitoring
        validation = TrigL2BMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexOnlineMonitoring
        online = TrigL2BMuMuFexOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class L2BMuMuFex_DiMu (TrigL2BMuMuFex):
    __slots__ = []
    def __init__(self, name = "L2BMuMuFex_DiMu"):
        super( TrigL2BMuMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        self.AcceptSameMuon = True

        # L2 Bmumu cuts
        # self.TrackPTthr       = 1.4
        # self.TrackAlgoId      = 2
        # self.pTsumTrackPair   = 0.
        self.LowerMassCut       = 1000.
        self.UpperMassCut       = 15000.
        self.ApplyUpperMassCut  = True
        self.doVertexFit        = True
        self.TrigL2VertexFitter = L2VFitTool
        self.TrigVertexingTool  = VertexTool

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexValidationMonitoring
        validation = TrigL2BMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexOnlineMonitoring
        online = TrigL2BMuMuFexOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class L2BMuMuFex_DiMu_DY (TrigL2BMuMuFex):
    __slots__ = []
    def __init__(self, name = "L2BMuMuFex_DiMu_DY"):
        super( TrigL2BMuMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        # self.TrackPTthr       = 1.4
        # self.TrackAlgoId      = 2
        # self.pTsumTrackPair   = 0.
        self.LowerMassCut       = 10000.
        self.UpperMassCut       = 140000.
        self.ApplyUpperMassCut  = True
        self.doVertexFit        = True
        self.TrigL2VertexFitter = L2VFitTool
        self.TrigVertexingTool  = VertexTool

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexValidationMonitoring
        validation = TrigL2BMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexOnlineMonitoring
        online = TrigL2BMuMuFexOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class L2BMuMuFex_DiMu_DY25 (TrigL2BMuMuFex):
    __slots__ = []
    def __init__(self, name = "L2BMuMuFex_DiMu_DY25"):
        super( TrigL2BMuMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        # self.TrackPTthr       = 1.4
        # self.TrackAlgoId      = 2
        # self.pTsumTrackPair   = 0.
        self.LowerMassCut       = 25000.
        self.UpperMassCut       = 140000.
        self.ApplyUpperMassCut  = True
        self.doVertexFit        = True
        self.TrigL2VertexFitter = L2VFitTool
        self.TrigVertexingTool  = VertexTool

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexValidationMonitoring
        validation = TrigL2BMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexOnlineMonitoring
        online = TrigL2BMuMuFexOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class L2BMuMuFex_DiMu_DY20 (TrigL2BMuMuFex):
    __slots__ = []
    def __init__(self, name = "L2BMuMuFex_DiMu_DY20"):
        super( TrigL2BMuMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        # self.TrackPTthr       = 1.4
        # self.TrackAlgoId      = 2
        # self.pTsumTrackPair   = 0.
        self.LowerMassCut       = 20000.
        self.UpperMassCut       = 140000.
        self.ApplyUpperMassCut  = True
        self.doVertexFit        = True
        self.TrigL2VertexFitter = L2VFitTool
        self.TrigVertexingTool  = VertexTool

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexValidationMonitoring
        validation = TrigL2BMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexOnlineMonitoring
        online = TrigL2BMuMuFexOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class L2BMuMuFex_DiMu_DY14 (TrigL2BMuMuFex):
    __slots__ = []
    def __init__(self, name = "L2BMuMuFex_DiMu_DY14"):
        super( TrigL2BMuMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        # self.TrackPTthr       = 1.4
        # self.TrackAlgoId      = 2
        # self.pTsumTrackPair   = 0.
        self.LowerMassCut       = 14000.
        self.UpperMassCut       = 140000.
        self.ApplyUpperMassCut  = True
        self.doVertexFit        = True
        self.TrigL2VertexFitter = L2VFitTool
        self.TrigVertexingTool  = VertexTool

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexValidationMonitoring
        validation = TrigL2BMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexOnlineMonitoring
        online = TrigL2BMuMuFexOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class L2BMuMuFex_DiMu_noOS (TrigL2BMuMuFex):
    __slots__ = []
    def __init__(self, name = "L2BMuMuFex_DiMu_noOS"):
        super( TrigL2BMuMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        self.AcceptSameMuon = True

        # L2 Bmumu cuts
        # self.TrackPTthr       = 1.4
        # self.TrackAlgoId      = 2
        # self.pTsumTrackPair   = 0.
        self.OppositeSign       = False
        self.LowerMassCut       = 1000.
        self.UpperMassCut       = 15000.
        self.ApplyUpperMassCut  = True
        self.doVertexFit        = True
        self.TrigL2VertexFitter = L2VFitTool
        self.TrigVertexingTool  = VertexTool

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexValidationMonitoring
        validation = TrigL2BMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexOnlineMonitoring
        online = TrigL2BMuMuFexOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class L2BMuMuFex_DiMu_DY_noOS (TrigL2BMuMuFex):
    __slots__ = []
    def __init__(self, name = "L2BMuMuFex_DiMu_DY_noOS"):
        super( TrigL2BMuMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        # self.TrackPTthr       = 1.4
        # self.TrackAlgoId      = 2
        # self.pTsumTrackPair   = 0.
        self.OppositeSign       = False
        self.LowerMassCut       = 10000.
        self.UpperMassCut       = 140000.
        self.ApplyUpperMassCut  = True
        self.doVertexFit        = True
        self.TrigL2VertexFitter = L2VFitTool
        self.TrigVertexingTool  = VertexTool

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexValidationMonitoring
        validation = TrigL2BMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexOnlineMonitoring
        online = TrigL2BMuMuFexOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class L2BMuMuFex_DiMu_DY14_noOS (TrigL2BMuMuFex):
    __slots__ = []
    def __init__(self, name = "L2BMuMuFex_DiMu_DY14_noOS"):
        super( TrigL2BMuMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        # self.TrackPTthr       = 1.4
        # self.TrackAlgoId      = 2
        # self.pTsumTrackPair   = 0.
        self.OppositeSign       = False
        self.LowerMassCut       = 14000.
        self.UpperMassCut       = 140000.
        self.ApplyUpperMassCut  = True
        self.doVertexFit        = True
        self.TrigL2VertexFitter = L2VFitTool
        self.TrigVertexingTool  = VertexTool

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexValidationMonitoring
        validation = TrigL2BMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexOnlineMonitoring
        online = TrigL2BMuMuFexOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class L2BMuMuFex_DiMu_DY14_noOS (TrigL2BMuMuFex):
    __slots__ = []
    def __init__(self, name = "L2BMuMuFex_DiMu_DY14_noOS"):
        super( TrigL2BMuMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        # self.TrackPTthr       = 1.4
        # self.TrackAlgoId      = 2
        # self.pTsumTrackPair   = 0.
        self.OppositeSign       = False
        self.LowerMassCut       = 14000.
        self.UpperMassCut       = 140000.
        self.ApplyUpperMassCut  = True
        self.doVertexFit        = True
        self.TrigL2VertexFitter = L2VFitTool
        self.TrigVertexingTool  = VertexTool

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexValidationMonitoring
        validation = TrigL2BMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexOnlineMonitoring
        online = TrigL2BMuMuFexOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class L2BMuMuFex_DiMu_7invm9_noOS (TrigL2BMuMuFex):
    __slots__ = []
    def __init__(self, name = "L2BMuMuFex_DiMu_7invm_noOS"):
        super( TrigL2BMuMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        # self.TrackPTthr       = 1.4
        # self.TrackAlgoId      = 2
        # self.pTsumTrackPair   = 0.
        self.OppositeSign       = False
        self.LowerMassCut       = 7000.
        self.UpperMassCut       = 9000.
        self.ApplyUpperMassCut  = True
        self.doVertexFit        = True
        self.TrigL2VertexFitter = L2VFitTool
        self.TrigVertexingTool  = VertexTool

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexValidationMonitoring
        validation = TrigL2BMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexOnlineMonitoring
        online = TrigL2BMuMuFexOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class L2BMuMuFex_DiMu_11invm60_noOS (TrigL2BMuMuFex):
    __slots__ = []
    def __init__(self, name = "L2BMuMuFex_DiMu_11invm60_noOS"):
        super( TrigL2BMuMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        # self.TrackPTthr       = 1.4
        # self.TrackAlgoId      = 2
        # self.pTsumTrackPair   = 0.
        self.OppositeSign       = False
        self.LowerMassCut       = 11000.
        self.UpperMassCut       = 60000.
        self.ApplyUpperMassCut  = True
        self.doVertexFit        = True
        self.TrigL2VertexFitter = L2VFitTool
        self.TrigVertexingTool  = VertexTool

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexValidationMonitoring
        validation = TrigL2BMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexOnlineMonitoring
        online = TrigL2BMuMuFexOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]
        
class L2BMuMuFex_DiMu_11invm24_noOS (L2BMuMuFex_DiMu_11invm60_noOS):
    __slots__ = []
    def __init__(self, name = "L2BMuMuFex_DiMu_11invm24_noOS"):
        super( L2BMuMuFex_DiMu_11invm24_noOS, self ).__init__( name )
        
        # L2 Bmumu cuts
        self.LowerMassCut       = 11000.
        self.UpperMassCut       = 24000.


class L2BMuMuFex_DiMu_24invm60_noOS (L2BMuMuFex_DiMu_11invm60_noOS):
    __slots__ = []
    def __init__(self, name = "L2BMuMuFex_DiMu_24invm60_noOS"):
        super( L2BMuMuFex_DiMu_24invm60_noOS, self ).__init__( name )
        
        # L2 Bmumu cuts
        self.LowerMassCut       = 24000.
        self.UpperMassCut       = 60000.


class L2BMuMuFex_DiMu_18invm60_noOS (TrigL2BMuMuFex):
    __slots__ = []
    def __init__(self, name = "L2BMuMuFex_DiMu_18invm60_noOS"):
        super( TrigL2BMuMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        # self.TrackPTthr       = 1.4
        # self.TrackAlgoId      = 2
        # self.pTsumTrackPair   = 0.
        self.OppositeSign       = False
        self.LowerMassCut       = 18000.
        self.UpperMassCut       = 60000.
        self.ApplyUpperMassCut  = True
        self.doVertexFit        = True
        self.TrigL2VertexFitter = L2VFitTool
        self.TrigVertexingTool  = VertexTool

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexValidationMonitoring
        validation = TrigL2BMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexOnlineMonitoring
        online = TrigL2BMuMuFexOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class L2BMuMuFex_noId (TrigL2BMuMuFex):
    __slots__ = []
    def __init__(self, name = "L2BMuMuFex_noId"):
        super( TrigL2BMuMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        # self.TrackPTthr       = 1.4
        # self.TrackAlgoId      = 2
        # self.pTsumTrackPair   = 0.
        self.LowerMassCut       = 2000.
        self.UpperMassCut       = 4800.
        self.ApplyUpperMassCut  = True
        self.doVertexFit        = True
        self.TrigL2VertexFitter = L2VFitTool
        self.TrigVertexingTool  = VertexTool
        self.noId               = True

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexValidationMonitoring
        validation = TrigL2BMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexOnlineMonitoring_noid
        online = TrigL2BMuMuFexOnlineMonitoring_noid()

        self.AthenaMonTools = [ validation, online, time ]

class L2BMuMuFex_noId_Z (TrigL2BMuMuFex):
    __slots__ = []
    def __init__(self, name = "L2BMuMuFex_noId_Z"):
        super( TrigL2BMuMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        # self.TrackPTthr       = 1.4
        # self.TrackAlgoId      = 2
        # self.pTsumTrackPair   = 0.
        self.LowerMassCut       = 60000.
        self.UpperMassCut       = 120000.
        self.ApplyUpperMassCut  = True
        self.doVertexFit        = True
        self.TrigL2VertexFitter = L2VFitTool
        self.TrigVertexingTool  = VertexTool
        self.noId               = True

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexValidationMonitoring
        validation = TrigL2BMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexOnlineMonitoring_noid
        online = TrigL2BMuMuFexOnlineMonitoring_noid()

        self.AthenaMonTools = [ validation, online, time ]

class L2BMuMuFex_Jpsi_passL2 (TrigL2BMuMuFex):
    __slots__ = []
    def __init__(self, name = "L2BMuMuFex_Jpsi_passL2"):
        super( TrigL2BMuMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = True

        # L2 Bmumu cuts
        # self.TrackPTthr       = 1.4
        # self.TrackAlgoId      = 2
        # self.pTsumTrackPair   = 0.
        self.LowerMassCut       = 2000.
        self.UpperMassCut       = 4800.
        self.ApplyUpperMassCut  = True
        self.doVertexFit        = True
        self.TrigL2VertexFitter = L2VFitTool
        self.TrigVertexingTool  = VertexTool

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexValidationMonitoring
        validation = TrigL2BMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexOnlineMonitoring_passL2
        online = TrigL2BMuMuFexOnlineMonitoring_passL2()

        self.AthenaMonTools = [ validation, online, time ]

class L2BMuMuFex_Upsi_passL2 (TrigL2BMuMuFex):
    __slots__ = []
    def __init__(self, name = "L2BMuMuFex_Upsi_passL2"):
        super( TrigL2BMuMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = True

        # L2 Bmumu cuts
        # self.TrackPTthr       = 1.4
        # self.TrackAlgoId      = 2
        # self.pTsumTrackPair   = 0.
        self.LowerMassCut       = 7000.
        self.UpperMassCut       = 13000.
        self.ApplyUpperMassCut  = True
        self.doVertexFit        = True
        self.TrigL2VertexFitter = L2VFitTool
        self.TrigVertexingTool  = VertexTool

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexValidationMonitoring
        validation = TrigL2BMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexOnlineMonitoring_passL2
        online = TrigL2BMuMuFexOnlineMonitoring_passL2()

        self.AthenaMonTools = [ validation, online, time ]

class L2BMuMuFex_B_passL2 (TrigL2BMuMuFex):
    __slots__ = []
    def __init__(self, name = "L2BMuMuFex_B_passL2"):
        super( TrigL2BMuMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = True

        # L2 Bmumu cuts
        # self.TrackPTthr       = 1.4
        # self.TrackAlgoId      = 2
        # self.pTsumTrackPair   = 0.
        self.LowerMassCut       = 4000.
        self.UpperMassCut       = 9000.
        self.ApplyUpperMassCut  = True
        self.doVertexFit        = True
        self.TrigL2VertexFitter = L2VFitTool
        self.TrigVertexingTool  = VertexTool

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexValidationMonitoring
        validation = TrigL2BMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexOnlineMonitoring_passL2
        online = TrigL2BMuMuFexOnlineMonitoring_passL2()

        self.AthenaMonTools = [ validation, online, time ]

class L2BMuMuFex_DiMu_passL2 (TrigL2BMuMuFex):
    __slots__ = []
    def __init__(self, name = "L2BMuMuFex_DiMu_passL2"):
        super( TrigL2BMuMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = True

        # L2 Bmumu cuts
        # self.TrackPTthr       = 1.4
        # self.TrackAlgoId      = 2
        # self.pTsumTrackPair   = 0.
        self.LowerMassCut       = 1000.
        self.UpperMassCut       = 15000.
        self.ApplyUpperMassCut  = True
        self.doVertexFit        = True
        self.TrigL2VertexFitter = L2VFitTool
        self.TrigVertexingTool  = VertexTool

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexValidationMonitoring
        validation = TrigL2BMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexOnlineMonitoring_passL2
        online = TrigL2BMuMuFexOnlineMonitoring_passL2()

        self.AthenaMonTools = [ validation, online, time ]

class L2BMuMuFex_DiMu_noOS_passL2 (TrigL2BMuMuFex):
    __slots__ = []
    def __init__(self, name = "L2BMuMuFex_DiMu_noOS_passL2"):
        super( TrigL2BMuMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = True

        # L2 Bmumu cuts
        # self.TrackPTthr       = 1.4
        # self.TrackAlgoId      = 2
        # self.pTsumTrackPair   = 0.
        self.OppositeSign       = False
        self.LowerMassCut       = 1000.
        self.UpperMassCut       = 15000.
        self.ApplyUpperMassCut  = True
        self.doVertexFit        = True
        self.TrigL2VertexFitter = L2VFitTool
        self.TrigVertexingTool  = VertexTool

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexValidationMonitoring
        validation = TrigL2BMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexOnlineMonitoring_passL2
        online = TrigL2BMuMuFexOnlineMonitoring_passL2()

        self.AthenaMonTools = [ validation, online, time ]

class L2BMuMuFex_noId_passL2 (TrigL2BMuMuFex):
    __slots__ = []
    def __init__(self, name = "L2BMuMuFex_noId_passL2"):
        super( TrigL2BMuMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = True

        # L2 Bmumu cuts
        # self.TrackPTthr       = 1.4
        # self.TrackAlgoId      = 2
        # self.pTsumTrackPair   = 0.
        self.LowerMassCut       = 2000.
        self.UpperMassCut       = 4800.
        self.ApplyUpperMassCut  = True
        self.doVertexFit        = True
        self.TrigL2VertexFitter = L2VFitTool
        self.TrigVertexingTool  = VertexTool
        self.noId               = True

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexValidationMonitoring
        validation = TrigL2BMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexOnlineMonitoring_passL2
        online = TrigL2BMuMuFexOnlineMonitoring_passL2()

        self.AthenaMonTools = [ validation, online, time ]

class L2BMuMuFex_noId_Z_passL2 (TrigL2BMuMuFex):
    __slots__ = []
    def __init__(self, name = "L2BMuMuFex_noId_Z_passL2"):
        super( TrigL2BMuMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = True

        # L2 Bmumu cuts
        # self.TrackPTthr       = 1.4
        # self.TrackAlgoId      = 2
        # self.pTsumTrackPair   = 0.
        self.LowerMassCut       = 60000.
        self.UpperMassCut       = 120000.
        self.ApplyUpperMassCut  = True
        self.doVertexFit        = True
        self.TrigL2VertexFitter = L2VFitTool
        self.TrigVertexingTool  = VertexTool
        self.noId               = True

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexValidationMonitoring
        validation = TrigL2BMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexOnlineMonitoring
        online = TrigL2BMuMuFexOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class L2BMuMuFex_DiMu_noinvm_SS (TrigL2BMuMuFex):
    __slots__ = []
    def __init__(self, name = "L2BMuMuFex_DiMu_noinvm_SS"):
        super( TrigL2BMuMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        self.AcceptSameMuon = True

        # L2 Bmumu cuts
        # self.TrackPTthr       = 1.4
        # self.TrackAlgoId      = 2
        # self.pTsumTrackPair   = 0.
        self.OppositeSign       = False
        self.SameSign           = True
        self.LowerMassCut       = 0.
        self.UpperMassCut       = 15000.
        self.ApplyUpperMassCut  = False
        self.doVertexFit        = False # not effective - vertexing is done always
        self.TrigL2VertexFitter = L2VFitTool
        self.TrigVertexingTool  = VertexTool

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexValidationMonitoring
        validation = TrigL2BMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuFexMonitoring import TrigL2BMuMuFexOnlineMonitoring
        online = TrigL2BMuMuFexOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]
