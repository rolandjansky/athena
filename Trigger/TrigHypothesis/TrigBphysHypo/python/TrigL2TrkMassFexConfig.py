# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBphysHypo.TrigBphysHypoConf import TrigL2TrkMassFex
from TrigVertexFitter.TrigVertexFitterConf import TrigL2VertexFitter
from TrigVertexFitter.TrigVertexFitterConf import TrigVertexFitter
from TrigVertexFitter.TrigVertexFitterConf import TrigVertexingTool

L2VFitTool = TrigL2VertexFitter()
VFitTool = TrigVertexFitter()
VertexTool = TrigVertexingTool()

from __main__ import ToolSvc

ToolSvc += L2VFitTool
ToolSvc += VFitTool
ToolSvc += VertexTool


# basic cut
class L2TrkMassFex_Jpsimumu (TrigL2TrkMassFex):
    __slots__ = []
    def __init__(self, name = "L2TrkMassFex_Jpsimumu"):
        super( TrigL2TrkMassFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 TrkMass cuts
        self.TrackPTthr          = 4000
        self.dEtaTrackRoI        = 0.1
        self.dPhiTrackRoI        = 0.1
        self.Mass_low_cut        = 2850.
        self.Mass_high_cut       = 3350.
        self.doVertexFit         = True
        self.DaughterMass        = 105.658357
        self.TrigL2VertexFitter = L2VFitTool
        self.TrigVertexFitter = VFitTool
        self.TrigVertexingTool = VertexTool
        
        from TrigBphysHypo.TrigL2TrkMassFexMonitoring import TrigL2TrkMassFexValidationMonitoring_RoI
        validation = TrigL2TrkMassFexValidationMonitoring_RoI()
        
        from TrigBphysHypo.TrigL2TrkMassFexMonitoring import TrigL2TrkMassFexOnlineMonitoring_RoI
        online = TrigL2TrkMassFexOnlineMonitoring_RoI()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
                
        self.AthenaMonTools = [ validation, online, time ]

# pass all
class L2TrkMassFex_Jpsimumu_passL2 (L2TrkMassFex_Jpsimumu):
    __slots__ = []
    def __init__(self, name = "L2TrkMassFex_Jpsimumu_passL2"):
        super( L2TrkMassFex_Jpsimumu_passL2, self ).__init__( name )
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = True

# loose cut
class L2TrkMassFex_Jpsimumu_loose (TrigL2TrkMassFex):
    __slots__ = []
    def __init__(self, name = "L2TrkMassFex_Jpsimumu_loose"):
        super( TrigL2TrkMassFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 TrkMass cuts
        self.OppositeCharge = False
        self.TrackPTthr          = 3500
        self.dEtaTrackRoI        = 0.1
        self.dPhiTrackRoI        = 0.1
        self.Mass_low_cut        = 2600.
        self.Mass_high_cut       = 3600.
        self.doVertexFit         = True
        self.DaughterMass        = 105.658357
        self.TrigL2VertexFitter = L2VFitTool
        self.TrigVertexFitter = VFitTool
        self.TrigVertexingTool = VertexTool
        
        from TrigBphysHypo.TrigL2TrkMassFexMonitoring import TrigL2TrkMassFexValidationMonitoring_RoI
        validation = TrigL2TrkMassFexValidationMonitoring_RoI()
        
        from TrigBphysHypo.TrigL2TrkMassFexMonitoring import TrigL2TrkMassFexOnlineMonitoring_RoI
        online = TrigL2TrkMassFexOnlineMonitoring_RoI()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
                
        self.AthenaMonTools = [ validation, online, time ]


# pass all
class L2TrkMassFex_Jpsimumu_loose_passL2 (L2TrkMassFex_Jpsimumu_loose):
    __slots__ = []
    def __init__(self, name = "L2TrkMassFex_Jpsimumu_loose_passL2"):
        super( L2TrkMassFex_Jpsimumu_loose_passL2, self ).__init__( name )
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = True

# looser cut
class L2TrkMassFex_Jpsimumu_looser (TrigL2TrkMassFex):
    __slots__ = []
    def __init__(self, name = "L2TrkMassFex_Jpsimumu_looser"):
        super( TrigL2TrkMassFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 TrkMass cuts
        self.OppositeCharge = False
        self.TrackPTthr          = 2000.
        self.dEtaTrackRoI        = 0.1
        self.dPhiTrackRoI        = 0.1
        self.Mass_low_cut        = 2600.
        self.Mass_high_cut       = 3600.
        self.doVertexFit         = True
        self.DaughterMass        = 105.658357
        self.TrigL2VertexFitter = L2VFitTool
        self.TrigVertexFitter = VFitTool
        self.TrigVertexingTool = VertexTool
        
        from TrigBphysHypo.TrigL2TrkMassFexMonitoring import TrigL2TrkMassFexValidationMonitoring_RoI
        validation = TrigL2TrkMassFexValidationMonitoring_RoI()
        
        from TrigBphysHypo.TrigL2TrkMassFexMonitoring import TrigL2TrkMassFexOnlineMonitoring_RoI
        online = TrigL2TrkMassFexOnlineMonitoring_RoI()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
                
        self.AthenaMonTools = [ validation, online, time ]


# pass all
class L2TrkMassFex_Jpsimumu_looser_passL2 (L2TrkMassFex_Jpsimumu_looser):
    __slots__ = []
    def __init__(self, name = "L2TrkMassFex_Jpsimumu_looser_passL2"):
        super( L2TrkMassFex_Jpsimumu_looser_passL2, self ).__init__( name )
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = True

# basic cut
class L2TrkMassFex_Jpsimumu_FS (TrigL2TrkMassFex):
    __slots__ = []
    def __init__(self, name = "L2TrkMassFex_Jpsimumu_FS"):
        super( TrigL2TrkMassFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 TrkMass cuts
        self.TrackPTthr          = 4000
        self.dEtaTrackRoI        = 0.1
        self.dPhiTrackRoI        = 0.1
        self.Mass_low_cut        = 2850.
        self.Mass_high_cut       = 3350.
        self.DaughterMass        = 105.658357
        self.doVertexFit         = True
        self.TrigL2VertexFitter = L2VFitTool
        self.TrigVertexFitter = VFitTool
        self.TrigVertexingTool = VertexTool
        
        from TrigBphysHypo.TrigL2TrkMassFexMonitoring import TrigL2TrkMassFexValidationMonitoring_FS
        validation = TrigL2TrkMassFexValidationMonitoring_FS()
        
        from TrigBphysHypo.TrigL2TrkMassFexMonitoring import TrigL2TrkMassFexOnlineMonitoring_FS
        online = TrigL2TrkMassFexOnlineMonitoring_FS()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
                
        self.AthenaMonTools = [ validation, online, time ]


# pass all
class L2TrkMassFex_Jpsimumu_FS_passL2 (L2TrkMassFex_Jpsimumu_FS):
    __slots__ = []
    def __init__(self, name = "L2TrkMassFex_Jpsimumu_FS_passL2"):
        super( L2TrkMassFex_Jpsimumu_FS_passL2, self ).__init__( name )
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = True

# FS loose cut
class L2TrkMassFex_Jpsimumu_loose_FS (TrigL2TrkMassFex):
    __slots__ = []
    def __init__(self, name = "L2TrkMassFex_Jpsimumu_loose_FS"):
        super( TrigL2TrkMassFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 TrkMass cuts
        self.OppositeCharge      = False
        self.TrackPTthr          = 3000
        self.dEtaTrackRoI        = 0.1
        self.dPhiTrackRoI        = 0.1
        self.Mass_low_cut        = 2500.
        self.Mass_high_cut       = 4000.
        self.DaughterMass        = 105.658357
        self.doVertexFit         = True
        self.TrigL2VertexFitter = L2VFitTool
        self.TrigVertexFitter = VFitTool
        self.TrigVertexingTool = VertexTool
        
        from TrigBphysHypo.TrigL2TrkMassFexMonitoring import TrigL2TrkMassFexValidationMonitoring_FS
        validation = TrigL2TrkMassFexValidationMonitoring_FS()
        
        from TrigBphysHypo.TrigL2TrkMassFexMonitoring import TrigL2TrkMassFexOnlineMonitoring_FS
        online = TrigL2TrkMassFexOnlineMonitoring_FS()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
                
        self.AthenaMonTools = [ validation, online, time ]

class L2TrkMassFex_Jpsimumu_loose_FS_passL2 (L2TrkMassFex_Jpsimumu_loose_FS):
    __slots__ = []
    def __init__(self, name = "L2TrkMassFex_Jpsimumu_loose_FS_passL2"):
        super(L2TrkMassFex_Jpsimumu_loose_FS_passL2, self ).__init__( name )
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = True

# Upsi FS basic cut
class L2TrkMassFex_Upsimumu_FS (TrigL2TrkMassFex):
    __slots__ = []
    def __init__(self, name = "L2TrkMassFex_Upsimumu_FS"):
        super( TrigL2TrkMassFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 TrkMass cuts
        self.TrackPTthr          = 4000
        self.dEtaTrackRoI        = 0.1
        self.dPhiTrackRoI        = 0.1
        self.Mass_low_cut        = 8000.
        self.Mass_high_cut       = 12000.
        self.DaughterMass        = 105.658357
        self.doVertexFit         = True
        self.TrigL2VertexFitter = L2VFitTool
        self.TrigVertexFitter = VFitTool
        self.TrigVertexingTool = VertexTool
        
        from TrigBphysHypo.TrigL2TrkMassFexMonitoring import TrigL2TrkMassFexValidationMonitoring_Upsi_FS
        validation = TrigL2TrkMassFexValidationMonitoring_Upsi_FS()
        
        from TrigBphysHypo.TrigL2TrkMassFexMonitoring import TrigL2TrkMassFexOnlineMonitoring_Upsi_FS
        online = TrigL2TrkMassFexOnlineMonitoring_Upsi_FS()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
                
        self.AthenaMonTools = [ validation, online, time ]


class L2TrkMassFex_Upsimumu_FS_passL2 (L2TrkMassFex_Upsimumu_FS):
    __slots__ = []
    def __init__(self, name = "L2TrkMassFex_Upsimumu_FS_passL2"):
        super(L2TrkMassFex_Upsimumu_FS_passL2, self ).__init__( name )
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = True

# FS loose cut
class L2TrkMassFex_Upsimumu_loose_FS (TrigL2TrkMassFex):
    __slots__ = []
    def __init__(self, name = "L2TrkMassFex_Upsimumu_loose_FS"):
        super( TrigL2TrkMassFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 TrkMass cuts
        self.OppositeCharge      = False
        self.TrackPTthr          = 3000
        self.dEtaTrackRoI        = 0.1
        self.dPhiTrackRoI        = 0.1
        self.Mass_low_cut        = 8000.
        self.Mass_high_cut       = 12000.
        self.DaughterMass        = 105.658357
        self.doVertexFit         = True
        self.TrigL2VertexFitter = L2VFitTool
        self.TrigVertexFitter = VFitTool
        self.TrigVertexingTool = VertexTool
        
        from TrigBphysHypo.TrigL2TrkMassFexMonitoring import TrigL2TrkMassFexValidationMonitoring_Upsi_FS
        validation = TrigL2TrkMassFexValidationMonitoring_Upsi_FS()
        
        from TrigBphysHypo.TrigL2TrkMassFexMonitoring import TrigL2TrkMassFexOnlineMonitoring_Upsi_FS
        online = TrigL2TrkMassFexOnlineMonitoring_Upsi_FS()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
                
        self.AthenaMonTools = [ validation, online, time ]

class L2TrkMassFex_Upsimumu_loose_FS_passL2 (L2TrkMassFex_Upsimumu_loose_FS):
    __slots__ = []
    def __init__(self, name = "L2TrkMassFex_Upsimumu_loose_FS_passL2"):
        super(L2TrkMassFex_Upsimumu_loose_FS_passL2, self ).__init__( name )
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = True


