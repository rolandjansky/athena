# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBphysHypo.TrigBphysHypoConf import TrigEFBMuMuFex 

from AthenaCommon.AppMgr import ToolSvc

#class EFBMuMuFex_1 (TrigEFBMuMuFex):
#    __slots__ = []
#    def __init__(self, name = "EFBMuMuFex_1"):
#        super( TrigEFBMuMuFex, self ).__init__( name )
#
#        # AcceptAll flag: if true take events regardless of cuts
#        self.AcceptAll = False
#
#        # L2 Bmumu cuts
#        self.LowerMassCut      = 100.
#        self.UpperMassCut      = 13000.
#        self.ApplyUpperMassCut = True
#        self.MuonAlgo          = "TrigMuSuperEF"
#
#        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
#        time = TrigTimeHistToolConfig("Time")
#        from TrigBphysHypo.TrigEFBMuMuFexMonitoring import TrigEFBMuMuFexValidationMonitoring
#        validation = TrigEFBMuMuFexValidationMonitoring()
#        from TrigBphysHypo.TrigEFBMuMuFexMonitoring import TrigEFBMuMuFexOnlineMonitoring
#        online = TrigEFBMuMuFexOnlineMonitoring()
#
#        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuFex_1 (TrigEFBMuMuFex):
    __slots__ = []
    def __init__(self, name = "EFBMuMuFex_1"):
        super( TrigEFBMuMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        self.LowerMassCut      = 100.
        self.UpperMassCut      = 13000.
        self.ApplyUpperMassCut = True
        self.MuonAlgo          = "TrigMuSuperEF"

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigEFBMuMuFexMonitoring import TrigEFBMuMuFexValidationMonitoring
        validation = TrigEFBMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigEFBMuMuFexMonitoring import TrigEFBMuMuFexOnlineMonitoring
        online = TrigEFBMuMuFexOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuFex_Jpsi (TrigEFBMuMuFex):
    __slots__ = []
    def __init__(self, name = "EFBMuMuFex_Jpsi"):
        super( TrigEFBMuMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        self.LowerMassCut      = 2000.
        self.UpperMassCut      = 4800.
        self.ApplyUpperMassCut = True
        self.MuonAlgo          = "TrigMuSuperEF"

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigEFBMuMuFexMonitoring import TrigEFBMuMuFexValidationMonitoring
        validation = TrigEFBMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigEFBMuMuFexMonitoring import TrigEFBMuMuFexOnlineMonitoring
        online = TrigEFBMuMuFexOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuFex_Upsi (TrigEFBMuMuFex):
    __slots__ = []
    def __init__(self, name = "EFBMuMuFex_Upsi"):
        super( TrigEFBMuMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        self.LowerMassCut      = 7000.
        self.UpperMassCut      = 13000.
        self.ApplyUpperMassCut = True
        self.MuonAlgo          = "TrigMuSuperEF"

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigEFBMuMuFexMonitoring import TrigEFBMuMuFexValidationMonitoring
        validation = TrigEFBMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigEFBMuMuFexMonitoring import TrigEFBMuMuFexOnlineMonitoring
        online = TrigEFBMuMuFexOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuFex_B (TrigEFBMuMuFex):
    __slots__ = []
    def __init__(self, name = "EFBMuMuFex_B"):
        super( TrigEFBMuMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        self.LowerMassCut      = 4000.
        self.UpperMassCut      = 9000.
        self.ApplyUpperMassCut = True
        self.MuonAlgo          = "TrigMuSuperEF"

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigEFBMuMuFexMonitoring import TrigEFBMuMuFexValidationMonitoring
        validation = TrigEFBMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigEFBMuMuFexMonitoring import TrigEFBMuMuFexOnlineMonitoring
        online = TrigEFBMuMuFexOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuFex_Z (TrigEFBMuMuFex):
    __slots__ = []
    def __init__(self, name = "EFBMuMuFex_Z"):
        super( TrigEFBMuMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        self.LowerMassCut      = 60000.
        self.UpperMassCut      = 120000.
        self.ApplyUpperMassCut = True
        self.MuonAlgo          = "TrigMuSuperEF"
        self.noId              = True

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigEFBMuMuFexMonitoring import TrigEFBMuMuFexValidationMonitoring
        validation = TrigEFBMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigEFBMuMuFexMonitoring import TrigEFBMuMuFexOnlineMonitoring
        online = TrigEFBMuMuFexOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuFex_muEF (TrigEFBMuMuFex):
    __slots__ = []
    def __init__(self, name = "EFBMuMuFex_muEF"):
        super( TrigEFBMuMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        self.LowerMassCut      = 100.
        self.UpperMassCut      = 13000.
        self.ApplyUpperMassCut = True

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigEFBMuMuFexMonitoring import TrigEFBMuMuFexValidationMonitoring
        validation = TrigEFBMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigEFBMuMuFexMonitoring import TrigEFBMuMuFexOnlineMonitoring
        online = TrigEFBMuMuFexOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuFex_DiMu (TrigEFBMuMuFex):
    __slots__ = []
    def __init__(self, name = "EFBMuMuFex_DiMu"):
        super( TrigEFBMuMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        #self.AcceptAll = False

        ## L2 Bmumu cuts
        self.LowerMassCut      = 1000.
        self.UpperMassCut      = 15000.
        self.ApplyUpperMassCut = True
        self.MuonAlgo          = "TrigMuSuperEF"

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigEFBMuMuFexMonitoring import TrigEFBMuMuFexValidationMonitoring
        validation = TrigEFBMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigEFBMuMuFexMonitoring import TrigEFBMuMuFexOnlineMonitoring
        online = TrigEFBMuMuFexOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuFex_DiMu_DY (TrigEFBMuMuFex):
    __slots__ = []
    def __init__(self, name = "EFBMuMuFex_DiMu_DY"):
        super( TrigEFBMuMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        self.LowerMassCut      = 10000.
        self.UpperMassCut      = 140000.
        self.ApplyUpperMassCut = True
        self.MuonAlgo          = "TrigMuSuperEF"

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigEFBMuMuFexMonitoring import TrigEFBMuMuFexValidationMonitoring
        validation = TrigEFBMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigEFBMuMuFexMonitoring import TrigEFBMuMuFexOnlineMonitoring
        online = TrigEFBMuMuFexOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuFex_DiMu_DY25 (TrigEFBMuMuFex):
    __slots__ = []
    def __init__(self, name = "EFBMuMuFex_DiMu_DY25"):
        super( TrigEFBMuMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        self.LowerMassCut      = 25000.
        self.UpperMassCut      = 140000.
        self.ApplyUpperMassCut = True
        self.MuonAlgo          = "TrigMuSuperEF"

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigEFBMuMuFexMonitoring import TrigEFBMuMuFexValidationMonitoring
        validation = TrigEFBMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigEFBMuMuFexMonitoring import TrigEFBMuMuFexOnlineMonitoring
        online = TrigEFBMuMuFexOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuFex_DiMu_DY20 (TrigEFBMuMuFex):
    __slots__ = []
    def __init__(self, name = "EFBMuMuFex_DiMu_DY20"):
        super( TrigEFBMuMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        self.LowerMassCut      = 20000.
        self.UpperMassCut      = 140000.
        self.ApplyUpperMassCut = True
        self.MuonAlgo          = "TrigMuSuperEF"

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigEFBMuMuFexMonitoring import TrigEFBMuMuFexValidationMonitoring
        validation = TrigEFBMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigEFBMuMuFexMonitoring import TrigEFBMuMuFexOnlineMonitoring
        online = TrigEFBMuMuFexOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuFex_DiMu_DY14 (TrigEFBMuMuFex):
    __slots__ = []
    def __init__(self, name = "EFBMuMuFex_DiMu_DY14"):
        super( TrigEFBMuMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        self.LowerMassCut      = 14000.
        self.UpperMassCut      = 140000.
        self.ApplyUpperMassCut = True
        self.MuonAlgo          = "TrigMuSuperEF"

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigEFBMuMuFexMonitoring import TrigEFBMuMuFexValidationMonitoring
        validation = TrigEFBMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigEFBMuMuFexMonitoring import TrigEFBMuMuFexOnlineMonitoring
        online = TrigEFBMuMuFexOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuFex_DiMu_noOS (TrigEFBMuMuFex):
    __slots__ = []
    def __init__(self, name = "EFBMuMuFex_DiMu_noOS"):
        super( TrigEFBMuMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        self.LowerMassCut      = 1000.
        self.UpperMassCut      = 15000.
        self.OppositeSign      = False
        self.ApplyUpperMassCut = True
        self.MuonAlgo          = "TrigMuSuperEF"

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigEFBMuMuFexMonitoring import TrigEFBMuMuFexValidationMonitoring
        validation = TrigEFBMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigEFBMuMuFexMonitoring import TrigEFBMuMuFexOnlineMonitoring
        online = TrigEFBMuMuFexOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuFex_DiMu_DY_noOS (TrigEFBMuMuFex):
    __slots__ = []
    def __init__(self, name = "EFBMuMuFex_DiMu_DY_noOS"):
        super( TrigEFBMuMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        self.LowerMassCut      = 10000.
        self.UpperMassCut      = 140000.
        self.OppositeSign      = False
        self.ApplyUpperMassCut = True
        self.MuonAlgo          = "TrigMuSuperEF"

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigEFBMuMuFexMonitoring import TrigEFBMuMuFexValidationMonitoring
        validation = TrigEFBMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigEFBMuMuFexMonitoring import TrigEFBMuMuFexOnlineMonitoring
        online = TrigEFBMuMuFexOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuFex_DiMu_DY14_noOS (TrigEFBMuMuFex):
    __slots__ = []
    def __init__(self, name = "EFBMuMuFex_DiMu_DY14_noOS"):
        super( TrigEFBMuMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        self.LowerMassCut      = 14000.
        self.UpperMassCut      = 140000.
        self.OppositeSign      = False
        self.ApplyUpperMassCut = True
        self.MuonAlgo          = "TrigMuSuperEF"

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigEFBMuMuFexMonitoring import TrigEFBMuMuFexValidationMonitoring
        validation = TrigEFBMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigEFBMuMuFexMonitoring import TrigEFBMuMuFexOnlineMonitoring
        online = TrigEFBMuMuFexOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuFex_DiMu_7invm9_noOS (TrigEFBMuMuFex):
    __slots__ = []
    def __init__(self, name = "EFBMuMuFex_DiMu_7invm9_noOS"):
        super( TrigEFBMuMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        self.LowerMassCut      = 7000.
        self.UpperMassCut      = 9000.
        self.OppositeSign      = False
        self.ApplyUpperMassCut = True
        self.MuonAlgo          = "TrigMuSuperEF"

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigEFBMuMuFexMonitoring import TrigEFBMuMuFexValidationMonitoring
        validation = TrigEFBMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigEFBMuMuFexMonitoring import TrigEFBMuMuFexOnlineMonitoring
        online = TrigEFBMuMuFexOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuFex_DiMu_11invm60_noOS (TrigEFBMuMuFex):
    __slots__ = []
    def __init__(self, name = "EFBMuMuFex_DiMu_11invm60_noOS"):
        super( TrigEFBMuMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        self.LowerMassCut      = 11000.
        self.UpperMassCut      = 60000.
        self.OppositeSign      = False
        self.ApplyUpperMassCut = True
        self.MuonAlgo          = "TrigMuSuperEF"

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigEFBMuMuFexMonitoring import TrigEFBMuMuFexValidationMonitoring
        validation = TrigEFBMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigEFBMuMuFexMonitoring import TrigEFBMuMuFexOnlineMonitoring
        online = TrigEFBMuMuFexOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuFex_DiMu_18invm60_noOS (TrigEFBMuMuFex):
    __slots__ = []
    def __init__(self, name = "EFBMuMuFex_DiMu_18invm60_noOS"):
        super( TrigEFBMuMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        self.LowerMassCut      = 11000.
        self.UpperMassCut      = 60000.
        self.OppositeSign      = False
        self.ApplyUpperMassCut = True
        self.MuonAlgo          = "TrigMuSuperEF"

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigEFBMuMuFexMonitoring import TrigEFBMuMuFexValidationMonitoring
        validation = TrigEFBMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigEFBMuMuFexMonitoring import TrigEFBMuMuFexOnlineMonitoring
        online = TrigEFBMuMuFexOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]



class EFBMuMuFex_noId (TrigEFBMuMuFex):
    __slots__ = []
    def __init__(self, name = "EFBMuMuFex_noId"):
        super( TrigEFBMuMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        self.LowerMassCut      = 2000.
        self.UpperMassCut      = 4800.
        self.ApplyUpperMassCut = True
        self.MuonAlgo          = "TrigMuSuperEF"
        self.noId              = True

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigEFBMuMuFexMonitoring import TrigEFBMuMuFexValidationMonitoring
        validation = TrigEFBMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigEFBMuMuFexMonitoring import TrigEFBMuMuFexOnlineMonitoring
        online = TrigEFBMuMuFexOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

