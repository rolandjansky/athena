# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 

class TrigL2DiMuHypoValidationMonitoring_B(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2DiMuHypoValidationMonitoring_B"):
        super(TrigL2DiMuHypoValidationMonitoring_B, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms = [ defineHistogram('MuMumass', type='TH1F', title="Inv.mass(mumu) [GeV]",
                                           xbins=60, xmin=4., xmax=7.) ]
        self.Histograms += [ defineHistogram('NBphys', type='TH1F', title="Number of L2 Bphys",
                                           xbins=10, xmin=0., xmax=10.) ]
        self.Histograms += [ defineHistogram('MuEta, MuPhi', type = 'TH2F', title = 'Muons eta-phi map ; #eta ; #phi ; # Events',
                                           xbins = 60, xmin = -3.0, xmax = 3.0,
                                           ybins = 64, ymin = -3.2, ymax = 3.2 ) ]

class TrigL2DiMuHypoOnlineMonitoring_B(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2DiMuHypoOnlineMonitoring_B"):
        super(TrigL2DiMuHypoOnlineMonitoring_B, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms = [ defineHistogram('MuMumass', type='TH1F', title="Inv.mass(mumu) [GeV]",
                                           xbins=60, xmin=4., xmax=7.) ]
        self.Histograms += [ defineHistogram('NBphys', type='TH1F', title="Number of L2 Bphys",
                                           xbins=10, xmin=0., xmax=10.) ]
        self.Histograms += [ defineHistogram('MuEta, MuPhi', type = 'TH2F', title = 'Muons eta-phi map ; #eta ; #phi ; # Events',
                                           xbins = 60, xmin = -3.0, xmax = 3.0,
                                           ybins = 64, ymin = -3.2, ymax = 3.2 ) ]

class TrigL2DiMuHypoValidationMonitoring_Jpsi(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2DiMuHypoValidationMonitoring_Jpsi"):
        super(TrigL2DiMuHypoValidationMonitoring_Jpsi, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms = [ defineHistogram('MuMumass', type='TH1F', title="Inv.mass(mumu) [GeV]",
                                           xbins=40, xmin=2., xmax=4.) ]
        self.Histograms += [ defineHistogram('NBphys', type='TH1F', title="Number of L2 Bphys",
                                           xbins=10, xmin=0., xmax=10.) ]
        self.Histograms += [ defineHistogram('MuEta, MuPhi', type = 'TH2F', title = 'Muons eta-phi map ; #eta ; #phi ; # Events',
                                           xbins = 60, xmin = -3.0, xmax = 3.0,
                                           ybins = 64, ymin = -3.2, ymax = 3.2 ) ]

class TrigL2DiMuHypoOnlineMonitoring_Jpsi(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2DiMuHypoOnlineMonitoring_Jpsi"):
        super(TrigL2DiMuHypoOnlineMonitoring_Jpsi, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms = [ defineHistogram('MuMumass', type='TH1F', title="Inv.mass(mumu) [GeV]",
                                           xbins=40, xmin=2., xmax=4.) ]
        self.Histograms += [ defineHistogram('NBphys', type='TH1F', title="Number of L2 Bphys",
                                           xbins=10, xmin=0., xmax=10.) ]
        self.Histograms += [ defineHistogram('MuEta, MuPhi', type = 'TH2F', title = 'Muons eta-phi map ; #eta ; #phi ; # Events',
                                           xbins = 60, xmin = -3.0, xmax = 3.0,
                                           ybins = 64, ymin = -3.2, ymax = 3.2 ) ]

class TrigL2DiMuHypoValidationMonitoring_Upsi(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2DiMuHypoValidationMonitoring_Upsi"):
        super(TrigL2DiMuHypoValidationMonitoring_Upsi, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms = [ defineHistogram('MuMumass', type='TH1F', title="Inv.mass(mumu) [GeV]",
                                           xbins=40, xmin=8., xmax=10.) ]
        self.Histograms += [ defineHistogram('NBphys', type='TH1F', title="Number of L2 Bphys",
                                           xbins=10, xmin=0., xmax=10.) ]
        self.Histograms += [ defineHistogram('MuEta, MuPhi', type = 'TH2F', title = 'Muons eta-phi map ; #eta ; #phi ; # Events',
                                           xbins = 60, xmin = -3.0, xmax = 3.0,
                                           ybins = 64, ymin = -3.2, ymax = 3.2 ) ]

class TrigL2DiMuHypoOnlineMonitoring_Upsi(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2DiMuHypoOnlineMonitoring_Upsi"):
        super(TrigL2DiMuHypoOnlineMonitoring_Upsi, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms = [ defineHistogram('MuMumass', type='TH1F', title="Inv.mass(mumu) [GeV]",
                                           xbins=40, xmin=8., xmax=10.) ]
        self.Histograms += [ defineHistogram('NBphys', type='TH1F', title="Number of L2 Bphys",
                                           xbins=10, xmin=0., xmax=10.) ]
        self.Histograms += [ defineHistogram('MuEta, MuPhi', type = 'TH2F', title = 'Muons eta-phi map ; #eta ; #phi ; # Events',
                                           xbins = 60, xmin = -3.0, xmax = 3.0,
                                           ybins = 64, ymin = -3.2, ymax = 3.2 ) ]

class TrigL2DiMuHypoValidationMonitoring_DiMu(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2DiMuHypoValidationMonitoring_DiMu"):
        super(TrigL2DiMuHypoValidationMonitoring_DiMu, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms = [ defineHistogram('MuMumass', type='TH1F', title="Inv.mass(mumu) [GeV]",
                                           xbins=130, xmin=0., xmax=13.) ]
        self.Histograms += [ defineHistogram('NBphys', type='TH1F', title="Number of L2 Bphys",
                                           xbins=10, xmin=0., xmax=10.) ]
        self.Histograms += [ defineHistogram('MuEta, MuPhi', type = 'TH2F', title = 'Muons eta-phi map ; #eta ; #phi ; # Events',
                                           xbins = 60, xmin = -3.0, xmax = 3.0,
                                           ybins = 64, ymin = -3.2, ymax = 3.2 ) ]

class TrigL2DiMuHypoOnlineMonitoring_DiMu(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2DiMuHypoOnlineMonitoring_DiMu"):
        super(TrigL2DiMuHypoOnlineMonitoring_DiMu, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms = [ defineHistogram('MuMumass', type='TH1F', title="Inv.mass(mumu) [GeV]",
                                           xbins=130, xmin=0., xmax=13.) ]
        self.Histograms += [ defineHistogram('NBphys', type='TH1F', title="Number of L2 Bphys",
                                           xbins=10, xmin=0., xmax=10.) ]
        self.Histograms += [ defineHistogram('MuEta, MuPhi', type = 'TH2F', title = 'Muons eta-phi map ; #eta ; #phi ; # Events',
                                           xbins = 60, xmin = -3.0, xmax = 3.0,
                                           ybins = 64, ymin = -3.2, ymax = 3.2 ) ]

