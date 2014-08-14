# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBphysHypo.TrigBphysHypoConf import TrigEFBMuMuHypo,TrigEFBMuMuHypo_xAOD

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 

class TrigEFBMuMuHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFBMuMuHypoValidationMonitoring"):
        super(TrigEFBMuMuHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
#        self.defineTarget("Never")
        self.Histograms += [ defineHistogram('CutCounter', type='TH1F', title= "EF cuts counter", xbins=8, xmin=-1.5, xmax=6.5) ]
        self.Histograms += [ defineHistogram('MuMumass', type='TH1F', title="Inv.mass(mumu) [GeV]",
                                           xbins=100, xmin=0., xmax=13.0) ]

class TrigEFBMuMuHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFBMuMuHypoOnlineMonitoring"):
        super(TrigEFBMuMuHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms += [ defineHistogram('CutCounter', type='TH1F', title= "EFBMuMu cuts counter", xbins=8, xmin=-1.5, xmax=6.5) ]
        self.Histograms += [ defineHistogram('MuMumass', type='TH1F', title="EFBMuMu Inv.mass(mumu) [GeV]",
                                           xbins=130, xmin=0., xmax=13.0) ]

class EFBMuMuHypo_1 (TrigEFBMuMuHypo_xAOD):
    __slots__ = []
    def __init__(self, name = "EFBMuMuHypo_1"):
        super( TrigEFBMuMuHypo_xAOD, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF Bmumu cuts
        self.LowerMassCut      = 500.
        self.UpperMassCut      = 13000.
        self.ApplyUpperMassCut = True
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        validation = TrigEFBMuMuHypoValidationMonitoring()
        online = TrigEFBMuMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuHypo_DiMu (TrigEFBMuMuHypo_xAOD):
    __slots__ = []
    def __init__(self, name = "EFBMuMuHypo_DiMu"):
        super( TrigEFBMuMuHypo_xAOD, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF Bmumu cuts
        self.LowerMassCut      = 1500.
        self.UpperMassCut      = 14000.
        self.ApplyUpperMassCut = True
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        validation = TrigEFBMuMuHypoValidationMonitoring()
        online = TrigEFBMuMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuHypo_DiMu_DY (TrigEFBMuMuHypo_xAOD):
    __slots__ = []
    def __init__(self, name = "EFBMuMuHypo_DiMu_DY"):
        super( TrigEFBMuMuHypo_xAOD, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF Bmumu cuts
        self.LowerMassCut      = 10000.
        self.UpperMassCut      = 120000.
        self.ApplyUpperMassCut = True
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        validation = TrigEFBMuMuHypoValidationMonitoring()
        online = TrigEFBMuMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuHypo_DiMu_DY25 (TrigEFBMuMuHypo_xAOD):
    __slots__ = []
    def __init__(self, name = "EFBMuMuHypo_DiMu_DY25"):
        super( TrigEFBMuMuHypo_xAOD, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF Bmumu cuts
        self.LowerMassCut      = 25000.
        self.UpperMassCut      = 120000.
        self.ApplyUpperMassCut = True
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        validation = TrigEFBMuMuHypoValidationMonitoring()
        online = TrigEFBMuMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuHypo_DiMu_DY20 (TrigEFBMuMuHypo_xAOD):
    __slots__ = []
    def __init__(self, name = "EFBMuMuHypo_DiMu_DY20"):
        super( TrigEFBMuMuHypo_xAOD, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF Bmumu cuts
        self.LowerMassCut      = 20000.
        self.UpperMassCut      = 120000.
        self.ApplyUpperMassCut = True
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        validation = TrigEFBMuMuHypoValidationMonitoring()
        online = TrigEFBMuMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuHypo_DiMu_DY14 (TrigEFBMuMuHypo_xAOD):
    __slots__ = []
    def __init__(self, name = "EFBMuMuHypo_DiMu_DY14"):
        super( TrigEFBMuMuHypo_xAOD, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF Bmumu cuts
        self.LowerMassCut      = 14000.
        self.UpperMassCut      = 120000.
        self.ApplyUpperMassCut = True
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        validation = TrigEFBMuMuHypoValidationMonitoring()
        online = TrigEFBMuMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuHypo_Jpsi (TrigEFBMuMuHypo_xAOD):
    __slots__ = []
    def __init__(self, name = "EFBMuMuHypo_Jpsi"):
        super( TrigEFBMuMuHypo_xAOD, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF Bmumu cuts
        self.LowerMassCut      = 2500.
        self.UpperMassCut      = 4300.
        self.ApplyUpperMassCut = True
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        validation = TrigEFBMuMuHypoValidationMonitoring()
        online = TrigEFBMuMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuHypo_Upsi (TrigEFBMuMuHypo_xAOD):
    __slots__ = []
    def __init__(self, name = "EFBMuMuHypo_Upsi"):
        super( TrigEFBMuMuHypo_xAOD, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF Bmumu cuts
        self.LowerMassCut      = 8000.
        self.UpperMassCut      = 12000.
        self.ApplyUpperMassCut = True
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        validation = TrigEFBMuMuHypoValidationMonitoring()
        online = TrigEFBMuMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuHypo_B (TrigEFBMuMuHypo_xAOD):
    __slots__ = []
    def __init__(self, name = "EFBMuMuHypo_B"):
        super( TrigEFBMuMuHypo_xAOD, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF Bmumu cuts
        self.LowerMassCut      = 4000.
        self.UpperMassCut      = 8500.
        self.ApplyUpperMassCut = True
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        validation = TrigEFBMuMuHypoValidationMonitoring()
        online = TrigEFBMuMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuHypo_B_Lxy (TrigEFBMuMuHypo_xAOD):
    __slots__ = []
    def __init__(self, name = "EFBMuMuHypo_B_Lxy"):
        super( TrigEFBMuMuHypo_xAOD, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF Bmumu cuts
        self.LowerMassCut      = 4000.
        self.UpperMassCut      = 8500.
        self.ApplyUpperMassCut = True
        self.Chi2VtxCut        = 20.
        self.ApplyLxyCut = True
        self.LxyCut        = 0.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        validation = TrigEFBMuMuHypoValidationMonitoring()
        online = TrigEFBMuMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuHypo_Z (TrigEFBMuMuHypo_xAOD):
    __slots__ = []
    def __init__(self, name = "EFBMuMuHypo_Z"):
        super( TrigEFBMuMuHypo_xAOD, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF Bmumu cuts
        self.LowerMassCut      = 60000.
        self.UpperMassCut      = 120000.
        self.ApplyUpperMassCut = True
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        validation = TrigEFBMuMuHypoValidationMonitoring()
        online = TrigEFBMuMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuHypo_DiMu_noVtx (TrigEFBMuMuHypo_xAOD):
    __slots__ = []
    def __init__(self, name = "EFBMuMuHypo_DiMu_noVtx"):
        super( TrigEFBMuMuHypo_xAOD, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF Bmumu cuts
        self.LowerMassCut      = 1500.
        self.UpperMassCut      = 14000.
        self.ApplyUpperMassCut = True
        self.ApplyChi2Cut      = False
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        validation = TrigEFBMuMuHypoValidationMonitoring()
        online = TrigEFBMuMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuHypo_DiMu_DY_noVtx (TrigEFBMuMuHypo_xAOD):
    __slots__ = []
    def __init__(self, name = "EFBMuMuHypo_DiMu_DY_noVtx"):
        super( TrigEFBMuMuHypo_xAOD, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF Bmumu cuts
        self.LowerMassCut      = 10000.
        self.UpperMassCut      = 140000.
        self.ApplyUpperMassCut = True
        self.ApplyChi2Cut      = False
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        validation = TrigEFBMuMuHypoValidationMonitoring()
        online = TrigEFBMuMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuHypo_DiMu_DY14_noVtx (TrigEFBMuMuHypo_xAOD):
    __slots__ = []
    def __init__(self, name = "EFBMuMuHypo_DiMu_DY14_noVtx"):
        super( TrigEFBMuMuHypo_xAOD, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF Bmumu cuts
        self.LowerMassCut      = 14000.
        self.UpperMassCut      = 140000.
        self.ApplyUpperMassCut = True
        self.ApplyChi2Cut      = False
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        validation = TrigEFBMuMuHypoValidationMonitoring()
        online = TrigEFBMuMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuHypo_DiMu_7invm9_noVtx (TrigEFBMuMuHypo_xAOD):
    __slots__ = []
    def __init__(self, name = "EFBMuMuHypo_DiMu_7invm9_noVtx"):
        super( TrigEFBMuMuHypo_xAOD, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF Bmumu cuts
        self.LowerMassCut      = 7000.
        self.UpperMassCut      = 9000.
        self.ApplyUpperMassCut = True
        self.ApplyChi2Cut      = False
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        validation = TrigEFBMuMuHypoValidationMonitoring()
        online = TrigEFBMuMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuHypo_DiMu_11invm60_noVtx (TrigEFBMuMuHypo_xAOD):
    __slots__ = []
    def __init__(self, name = "EFBMuMuHypo_DiMu_11invm60_noVtx"):
        super( TrigEFBMuMuHypo_xAOD, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF Bmumu cuts
        self.LowerMassCut      = 11000.
        self.UpperMassCut      = 60000.
        self.ApplyUpperMassCut = True
        self.ApplyChi2Cut      = False
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        validation = TrigEFBMuMuHypoValidationMonitoring()
        online = TrigEFBMuMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]

class EFBMuMuHypo_DiMu_18invm60_noVtx (TrigEFBMuMuHypo_xAOD):
    __slots__ = []
    def __init__(self, name = "EFBMuMuHypo_DiMu_11invm60_noVtx"):
        super( TrigEFBMuMuHypo_xAOD, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF Bmumu cuts
        self.LowerMassCut      = 11000.
        self.UpperMassCut      = 60000.
        self.ApplyUpperMassCut = True
        self.ApplyChi2Cut      = False
        self.Chi2VtxCut        = 20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        validation = TrigEFBMuMuHypoValidationMonitoring()
        online = TrigEFBMuMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]


