# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# include the python fragment to set up the default bphysics vertex fitter
from TrigBphysHypo import TrigBphysVertexingConfig

from TrigBphysHypo.TrigBphysHypoConf import TrigEFTrkMassFex 
from __main__ import ToolSvc


# basic cut
class EFTrkMassFex_Jpsimumu (TrigEFTrkMassFex):
    __slots__ = []
    def __init__(self, name = "EFTrkMassFex_Jpsimumu"):
        super( TrigEFTrkMassFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF TrkMass cuts
        self.TrackPTthr          = 4000
        self.dEtaTrackRoI        = 0.1
        self.dPhiTrackRoI        = 0.1
        self.Mass_low_cut        = 2850.
        self.Mass_high_cut       = 3350.
#        self.doVertexFit         = True
        self.DaughterMass        = 105.658357
#        self.TrigEFVertexFitter = EFVFitTool
#        self.TrigVertexFitter = VFitTool
#        self.TrigVertexingTool = VertexTool
        
        from TrigBphysHypo.TrigEFTrkMassFexMonitoring import TrigEFTrkMassFexValidationMonitoring_RoI
        validation = TrigEFTrkMassFexValidationMonitoring_RoI()
        
        from TrigBphysHypo.TrigEFTrkMassFexMonitoring import TrigEFTrkMassFexOnlineMonitoring_RoI
        online = TrigEFTrkMassFexOnlineMonitoring_RoI()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
                
        self.AthenaMonTools = [ validation, online, time ]


# loose cut
class EFTrkMassFex_Jpsimumu_loose (TrigEFTrkMassFex):
    __slots__ = []
    def __init__(self, name = "EFTrkMassFex_Jpsimumu_loose"):
        super( TrigEFTrkMassFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF TrkMass cuts
        self.TrackPTthr          = 3500.
        self.dEtaTrackRoI        = 0.1
        self.dPhiTrackRoI        = 0.1
        self.Mass_low_cut        = 2600.
        self.Mass_high_cut       = 3600.
#        self.doVertexFit         = True
        self.DaughterMass        = 105.658357
#        self.TrigEFVertexFitter = EFVFitTool
#        self.TrigVertexFitter = VFitTool
#        self.TrigVertexingTool = VertexTool
        
        from TrigBphysHypo.TrigEFTrkMassFexMonitoring import TrigEFTrkMassFexValidationMonitoring_RoI
        validation = TrigEFTrkMassFexValidationMonitoring_RoI()
        
        from TrigBphysHypo.TrigEFTrkMassFexMonitoring import TrigEFTrkMassFexOnlineMonitoring_RoI
        online = TrigEFTrkMassFexOnlineMonitoring_RoI()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
                
        self.AthenaMonTools = [ validation, online, time ]

# loose cut
class EFTrkMassFex_Jpsimumu_looser (TrigEFTrkMassFex):
    __slots__ = []
    def __init__(self, name = "EFTrkMassFex_Jpsimumu_looser"):
        super( TrigEFTrkMassFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF TrkMass cuts
        self.TrackPTthr          = 2000
        self.dEtaTrackRoI        = 0.1
        self.dPhiTrackRoI        = 0.1
        self.Mass_low_cut        = 2600.
        self.Mass_high_cut       = 3600.
#        self.doVertexFit         = True
        self.DaughterMass        = 105.658357
#        self.TrigEFVertexFitter = EFVFitTool
#        self.TrigVertexFitter = VFitTool
#        self.TrigVertexingTool = VertexTool
        
        from TrigBphysHypo.TrigEFTrkMassFexMonitoring import TrigEFTrkMassFexValidationMonitoring_RoI
        validation = TrigEFTrkMassFexValidationMonitoring_RoI()
        
        from TrigBphysHypo.TrigEFTrkMassFexMonitoring import TrigEFTrkMassFexOnlineMonitoring_RoI
        online = TrigEFTrkMassFexOnlineMonitoring_RoI()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
                
        self.AthenaMonTools = [ validation, online, time ]


# FS basic cut
class EFTrkMassFex_Jpsimumu_FS (TrigEFTrkMassFex):
    __slots__ = []
    def __init__(self, name = "EFTrkMassFex_Jpsimumu_FS"):
        super( TrigEFTrkMassFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF TrkMass cuts
        self.TrackPTthr          = 4000
        self.dEtaTrackRoI        = 0.1
        self.dPhiTrackRoI        = 0.1
        self.Mass_low_cut        = 2800.
        self.Mass_high_cut       = 3350.
        self.DaughterMass        = 105.658357
#        self.doVertexFit         = True
#        self.TrigEFVertexFitter = EFVFitTool
#        self.TrigVertexFitter = VFitTool
#        self.TrigVertexingTool = VertexTool
        
        from TrigBphysHypo.TrigEFTrkMassFexMonitoring import TrigEFTrkMassFexValidationMonitoring_FS
        validation = TrigEFTrkMassFexValidationMonitoring_FS()
        
        from TrigBphysHypo.TrigEFTrkMassFexMonitoring import TrigEFTrkMassFexOnlineMonitoring_FS
        online = TrigEFTrkMassFexOnlineMonitoring_FS()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
                
        self.AthenaMonTools = [ validation, online, time ]


# FS loose cut
class EFTrkMassFex_Jpsimumu_loose_FS (TrigEFTrkMassFex):
    __slots__ = []
    def __init__(self, name = "EFTrkMassFex_Jpsimumu_loose_FS"):
        super( TrigEFTrkMassFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF TrkMass cuts
        self.TrackPTthr          = 2000
        self.dEtaTrackRoI        = 0.1
        self.dPhiTrackRoI        = 0.1
        self.Mass_low_cut        = 2500.
        self.Mass_high_cut       = 4000.
        self.DaughterMass        = 105.658357
#        self.doVertexFit         = True
#        self.TrigEFVertexFitter = EFVFitTool
#        self.TrigVertexFitter = VFitTool
#        self.TrigVertexingTool = VertexTool
        
        from TrigBphysHypo.TrigEFTrkMassFexMonitoring import TrigEFTrkMassFexValidationMonitoring_FS
        validation = TrigEFTrkMassFexValidationMonitoring_FS()
        
        from TrigBphysHypo.TrigEFTrkMassFexMonitoring import TrigEFTrkMassFexOnlineMonitoring_FS
        online = TrigEFTrkMassFexOnlineMonitoring_FS()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
                
        self.AthenaMonTools = [ validation, online, time ]

# FS Upsi basic cut
class EFTrkMassFex_Upsimumu_FS (TrigEFTrkMassFex):
    __slots__ = []
    def __init__(self, name = "EFTrkMassFex_Upsimumu_FS"):
        super( TrigEFTrkMassFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF TrkMass cuts
        self.TrackPTthr          = 4000
        self.dEtaTrackRoI        = 0.1
        self.dPhiTrackRoI        = 0.1
        self.Mass_low_cut        = 8000.
        self.Mass_high_cut       = 12000.
        self.DaughterMass        = 105.658357
#        self.doVertexFit         = True
#        self.TrigEFVertexFitter = EFVFitTool
#        self.TrigVertexFitter = VFitTool
#        self.TrigVertexingTool = VertexTool
        
        from TrigBphysHypo.TrigEFTrkMassFexMonitoring import TrigEFTrkMassFexValidationMonitoring_Upsi_FS
        validation = TrigEFTrkMassFexValidationMonitoring_Upsi_FS()
        
        from TrigBphysHypo.TrigEFTrkMassFexMonitoring import TrigEFTrkMassFexOnlineMonitoring_Upsi_FS
        online = TrigEFTrkMassFexOnlineMonitoring_Upsi_FS()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
                
        self.AthenaMonTools = [ validation, online, time ]

# FS Upsi loose cut
class EFTrkMassFex_Upsimumu_loose_FS (TrigEFTrkMassFex):
    __slots__ = []
    def __init__(self, name = "EFTrkMassFex_Upsimumu_loose_FS"):
        super( TrigEFTrkMassFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF TrkMass cuts
        self.TrackPTthr          = 2000
        self.dEtaTrackRoI        = 0.1
        self.dPhiTrackRoI        = 0.1
        self.Mass_low_cut        = 8000.
        self.Mass_high_cut       = 12000.
        self.DaughterMass        = 105.658357
#        self.doVertexFit         = True
#        self.TrigEFVertexFitter = EFVFitTool
#        self.TrigVertexFitter = VFitTool
#        self.TrigVertexingTool = VertexTool
        
        from TrigBphysHypo.TrigEFTrkMassFexMonitoring import TrigEFTrkMassFexValidationMonitoring_Upsi_FS
        validation = TrigEFTrkMassFexValidationMonitoring_Upsi_FS()
        
        from TrigBphysHypo.TrigEFTrkMassFexMonitoring import TrigEFTrkMassFexOnlineMonitoring_Upsi_FS
        online = TrigEFTrkMassFexOnlineMonitoring_Upsi_FS()
        
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
                
        self.AthenaMonTools = [ validation, online, time ]


