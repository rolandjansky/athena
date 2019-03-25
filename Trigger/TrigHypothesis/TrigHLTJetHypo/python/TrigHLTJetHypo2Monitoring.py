# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TrigHLTJetHypo2ValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="HLTJetHypo_Validation"):
        super(TrigHLTJetHypo2ValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")

        cutNames = ['No Jet obj', 'Rejected', 'Accepted']
        labelsDescription = ''
        for a in cutNames:
            labelsDescription += a+':'

        self.Histograms = [ defineHistogram('CutCounter', type='TH1F', title="HLTJetHypo cut counter", xbins=3, xmin=-1.5, xmax=1.5, labels=labelsDescription) ]
        self.Histograms += [ defineHistogram('NJet', type='TH1F', title="HLTJetHypo NJet; Number of jets; nevents", xbins=20, xmin=0, xmax=20) ]
        self.Histograms += [ defineHistogram('Et', type='TH1F', title="HLTJetHypo Et; Et [MeV]; nevents", xbins=200, xmin=0., xmax=1000000.) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="HLTJetHypo Eta;Eta; nevents", xbins=100, xmin=-5., xmax=5.) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="HLTJetHypo Phi;Phi; nevents", xbins=64, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta, Phi', type='TH2F', title="HLTJetHypo Phi vs. Eta;#eta;#phi", xbins=100, xmin=-5., xmax=5., ybins=64, ymin=-3.1416, ymax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta, Et', type='TH2F', title="HLTJetHypo Et vs. Eta;#eta;E_{T} [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100, ymin=0, ymax=500000.) ]
        self.Histograms += [ defineHistogram('Phi, Et', type='TH2F', title="HLTJetHypo Et vs. Phi;#phi;E_{T} [MeV]", xbins=64, xmin=-3.1416, xmax=3.1416, ybins=100, ymin=0, ymax=500000.) ]




#more histograms to be added later


class TrigHLTJetHypo2OnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="HLTJetHypo_Online"):
        """ defines histograms for online """
        super(TrigHLTJetHypo2OnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")

        cutNames = ['No Jet obj', 'Rejected', 'Accepted']
        labelsDescription = ''
        for a in cutNames:
            labelsDescription += a+':'

        self.Histograms = [ defineHistogram('CutCounter', type='TH1F', title="HLTJetHypo cut counter", xbins=3, xmin=-1.5, xmax=1.5, labels=labelsDescription) ]
        self.Histograms += [ defineHistogram('NJet', type='TH1F', title="HLTJetHypo NJet; Number of jets; nevents", xbins=20, xmin=0, xmax=20) ]
        self.Histograms += [ defineHistogram('Et', type='TH1F', title="HLTJetHypo Et; Et [MeV]; nevents", xbins=200, xmin=0., xmax=1000000.) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="HLTJetHypo Eta;Eta; nevents", xbins=100, xmin=-5., xmax=5.) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="HLTJetHypo Phi;Phi; nevents", xbins=64, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta, Phi', type='TH2F', title="HLTJetHypo Phi vs. Eta;#eta;#phi", xbins=100, xmin=-5., xmax=5., ybins=64, ymin=-3.1416, ymax=3.1416) ]
        #self.Histograms += [ defineHistogram('Eta, Et', type='TH2F', title="HLTJetHypo Et vs. Eta;#eta;E_{T} [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100, ymin=0, ymax=500000.) ]
        #self.Histograms += [ defineHistogram('Phi, Et', type='TH2F', title="HLTJetHypo Et vs. Phi;#phi;E_{T} [MeV]", xbins=64, xmin=-3.1416, xmax=3.1416, ybins=100, ymin=0, ymax=500000.) ]


class TrigHLTJetHypo2CosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="HLTJetHypo_Cosmic"):
        """ defines histograms for cosmic """
        super(TrigHLTJetHypo2CosmicMonitoring, self).__init__(name)

        self.defineTarget("Cosmic")

        cutNames = ['No Jet obj', 'Rejected', 'Accepted']
        labelsDescription = ''
        for a in cutNames:
            labelsDescription += a+':'

        self.Histograms = [ defineHistogram('CutCounter', type='TH1F', title="HLTJetHypo cut counter", xbins=3, xmin=-1.5, xmax=1.5, labels=labelsDescription) ]
        self.Histograms += [ defineHistogram('NJet', type='TH1F', title="HLTJetHypo NJet; Number of jets; nevents", xbins=20, xmin=0, xmax=20) ]
        self.Histograms += [ defineHistogram('Et', type='TH1F', title="HLTJetHypo Et; Et [MeV]; nevents", xbins=200, xmin=0., xmax=1000000.) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="HLTJetHypo Eta;Eta; nevents", xbins=100, xmin=-5., xmax=5.) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="HLTJetHypo Phi;Phi; nevents", xbins=64, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta, Phi', type='TH2F', title="HLTJetHypo Phi vs. Eta;#eta;#phi", xbins=100, xmin=-5., xmax=5., ybins=64, ymin=-3.1416, ymax=3.1416) ]
        #self.Histograms += [ defineHistogram('Eta, Et', type='TH2F', title="HLTJetHypo Et vs. Eta;#eta;E_{T} [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100, ymin=0, ymax=300000.) ]
        #self.Histograms += [ defineHistogram('Phi, Et', type='TH2F', title="HLTJetHypo Et vs. Phi;#phi;E_{T} [MeV]", xbins=64, xmin=-3.1416, xmax=3.1416, ybins=100, ymin=0, ymax=300000.) ]
