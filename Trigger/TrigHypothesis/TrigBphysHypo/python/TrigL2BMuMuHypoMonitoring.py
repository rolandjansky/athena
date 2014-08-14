# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 

class TrigL2BMuMuHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2BMuMuHypoValidationMonitoring"):
        super(TrigL2BMuMuHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms = [ defineHistogram('CutCounter', type='TH1F', title= "cuts counter", xbins=8, xmin=-1.5, xmax=6.5) ]
        self.Histograms += [ defineHistogram('MuMumass', type='TH1F', title="L2BMuMuHypo inv.mass(mumu) [GeV]",
                                           xbins=100, xmin=0., xmax=13.) ]

class TrigL2BMuMuHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2BMuMuHypoOnlineMonitoring"):
        super(TrigL2BMuMuHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms = [ defineHistogram('CutCounter', type='TH1F', title= "cuts counter", xbins=8, xmin=-1.5, xmax=6.5) ]
        self.Histograms += [ defineHistogram('MuMumass', type='TH1F', title="L2BMuMuHypo inv.mass(mumu) [GeV]",
                                           xbins=100, xmin=0., xmax=13.) ]
