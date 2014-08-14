# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 

class TrigL2TrkMassHypoValidationMonitoring_RoI(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2TrkMassHypoValidationMonitoring_RoI"):
        super(TrigL2TrkMassHypoValidationMonitoring_RoI, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms += [ defineHistogram('CutCounter', type='TH1F', title="cut info",
                                           xbins=10, xmin=-1.5, xmax=8.5) ]
        self.Histograms += [ defineHistogram('Mass', type='TH1F', title="L2TrkMassHypo Mass_RoI; Mass[GeV]",
                                           xbins=100, xmin=1., xmax=10.) ]
#        self.Histograms += [ defineHistogram('FitMass', type='TH1F', title="L2TrkMassFex FitMass; FitMass[MeV]",
#                                           xbins=100, xmin=1000., xmax=5000.) ]
#        self.Histograms += [ defineHistogram('Chi2', type='TH1F', title="L2TrkMassFex Chi2; Chi2",
#                                           xbins=100, xmin=0., xmax=10.) ]
#        self.Histograms += [ defineHistogram('Chi2Prob', type='TH1F', title="L2TrkMassFex Chi2Prob; Chi2Prob",
#                                           xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('NBphys', type='TH1F', title="L2TrkMassHypo NBphys created",
                                           xbins=50, xmin=0.0, xmax=50.0) ]

class TrigL2TrkMassHypoOnlineMonitoring_RoI(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2TrkMassHypoOnlineMonitoring_RoI"):
        super(TrigL2TrkMassHypoOnlineMonitoring_RoI, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms += [ defineHistogram('CutCounter', type='TH1F', title="cut info",
                                           xbins=10, xmin=-1.5, xmax=8.5) ]
        self.Histograms += [ defineHistogram('Mass', type='TH1F', title="L2TrkMassHypo Mass_RoI; Mass[GeV]",
                                           xbins=100, xmin=1., xmax=10.) ]
#        self.Histograms += [ defineHistogram('FitMass', type='TH1F', title="L2TrkMassFex FitMass; FitMass[MeV]",
#                                           xbins=100, xmin=1000., xmax=5000.) ]
#        self.Histograms += [ defineHistogram('Chi2', type='TH1F', title="L2TrkMassFex Chi2; Chi2",
#                                           xbins=100, xmin=0., xmax=10.) ]
#        self.Histograms += [ defineHistogram('Chi2Prob', type='TH1F', title="L2TrkMassFex Chi2Prob; Chi2Prob",
#                                           xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('NBphys', type='TH1F', title="L2TrkMassHypo NBphys created",
                                           xbins=50, xmin=0.0, xmax=50.0) ]


class TrigL2TrkMassHypoValidationMonitoring_FS(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2TrkMassHypoValidationMonitoring_FS"):
        super(TrigL2TrkMassHypoValidationMonitoring_FS, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms += [ defineHistogram('CutCounter', type='TH1F', title="cut info",
                                           xbins=10, xmin=-1.5, xmax=8.5) ]
        self.Histograms += [ defineHistogram('Mass', type='TH1F', title="L2TrkMassHypo Mass_FS; Mass[GeV]",
                                           xbins=100, xmin=1., xmax=10.) ]
#        self.Histograms += [ defineHistogram('FitMass', type='TH1F', title="L2TrkMassFex FitMass; FitMass[MeV]",
#                                           xbins=100, xmin=1000., xmax=5000.) ]
#        self.Histograms += [ defineHistogram('Chi2', type='TH1F', title="L2TrkMassFex Chi2; Chi2",
#                                           xbins=100, xmin=0., xmax=10.) ]
#        self.Histograms += [ defineHistogram('Chi2Prob', type='TH1F', title="L2TrkMassFex Chi2Prob; Chi2Prob",
#                                           xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('NBphys', type='TH1F', title="L2TrkMassHypo NBphys created",
                                           xbins=50, xmin=0.0, xmax=50.0) ]

class TrigL2TrkMassHypoOnlineMonitoring_FS(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2TrkMassHypoOnlineMonitoring_FS"):
        super(TrigL2TrkMassHypoOnlineMonitoring_FS, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms += [ defineHistogram('CutCounter', type='TH1F', title="cut info",
                                           xbins=10, xmin=-1.5, xmax=8.5) ]
        self.Histograms += [ defineHistogram('Mass', type='TH1F', title="L2TrkMassHypo Mass_FS; Mass[GeV]",
                                           xbins=100, xmin=1., xmax=10.) ]
#        self.Histograms += [ defineHistogram('FitMass', type='TH1F', title="L2TrkMassFex FitMass; FitMass[MeV]",
#                                           xbins=100, xmin=1000., xmax=5000.) ]
#        self.Histograms += [ defineHistogram('Chi2', type='TH1F', title="L2TrkMassFex Chi2; Chi2",
#                                           xbins=100, xmin=0., xmax=10.) ]
#        self.Histograms += [ defineHistogram('Chi2Prob', type='TH1F', title="L2TrkMassFex Chi2Prob; Chi2Prob",
#                                           xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('NBphys', type='TH1F', title="L2TrkMassHypo NBphys created",
                                           xbins=50, xmin=0.0, xmax=50.0) ]

class TrigL2TrkMassHypoValidationMonitoring_Upsi_FS(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2TrkMassHypoValidationMonitoring_Upsi_FS"):
        super(TrigL2TrkMassHypoValidationMonitoring_Upsi_FS, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms += [ defineHistogram('CutCounter', type='TH1F', title="cut info",
                                           xbins=10, xmin=-1.5, xmax=8.5) ]
        self.Histograms += [ defineHistogram('Mass', type='TH1F', title="L2TrkMassHypo Mass_FS; Mass[GeV]",
                                           xbins=140, xmin=1., xmax=15.) ]
#        self.Histograms += [ defineHistogram('FitMass', type='TH1F', title="L2TrkMassFex FitMass; FitMass[MeV]",
#                                           xbins=100, xmin=1000., xmax=5000.) ]
#        self.Histograms += [ defineHistogram('Chi2', type='TH1F', title="L2TrkMassFex Chi2; Chi2",
#                                           xbins=100, xmin=0., xmax=10.) ]
#        self.Histograms += [ defineHistogram('Chi2Prob', type='TH1F', title="L2TrkMassFex Chi2Prob; Chi2Prob",
#                                           xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('NBphys', type='TH1F', title="L2TrkMassHypo NBphys created",
                                           xbins=50, xmin=0.0, xmax=50.0) ]

class TrigL2TrkMassHypoOnlineMonitoring_Upsi_FS(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2TrkMassHypoOnlineMonitoring_Upsi_FS"):
        super(TrigL2TrkMassHypoOnlineMonitoring_Upsi_FS, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms += [ defineHistogram('CutCounter', type='TH1F', title="cut info",
                                           xbins=10, xmin=-1.5, xmax=8.5) ]
        self.Histograms += [ defineHistogram('Mass', type='TH1F', title="L2TrkMassHypo Mass_FS; Mass[GeV]",
                                           xbins=140, xmin=1., xmax=15.) ]
#        self.Histograms += [ defineHistogram('FitMass', type='TH1F', title="L2TrkMassFex FitMass; FitMass[MeV]",
#                                           xbins=100, xmin=1000., xmax=5000.) ]
#        self.Histograms += [ defineHistogram('Chi2', type='TH1F', title="L2TrkMassFex Chi2; Chi2",
#                                           xbins=100, xmin=0., xmax=10.) ]
#        self.Histograms += [ defineHistogram('Chi2Prob', type='TH1F', title="L2TrkMassFex Chi2Prob; Chi2Prob",
#                                           xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('NBphys', type='TH1F', title="L2TrkMassHypo NBphys created",
                                           xbins=50, xmin=0.0, xmax=50.0) ]

