# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 

class TrigEFDiMuXFexValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="EFDiMuXFexValidation"):
        super(TrigEFDiMuXFexValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms  = [ defineHistogram('MuMuMass', type='TH1F', title="EFDiMuXFex MuMu Inv.mass [GeV]", xbins=100, xmin=0., xmax=10.) ]
        self.Histograms += [ defineHistogram('KplusMuMuMass', type='TH1F', title="EFDiMuXFex K^{+}MuMu Inv.mass [GeV]", xbins=100, xmin=0., xmax=10.) ]

class TrigEFDiMuXFexOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="EFDiMuXFexOnline"):
        super(TrigEFDiMuXFexOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms  = [ defineHistogram('MuMuMass', type='TH1F', title="EFDiMuXFex MuMu Inv.mass [GeV]", xbins=100, xmin=0., xmax=10.) ]
        self.Histograms += [ defineHistogram('KplusMuMuMass', type='TH1F', title="EFDiMuXFex K^{+}MuMu Inv.mass [GeV]", xbins=100, xmin=0., xmax=10.) ]

