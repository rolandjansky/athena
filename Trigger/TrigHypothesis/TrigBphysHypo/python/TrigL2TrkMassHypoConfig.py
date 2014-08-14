# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBphysHypo.TrigBphysHypoConf import TrigL2TrkMassHypo

# Jpsi
class L2TrkMassHypo_Jpsimumu (TrigL2TrkMassHypo):
    __slots__ = []
    def __init__(self, name = "L2TrkMassHypo_Jpsimumu"):
        super( TrigL2TrkMassHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # mass cuts
        self.AcceptAll = False
        from TrigBphysHypo.TrigL2TrkMassHypoMonitoring import TrigL2TrkMassHypoValidationMonitoring_RoI
        validation = TrigL2TrkMassHypoValidationMonitoring_RoI()

        from TrigBphysHypo.TrigL2TrkMassHypoMonitoring import TrigL2TrkMassHypoOnlineMonitoring_RoI
        online = TrigL2TrkMassHypoOnlineMonitoring_RoI()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
	
        self.AthenaMonTools = [ validation, online, time ]

# Jpsi passL2
class L2TrkMassHypo_Jpsimumu_passL2 (L2TrkMassHypo_Jpsimumu):
    __slots__ = []
    def __init__(self, name = "L2TrkMassHypo_Jpsimumu_passL2"):
        super( L2TrkMassHypo_Jpsimumu_passL2, self ).__init__( name )
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = True


# Jpsi FS
class L2TrkMassHypo_Jpsimumu_FS (TrigL2TrkMassHypo):
    __slots__ = []
    def __init__(self, name = "L2TrkMassHypo_Jpsimumu_FS"):
        super( TrigL2TrkMassHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        from TrigBphysHypo.TrigL2TrkMassHypoMonitoring import TrigL2TrkMassHypoValidationMonitoring_FS
        validation = TrigL2TrkMassHypoValidationMonitoring_FS()

        from TrigBphysHypo.TrigL2TrkMassHypoMonitoring import TrigL2TrkMassHypoOnlineMonitoring_FS
        online = TrigL2TrkMassHypoOnlineMonitoring_FS()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
	
        self.AthenaMonTools = [ validation, online, time ]

# Jpsi passL2
class L2TrkMassHypo_Jpsimumu_FS_passL2 (L2TrkMassHypo_Jpsimumu_FS):
    __slots__ = []
    def __init__(self, name = "L2TrkMassHypo_Jpsimumu_FS_passL2"):
        super( L2TrkMassHypo_Jpsimumu_FS_passL2, self ).__init__( name )
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = True


# Jpsi loose
class L2TrkMassHypo_Jpsimumu_loose (TrigL2TrkMassHypo):
    __slots__ = []
    def __init__(self, name = "L2TrkMassHypo_Jpsimumu_loose"):
        super( TrigL2TrkMassHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # mass cuts
        self.AcceptAll = False
        from TrigBphysHypo.TrigL2TrkMassHypoMonitoring import TrigL2TrkMassHypoValidationMonitoring_RoI
        validation = TrigL2TrkMassHypoValidationMonitoring_RoI()

        from TrigBphysHypo.TrigL2TrkMassHypoMonitoring import TrigL2TrkMassHypoOnlineMonitoring_RoI
        online = TrigL2TrkMassHypoOnlineMonitoring_RoI()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
	
        self.AthenaMonTools = [ validation, online, time ]

# Jpsi loose passL2
class L2TrkMassHypo_Jpsimumu_loose_passL2 (L2TrkMassHypo_Jpsimumu_loose):
    __slots__ = []
    def __init__(self, name = "L2TrkMassHypo_Jpsimumu_loose_passL2"):
        super( L2TrkMassHypo_Jpsimumu_loose_passL2, self ).__init__( name )
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = True

# Jpsi loose
class L2TrkMassHypo_Jpsimumu_looser (TrigL2TrkMassHypo):
    __slots__ = []
    def __init__(self, name = "L2TrkMassHypo_Jpsimumu_looser"):
        super( TrigL2TrkMassHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # mass cuts
        self.AcceptAll = False
        from TrigBphysHypo.TrigL2TrkMassHypoMonitoring import TrigL2TrkMassHypoValidationMonitoring_RoI
        validation = TrigL2TrkMassHypoValidationMonitoring_RoI()

        from TrigBphysHypo.TrigL2TrkMassHypoMonitoring import TrigL2TrkMassHypoOnlineMonitoring_RoI
        online = TrigL2TrkMassHypoOnlineMonitoring_RoI()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
	
        self.AthenaMonTools = [ validation, online, time ]

# Jpsi loose passL2
class L2TrkMassHypo_Jpsimumu_looser_passL2 (L2TrkMassHypo_Jpsimumu_looser):
    __slots__ = []
    def __init__(self, name = "L2TrkMassHypo_Jpsimumu_looser_passL2"):
        super( L2TrkMassHypo_Jpsimumu_looser_passL2, self ).__init__( name )
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = True

# Jpsi FS loose
class L2TrkMassHypo_Jpsimumu_loose_FS (TrigL2TrkMassHypo):
    __slots__ = []
    def __init__(self, name = "L2TrkMassHypo_Jpsimumu_loose_FS"):
        super( TrigL2TrkMassHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        from TrigBphysHypo.TrigL2TrkMassHypoMonitoring import TrigL2TrkMassHypoValidationMonitoring_FS
        validation = TrigL2TrkMassHypoValidationMonitoring_FS()

        from TrigBphysHypo.TrigL2TrkMassHypoMonitoring import TrigL2TrkMassHypoOnlineMonitoring_FS
        online = TrigL2TrkMassHypoOnlineMonitoring_FS()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
	
        self.AthenaMonTools = [ validation, online, time ]


class L2TrkMassHypo_Jpsimumu_loose_FS_passL2 (L2TrkMassHypo_Jpsimumu_loose_FS):
    __slots__ = []
    def __init__(self, name = "L2TrkMassHypo_Jpsimumu_loose_FS_passL2"):
        super( L2TrkMassHypo_Jpsimumu_loose_FS_passL2, self ).__init__( name )
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = True

# Upsi FS 
class L2TrkMassHypo_Upsimumu_FS (TrigL2TrkMassHypo):
    __slots__ = []
    def __init__(self, name = "L2TrkMassHypo_Upsimumu_FS"):
        super( TrigL2TrkMassHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        from TrigBphysHypo.TrigL2TrkMassHypoMonitoring import TrigL2TrkMassHypoValidationMonitoring_Upsi_FS
        validation = TrigL2TrkMassHypoValidationMonitoring_Upsi_FS()

        from TrigBphysHypo.TrigL2TrkMassHypoMonitoring import TrigL2TrkMassHypoOnlineMonitoring_Upsi_FS
        online = TrigL2TrkMassHypoOnlineMonitoring_Upsi_FS()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
	
        self.AthenaMonTools = [ validation, online, time ]


class L2TrkMassHypo_Upsimumu_FS_passL2 (L2TrkMassHypo_Upsimumu_FS):
    __slots__ = []
    def __init__(self, name = "L2TrkMassHypo_Upsimumu_FS_passL2"):
        super( L2TrkMassHypo_Upsimumu_FS_passL2, self ).__init__( name )
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = True

# Upsi FS loose
class L2TrkMassHypo_Upsimumu_loose_FS (TrigL2TrkMassHypo):
    __slots__ = []
    def __init__(self, name = "L2TrkMassHypo_Upsimumu_loose_FS"):
        super( TrigL2TrkMassHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        from TrigBphysHypo.TrigL2TrkMassHypoMonitoring import TrigL2TrkMassHypoValidationMonitoring_Upsi_FS
        validation = TrigL2TrkMassHypoValidationMonitoring_Upsi_FS()

        from TrigBphysHypo.TrigL2TrkMassHypoMonitoring import TrigL2TrkMassHypoOnlineMonitoring_Upsi_FS
        online = TrigL2TrkMassHypoOnlineMonitoring_Upsi_FS()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
	
        self.AthenaMonTools = [ validation, online, time ]


class L2TrkMassHypo_Upsimumu_loose_FS_passL2 (L2TrkMassHypo_Upsimumu_loose_FS):
    __slots__ = []
    def __init__(self, name = "L2TrkMassHypo_Upsimumu_loose_FS_passL2"):
        super( L2TrkMassHypo_Upsimumu_loose_FS_passL2, self ).__init__( name )
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = True

