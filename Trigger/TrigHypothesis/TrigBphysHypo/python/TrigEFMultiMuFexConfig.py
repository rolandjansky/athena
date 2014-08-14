# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBphysHypo.TrigBphysHypoConf import TrigEFMultiMuFex,TrigEFMultiMuFex_xAOD

from AthenaCommon.AppMgr import ToolSvc

class EFMultiMuFex_DiMu (TrigEFMultiMuFex_xAOD):
    __slots__ = []
    def __init__(self, name = "EFMultiMuFex_DiMu"):
        super( TrigEFMultiMuFex_xAOD, self ).__init__( name )

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

class EFMultiMuFex_Jpsi (TrigEFMultiMuFex_xAOD):
    __slots__ = []
    def __init__(self, name = "EFMultiMuFex_Jpsi"):
        super( TrigEFMultiMuFex_xAOD, self ).__init__( name )

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

class EFMultiMuFex_Upsi (TrigEFMultiMuFex_xAOD):
    __slots__ = []
    def __init__(self, name = "EFMultiMuFex_Upsi"):
        super( TrigEFMultiMuFex_xAOD, self ).__init__( name )

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

class EFMultiMuFex_Tau (TrigEFMultiMuFex_xAOD):
    __slots__ = []
    def __init__(self, name = "EFMultiMuFex_Tau"):
        super( TrigEFMultiMuFex_xAOD, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        self.LowerMassCut      = 1000.
        self.UpperMassCut     = 2500.
        self.MuonAlgo = "TrigMuSuperEF"
        self.NMassMuon = 3
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigEFMultiMuFexMonitoring import TrigEFMultiMuFexValidationMonitoring
        validation = TrigEFMultiMuFexValidationMonitoring()
        from TrigBphysHypo.TrigEFMultiMuFexMonitoring import TrigEFMultiMuFexOnlineMonitoring
        online = TrigEFMultiMuFexOnlineMonitoring()
                                
        self.AthenaMonTools = [ validation, online, time ]

