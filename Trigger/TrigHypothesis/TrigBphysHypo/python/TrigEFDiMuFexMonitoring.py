# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 

class TrigEFDiMuFexValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFDiMuFexValidationMonitoring"):
        super(TrigEFDiMuFexValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms  = [ defineHistogram('pT', type='TH1F', title="Jpsi's muons pT [GeV]", xbins=100, xmin=0, xmax=100.0) ]
        self.Histograms += [ defineHistogram('vtx_pT', type='TH1F', title="Jpsi's muons vtx_pT [GeV] TEST", xbins=100, xmin=0, xmax=100.0) ]
        self.Histograms += [ defineHistogram('vtx_jpsiMass', type='TH1F', title="Jpsi vtx_mass [GeV]", xbins=60, xmin=2.0, xmax=5.0) ]
        self.Histograms += [ defineHistogram('eta, phi', type = 'TH2F', title = 'Muons eta-phi map ; #eta ; #phi ; # Events',
                                           xbins = 60, xmin = -3.0, xmax = 3.0,
                                           ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
#        self.Histograms += [ defineHistogram('E', type='TH1F', title="Jpsi E", xbins=100, xmin=2000, xmax=150000) ]
#        self.Histograms += [ defineHistogram('ECut', type='TH1F', title="Jpsi E", xbins=100, xmin=2000, xmax=150000) ]

class TrigEFDiMuFexOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFDiMuFexOnlineMonitoring"):
        super(TrigEFDiMuFexOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms  = [ defineHistogram('pT', type='TH1F', title="Jpsi's muons pT [GeV]", xbins=100, xmin=0, xmax=100.0) ]
        self.Histograms += [ defineHistogram('vtx_pT', type='TH1F', title="Jpsi's muons vtx_pT [GeV] TEST", xbins=100, xmin=0, xmax=100.0) ]
        self.Histograms += [ defineHistogram('vtx_jpsiMass', type='TH1F', title="Jpsi vtx_mass [GeV]", xbins=60, xmin=2.0, xmax=5.0) ]
        self.Histograms += [ defineHistogram('eta, phi', type = 'TH2F', title = 'Muons eta-phi map ; #eta ; #phi ; # Events',
                                           xbins = 60, xmin = -3.0, xmax = 3.0,
                                           ybins = 64, ymin = -3.2, ymax = 3.2 ) ]

