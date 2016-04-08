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
        super( TrigEFMultiMuHypo, self ).__init__( name )

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

class EFMultiMuHypo_Jpsi (TrigEFMultiMuHypo):
    __slots__ = []
    def __init__(self, name = "EFMultiMuHypo_Jpsi"):
        super( TrigEFMultiMuHypo, self ).__init__( name )

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
        super( TrigEFMultiMuHypo, self ).__init__( name )

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

class EFMultiMuHypo_Tau (TrigEFMultiMuHypo):
    __slots__ = []
    def __init__(self, name = "EFMultiMuHypo_Tau"):
        super( TrigEFMultiMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF bTau cuts
        self.OppositeSign = False
        self.ApplyUpperMassCut     = True
        self.LowerMassCut     = 0.
        self.UpperMassCut     = 2700.
        self.ApplyChi2Cut     = True
        self.Chi2VtxCut       =   50.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        validation = TrigEFMultiMuHypoValidationMonitoring()
        online = TrigEFMultiMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]

class EFMultiMuHypo_Tau2 (TrigEFMultiMuHypo):
    __slots__ = []
    def __init__(self, name = "EFMultiMuHypo_Tau2"):
        super( TrigEFMultiMuHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF bTau cuts
        self.OppositeSign = False
        self.ApplyUpperMassCut     = True
        self.LowerMassCut     = 0.
        self.UpperMassCut     = 2700.
        self.ApplyChi2Cut     = True
        self.Chi2VtxCut       =   10.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        validation = TrigEFMultiMuHypoValidationMonitoring()
        online = TrigEFMultiMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]

