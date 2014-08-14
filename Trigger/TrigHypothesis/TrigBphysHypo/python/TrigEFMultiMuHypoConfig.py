# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBphysHypo.TrigBphysHypoConf import TrigEFMultiMuHypo,TrigEFMultiMuHypo_xAOD

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 

class TrigEFMultiMuHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFMultiMuHypoValidationMonitoring"):
        super(TrigEFMultiMuHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
#        self.defineTarget("Never")
        self.Histograms += [ defineHistogram('CutCounter', type='TH1F', title= "EF cuts counter", xbins=8, xmin=-1.5, xmax=6.5) ]
        self.Histograms += [ defineHistogram('MuMumass', type='TH1F', title="Inv.mass(mumu) [GeV]",
                                           xbins=100, xmin=0., xmax=13.0) ]
                                             
class TrigEFMultiMuHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFMultiMuHypoOnlineMonitoring"):
        super(TrigEFMultiMuHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms += [ defineHistogram('CutCounter', type='TH1F', title= "EFMultiMu cuts counter", xbins=8, xmin=-1.5, xmax=6.5) ]
        self.Histograms += [ defineHistogram('MuMumass', type='TH1F', title="EFMultiMu Inv.mass(mumu) [GeV]",
                                           xbins=130, xmin=0., xmax=13.0) ]

class EFMultiMuHypo_DiMu (TrigEFMultiMuHypo_xAOD):
    __slots__ = []
    def __init__(self, name = "EFMultiMuHypo_DiMu"):
        super( TrigEFMultiMuHypo_xAOD, self ).__init__( name )

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

class EFMultiMuHypo_Jpsi (TrigEFMultiMuHypo_xAOD):
    __slots__ = []
    def __init__(self, name = "EFMultiMuHypo_Jpsi"):
        super( TrigEFMultiMuHypo_xAOD, self ).__init__( name )

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

class EFMultiMuHypo_Upsi (TrigEFMultiMuHypo_xAOD):
    __slots__ = []
    def __init__(self, name = "EFMultiMuHypo_Upsi"):
        super( TrigEFMultiMuHypo_xAOD, self ).__init__( name )

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

class EFMultiMuHypo_Tau (TrigEFMultiMuHypo_xAOD):
    __slots__ = []
    def __init__(self, name = "EFMultiMuHypo_Tau"):
        super( TrigEFMultiMuHypo_xAOD, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # EF Bmumu cuts
        self.LowerMassCut      = 1000.
        self.UpperMassCut     = 2500.
        self.ApplyUpperMassCut     = True
        self.Chi2VtxCut       =   20.

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        validation = TrigEFMultiMuHypoValidationMonitoring()
        online = TrigEFMultiMuHypoOnlineMonitoring()
        self.AthenaMonTools = [ validation, online, time ]

