# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

################ Create Instance for Online Monitoring (basic histograms)
from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TrigmuIsoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="muIso_Online"):
        super(TrigmuIsoOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")

        self.Histograms  = [ defineHistogram('ErrorFlagMI', type='TH1F', title="MuIso Error Flags during event processing; Error Code", xbins=15, xmin=-1.5, xmax=13.5) ]
        self.Histograms += [ defineHistogram('IDiso', type='TH1F',   title="MuIso SumPt/muPt ; ; nevents", xbins=200, xmin=-0.01, xmax=1.) ]
        self.Histograms += [ defineHistogram('TEMuPt', type='TH1F',  title="MuIso Input TE Muon Pt;Pt [GeV/c]; nevents", xbins=210, xmin=-105., xmax=105.) ]
        self.Histograms += [ defineHistogram('PtFL', type='TH1F', title="MuIso p_{T} of events failing isolation; p_{T} [GeV/c]", xbins=210, xmin=-105., xmax=105.) ]

##########   Create instance for validation (additional specific histograms)
class TrigmuIsoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="muIso_Validation"):
        super(TrigmuIsoValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")
        
        self.Histograms  = [ defineHistogram('IsIsolated', type='TH1F', title="MuIso Global Isolation Result;Isolation; nevents", xbins=2, xmin=-0.5, xmax=1.5) ]
        self.Histograms += [ defineHistogram('ErrorFlagMI', type='TH1F', title="MuIso Error Flags during event processing; Error Code", xbins=15, xmin=-1.5, xmax=13.5) ]
        self.Histograms += [ defineHistogram('IDiso', type='TH1F',   title="MuIso SumPt/muPt ; ; nevents", xbins=200, xmin=-0.01, xmax=1.) ]
        self.Histograms += [ defineHistogram('TEMuPt', type='TH1F', title="MuIso Input TE Muon Pt;Pt [GeV/c]; nevents", xbins=210, xmin=-105., xmax=105.) ]
        self.Histograms += [ defineHistogram('TEMuEta', type='TH1F', title="MuIso Input TE Muon Eta;Eta; nevents", xbins=108, xmin=-2.7, xmax=2.7) ]
        self.Histograms += [ defineHistogram('TEMuPhi', type='TH1F', title="MuIso Input TE Muon Phi;Phi [rad]; nevents", xbins=96, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('PtFL', type='TH1F', title="MuIso p_{T} of events failing isolation; p_{T} [GeV/c]", xbins=210, xmin=-105., xmax=105.) ]

class TrigmuIsoCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="muIso_Cosmic"):
        super(TrigmuIsoCosmicMonitoring, self).__init__(name)

        self.defineTarget("Cosmic")
        
        self.Histograms  = [ defineHistogram('ErrorFlagMI', type='TH1F', title="MuIso Error Flags during event processing; Error Code", xbins=15, xmin=-1.5, xmax=13.5) ]
        self.Histograms += [ defineHistogram('IDiso', type='TH1F',   title="MuIso SumPt/muPt ; ; nevents", xbins=200, xmin=-0.01, xmax=1.) ]
        self.Histograms += [ defineHistogram('TEMuPt', type='TH1F', title="MuIso Input TE Muon Pt;Pt [GeV/c]; nevents", xbins=210, xmin=-105., xmax=105.) ]
        self.Histograms += [ defineHistogram('PtFL', type='TH1F', title="MuIso p_{T} of events failing isolation; p_{T} [GeV/c]", xbins=210, xmin=-105., xmax=105.) ]
