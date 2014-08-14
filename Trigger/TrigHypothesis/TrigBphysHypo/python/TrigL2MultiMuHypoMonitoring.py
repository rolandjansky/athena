# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 

class TrigL2MultiMuHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2MultiMuHypoValidationMonitoring"):
        super(TrigL2MultiMuHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms = [ defineHistogram('CutCounter', type='TH1F', title= "cuts counter", xbins=8, xmin=-1.5, xmax=6.5) ]
        self.Histograms += [ defineHistogram('MuMumass', type='TH1F', title="L2MultiMuHypo inv.mass(mumu) [GeV]",
                                           xbins=100, xmin=0., xmax=13.) ]

class TrigL2MultiMuHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2MultiMuHypoOnlineMonitoring"):
        super(TrigL2MultiMuHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms = [ defineHistogram('CutCounter', type='TH1F', title= "cuts counter", xbins=8, xmin=-1.5, xmax=6.5) ]
        self.Histograms += [ defineHistogram('MuMumass', type='TH1F', title="L2MultiMuHypo inv.mass(mumu) [GeV]",
                                           xbins=100, xmin=0., xmax=13.) ]
