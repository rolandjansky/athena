# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBphysHypo.TrigBphysHypoConf import TrigL2DiMuHypo

# Jpsi
class TrigL2DiMuHypo_Jpsi (TrigL2DiMuHypo):
    __slots__ = []
    def __init__(self, name = "TrigL2DiMuHypo_Jpsi"):
        super( TrigL2DiMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # mass cuts
        self.MuMuMassMin = 2500.
        self.MuMuMassMax = 4300.
        self.ApplyMuMuMassMax = True
        from TrigBphysHypo.TrigL2DiMuHypoMonitoring import TrigL2DiMuHypoValidationMonitoring_Jpsi
        validation = TrigL2DiMuHypoValidationMonitoring_Jpsi()

        from TrigBphysHypo.TrigL2DiMuHypoMonitoring import TrigL2DiMuHypoOnlineMonitoring_Jpsi
        online = TrigL2DiMuHypoOnlineMonitoring_Jpsi()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
	
        self.AthenaMonTools = [ validation, online, time ]

# tight jpsi
class TrigL2DiMuHypo_Jpsi_tight (TrigL2DiMuHypo):
    __slots__ = []
    def __init__(self, name = "TrigL2DiMuHypo_Jpsi_tight"):
        super( TrigL2DiMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # mass cuts
        self.MuMuMassMin = 2500.
        self.MuMuMassMax = 4300.
        self.ApplyMuMuMassMax = True

        ## tighter cuts
        self.NHits = 6
        self.ApplyChi2Cut = True
        self.Chi2VtxCut = 20      # same as topological trigger

        from TrigBphysHypo.TrigL2DiMuHypoMonitoring import TrigL2DiMuHypoValidationMonitoring_Jpsi
        validation = TrigL2DiMuHypoValidationMonitoring_Jpsi()

        from TrigBphysHypo.TrigL2DiMuHypoMonitoring import TrigL2DiMuHypoOnlineMonitoring_Jpsi
        online = TrigL2DiMuHypoOnlineMonitoring_Jpsi()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        self.AthenaMonTools = [ validation, online, time ]


# Upsilon
class TrigL2DiMuHypo_Upsi (TrigL2DiMuHypo):
    __slots__ = []
    def __init__(self, name = "TrigL2DiMuHypo_Upsi"):
        super( TrigL2DiMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # mass cuts
        self.MuMuMassMin = 8000.
        self.MuMuMassMax = 12000.
        self.ApplyMuMuMassMax = True

        from TrigBphysHypo.TrigL2DiMuHypoMonitoring import TrigL2DiMuHypoValidationMonitoring_Upsi
        validation = TrigL2DiMuHypoValidationMonitoring_Upsi()

        from TrigBphysHypo.TrigL2DiMuHypoMonitoring import TrigL2DiMuHypoOnlineMonitoring_Upsi
        online = TrigL2DiMuHypoOnlineMonitoring_Upsi()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        self.AthenaMonTools = [ validation, online, time ]

# tight Upsilon
class TrigL2DiMuHypo_Upsi_tight (TrigL2DiMuHypo):
    __slots__ = []
    def __init__(self, name = "TrigL2DiMuHypo_Upsi_tight"):
        super( TrigL2DiMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # mass cuts
        self.MuMuMassMin = 8000.
        self.MuMuMassMax = 12000.
        self.ApplyMuMuMassMax = True

        ## tighter cuts
        self.NHits = 6
        self.ApplyChi2Cut = True
        self.Chi2VtxCut = 20      # same as topological trigger

        from TrigBphysHypo.TrigL2DiMuHypoMonitoring import TrigL2DiMuHypoValidationMonitoring_Upsi
        validation = TrigL2DiMuHypoValidationMonitoring_Upsi()

        from TrigBphysHypo.TrigL2DiMuHypoMonitoring import TrigL2DiMuHypoOnlineMonitoring_Upsi
        online = TrigL2DiMuHypoOnlineMonitoring_Upsi()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        self.AthenaMonTools = [ validation, online, time ]

# Bmumu
class TrigL2DiMuHypo_B (TrigL2DiMuHypo):
    __slots__ = []
    def __init__(self, name = "TrigL2DiMuHypo_B"):
        super( TrigL2DiMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # mass cuts
        self.MuMuMassMin = 4000.
        self.MuMuMassMax = 8500.
        self.ApplyMuMuMassMax = True

        from TrigBphysHypo.TrigL2DiMuHypoMonitoring import TrigL2DiMuHypoValidationMonitoring_B
        validation = TrigL2DiMuHypoValidationMonitoring_B()

        from TrigBphysHypo.TrigL2DiMuHypoMonitoring import TrigL2DiMuHypoOnlineMonitoring_B
        online = TrigL2DiMuHypoOnlineMonitoring_B()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        self.AthenaMonTools = [ validation, online, time ]

# DiMu
class TrigL2DiMuHypo_DiMu (TrigL2DiMuHypo):
    __slots__ = []
    def __init__(self, name = "TrigL2DiMuHypo_DiMu"):
        super( TrigL2DiMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # mass cuts
        self.MuMuMassMin = 1500.
        self.MuMuMassMax = 14000.
        self.ApplyMuMuMassMax = True

        from TrigBphysHypo.TrigL2DiMuHypoMonitoring import TrigL2DiMuHypoValidationMonitoring_DiMu
        validation = TrigL2DiMuHypoValidationMonitoring_DiMu()

        from TrigBphysHypo.TrigL2DiMuHypoMonitoring import TrigL2DiMuHypoOnlineMonitoring_DiMu
        online = TrigL2DiMuHypoOnlineMonitoring_DiMu()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        self.AthenaMonTools = [ validation, online, time ]

# Jpsi
class TrigL2DiMuHypo_Jpsi_passL2 (TrigL2DiMuHypo):
    __slots__ = []
    def __init__(self, name = "TrigL2DiMuHypo_Jpsi_passL2"):
        super( TrigL2DiMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = True
        # mass cuts
        self.MuMuMassMin = 2500.
        self.MuMuMassMax = 4300.
        self.ApplyMuMuMassMax = True
        from TrigBphysHypo.TrigL2DiMuHypoMonitoring import TrigL2DiMuHypoValidationMonitoring_Jpsi
        validation = TrigL2DiMuHypoValidationMonitoring_Jpsi()

        from TrigBphysHypo.TrigL2DiMuHypoMonitoring import TrigL2DiMuHypoOnlineMonitoring_Jpsi
        online = TrigL2DiMuHypoOnlineMonitoring_Jpsi()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
	
        self.AthenaMonTools = [ validation, online, time ]

# Upsilon
class TrigL2DiMuHypo_Upsi_passL2 (TrigL2DiMuHypo):
    __slots__ = []
    def __init__(self, name = "TrigL2DiMuHypo_Upsi_passL2"):
        super( TrigL2DiMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = True
        # mass cuts
        self.MuMuMassMin = 8000.
        self.MuMuMassMax = 12000.
        self.ApplyMuMuMassMax = True

        from TrigBphysHypo.TrigL2DiMuHypoMonitoring import TrigL2DiMuHypoValidationMonitoring_Upsi
        validation = TrigL2DiMuHypoValidationMonitoring_Upsi()

        from TrigBphysHypo.TrigL2DiMuHypoMonitoring import TrigL2DiMuHypoOnlineMonitoring_Upsi
        online = TrigL2DiMuHypoOnlineMonitoring_Upsi()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        self.AthenaMonTools = [ validation, online, time ]

# Bmumu
class TrigL2DiMuHypo_B_passL2 (TrigL2DiMuHypo):
    __slots__ = []
    def __init__(self, name = "TrigL2DiMuHypo_B_passL2"):
        super( TrigL2DiMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = True
        # mass cuts
        self.MuMuMassMin = 4000.
        self.MuMuMassMax = 8500.
        self.ApplyMuMuMassMax = True

        from TrigBphysHypo.TrigL2DiMuHypoMonitoring import TrigL2DiMuHypoValidationMonitoring_B
        validation = TrigL2DiMuHypoValidationMonitoring_B()

        from TrigBphysHypo.TrigL2DiMuHypoMonitoring import TrigL2DiMuHypoOnlineMonitoring_B
        online = TrigL2DiMuHypoOnlineMonitoring_B()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        self.AthenaMonTools = [ validation, online, time ]

# DiMu
class TrigL2DiMuHypo_DiMu_passL2 (TrigL2DiMuHypo):
    __slots__ = []
    def __init__(self, name = "TrigL2DiMuHypo_DiMu_passL2"):
        super( TrigL2DiMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = True
        # mass cuts
        self.MuMuMassMin = 1500.
        self.MuMuMassMax = 14000.
        self.ApplyMuMuMassMax = True

        from TrigBphysHypo.TrigL2DiMuHypoMonitoring import TrigL2DiMuHypoValidationMonitoring_DiMu
        validation = TrigL2DiMuHypoValidationMonitoring_DiMu()

        from TrigBphysHypo.TrigL2DiMuHypoMonitoring import TrigL2DiMuHypoOnlineMonitoring_DiMu
        online = TrigL2DiMuHypoOnlineMonitoring_DiMu()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        self.AthenaMonTools = [ validation, online, time ]

# DiMu no Opp sign
class TrigL2DiMuHypo_DiMu_noOS (TrigL2DiMuHypo):
    __slots__ = []
    def __init__(self, name = "TrigL2DiMuHypo_DiMu_noOS"):
        super( TrigL2DiMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # mass cuts
        self.MuMuMassMin = 1500.
        self.MuMuMassMax = 14000.
        self.ApplyOppositeCharge = False
        self.ApplyMuMuMassMax = True

        from TrigBphysHypo.TrigL2DiMuHypoMonitoring import TrigL2DiMuHypoValidationMonitoring_DiMu
        validation = TrigL2DiMuHypoValidationMonitoring_DiMu()

        from TrigBphysHypo.TrigL2DiMuHypoMonitoring import TrigL2DiMuHypoOnlineMonitoring_DiMu
        online = TrigL2DiMuHypoOnlineMonitoring_DiMu()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        self.AthenaMonTools = [ validation, online, time ]

# DiMu no Opp sign
class TrigL2DiMuHypo_DiMu_noOS_passL2 (TrigL2DiMuHypo):
    __slots__ = []
    def __init__(self, name = "TrigL2DiMuHypo_DiMu_noOS_passL2"):
        super( TrigL2DiMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = True
        # mass cuts
        self.MuMuMassMin = 1500.
        self.MuMuMassMax = 14000.
        self.ApplyOppositeCharge = False
        self.ApplyMuMuMassMax = True

        from TrigBphysHypo.TrigL2DiMuHypoMonitoring import TrigL2DiMuHypoValidationMonitoring_DiMu
        validation = TrigL2DiMuHypoValidationMonitoring_DiMu()

        from TrigBphysHypo.TrigL2DiMuHypoMonitoring import TrigL2DiMuHypoOnlineMonitoring_DiMu
        online = TrigL2DiMuHypoOnlineMonitoring_DiMu()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        self.AthenaMonTools = [ validation, online, time ]

