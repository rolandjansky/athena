# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 

class TrigL2DiMuXHypoValidationMonitoring_RoI(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2DiMuXHypoValidation_RoI"):
        super(TrigL2DiMuXHypoValidationMonitoring_RoI, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms  = [ defineHistogram('BplusMass', type='TH1F', title="L2DiMuXHypo B^{+} Inv.mass [GeV]",
                                           xbins=100, xmin=0., xmax=10.) ]

class TrigL2DiMuXHypoOnlineMonitoring_RoI(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2DiMuXHypoOnline_RoI"):
        super(TrigL2DiMuXHypoOnlineMonitoring_RoI, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms  = [ defineHistogram('BplusMass', type='TH1F', title="L2DiMuXHypo B^{+} Inv.mass [GeV]",
                                           xbins=100, xmin=0., xmax=10.) ]


