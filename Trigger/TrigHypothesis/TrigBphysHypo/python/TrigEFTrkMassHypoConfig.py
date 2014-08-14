# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBphysHypo.TrigBphysHypoConf import TrigEFTrkMassHypo,TrigEFTrkMassHypo_xAOD

# Jpsi
class EFTrkMassHypo_Jpsimumu (TrigEFTrkMassHypo_xAOD):
    __slots__ = []
    def __init__(self, name = "EFTrkMassHypo_Jpsimumu"):
        super( TrigEFTrkMassHypo_xAOD, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # mass cuts
        self.AcceptAll = False
        from TrigBphysHypo.TrigEFTrkMassHypoMonitoring import TrigEFTrkMassHypoValidationMonitoring_RoI
        validation = TrigEFTrkMassHypoValidationMonitoring_RoI()

        from TrigBphysHypo.TrigEFTrkMassHypoMonitoring import TrigEFTrkMassHypoOnlineMonitoring_RoI
        online = TrigEFTrkMassHypoOnlineMonitoring_RoI()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
	
        self.AthenaMonTools = [ validation, online, time ]
# Jpsi FS
class EFTrkMassHypo_Jpsimumu_FS (TrigEFTrkMassHypo_xAOD):
    __slots__ = []
    def __init__(self, name = "EFTrkMassHypo_Jpsimumu_FS"):
        super( TrigEFTrkMassHypo_xAOD, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        from TrigBphysHypo.TrigEFTrkMassHypoMonitoring import TrigEFTrkMassHypoValidationMonitoring_FS
        validation = TrigEFTrkMassHypoValidationMonitoring_FS()

        from TrigBphysHypo.TrigEFTrkMassHypoMonitoring import TrigEFTrkMassHypoOnlineMonitoring_FS
        online = TrigEFTrkMassHypoOnlineMonitoring_FS()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
	
        self.AthenaMonTools = [ validation, online, time ]

# Jpsi loose
class EFTrkMassHypo_Jpsimumu_loose (TrigEFTrkMassHypo_xAOD):
    __slots__ = []
    def __init__(self, name = "EFTrkMassHypo_Jpsimumu_loose"):
        super( TrigEFTrkMassHypo_xAOD, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # mass cuts
        self.AcceptAll = False
        from TrigBphysHypo.TrigEFTrkMassHypoMonitoring import TrigEFTrkMassHypoValidationMonitoring_RoI
        validation = TrigEFTrkMassHypoValidationMonitoring_RoI()

        from TrigBphysHypo.TrigEFTrkMassHypoMonitoring import TrigEFTrkMassHypoOnlineMonitoring_RoI
        online = TrigEFTrkMassHypoOnlineMonitoring_RoI()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
	
        self.AthenaMonTools = [ validation, online, time ]
# Jpsi looser
class EFTrkMassHypo_Jpsimumu_looser (TrigEFTrkMassHypo_xAOD):
    __slots__ = []
    def __init__(self, name = "EFTrkMassHypo_Jpsimumu_looser"):
        super( TrigEFTrkMassHypo_xAOD, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # mass cuts
        self.AcceptAll = False
        from TrigBphysHypo.TrigEFTrkMassHypoMonitoring import TrigEFTrkMassHypoValidationMonitoring_RoI
        validation = TrigEFTrkMassHypoValidationMonitoring_RoI()

        from TrigBphysHypo.TrigEFTrkMassHypoMonitoring import TrigEFTrkMassHypoOnlineMonitoring_RoI
        online = TrigEFTrkMassHypoOnlineMonitoring_RoI()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
	
        self.AthenaMonTools = [ validation, online, time ]
        
# Jpsi loose FS
class EFTrkMassHypo_Jpsimumu_loose_FS (TrigEFTrkMassHypo_xAOD):
    __slots__ = []
    def __init__(self, name = "EFTrkMassHypo_Jpsimumu_loose_FS"):
        super( TrigEFTrkMassHypo_xAOD, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        from TrigBphysHypo.TrigEFTrkMassHypoMonitoring import TrigEFTrkMassHypoValidationMonitoring_FS
        validation = TrigEFTrkMassHypoValidationMonitoring_FS()

        from TrigBphysHypo.TrigEFTrkMassHypoMonitoring import TrigEFTrkMassHypoOnlineMonitoring_FS
        online = TrigEFTrkMassHypoOnlineMonitoring_FS()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
	
        self.AthenaMonTools = [ validation, online, time ]

# Upsi FS
class EFTrkMassHypo_Upsimumu_FS (TrigEFTrkMassHypo_xAOD):
    __slots__ = []
    def __init__(self, name = "EFTrkMassHypo_Upsimumu_FS"):
        super( TrigEFTrkMassHypo_xAOD, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        from TrigBphysHypo.TrigEFTrkMassHypoMonitoring import TrigEFTrkMassHypoValidationMonitoring_Upsi_FS
        validation = TrigEFTrkMassHypoValidationMonitoring_Upsi_FS()

        from TrigBphysHypo.TrigEFTrkMassHypoMonitoring import TrigEFTrkMassHypoOnlineMonitoring_Upsi_FS
        online = TrigEFTrkMassHypoOnlineMonitoring_Upsi_FS()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
	
        self.AthenaMonTools = [ validation, online, time ]


# Upsi loose FS
class EFTrkMassHypo_Upsimumu_loose_FS (TrigEFTrkMassHypo_xAOD):
    __slots__ = []
    def __init__(self, name = "EFTrkMassHypo_Upsimumu_loose_FS"):
        super( TrigEFTrkMassHypo_xAOD, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        from TrigBphysHypo.TrigEFTrkMassHypoMonitoring import TrigEFTrkMassHypoValidationMonitoring_Upsi_FS
        validation = TrigEFTrkMassHypoValidationMonitoring_Upsi_FS()

        from TrigBphysHypo.TrigEFTrkMassHypoMonitoring import TrigEFTrkMassHypoOnlineMonitoring_Upsi_FS
        online = TrigEFTrkMassHypoOnlineMonitoring_Upsi_FS()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
	
        self.AthenaMonTools = [ validation, online, time ]


