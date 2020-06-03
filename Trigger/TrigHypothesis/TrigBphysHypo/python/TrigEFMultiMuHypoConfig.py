# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBphysHypo.TrigBphysHypoConf import TrigEFMultiMuHypo

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 

class TrigEFMultiMuHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFMultiMuHypoValidationMonitoring"):
        super(TrigEFMultiMuHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
#        self.defineTarget("Never")
        self.Histograms += [ defineHistogram('CutCounter', type='TH1F', title= "EF cuts counter", xbins=8, xmin=-1.5, xmax=6.5) ]
        self.Histograms += [ defineHistogram('MuMumass', type='TH1F', title="Inv.mass(mumu) [GeV]",
                                           xbins=100, xmin=0., xmax=13.0) ]
        self.Histograms += [ defineHistogram('FitChi2', type='TH1F', title="vertex chi2 (Nmu) ",
                                           xbins=50, xmin=0., xmax=50.0) ]
                                             
class TrigEFMultiMuHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFMultiMuHypoOnlineMonitoring"):
        super(TrigEFMultiMuHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms += [ defineHistogram('CutCounter', type='TH1F', title= "EFMultiMu cuts counter", xbins=8, xmin=-1.5, xmax=6.5) ]
        self.Histograms += [ defineHistogram('MuMumass', type='TH1F', title="EFMultiMu Inv.mass(mumu) [GeV]",
                                           xbins=130, xmin=0., xmax=13.0) ]
        self.Histograms += [ defineHistogram('FitChi2', type='TH1F', title="vertex chi2 (Nmu) ",
                                           xbins=50, xmin=0., xmax=50.0) ]

class EFMultiMuHypo_DiMu (TrigEFMultiMuHypo):
    __slots__ = []
    def __init__(self, name = "EFMultiMuHypo_DiMu"):
        super( EFMultiMuHypo_DiMu, self ).__init__( name )
        
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
	
        # EF Bmumu cuts
        self.LowerMassCut      = 1500.
        self.UpperMassCut     = 14000.
        self.ApplyUpperMassCut     = True
        self.Chi2VtxCut       =   20.
	
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
	
        validation = TrigEFMultiMuHypoValidationMonitoring()
        online = TrigEFMultiMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]

class EFMultiMuHypo_DiMu2700 (TrigEFMultiMuHypo):
    __slots__ = []
    def __init__(self, name = "EFMultiMuHypo_DiMu2700"):
        super( EFMultiMuHypo_DiMu2700, self ).__init__( name )
        
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
	
        # 
        self.OppositeSign = True
        self.LowerMassCut      = 100.
        self.UpperMassCut     = 2700.
        self.ApplyUpperMassCut     = True
        self.Chi2VtxCut       =   20.
	
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
	
        validation = TrigEFMultiMuHypoValidationMonitoring()
        online = TrigEFMultiMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]

class EFMultiMuHypo_DiMu6000 (TrigEFMultiMuHypo):
    __slots__ = []
    def __init__(self, name = "EFMultiMuHypo_DiMu6000"):
        super( EFMultiMuHypo_DiMu6000, self ).__init__( name )
        
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
	
        # 
        self.OppositeSign = True
        self.LowerMassCut      = 100.
        self.UpperMassCut     = 6000.
        self.ApplyUpperMassCut     = True
        self.Chi2VtxCut       =   20.
	
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
	
        validation = TrigEFMultiMuHypoValidationMonitoring()
        online = TrigEFMultiMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]

class EFMultiMuHypo_DiMu_noCut (EFMultiMuHypo_DiMu):
    __slots__ = []
    def __init__(self, name = "EFMultiMuHypo_DiMu_noCut"):
        super( EFMultiMuHypo_DiMu_noCut, self ).__init__( name )
        self.ApplyChi2Cut      = False
        self.ApplyUpperMassCut = False
        self.LowerMassCut    = 0.
	
class EFMultiMuHypo_DiMu_noVtx (EFMultiMuHypo_DiMu):
    __slots__ = []
    def __init__(self, name = "EFMultiMuHypo_DiMu_noVtx"):
        super( EFMultiMuHypo_DiMu_noVtx, self ).__init__( name )
        self.ApplyChi2Cut      = False
        self.ApplyUpperMassCut = False
        
class EFMultiMuHypo_Vtx20 (TrigEFMultiMuHypo):
    __slots__ = []
    def __init__(self, name = "EFMultiMuHypo_Vtx20"):
        super( EFMultiMuHypo_Vtx20, self ).__init__( name )
        
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        self.LowerMassCut      = -1.
        self.UpperMassCut     = 1e+10
        self.ApplyUpperMassCut     = False
        self.ApplyChi2Cut      = True
        self.Chi2VtxCut       =   20.
	
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
	
        validation = TrigEFMultiMuHypoValidationMonitoring()
        online = TrigEFMultiMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]

class EFMultiMuHypo_Vtx60 (EFMultiMuHypo_Vtx20):
    __slots__ = []
    def __init__(self, name = "EFMultiMuHypo_Vtx60"):
        super( EFMultiMuHypo_Vtx60, self ).__init__( name )
        self.ApplyChi2Cut      = True
        self.Chi2VtxCut       =   60.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
	
        validation = TrigEFMultiMuHypoValidationMonitoring()
        online = TrigEFMultiMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]
        

class EFMultiMuHypo_BMeson (TrigEFMultiMuHypo):
    __slots__ = []
    def __init__(self, name = "EFMultiMuHypo_BMeson"):
        super( EFMultiMuHypo_BMeson, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF Bmumu cuts
        self.LowerMassCut      = 4000. # put some loose mass cuts
        self.UpperMassCut     = 8500.  # OI 6500?
        self.ApplyUpperMassCut     = True
        self.Chi2VtxCut       =   60.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        validation = TrigEFMultiMuHypoValidationMonitoring()
        online = TrigEFMultiMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]

class EFMultiMuHypo_Bmumux (TrigEFMultiMuHypo):
    __slots__ = []
    def __init__(self, name = "EFMultiMuHypo_BMeson"):
        super( EFMultiMuHypo_Bmumux, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF Bmumu cuts
        self.LowerMassCut      = 100. # put some loose mass cuts
        self.UpperMassCut     = 6500.  # OI 6500?
        self.ApplyUpperMassCut     = True
        self.Chi2VtxCut       =   20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        validation = TrigEFMultiMuHypoValidationMonitoring()
        online = TrigEFMultiMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]

class EFMultiMuHypo_Jpsi (TrigEFMultiMuHypo):
    __slots__ = []
    def __init__(self, name = "EFMultiMuHypo_Jpsi"):
        super( EFMultiMuHypo_Jpsi, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF Bmumu cuts
        self.LowerMassCut      = 2500.
        self.UpperMassCut     = 4300.
        self.ApplyUpperMassCut     = True
        self.Chi2VtxCut       =   20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        validation = TrigEFMultiMuHypoValidationMonitoring()
        online = TrigEFMultiMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]

class EFMultiMuHypo_Upsi (TrigEFMultiMuHypo):
    __slots__ = []
    def __init__(self, name = "EFMultiMuHypo_Upsi"):
        super( EFMultiMuHypo_Upsi, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF Bmumu cuts
        self.LowerMassCut      = 8000.
        self.UpperMassCut     = 12000.
        self.ApplyUpperMassCut     = True
        self.Chi2VtxCut       =   20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        validation = TrigEFMultiMuHypoValidationMonitoring()
        online = TrigEFMultiMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]

class EFMultiMuHypo_2700 (TrigEFMultiMuHypo):
    __slots__ = []
    def __init__(self, name = "EFMultiMuHypo_2700"):
        super( EFMultiMuHypo_2700, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF bTau cuts
        self.OppositeSign = False
        self.ApplyUpperMassCut     = True
        self.LowerMassCut     = 0.
        self.UpperMassCut     = 2700.
        self.ApplyChi2Cut     = True
        self.Chi2VtxCut       =   50.
        self.bphysCollectionKey = "MultiTrkFex"

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        validation = TrigEFMultiMuHypoValidationMonitoring()
        online = TrigEFMultiMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]

class EFMultiMuHypo_L26000 (TrigEFMultiMuHypo):
    __slots__ = []
    def __init__(self, name = "EFMultiMuHypo_L26000"):
        super( EFMultiMuHypo_L26000, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF bTau cuts
        self.OppositeSign = False
        self.ApplyUpperMassCut     = True
        self.LowerMassCut     = 0.
        self.UpperMassCut     = 6000.
        self.ApplyChi2Cut     = True
        self.Chi2VtxCut       =   50.
        self.bphysCollectionKey = "MultiTrkFex"

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        validation = TrigEFMultiMuHypoValidationMonitoring()
        online = TrigEFMultiMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]



class EFMultiMuHypo_Phi (TrigEFMultiMuHypo):
    __slots__ = []
    def __init__(self, name = "EFMultiMuHypo_Phi"):
        super( EFMultiMuHypo_Phi, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF bPhi cuts
        self.OppositeSign = False
        self.ApplyUpperMassCut     = True
        self.LowerMassCut     = 940.
        self.UpperMassCut     = 1100.
        self.ApplyChi2Cut     = True
        self.Chi2VtxCut       =   10.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        validation = TrigEFMultiMuHypoValidationMonitoring()
        online = TrigEFMultiMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]

