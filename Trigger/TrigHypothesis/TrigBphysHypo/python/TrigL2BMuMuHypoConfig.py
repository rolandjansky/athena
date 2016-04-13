# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBphysHypo.TrigBphysHypoConf import TrigL2BMuMuHypo

class L2BMuMuHypo_1 (TrigL2BMuMuHypo):
    __slots__ = []
    def __init__(self, name = "L2BMuMuHypo_1"):
        super( TrigL2BMuMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        self.AcceptSameMuon = True

        # L2 Bmumu cuts
        self.LowerMassCut      = 100.
        self.UpperMassCut      = 13000.
        self.ApplyUpperMassCut = True
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoValidationMonitoring
        validation = TrigL2BMuMuHypoValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoOnlineMonitoring
        online = TrigL2BMuMuHypoOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class L2BMuMuHypo_Jpsi (TrigL2BMuMuHypo):
    __slots__ = []
    def __init__(self, name = "L2BMuMuHypo_Jpsi"):
        super( TrigL2BMuMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        self.AcceptSameMuon = True

        # L2 Bmumu cuts
        self.LowerMassCut      = 2500.
        self.UpperMassCut      = 4300.
        self.ApplyUpperMassCut = True
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoValidationMonitoring
        validation = TrigL2BMuMuHypoValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoOnlineMonitoring
        online = TrigL2BMuMuHypoOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class L2BMuMuHypo_Jpsi_noSameMu (L2BMuMuHypo_Jpsi):
    __slots__ = []
    def __init__(self, name = "L2BMuMuHypo_Jpsi_noSameMu"):
        L2BMuMuHypo_Jpsi.__init__( self, name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptSameMuon = False
        
class L2BMuMuHypo_Jpsi_noId (TrigL2BMuMuHypo):
    __slots__ = []
    def __init__(self, name = "L2BMuMuHypo_Jpsi_noId"):
        super( TrigL2BMuMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        self.LowerMassCut      = 2500.
        self.UpperMassCut      = 4300.
        self.ApplyUpperMassCut = True
        self.ApplyChi2Cut      = False
        self.Chi2VtxCut        = 0.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoValidationMonitoring
        validation = TrigL2BMuMuHypoValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoOnlineMonitoring
        online = TrigL2BMuMuHypoOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class L2BMuMuHypo_Upsi (TrigL2BMuMuHypo):
    __slots__ = []
    def __init__(self, name = "L2BMuMuHypo_Upsi"):
        super( TrigL2BMuMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        self.AcceptSameMuon = True

        # L2 Bmumu cuts
        self.LowerMassCut      = 8000.
        self.UpperMassCut      = 12000.
        self.ApplyUpperMassCut = True
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoValidationMonitoring
        validation = TrigL2BMuMuHypoValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoOnlineMonitoring
        online = TrigL2BMuMuHypoOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class L2BMuMuHypo_B (TrigL2BMuMuHypo):
    __slots__ = []
    def __init__(self, name = "L2BMuMuHypo_B"):
        super( TrigL2BMuMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        self.AcceptSameMuon = True

        # L2 Bmumu cuts
        self.LowerMassCut      = 4000.
        self.UpperMassCut      = 8500.
        self.ApplyUpperMassCut = True
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoValidationMonitoring
        validation = TrigL2BMuMuHypoValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoOnlineMonitoring
        online = TrigL2BMuMuHypoOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class L2BMuMuHypo_Z (TrigL2BMuMuHypo):
    __slots__ = []
    def __init__(self, name = "L2BMuMuHypo_Z"):
        super( TrigL2BMuMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        self.LowerMassCut      = 60000.
        self.UpperMassCut      = 120000.
        self.ApplyUpperMassCut = True
        self.ApplyChi2Cut      = False
        self.Chi2VtxCut        = 0.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoValidationMonitoring
        validation = TrigL2BMuMuHypoValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoOnlineMonitoring
        online = TrigL2BMuMuHypoOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class L2BMuMuHypo_DiMu (TrigL2BMuMuHypo):
    __slots__ = []
    def __init__(self, name = "L2BMuMuHypo_DiMu"):
        super( TrigL2BMuMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        self.AcceptSameMuon = True

        # L2 Bmumu cuts
        self.LowerMassCut      = 1500.
        self.UpperMassCut      = 14000.
        self.ApplyUpperMassCut = True
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoValidationMonitoring
        validation = TrigL2BMuMuHypoValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoOnlineMonitoring
        online = TrigL2BMuMuHypoOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class L2BMuMuHypo_DiMu_DY (TrigL2BMuMuHypo):
    __slots__ = []
    def __init__(self, name = "L2BMuMuHypo_DiMu_DY"):
        super( TrigL2BMuMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        self.LowerMassCut      = 10000.
        self.UpperMassCut      = 120000.
        self.ApplyUpperMassCut = True
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoValidationMonitoring
        validation = TrigL2BMuMuHypoValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoOnlineMonitoring
        online = TrigL2BMuMuHypoOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class L2BMuMuHypo_DiMu_DY25 (TrigL2BMuMuHypo):
    __slots__ = []
    def __init__(self, name = "L2BMuMuHypo_DiMu_DY25"):
        super( TrigL2BMuMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        self.LowerMassCut      = 25000.
        self.UpperMassCut      = 120000.
        self.ApplyUpperMassCut = True
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoValidationMonitoring
        validation = TrigL2BMuMuHypoValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoOnlineMonitoring
        online = TrigL2BMuMuHypoOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class L2BMuMuHypo_DiMu_DY20 (TrigL2BMuMuHypo):
    __slots__ = []
    def __init__(self, name = "L2BMuMuHypo_DiMu_DY20"):
        super( TrigL2BMuMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        self.LowerMassCut      = 20000.
        self.UpperMassCut      = 120000.
        self.ApplyUpperMassCut = True
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoValidationMonitoring
        validation = TrigL2BMuMuHypoValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoOnlineMonitoring
        online = TrigL2BMuMuHypoOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class L2BMuMuHypo_DiMu_DY14 (TrigL2BMuMuHypo):
    __slots__ = []
    def __init__(self, name = "L2BMuMuHypo_DiMu_DY14"):
        super( TrigL2BMuMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        self.LowerMassCut      = 14000.
        self.UpperMassCut      = 120000.
        self.ApplyUpperMassCut = True
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoValidationMonitoring
        validation = TrigL2BMuMuHypoValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoOnlineMonitoring
        online = TrigL2BMuMuHypoOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class L2BMuMuHypo_DiMu_noVtx (TrigL2BMuMuHypo):
    __slots__ = []
    def __init__(self, name = "L2BMuMuHypo_DiMu_noVtx"):
        super( TrigL2BMuMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        self.AcceptSameMuon = True

        # L2 Bmumu cuts
        self.LowerMassCut      = 1500.
        self.UpperMassCut      = 14000.
        self.ApplyUpperMassCut = True
        self.ApplyChi2Cut      = False
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoValidationMonitoring
        validation = TrigL2BMuMuHypoValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoOnlineMonitoring
        online = TrigL2BMuMuHypoOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class L2BMuMuHypo_DiMu_DY_noVtx (TrigL2BMuMuHypo):
    __slots__ = []
    def __init__(self, name = "L2BMuMuHypo_DiMu_DY_noVtx"):
        super( TrigL2BMuMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        self.LowerMassCut      = 10000.
        self.UpperMassCut      = 140000.
        self.ApplyUpperMassCut = True
        self.ApplyChi2Cut      = False
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoValidationMonitoring
        validation = TrigL2BMuMuHypoValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoOnlineMonitoring
        online = TrigL2BMuMuHypoOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class L2BMuMuHypo_DiMu_DY14_noVtx (TrigL2BMuMuHypo):
    __slots__ = []
    def __init__(self, name = "L2BMuMuHypo_DiMu_DY14_noVtx"):
        super( TrigL2BMuMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        self.LowerMassCut      = 14000.
        self.UpperMassCut      = 140000.
        self.ApplyUpperMassCut = True
        self.ApplyChi2Cut      = False
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoValidationMonitoring
        validation = TrigL2BMuMuHypoValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoOnlineMonitoring
        online = TrigL2BMuMuHypoOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class L2BMuMuHypo_DiMu_7invm9_noVtx (TrigL2BMuMuHypo):
    __slots__ = []
    def __init__(self, name = "L2BMuMuHypo_DiMu_7invm9_noVtx"):
        super( TrigL2BMuMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        self.LowerMassCut      = 7000.
        self.UpperMassCut      = 9000.
        self.ApplyUpperMassCut = True
        self.ApplyChi2Cut      = False
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoValidationMonitoring
        validation = TrigL2BMuMuHypoValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoOnlineMonitoring
        online = TrigL2BMuMuHypoOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class L2BMuMuHypo_DiMu_7invm9 (L2BMuMuHypo_DiMu_7invm9_noVtx):
    __slots__ = []
    def __init__(self, name = "L2BMuMuHypo_DiMu_7invm9"):
        super( L2BMuMuHypo_DiMu_7invm9, self ).__init__( name )
        
        # L2 Bmumu cuts
        self.ApplyChi2Cut      = True
        
        
class L2BMuMuHypo_DiMu_11invm60_noVtx (TrigL2BMuMuHypo):
    __slots__ = []
    def __init__(self, name = "L2BMuMuHypo_DiMu_11invm60_noVtx"):
        super( TrigL2BMuMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        self.LowerMassCut      = 11000.
        self.UpperMassCut      = 60000.
        self.ApplyUpperMassCut = True
        self.ApplyChi2Cut      = False
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoValidationMonitoring
        validation = TrigL2BMuMuHypoValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoOnlineMonitoring
        online = TrigL2BMuMuHypoOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]
        
class L2BMuMuHypo_DiMu_11invm60 (L2BMuMuHypo_DiMu_11invm60_noVtx):
    __slots__ = []
    def __init__(self, name = "L2BMuMuHypo_DiMu_11invm60"):
        super( L2BMuMuHypo_DiMu_11invm60, self ).__init__( name )
        
        # L2 Bmumu cuts
        self.ApplyChi2Cut      = True
        
        
class L2BMuMuHypo_DiMu_11invm24_noVtx (L2BMuMuHypo_DiMu_11invm60_noVtx):
    __slots__ = []
    def __init__(self, name = "L2BMuMuHypo_DiMu_11invm24_noVtx"):
        super( L2BMuMuHypo_DiMu_11invm24_noVtx, self ).__init__( name )
        
        # L2 Bmumu cuts
        self.LowerMassCut      = 11000.
        self.UpperMassCut      = 24000.
        
class L2BMuMuHypo_DiMu_11invm24 (L2BMuMuHypo_DiMu_11invm24_noVtx):
    __slots__ = []
    def __init__(self, name = "L2BMuMuHypo_DiMu_11invm24"):
        super( L2BMuMuHypo_DiMu_11invm24, self ).__init__( name )
        
        # L2 Bmumu cuts
        self.ApplyChi2Cut      = True
        
        
class L2BMuMuHypo_DiMu_24invm60_noVtx (L2BMuMuHypo_DiMu_11invm60_noVtx):
    __slots__ = []
    def __init__(self, name = "L2BMuMuHypo_DiMu_24invm60_noVtx"):
        super( L2BMuMuHypo_DiMu_24invm60_noVtx, self ).__init__( name )
        
        # L2 Bmumu cuts
        self.LowerMassCut      = 24000.
        self.UpperMassCut      = 60000.
        
class L2BMuMuHypo_DiMu_24invm60 (L2BMuMuHypo_DiMu_24invm60_noVtx):
    __slots__ = []
    def __init__(self, name = "L2BMuMuHypo_DiMu_24invm60"):
        super( L2BMuMuHypo_DiMu_24invm60, self ).__init__( name )
        
        # L2 Bmumu cuts
        self.ApplyChi2Cut      = True
        
        
class L2BMuMuHypo_DiMu_18invm60_noVtx (TrigL2BMuMuHypo):
    __slots__ = []
    def __init__(self, name = "L2BMuMuHypo_DiMu_18invm60_noVtx"):
        super( TrigL2BMuMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        self.LowerMassCut      = 18000.
        self.UpperMassCut      = 60000.
        self.ApplyUpperMassCut = True
        self.ApplyChi2Cut      = False
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoValidationMonitoring
        validation = TrigL2BMuMuHypoValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoOnlineMonitoring
        online = TrigL2BMuMuHypoOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]



class L2BMuMuHypo_Jpsi_passL2 (TrigL2BMuMuHypo):
    __slots__ = []
    def __init__(self, name = "L2BMuMuHypo_Jpsi_passL2"):
        super( TrigL2BMuMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = True

        # L2 Bmumu cuts
        self.LowerMassCut      = 2500.
        self.UpperMassCut      = 4300.
        self.ApplyUpperMassCut = True
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoValidationMonitoring
        validation = TrigL2BMuMuHypoValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoOnlineMonitoring
        online = TrigL2BMuMuHypoOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class L2BMuMuHypo_Upsi_passL2 (TrigL2BMuMuHypo):
    __slots__ = []
    def __init__(self, name = "L2BMuMuHypo_Upsi_passL2"):
        super( TrigL2BMuMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = True

        # L2 Bmumu cuts
        self.LowerMassCut      = 8000.
        self.UpperMassCut      = 12000.
        self.ApplyUpperMassCut = True
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoValidationMonitoring
        validation = TrigL2BMuMuHypoValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoOnlineMonitoring
        online = TrigL2BMuMuHypoOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class L2BMuMuHypo_B_passL2 (TrigL2BMuMuHypo):
    __slots__ = []
    def __init__(self, name = "L2BMuMuHypo_B_passL2"):
        super( TrigL2BMuMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = True

        # L2 Bmumu cuts
        self.LowerMassCut      = 4000.
        self.UpperMassCut      = 8500.
        self.ApplyUpperMassCut = True
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoValidationMonitoring
        validation = TrigL2BMuMuHypoValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoOnlineMonitoring
        online = TrigL2BMuMuHypoOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class L2BMuMuHypo_DiMu_passL2 (TrigL2BMuMuHypo):
    __slots__ = []
    def __init__(self, name = "L2BMuMuHypo_DiMu_passL2"):
        super( TrigL2BMuMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = True

        # L2 Bmumu cuts
        self.LowerMassCut      = 1500.
        self.UpperMassCut      = 14000.
        self.ApplyUpperMassCut = True
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoValidationMonitoring
        validation = TrigL2BMuMuHypoValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoOnlineMonitoring
        online = TrigL2BMuMuHypoOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class L2BMuMuHypo_DiMu_noVtx_passL2 (TrigL2BMuMuHypo):
    __slots__ = []
    def __init__(self, name = "L2BMuMuHypo_DiMu_noVtx_passL2"):
        super( TrigL2BMuMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = True

        # L2 Bmumu cuts
        self.LowerMassCut      = 1500.
        self.UpperMassCut      = 14000.
        self.ApplyUpperMassCut = True
        self.ApplyChi2Cut      = False
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoValidationMonitoring
        validation = TrigL2BMuMuHypoValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoOnlineMonitoring
        online = TrigL2BMuMuHypoOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class L2BMuMuHypo_Z_passL2 (TrigL2BMuMuHypo):
    __slots__ = []
    def __init__(self, name = "L2BMuMuHypo_Jpsi_passL2"):
        super( TrigL2BMuMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = True

        # L2 Bmumu cuts
        self.LowerMassCut      = 60000.
        self.UpperMassCut      = 120000.
        self.ApplyUpperMassCut = True
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoValidationMonitoring
        validation = TrigL2BMuMuHypoValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoOnlineMonitoring
        online = TrigL2BMuMuHypoOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class L2BMuMuHypo_DiMu_noinvm_noVtx (TrigL2BMuMuHypo):
    __slots__ = []
    def __init__(self, name = "L2BMuMuHypo_DiMu_noinvm_noVtx"):
        super( TrigL2BMuMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        self.LowerMassCut      = 0.
        self.UpperMassCut      = 14000.
        self.ApplyUpperMassCut = False
        self.ApplyChi2Cut      = False
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoValidationMonitoring
        validation = TrigL2BMuMuHypoValidationMonitoring()
        from TrigBphysHypo.TrigL2BMuMuHypoMonitoring import TrigL2BMuMuHypoOnlineMonitoring
        online = TrigL2BMuMuHypoOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]
