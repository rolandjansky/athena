# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 

class TrigEFTrkMassHypoValidationMonitoring_RoI(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFTrkMassHypoValidationMonitoring_RoI"):
        super(TrigEFTrkMassHypoValidationMonitoring_RoI, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms += [ defineHistogram('CutCounter', type='TH1F', title="cut info",
                                           xbins=10, xmin=-1.5, xmax=8.5) ]
#        self.Histograms += [ defineHistogram('Mass', type='TH1F', title="EFTrkMassFex Mass; Mass[MeV]",
#                                           xbins=100, xmin=1000., xmax=5000.) ]
#        self.Histograms += [ defineHistogram('FitMass', type='TH1F', title="EFTrkMassFex FitMass; FitMass[MeV]",
#                                           xbins=100, xmin=1000., xmax=5000.) ]
#        self.Histograms += [ defineHistogram('Chi2', type='TH1F', title="EFTrkMassFex Chi2; Chi2",
#                                           xbins=100, xmin=0., xmax=10.) ]
#        self.Histograms += [ defineHistogram('Chi2Prob', type='TH1F', title="EFTrkMassFex Chi2Prob; Chi2Prob",
#                                           xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('NBphys', type='TH1F', title="EFTrkMassFex NBphys created",
                                           xbins=50, xmin=0.0, xmax=50.0) ]

class TrigEFTrkMassHypoOnlineMonitoring_RoI(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFTrkMassHypoOnlineMonitoring_RoI"):
        super(TrigEFTrkMassHypoOnlineMonitoring_RoI, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms += [ defineHistogram('CutCounter', type='TH1F', title="cut info",
                                           xbins=10, xmin=-1.5, xmax=8.5) ]
#        self.Histograms += [ defineHistogram('Mass', type='TH1F', title="EFTrkMassFex Mass; Mass[MeV]",
#                                           xbins=100, xmin=1000., xmax=5000.) ]
#        self.Histograms += [ defineHistogram('FitMass', type='TH1F', title="EFTrkMassFex FitMass; FitMass[MeV]",
#                                           xbins=100, xmin=1000., xmax=5000.) ]
#        self.Histograms += [ defineHistogram('Chi2', type='TH1F', title="EFTrkMassFex Chi2; Chi2",
#                                           xbins=100, xmin=0., xmax=10.) ]
#        self.Histograms += [ defineHistogram('Chi2Prob', type='TH1F', title="EFTrkMassFex Chi2Prob; Chi2Prob",
#                                           xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('NBphys', type='TH1F', title="EFTrkMassFex NBphys created",
                                           xbins=50, xmin=0.0, xmax=50.0) ]


class TrigEFTrkMassHypoValidationMonitoring_FS(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFTrkMassHypoValidationMonitoring_FS"):
        super(TrigEFTrkMassHypoValidationMonitoring_FS, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms += [ defineHistogram('CutCounter', type='TH1F', title="cut info",
                                           xbins=10, xmin=-1.5, xmax=8.5) ]
#        self.Histograms += [ defineHistogram('Mass', type='TH1F', title="EFTrkMassFex Mass; Mass[MeV]",
#                                           xbins=100, xmin=1000., xmax=5000.) ]
#        self.Histograms += [ defineHistogram('FitMass', type='TH1F', title="EFTrkMassFex FitMass; FitMass[MeV]",
#                                           xbins=100, xmin=1000., xmax=5000.) ]
#        self.Histograms += [ defineHistogram('Chi2', type='TH1F', title="EFTrkMassFex Chi2; Chi2",
#                                           xbins=100, xmin=0., xmax=10.) ]
#        self.Histograms += [ defineHistogram('Chi2Prob', type='TH1F', title="EFTrkMassFex Chi2Prob; Chi2Prob",
#                                           xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('NBphys', type='TH1F', title="EFTrkMassFex NBphys created",
                                           xbins=50, xmin=0.0, xmax=50.0) ]

class TrigEFTrkMassHypoOnlineMonitoring_FS(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFTrkMassHypoOnlineMonitoring_FS"):
        super(TrigEFTrkMassHypoOnlineMonitoring_FS, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms += [ defineHistogram('CutCounter', type='TH1F', title="cut info",
                                           xbins=10, xmin=-1.5, xmax=8.5) ]
#        self.Histograms += [ defineHistogram('Mass', type='TH1F', title="EFTrkMassFex Mass; Mass[MeV]",
#                                           xbins=100, xmin=1000., xmax=5000.) ]
#        self.Histograms += [ defineHistogram('FitMass', type='TH1F', title="EFTrkMassFex FitMass; FitMass[MeV]",
#                                           xbins=100, xmin=1000., xmax=5000.) ]
#        self.Histograms += [ defineHistogram('Chi2', type='TH1F', title="EFTrkMassFex Chi2; Chi2",
#                                           xbins=100, xmin=0., xmax=10.) ]
#        self.Histograms += [ defineHistogram('Chi2Prob', type='TH1F', title="EFTrkMassFex Chi2Prob; Chi2Prob",
#                                           xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('NBphys', type='TH1F', title="EFTrkMassFex NBphys created",
                                           xbins=50, xmin=0.0, xmax=50.0) ]

class TrigEFTrkMassHypoValidationMonitoring_Upsi_FS(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFTrkMassHypoValidationMonitoring_Upsi_FS"):
        super(TrigEFTrkMassHypoValidationMonitoring_Upsi_FS, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms += [ defineHistogram('CutCounter', type='TH1F', title="cut info",
                                           xbins=10, xmin=-1.5, xmax=8.5) ]
#        self.Histograms += [ defineHistogram('Mass', type='TH1F', title="EFTrkMassFex Mass; Mass[MeV]",
#                                           xbins=100, xmin=1000., xmax=5000.) ]
#        self.Histograms += [ defineHistogram('FitMass', type='TH1F', title="EFTrkMassFex FitMass; FitMass[MeV]",
#                                           xbins=100, xmin=1000., xmax=5000.) ]
#        self.Histograms += [ defineHistogram('Chi2', type='TH1F', title="EFTrkMassFex Chi2; Chi2",
#                                           xbins=100, xmin=0., xmax=10.) ]
#        self.Histograms += [ defineHistogram('Chi2Prob', type='TH1F', title="EFTrkMassFex Chi2Prob; Chi2Prob",
#                                           xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('NBphys', type='TH1F', title="EFTrkMassFex NBphys created",
                                           xbins=50, xmin=0.0, xmax=50.0) ]

class TrigEFTrkMassHypoOnlineMonitoring_Upsi_FS(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFTrkMassHypoOnlineMonitoring_Upsi_FS"):
        super(TrigEFTrkMassHypoOnlineMonitoring_Upsi_FS, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms += [ defineHistogram('CutCounter', type='TH1F', title="cut info",
                                           xbins=10, xmin=-1.5, xmax=8.5) ]
#        self.Histograms += [ defineHistogram('Mass', type='TH1F', title="EFTrkMassFex Mass; Mass[MeV]",
#                                           xbins=100, xmin=1000., xmax=5000.) ]
#        self.Histograms += [ defineHistogram('FitMass', type='TH1F', title="EFTrkMassFex FitMass; FitMass[MeV]",
#                                           xbins=100, xmin=1000., xmax=5000.) ]
#        self.Histograms += [ defineHistogram('Chi2', type='TH1F', title="EFTrkMassFex Chi2; Chi2",
#                                           xbins=100, xmin=0., xmax=10.) ]
#        self.Histograms += [ defineHistogram('Chi2Prob', type='TH1F', title="EFTrkMassFex Chi2Prob; Chi2Prob",
#                                           xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('NBphys', type='TH1F', title="EFTrkMassFex NBphys created",
                                           xbins=50, xmin=0.0, xmax=50.0) ]


