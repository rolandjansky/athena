# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 

class TrigL2DiMuXFexValidationMonitoring_RoI(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2DiMuXFexValidation_RoI"):
        super(TrigL2DiMuXFexValidationMonitoring_RoI, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms  = [ defineHistogram('MuMuMass', type='TH1F', title="L2DiMuXFex MuMu Inv.mass [GeV]",
                                           xbins=100, xmin=0., xmax=10.) ]
        self.Histograms += [ defineHistogram('KplusMuMuMass', type='TH1F', title="L2DiMuXFex K^{+}MuMu Inv.mass [GeV]",
                                           xbins=100, xmin=0., xmax=10.) ]

class TrigL2DiMuXFexOnlineMonitoring_RoI(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2DiMuXFexOnline_RoI"):
        super(TrigL2DiMuXFexOnlineMonitoring_RoI, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms  = [ defineHistogram('MuMuMass', type='TH1F', title="L2DiMuXFex MuMu Inv.mass [GeV]",
                                           xbins=100, xmin=0., xmax=10.) ]
        self.Histograms += [ defineHistogram('KplusMuMuMass', type='TH1F', title="L2DiMuXFex K^{+}MuMu Inv.mass [GeV]",
                                             xbins=100, xmin=0., xmax=10.) ]

