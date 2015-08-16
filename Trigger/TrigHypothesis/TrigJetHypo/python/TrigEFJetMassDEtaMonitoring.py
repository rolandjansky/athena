# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 


class TrigEFJetMassDEtaValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFJetMassDEtaValidationMonitoring"):
        super(TrigEFJetMassDEtaValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")

        self.Histograms += [ defineHistogram('NJet', type='TH1F', title="Number Jets", xbins=20, xmin=-0.5, xmax=19.5) ]
        self.Histograms += [ defineHistogram('Complexity', type='TH1F', title="Number of Iterations", xbins=20, xmin=-0.5, xmax=100) ]
        self.Histograms += [ defineHistogram('dEta', type='TH1F', title="Maximal #Delta #Eta Two Jets", xbins=100, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('Mjj',type='TH1F', title="Mjj Two Jets", xbins=100, xmin=0, xmax=2000000) ]
        self.Histograms += [ defineHistogram('dEta, Mjj',type='TH2F', title="Mjj Two Jets vs. dEta", xbins=20, xmin=0, xmax=10, ybins=20, ymin=0, ymax=1000000) ]
        self.Histograms += [ defineHistogram('dEta_cuted', type='TH1F', title="dEta Two Jets after cut", xbins=100, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('Mjj_cuted',type='TH1F', title="Mjj Two Jets after cut", xbins=100, xmin=0, xmax=2000000) ]
        self.Histograms += [ defineHistogram('dEta, Mjj_cuted',type='TH2F', title="Mjj Two Jets vs. dEta after cut", xbins=20, xmin=0, xmax=10, ybins=20, ymin=0, ymax=1000000) ]
        self.Histograms += [ defineHistogram('pt', type='TH1F', title="pt Two Jet", xbins=60, xmin=0, xmax=300000) ]
        self.Histograms += [ defineHistogram('pt_cut', type='TH1F', title="pt Two Jet after cut", xbins=60, xmin=0, xmax=300000) ]
        self.Histograms += [ defineHistogram('eta', type='TH1F', title="eta Two Jet", xbins=98, xmin=-4.9, xmax=4.9) ]
        self.Histograms += [ defineHistogram('eta_cut', type='TH1F', title="eta Two Jet after cut", xbins=98, xmin=-4.9, xmax=4.9) ]
        

class TrigEFJetMassDEtaOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFJetMassDEtaOnlineMonitoring"):
        """ defines histograms for online """
        super(TrigEFJetMassDEtaOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")

        self.Histograms += [ defineHistogram('NJet', type='TH1F', title="Number Jets", xbins=20, xmin=-0.5, xmax=19.5) ]
        self.Histograms += [ defineHistogram('Complexity', type='TH1F', title="Number of Iterations", xbins=20, xmin=-0.5, xmax=100) ]
        self.Histograms += [ defineHistogram('dEta', type='TH1F', title="Maximal #Delta #Eta Two Jets", xbins=100, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('Mjj',type='TH1F', title="Mjj Two Jets", xbins=100, xmin=0, xmax=2000000) ]
        self.Histograms += [ defineHistogram('dEta, Mjj',type='TH2F', title="Mjj Two Jets vs. dEta", xbins=20, xmin=0, xmax=10, ybins=20, ymin=0, ymax=1000000) ]
        self.Histograms += [ defineHistogram('dEta_cuted', type='TH1F', title="dEta Two Jets after cut", xbins=100, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('Mjj_cuted',type='TH1F', title="Mjj Two Jets after cut", xbins=100, xmin=0, xmax=2000000) ]
        self.Histograms += [ defineHistogram('dEta, Mjj_cuted',type='TH2F', title="Mjj Two Jets vs. dEta after cut", xbins=20, xmin=0, xmax=10, ybins=20, ymin=0, ymax=1000000) ]
        self.Histograms += [ defineHistogram('pt', type='TH1F', title="pt Two Jet", xbins=60, xmin=0, xmax=300000) ]
        self.Histograms += [ defineHistogram('pt_cut', type='TH1F', title="pt Two Jet after cut", xbins=60, xmin=0, xmax=300000) ]
        self.Histograms += [ defineHistogram('eta', type='TH1F', title="eta Two Jet", xbins=98, xmin=-4.9, xmax=4.9) ]
        self.Histograms += [ defineHistogram('eta_cut', type='TH1F', title="eta Two Jet after cut", xbins=98, xmin=-4.9, xmax=4.9) ]



        
