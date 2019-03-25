# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 
        

class TrigEFRazorAllTEOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFRazorAllTEOnlineMonitoring"):
        """ defines histograms for online """
        super(TrigEFRazorAllTEOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")


        self.Histograms += [ defineHistogram('Razor',type='TH1F', title="EFRazor Razor before cuts", xbins=200, xmin=0, xmax=10000) ]
        self.Histograms += [ defineHistogram('Razor_passedRazor',type='TH1F', title="EFRazor Razor passed Razor cut", xbins=200, xmin=0, xmax=10000) ]

        self.Histograms += [ defineHistogram('passedJetCuts_et_0',type='TH1F', title="EFRazor hemisphere 0 Et after jet cuts", xbins=198, xmin=10, xmax=1000) ]
        self.Histograms += [ defineHistogram('passedJetCuts_eta_0',type='TH1F', title="EFRazor hemisphere 0 eta after jet cuts", xbins=80, xmin=-4, xmax=4) ]
        self.Histograms += [ defineHistogram('passedJetCuts_phi_0',type='TH1F', title="EFRazor hemisphere 0 phi after jet cuts", xbins=64, xmin=-3.2, xmax=3.2) ]

        self.Histograms += [ defineHistogram('passedJetCuts_et_1',type='TH1F', title="EFRazor hemisphere 1 Et after jet cuts", xbins=198, xmin=10, xmax=1000) ]
        self.Histograms += [ defineHistogram('passedJetCuts_eta_1',type='TH1F', title="EFRazor hemisphere 1 eta after jet cuts", xbins=80, xmin=-4, xmax=4) ]
        self.Histograms += [ defineHistogram('passedJetCuts_phi_1',type='TH1F', title="EFRazor hemisphere 1 phi after jet cuts", xbins=64, xmin=-3.2, xmax=3.2) ]

        self.Histograms += [ defineHistogram('nJets',type='TH1F', title="EFRazor no.of hemispheres considered", xbins=50, xmin=0, xmax=50) ]

        self.Histograms += [ defineHistogram('cutCounter',type='TH1F', title="EFRazor Cut Counter", xbins=10, xmin=0, xmax=10) ]
        

        self.Histograms += [ defineHistogram('gaminvRp1', type='TH1F', title="EFRazor Razor variable - 1/gamRp1", xbins=200, xmin=0, xmax=10 )]
        self.Histograms += [ defineHistogram('shatR', type='TH1F', title="EFRazor Razor variable - s_R hat", xbins=200, xmin=0, xmax=10000 )]
        self.Histograms += [ defineHistogram('mdeltaR', type='TH1F', title="EFRazor Razor variable - m^Delta_R", xbins=200, xmin=0, xmax=10000 )]
        self.Histograms += [ defineHistogram('cosptR', type='TH1F', title="EFRazor Razor variable - cos Theta_pt^R", xbins=200, xmin=0, xmax=10000 )]
        self.Histograms += [ defineHistogram('Minv2', type='TH1F', title="EFRazor Razor variable - Minv2", xbins=200, xmin=0, xmax=10000 )]
        self.Histograms += [ defineHistogram('Einv', type='TH1F', title="EFRazor Razor variable - Einv", xbins=200, xmin=0, xmax=10000 )]


class TrigEFRazorAllTEValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFRazorAllTEValidationMonitoring"):
        """ defines histograms for validation """
        super(TrigEFRazorAllTEValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")

        self.Histograms += [ defineHistogram('Razor',type='TH1F', title="EFRazor Razor before cuts", xbins=200, xmin=0, xmax=10000) ]
        self.Histograms += [ defineHistogram('Razor_passedRazor',type='TH1F', title="EFRazor Razor passed Razor cut", xbins=200, xmin=0, xmax=10000) ]

        self.Histograms += [ defineHistogram('passedJetCuts_et_0',type='TH1F', title="EFRazor hemisphere 0 Et after jet cuts", xbins=198, xmin=10, xmax=1000) ]
        self.Histograms += [ defineHistogram('passedJetCuts_eta_0',type='TH1F', title="EFRazor hemisphere 0 eta after jet cuts", xbins=80, xmin=-4, xmax=4) ]
        self.Histograms += [ defineHistogram('passedJetCuts_phi_0',type='TH1F', title="EFRazor hemisphere 0 phi after jet cuts", xbins=64, xmin=-3.2, xmax=3.2) ]

        self.Histograms += [ defineHistogram('passedJetCuts_et_1',type='TH1F', title="EFRazor hemisphere 1 Et after jet cuts", xbins=198, xmin=10, xmax=1000) ]
        self.Histograms += [ defineHistogram('passedJetCuts_eta_1',type='TH1F', title="EFRazor hemisphere 1 eta after jet cuts", xbins=80, xmin=-4, xmax=4) ]
        self.Histograms += [ defineHistogram('passedJetCuts_phi_1',type='TH1F', title="EFRazor hemisphere 1 phi after jet cuts", xbins=64, xmin=-3.2, xmax=3.2) ]

        self.Histograms += [ defineHistogram('nJets',type='TH1F', title="EFRazor no.of hemispheres considered", xbins=50, xmin=0, xmax=50) ]

        self.Histograms += [ defineHistogram('cutCounter',type='TH1F', title="EFRazor Cut Counter", xbins=10, xmin=0, xmax=10) ]

        
        self.Histograms += [ defineHistogram('gaminvRp1', type='TH1F', title="EFRazor Razor variable - 1/gamRp1", xbins=200, xmin=0, xmax=10 )]
        self.Histograms += [ defineHistogram('shatR', type='TH1F', title="EFRazor Razor variable - s_R hat", xbins=200, xmin=0, xmax=10000 )]
        self.Histograms += [ defineHistogram('mdeltaR', type='TH1F', title="EFRazor Razor variable - m^Delta_R", xbins=200, xmin=0, xmax=10000 )]
        self.Histograms += [ defineHistogram('cosptR', type='TH1F', title="EFRazor Razor variable - cos Theta_pt^R", xbins=200, xmin=0, xmax=10000 )]
        self.Histograms += [ defineHistogram('Minv2', type='TH1F', title="EFRazor Razor variable - Minv2", xbins=200, xmin=0, xmax=10000 )]
        self.Histograms += [ defineHistogram('Einv', type='TH1F', title="EFRazor Razor variable - Einv", xbins=200, xmin=0, xmax=10000 )]

        
