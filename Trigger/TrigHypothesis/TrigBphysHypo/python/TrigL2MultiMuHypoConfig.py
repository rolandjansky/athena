# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from TrigBphysHypo.TrigBphysHypoConf import TrigL2MultiMuHypo

class L2MultiMuHypo_DiMu (TrigL2MultiMuHypo):
    __slots__ = []
    def __init__(self, name = "L2MultiMuHypo_DiMu"):
        super( TrigL2MultiMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        self.LowerMassCut      = 1500.
        self.UpperMassCut     = 14000.
        self.ApplyUpperMassCut     = True
        self.Chi2VtxCut       =   20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2MultiMuHypoMonitoring import TrigL2MultiMuHypoValidationMonitoring
        validation = TrigL2MultiMuHypoValidationMonitoring()
        from TrigBphysHypo.TrigL2MultiMuHypoMonitoring import TrigL2MultiMuHypoOnlineMonitoring
        online = TrigL2MultiMuHypoOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class L2MultiMuHypo_Jpsi (TrigL2MultiMuHypo):
    __slots__ = []
    def __init__(self, name = "L2MultiMuHypo_Jpsi"):
        super( TrigL2MultiMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        self.LowerMassCut      = 2500.
        self.UpperMassCut     = 4300.
        self.ApplyUpperMassCut     = True
        self.Chi2VtxCut       =   20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2MultiMuHypoMonitoring import TrigL2MultiMuHypoValidationMonitoring
        validation = TrigL2MultiMuHypoValidationMonitoring()
        from TrigBphysHypo.TrigL2MultiMuHypoMonitoring import TrigL2MultiMuHypoOnlineMonitoring
        online = TrigL2MultiMuHypoOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class L2MultiMuHypo_Upsi (TrigL2MultiMuHypo):
    __slots__ = []
    def __init__(self, name = "L2MultiMuHypo_Upsi"):
        super( TrigL2MultiMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        self.LowerMassCut      = 8000.
        self.UpperMassCut     = 12000.
        self.ApplyUpperMassCut     = True
        self.Chi2VtxCut       =   20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2MultiMuHypoMonitoring import TrigL2MultiMuHypoValidationMonitoring
        validation = TrigL2MultiMuHypoValidationMonitoring()
        from TrigBphysHypo.TrigL2MultiMuHypoMonitoring import TrigL2MultiMuHypoOnlineMonitoring
        online = TrigL2MultiMuHypoOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class L2MultiMuHypo_Tau (TrigL2MultiMuHypo):
    __slots__ = []
    def __init__(self, name = "L2MultiMuHypo_Tau"):
        super( TrigL2MultiMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Bmumu cuts
        self.LowerMassCut      = 0.
        self.UpperMassCut     = 2900.
        self.ApplyUpperMassCut     = True
        self.Chi2VtxCut       =   150.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2MultiMuHypoMonitoring import TrigL2MultiMuHypoValidationMonitoring
        validation = TrigL2MultiMuHypoValidationMonitoring()
        from TrigBphysHypo.TrigL2MultiMuHypoMonitoring import TrigL2MultiMuHypoOnlineMonitoring
        online = TrigL2MultiMuHypoOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

class L2MultiMuHypo_Tau_passL2 (TrigL2MultiMuHypo):
    __slots__ = []
    def __init__(self, name = "L2MultiMuHypo_Tau_passL2"):
        super( TrigL2MultiMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = True

        # L2 Bmumu cuts
        self.LowerMassCut      = 0.
        self.UpperMassCut     = 2900.
        self.ApplyUpperMassCut     = True
        self.Chi2VtxCut       =   150.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        from TrigBphysHypo.TrigL2MultiMuHypoMonitoring import TrigL2MultiMuHypoValidationMonitoring
        validation = TrigL2MultiMuHypoValidationMonitoring()
        from TrigBphysHypo.TrigL2MultiMuHypoMonitoring import TrigL2MultiMuHypoOnlineMonitoring
        online = TrigL2MultiMuHypoOnlineMonitoring()

        self.AthenaMonTools = [ validation, online, time ]

