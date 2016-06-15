# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# include the python fragment to set up the default bphysics vertex fitter
from TrigBphysHypo import TrigBphysVertexingConfig

from TrigBphysHypo.TrigBphysHypoConf import TrigEFMultiMuFex

from AthenaCommon.AppMgr import ToolSvc

class EFMultiMuFex_DiMu (TrigEFMultiMuFex):
    __slots__ = []
    def __init__(self, name = "EFMultiMuFex_DiMu"):
        super( TrigEFMultiMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        self.LowerMassCut      = 1000.
        self.UpperMassCut     = 14000.
        self.MuonAlgo = "TrigMuSuperEF"

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigEFMultiMuFexMonitoring import TrigEFMultiMuFexValidationMonitoring
        validation = TrigEFMultiMuFexValidationMonitoring()
        from TrigBphysHypo.TrigEFMultiMuFexMonitoring import TrigEFMultiMuFexOnlineMonitoring
        online = TrigEFMultiMuFexOnlineMonitoring()
                                
        self.AthenaMonTools = [ validation, online, time ]

class EFMultiMuFex_Jpsi (TrigEFMultiMuFex):
    __slots__ = []
    def __init__(self, name = "EFMultiMuFex_Jpsi"):
        super( TrigEFMultiMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        self.LowerMassCut      = 2000.
        self.UpperMassCut     = 4800.
        self.MuonAlgo = "TrigMuSuperEF"

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigEFMultiMuFexMonitoring import TrigEFMultiMuFexValidationMonitoring
        validation = TrigEFMultiMuFexValidationMonitoring()
        from TrigBphysHypo.TrigEFMultiMuFexMonitoring import TrigEFMultiMuFexOnlineMonitoring
        online = TrigEFMultiMuFexOnlineMonitoring()
                                
        self.AthenaMonTools = [ validation, online, time ]

class EFMultiMuFex_Upsi (TrigEFMultiMuFex):
    __slots__ = []
    def __init__(self, name = "EFMultiMuFex_Upsi"):
        super( TrigEFMultiMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        self.LowerMassCut      = 7000.
        self.UpperMassCut     = 13000.
        self.MuonAlgo = "TrigMuSuperEF"

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigEFMultiMuFexMonitoring import TrigEFMultiMuFexValidationMonitoring
        validation = TrigEFMultiMuFexValidationMonitoring()
        from TrigBphysHypo.TrigEFMultiMuFexMonitoring import TrigEFMultiMuFexOnlineMonitoring
        online = TrigEFMultiMuFexOnlineMonitoring()
                                
        self.AthenaMonTools = [ validation, online, time ]

class EFMultiMuFex_Tau (TrigEFMultiMuFex):
    __slots__ = []
    def __init__(self, name = "EFMultiMuFex_Tau"):
        super( TrigEFMultiMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        self.LowerMassCut      = 0.
        self.UpperMassCut     = 2900.
        self.OppositeSign = False
        self.MuonAlgo = "TrigMuSuperEF"
        self.NMassMuon = 3
        self.CheckNinputTE = False
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigEFMultiMuFexMonitoring import TrigEFMultiMuFexValidationMonitoring
        validation = TrigEFMultiMuFexValidationMonitoring()
        from TrigBphysHypo.TrigEFMultiMuFexMonitoring import TrigEFMultiMuFexOnlineMonitoring
        online = TrigEFMultiMuFexOnlineMonitoring()
                                
        self.AthenaMonTools = [ validation, online, time ]

class EFMultiMuFex_Tau2 (TrigEFMultiMuFex):
    __slots__ = []
    def __init__(self, name = "EFMultiMuFex_Tau2"):
        super( TrigEFMultiMuFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        self.OppositeSign = False
        self.LowerMassCut      = 0.
        self.UpperMassCut     = 2900.
        self.MuonAlgo = "TrigMuSuperEF"
        self.NMassMuon = 2
        self.CheckNinputTE = False
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigEFMultiMuFexMonitoring import TrigEFMultiMuFexValidationMonitoring
        validation = TrigEFMultiMuFexValidationMonitoring()
        from TrigBphysHypo.TrigEFMultiMuFexMonitoring import TrigEFMultiMuFexOnlineMonitoring
        online = TrigEFMultiMuFexOnlineMonitoring()
                                
        self.AthenaMonTools = [ validation, online, time ]

