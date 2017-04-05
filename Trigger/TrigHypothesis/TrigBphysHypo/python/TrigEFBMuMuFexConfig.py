# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


# include the python fragment to set up the default bphysics vertex fitter
#include("TrigBphysHypo/TrigBphysVertexingConfig.py")

from TrigBphysHypo.TrigBphysHypoConf       import TrigBphysHelperUtilsTool
from TrigBphysHypo.TrigBphysHypoConf       import TrigEFBMuMuFex 
from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
from TrigBphysHypo import TrigBphysVertexingConfig

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
        super( EFBMuMuFex_1, self ).__init__( name )

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
        super( EFBMuMuFex_Jpsi, self ).__init__( name )

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

class EFBMuMuFex_Jpsi_oneTE (EFBMuMuFex_Jpsi):
    __slots__ = []
    def __init__(self, name = "EFBMuMuFex_Jpsi_oneTE"):
        EFBMuMuFex_Jpsi.__init__( self, name )

        self.NumberOfInputs     = 1
        self.UseRoIs = False

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
        super( EFBMuMuFex_Upsi, self ).__init__( name )

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
        super( EFBMuMuFex_B, self ).__init__( name )

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
        super( EFBMuMuFex_Z, self ).__init__( name )

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
        super( EFBMuMuFex_muEF, self ).__init__( name )

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
        super( EFBMuMuFex_DiMu, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        ## L2 Bmumu cuts
        self.LowerMassCut      = 100.
        self.UpperMassCut      = 15000.
        self.ApplyUpperMassCut = True
        self.MuonAlgo          = "TrigMuSuperEF"
        self.UseRoIs           = False
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigEFBMuMuFexMonitoring import TrigEFBMuMuFexValidationMonitoring
        validation = TrigEFBMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigEFBMuMuFexMonitoring import TrigEFBMuMuFexOnlineMonitoring
        online = TrigEFBMuMuFexOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]
        
class EFBMuMuFex_DiMu2700 (TrigEFBMuMuFex):
    __slots__ = []
    def __init__(self, name = "EFBMuMuFex_DiMu"):
        super( EFBMuMuFex_DiMu2700, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        ## L2 Bmumu cuts
        self.LowerMassCut      = 100.
        self.UpperMassCut      = 2700.
        self.ApplyUpperMassCut = True
        self.MuonAlgo          = "TrigMuSuperEF"
        self.UseRoIs           = False
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigEFBMuMuFexMonitoring import TrigEFBMuMuFexValidationMonitoring
        validation = TrigEFBMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigEFBMuMuFexMonitoring import TrigEFBMuMuFexOnlineMonitoring
        online = TrigEFBMuMuFexOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuFex_DiMu_legacyVtx (EFBMuMuFex_DiMu):
    __slots__ = []
    def __init__(self, name = "EFBMuMuFex_DiMu_legacyVtx"):
        EFBMuMuFex_DiMu.__init__( self, name )
        
        # set lecacy helper tool with 'broken' vertex fitter
        self.TrigBphysHelperTool = ToolSvc.TrigBphysHelperUtilsToolLegacy
        
        
class EFBMuMuFex_DiMu_oneTE (EFBMuMuFex_DiMu):
    __slots__ = []
    def __init__(self, name = "EFBMuMuFex_DiMu_oneTE"):
        EFBMuMuFex_DiMu.__init__( self, name )

        self.NumberOfInputs     = 1
        self.UseRoIs = False

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
        super( EFBMuMuFex_DiMu_DY, self ).__init__( name )

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
        super( EFBMuMuFex_DiMu_DY25, self ).__init__( name )

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
        super( EFBMuMuFex_DiMu_DY20, self ).__init__( name )

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
        super( EFBMuMuFex_DiMu_DY14, self ).__init__( name )

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
        super( EFBMuMuFex_DiMu_noOS, self ).__init__( name )

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
        super( EFBMuMuFex_DiMu_DY_noOS, self ).__init__( name )

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

class EFBMuMuFex_DiMu_11invm24_noOS (EFBMuMuFex_DiMu_11invm60_noOS):
    __slots__ = []
    def __init__(self, name = "EFBMuMuFex_DiMu_11invm24_noOS"):
        super( EFBMuMuFex_DiMu_11invm24_noOS, self ).__init__( name )
        
        # L2 Bmumu cuts
        self.LowerMassCut      = 11000.
        self.UpperMassCut      = 24000.
        
        
class EFBMuMuFex_DiMu_24invm60_noOS (EFBMuMuFex_DiMu_11invm60_noOS):
    __slots__ = []
    def __init__(self, name = "EFBMuMuFex_DiMu_24invm60_noOS"):
        super( EFBMuMuFex_DiMu_24invm60_noOS, self ).__init__( name )
        
        # L2 Bmumu cuts
        self.LowerMassCut      = 24000.
        self.UpperMassCut      = 60000.
        
        
class EFBMuMuFex_DiMu_18invm60_noOS (TrigEFBMuMuFex):
    __slots__ = []
    def __init__(self, name = "EFBMuMuFex_DiMu_18invm60_noOS"):
        super( TrigEFBMuMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        self.LowerMassCut      = 18000.
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
        super( EFBMuMuFex_noId, self ).__init__( name )

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

class EFBMuMuFex_DiMu_noinvm_SS (TrigEFBMuMuFex):
    __slots__ = []
    def __init__(self, name = "EFBMuMuFex_DiMu_noinvm_SS"):
        super( EFBMuMuFex_DiMu_noinvm_SS, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        self.LowerMassCut      = 0.
        self.UpperMassCut      = 15000.
        self.OppositeSign      = False
        self.SameSign          = True
        self.ApplyUpperMassCut = False
        self.MuonAlgo          = "TrigMuSuperEF"

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigEFBMuMuFexMonitoring import TrigEFBMuMuFexValidationMonitoring
        validation = TrigEFBMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigEFBMuMuFexMonitoring import TrigEFBMuMuFexOnlineMonitoring
        online = TrigEFBMuMuFexOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuFex_passEF (TrigEFBMuMuFex):
    __slots__ = []
    def __init__(self, name = "EFBMuMuFex_passEF"):
        super( EFBMuMuFex_passEF, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = True

        # L2 Bmumu cuts
        self.LowerMassCut      = 100.
        self.UpperMassCut      = 13000.
        self.ApplyUpperMassCut = True
        self.MuonAlgo          = "TrigMuSuperEF"

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigEFBMuMuFexMonitoring import TrigEFBMuMuFexValidationMonitoring
        validation = TrigEFBMuMuFexValidationMonitoring()
        from TrigBphysHypo.TrigEFBMuMuFexMonitoring import TrigEFBMuMuFexOnlineMonitoring_passEF
        online = TrigEFBMuMuFexOnlineMonitoring_passEF()

        self.AthenaMonTools = [ validation, online, time ]
