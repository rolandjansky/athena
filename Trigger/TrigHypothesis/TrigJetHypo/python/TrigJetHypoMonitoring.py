# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 

class TrigL2JetHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2JetHypo_Validation"):
        super(TrigL2JetHypoValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")

        cutNames = ['No Jet obj', 'Rejected', 'Accepted']
        labelsDescription = ''
        for a in cutNames:
            labelsDescription += a+':'

        self.Histograms = [ defineHistogram('CutCounter', type='TH1F', title="L2JetHypo cut counter", xbins=3, xmin=-1.5, xmax=1.5, labels=labelsDescription) ]
        self.Histograms += [ defineHistogram('E', type='TH1F', title="L2JetHypo E;E [MeV]; nevents", xbins=100, xmin=0., xmax=500000.) ]
        self.Histograms += [ defineHistogram('Et', type='TH1F', title="L2JetHypo Et;Et [MeV]; nevents", xbins=100, xmin=0., xmax=500000.) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="L2JetHypo Eta;Eta; nevents", xbins=100, xmin=-5., xmax=5.) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="L2JetHypo Phi;Phi; nevents", xbins=64, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta,Phi', type='TH2F', title="L2JetHypo Phi vs. Eta;#eta;#phi", xbins=100, xmin=-5., xmax=5., ybins=64, ymin=-3.1416, ymax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta,Et', type='TH2F', title="L2JetHypo Et vs. Eta;#eta;E_{T} [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=500000.) ]
        self.Histograms += [ defineHistogram('Phi,Et', type='TH2F', title="L2JetHypo Et vs. Phi;#phi;E_{T} [MeV]", xbins=64, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=500000.) ]
        self.Histograms += [ defineHistogram('HECf', type='TH1F', title="L2JetHypo hecf; hecf; nevents", xbins=200, xmin=-0.5, xmax=1.5) ]
        self.Histograms += [ defineHistogram('EMf', type='TH1F', title="L2JetHypo emf; emf; nevents", xbins=200, xmin=-0.5, xmax=1.5) ]
        self.Histograms += [ defineHistogram('nLeadingCells', type='TH1F', title="L2JetHypo nLeadingCells; nLeadingCells; nevents", xbins=400, xmin=0., xmax=400.) ]
        self.Histograms += [ defineHistogram('jetQuality', type='TH1F', title="L2JetHypo jetQuality; jetQuality; nevents", xbins=170, xmin=-0.2, xmax=1.5) ]
        self.Histograms += [ defineHistogram('jetTimeCells', type='TH1F', title="L2JetHypo jetTimeCells; jetTimeCells [ns]; nevents", xbins=200, xmin=-100., xmax=100.) ]   
        #more histograms to be added later


class TrigL2JetHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2JetHypo_Online"):
        """ defines histograms for online """
        super(TrigL2JetHypoOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")

        cutNames = ['No Jet obj', 'Rejected', 'Accepted']
        labelsDescription = ''
        for a in cutNames:
            labelsDescription += a+':'

        self.Histograms = [ defineHistogram('CutCounter', type='TH1F', title="L2JetHypo cut counter", xbins=3, xmin=-1.5, xmax=1.5, labels=labelsDescription) ]
        self.Histograms += [ defineHistogram('E', type='TH1F', title="L2JetHypo E;E [MeV]; nevents", xbins=100, xmin=0., xmax=500000.) ]
        self.Histograms += [ defineHistogram('Et', type='TH1F', title="L2JetHypo Et;Et [MeV]; nevents", xbins=100, xmin=0., xmax=500000.) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="L2JetHypo Eta;Eta; nevents", xbins=100, xmin=-5., xmax=5.) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="L2JetHypo Phi;Phi; nevents", xbins=64, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta,Phi', type='TH2F', title="L2JetHypo Phi vs. Eta;#eta;#phi", xbins=100, xmin=-5., xmax=5., ybins=64, ymin=-3.1416, ymax=3.1416) ]
        #self.Histograms += [ defineHistogram('Eta,Et', type='TH2F', title="L2JetHypo Et vs. Eta;#eta;E_{T} [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=500000.) ]
        #self.Histograms += [ defineHistogram('Phi,Et', type='TH2F', title="L2JetHypo Et vs. Phi;#phi;E_{T} [MeV]", xbins=64, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=500000.) ]
        self.Histograms += [ defineHistogram('HECf', type='TH1F', title="L2JetHypo hecf; hecf; nevents", xbins=200, xmin=-0.5, xmax=1.5) ]
        self.Histograms += [ defineHistogram('EMf', type='TH1F', title="L2JetHypo emf; emf; nevents", xbins=200, xmin=-0.5, xmax=1.5) ]
        self.Histograms += [ defineHistogram('nLeadingCells', type='TH1F', title="L2JetHypo nLeadingCells; nLeadingCells; nevents", xbins=400, xmin=0., xmax=400.) ]
        self.Histograms += [ defineHistogram('jetQuality', type='TH1F', title="L2JetHypo jetQuality; jetQuality; nevents", xbins=170, xmin=-0.2, xmax=1.5) ]
        self.Histograms += [ defineHistogram('jetTimeCells', type='TH1F', title="L2JetHypo jetTimeCells; jetTimeCells [ns]; nevents", xbins=200, xmin=-100., xmax=100.) ]   


class TrigL2JetHypoCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2JetHypo_Cosmic"):
        """ defines histograms for cosmic """
        super(TrigL2JetHypoCosmicMonitoring, self).__init__(name)

        self.defineTarget("Cosmic")

        cutNames = ['No Jet obj', 'Rejected', 'Accepted']
        labelsDescription = ''
        for a in cutNames:
            labelsDescription += a+':'

        self.Histograms = [ defineHistogram('CutCounter', type='TH1F', title="L2JetHypo cut counter", xbins=3, xmin=-1.5, xmax=1.5, labels=labelsDescription) ]
        self.Histograms += [ defineHistogram('E', type='TH1F', title="L2JetHypo E;E [MeV]; nevents", xbins=100, xmin=0., xmax=300000.) ]
        self.Histograms += [ defineHistogram('Et', type='TH1F', title="L2JetHypo Et;Et [MeV]; nevents", xbins=100, xmin=0., xmax=300000.) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="L2JetHypo Eta;Eta; nevents", xbins=100, xmin=-5., xmax=5.) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="L2JetHypo Phi;Phi; nevents", xbins=64, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta,Phi', type='TH2F', title="L2JetHypo Phi vs. Eta;#eta;#phi", xbins=100, xmin=-5., xmax=5., ybins=64, ymin=-3.1416, ymax=3.1416) ]
        #self.Histograms += [ defineHistogram('Eta,Et', type='TH2F', title="L2JetHypo Et vs. Eta;#eta;E_{T} [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=300000.) ]
        #self.Histograms += [ defineHistogram('Phi,Et', type='TH2F', title="L2JetHypo Et vs. Phi;#phi;E_{T} [MeV]", xbins=64, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=300000.) ]
        self.Histograms += [ defineHistogram('HECf', type='TH1F', title="L2JetHypo hecf; hecf; nevents", xbins=200, xmin=-0.5, xmax=1.5) ]
        self.Histograms += [ defineHistogram('EMf', type='TH1F', title="L2JetHypo emf; emf; nevents", xbins=200, xmin=-0.5, xmax=1.5) ]
        self.Histograms += [ defineHistogram('nLeadingCells', type='TH1F', title="L2JetHypo nLeadingCells; nLeadingCells; nevents", xbins=400, xmin=0., xmax=400.) ]
        self.Histograms += [ defineHistogram('jetQuality', type='TH1F', title="L2JetHypo jetQuality; jetQuality; nevents", xbins=170, xmin=-0.2, xmax=1.5) ]
        self.Histograms += [ defineHistogram('jetTimeCells', type='TH1F', title="L2JetHypo jetTimeCells; jetTimeCells [ns]; nevents", xbins=200, xmin=-100., xmax=100.) ]   


class TrigL2MultiJetHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2MultiJetHypo_Validation"):
        super(TrigL2MultiJetHypoValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")

        cutNames = ['No Jet obj', 'Rejected', 'Accepted']
        labelsDescription = ''
        for a in cutNames:
            labelsDescription += a+':'

        self.Histograms = [ defineHistogram('CutCounter', type='TH1F', title="L2MultiJetHypo cut counter", xbins=3, xmin=-1.5, xmax=1.5, labels=labelsDescription) ]
        self.Histograms = [ defineHistogram('nJets', type='TH1F', title="L2MultiJetHypo number of jets;N; nevents", xbins=21, xmin=-.5, xmax=20.5) ]
        self.Histograms += [ defineHistogram('E', type='TH1F', title="L2MultiJetHypo E;E [MeV]; nevents", xbins=100, xmin=0., xmax=500000.) ]
        self.Histograms += [ defineHistogram('Et', type='TH1F', title="L2MultiJetHypo Et;Et [MeV]; nevents", xbins=100, xmin=0., xmax=500000.) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="L2MultiJetHypo Eta;Eta; nevents", xbins=100, xmin=-5., xmax=5.) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="L2MultiJetHypo Phi;Phi; nevents", xbins=64, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta,Phi', type='TH2F', title="L2MultiJetHypo Phi vs. Eta;#eta;#phi", xbins=100, xmin=-5., xmax=5., ybins=64, ymin=-3.1416, ymax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta,Et', type='TH2F', title="L2MultiJetHypo Et vs. Eta;#eta;E_{T} [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=500000.) ]
        self.Histograms += [ defineHistogram('Phi,Et', type='TH2F', title="L2MultiJetHypo Et vs. Phi;#phi;E_{T} [MeV]", xbins=64, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=500000.) ]
        self.Histograms += [ defineHistogram('nLeadingTowers', type='TH1F', title="L2MultiJetHypo nLeadingTowers; nLeadingTowers; nevents", xbins=21, xmin=-0.5, xmax=20.5) ]
        


class TrigL2MultiJetHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2MultiJetHypo_Online"):
        """ defines histograms for online """
        super(TrigL2MultiJetHypoOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")

        cutNames = ['No Jet obj', 'Rejected', 'Accepted']
        labelsDescription = ''
        for a in cutNames:
            labelsDescription += a+':'

        self.Histograms = [ defineHistogram('CutCounter', type='TH1F', title="L2MultiJetHypo cut counter", xbins=3, xmin=-1.5, xmax=1.5, labels=labelsDescription) ]
        self.Histograms = [ defineHistogram('nJets', type='TH1F', title="L2MultiJetHypo number of jets;N; nevents", xbins=21, xmin=-.5, xmax=20.5) ]
        self.Histograms += [ defineHistogram('E', type='TH1F', title="L2MultiJetHypo E;E [MeV]; nevents", xbins=100, xmin=0., xmax=500000.) ]
        self.Histograms += [ defineHistogram('Et', type='TH1F', title="L2MultiJetHypo Et;Et [MeV]; nevents", xbins=100, xmin=0., xmax=500000.) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="L2MultiJetHypo Eta;Eta; nevents", xbins=100, xmin=-5., xmax=5.) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="L2MultiJetHypo Phi;Phi; nevents", xbins=64, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta,Phi', type='TH2F', title="L2MultiJetHypo Phi vs. Eta;#eta;#phi", xbins=100, xmin=-5., xmax=5., ybins=64, ymin=-3.1416, ymax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta,Et', type='TH2F', title="L2MultiJetHypo Et vs. Eta;#eta;E_{T} [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=500000.) ]
        self.Histograms += [ defineHistogram('Phi,Et', type='TH2F', title="L2MultiJetHypo Et vs. Phi;#phi;E_{T} [MeV]", xbins=64, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=500000.) ]
        self.Histograms += [ defineHistogram('nLeadingTowers', type='TH1F', title="L2MultiJetHypo nLeadingTowers; nLeadingTowers; nevents", xbins=21, xmin=-0.5, xmax=20.5) ]
        

class TrigL2MultiJetHypoCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2MultiJetHypo_Cosmic"):
        """ defines histograms for cosmic """
        super(TrigL2MultiJetHypoCosmicMonitoring, self).__init__(name)

        self.defineTarget("Cosmic")

        cutNames = ['No Jet obj', 'Rejected', 'Accepted']
        labelsDescription = ''
        for a in cutNames:
            labelsDescription += a+':'

        self.Histograms = [ defineHistogram('CutCounter', type='TH1F', title="L2MultiJetHypo cut counter", xbins=3, xmin=-1.5, xmax=1.5, labels=labelsDescription) ]
        self.Histograms = [ defineHistogram('nJets', type='TH1F', title="L2MultiJetHypo number of jets;N; nevents", xbins=21, xmin=-.5, xmax=20.5) ]
        self.Histograms += [ defineHistogram('E', type='TH1F', title="L2MultiJetHypo E;E [MeV]; nevents", xbins=100, xmin=0., xmax=300000.) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="L2MultiJetHypo Eta;Eta; nevents", xbins=100, xmin=-5., xmax=5.) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="L2MultiJetHypo Phi;Phi; nevents", xbins=64, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta,Phi', type='TH2F', title="L2MultiJetHypo Phi vs. Eta;#eta;#phi", xbins=100, xmin=-5., xmax=5., ybins=64, ymin=-3.1416, ymax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta,Et', type='TH2F', title="L2MultiJetHypo Et vs. Eta;#eta;E_{T} [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=500000.) ]
        self.Histograms += [ defineHistogram('Phi,Et', type='TH2F', title="L2MultiJetHypo Et vs. Phi;#phi;E_{T} [MeV]", xbins=64, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=500000.) ]
        self.Histograms += [ defineHistogram('nLeadingTowers', type='TH1F', title="L2MultiJetHypo nLeadingTowers; nLeadingTowers; nevents", xbins=21, xmin=-0.5, xmax=20.5) ]
 
class TrigL2MultiJetAllTEValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2MultiJetAllTE_Validation"):
        super(TrigL2MultiJetAllTEValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")

        cutNames = ['No Jet obj', 'Rejected', 'Accepted']
        labelsDescription = ''
        for a in cutNames:
            labelsDescription += a+':'

        self.Histograms = [ defineHistogram('CutCounter', type='TH1F', title="L2MultiJetAllTE cut counter", xbins=3, xmin=-1.5, xmax=1.5, labels=labelsDescription) ]
        self.Histograms = [ defineHistogram('nJets', type='TH1F', title="L2MultiJetAllTE number of jets;N; nevents", xbins=21, xmin=-.5, xmax=20.5) ]
        self.Histograms += [ defineHistogram('E', type='TH1F', title="L2MultiJetAllTE E;E [MeV]; nevents", xbins=100, xmin=0., xmax=500000.) ]
        self.Histograms += [ defineHistogram('Et', type='TH1F', title="L2MultiJetAllTE Et;Et [MeV]; nevents", xbins=100, xmin=0., xmax=500000.) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="L2MultiJetAllTE Eta;Eta; nevents", xbins=100, xmin=-5., xmax=5.) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="L2MultiJetAllTE Phi;Phi; nevents", xbins=64, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta,Phi', type='TH2F', title="L2MultiJetAllTE Phi vs. Eta;#eta;#phi", xbins=100, xmin=-5., xmax=5., ybins=64, ymin=-3.1416, ymax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta,Et', type='TH2F', title="L2MultiJetAllTE Et vs. Eta;#eta;E_{T} [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=500000.) ]
        self.Histograms += [ defineHistogram('Phi,Et', type='TH2F', title="L2MultiJetAllTE Et vs. Phi;#phi;E_{T} [MeV]", xbins=64, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=500000.) ]
        self.Histograms += [ defineHistogram('nLeadingTowers', type='TH1F', title="L2MultiJetAllTE nLeadingTowers; nLeadingTowers; nevents", xbins=21, xmin=-0.5, xmax=20.5) ]
        


class TrigL2MultiJetAllTEOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2MultiJetAllTE_Online"):
        """ defines histograms for online """
        super(TrigL2MultiJetAllTEOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")

        cutNames = ['No Jet obj', 'Rejected', 'Accepted']
        labelsDescription = ''
        for a in cutNames:
            labelsDescription += a+':'

        self.Histograms = [ defineHistogram('CutCounter', type='TH1F', title="L2MultiJetAllTE cut counter", xbins=3, xmin=-1.5, xmax=1.5, labels=labelsDescription) ]
        self.Histograms = [ defineHistogram('nJets', type='TH1F', title="L2MultiJetAllTE number of jets;N; nevents", xbins=21, xmin=-.5, xmax=20.5) ]
        self.Histograms += [ defineHistogram('E', type='TH1F', title="L2MultiJetAllTE E;E [MeV]; nevents", xbins=100, xmin=0., xmax=500000.) ]
        self.Histograms += [ defineHistogram('Et', type='TH1F', title="L2MultiJetAllTE Et;Et [MeV]; nevents", xbins=100, xmin=0., xmax=500000.) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="L2MultiJetAllTE Eta;Eta; nevents", xbins=100, xmin=-5., xmax=5.) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="L2MultiJetAllTE Phi;Phi; nevents", xbins=64, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta,Phi', type='TH2F', title="L2MultiJetAllTE Phi vs. Eta;#eta;#phi", xbins=100, xmin=-5., xmax=5., ybins=64, ymin=-3.1416, ymax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta,Et', type='TH2F', title="L2MultiJetAllTE Et vs. Eta;#eta;E_{T} [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=500000.) ]
        self.Histograms += [ defineHistogram('Phi,Et', type='TH2F', title="L2MultiJetAllTE Et vs. Phi;#phi;E_{T} [MeV]", xbins=64, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=500000.) ]
        self.Histograms += [ defineHistogram('nLeadingTowers', type='TH1F', title="L2MultiJetAllTE nLeadingTowers; nLeadingTowers; nevents", xbins=21, xmin=-0.5, xmax=20.5) ]
        

class TrigL2MultiJetAllTECosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2MultiJetAllTE_Cosmic"):
        """ defines histograms for cosmic """
        super(TrigL2MultiJetAllTECosmicMonitoring, self).__init__(name)

        self.defineTarget("Cosmic")

        cutNames = ['No Jet obj', 'Rejected', 'Accepted']
        labelsDescription = ''
        for a in cutNames:
            labelsDescription += a+':'

        self.Histograms = [ defineHistogram('CutCounter', type='TH1F', title="L2MultiJetAllTE cut counter", xbins=3, xmin=-1.5, xmax=1.5, labels=labelsDescription) ]
        self.Histograms = [ defineHistogram('nJets', type='TH1F', title="L2MultiJetAllTE number of jets;N; nevents", xbins=21, xmin=-.5, xmax=20.5) ]
        self.Histograms += [ defineHistogram('E', type='TH1F', title="L2MultiJetAllTE E;E [MeV]; nevents", xbins=100, xmin=0., xmax=300000.) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="L2MultiJetAllTE Eta;Eta; nevents", xbins=100, xmin=-5., xmax=5.) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="L2MultiJetAllTE Phi;Phi; nevents", xbins=64, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta,Phi', type='TH2F', title="L2MultiJetAllTE Phi vs. Eta;#eta;#phi", xbins=100, xmin=-5., xmax=5., ybins=64, ymin=-3.1416, ymax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta,Et', type='TH2F', title="L2MultiJetAllTE Et vs. Eta;#eta;E_{T} [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=500000.) ]
        self.Histograms += [ defineHistogram('Phi,Et', type='TH2F', title="L2MultiJetAllTE Et vs. Phi;#phi;E_{T} [MeV]", xbins=64, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=500000.) ]
        self.Histograms += [ defineHistogram('nLeadingTowers', type='TH1F', title="L2MultiJetAllTE nLeadingTowers; nLeadingTowers; nevents", xbins=21, xmin=-0.5, xmax=20.5) ]
 


class TrigL2HTFullScanHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2HTFullScanHypo_Validation"):
        super(TrigL2HTFullScanHypoValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")

        cutNames = ['No Jet obj', 'Rejected', 'Accepted']
        labelsDescription = ''
        for a in cutNames:
            labelsDescription += a+':'

        self.Histograms += [ defineHistogram('CutCounter', type='TH1F', title="TrigL2HTFullScanHypo cut counter", xbins=3, xmin=-1.5, xmax=1.5, labels=labelsDescription) ]
        self.Histograms += [ defineHistogram('n_tot_Jets', type='TH1F', title="TrigL2HTFullScanHypo total number of jets;N; nevents", xbins=21, xmin=-.5, xmax=20.5) ]
        self.Histograms += [ defineHistogram('n_acc_Jets', type='TH1F', title="TrigL2HTFullScanHypo number of accepted jets;N; nevents", xbins=21, xmin=-.5, xmax=20.5) ]
        self.Histograms += [ defineHistogram('n_rej_Jets', type='TH1F', title="TrigL2HTFullScanHypo number of rejected jets;N; nevents", xbins=21, xmin=-.5, xmax=20.5) ]
        self.Histograms += [ defineHistogram('HT', type='TH1F', title="TrigL2HTFullScanHypo HT; HT; nevents", xbins=100, xmin=0., xmax=500000.) ]
        self.Histograms += [ defineHistogram('E', type='TH1F', title="TrigL2HTFullScanHypo E;E [MeV]; nevents", xbins=100, xmin=0., xmax=500000.) ]
#        self.Histograms += [ defineHistogram('Et', type='TH1F', title="TrigL2HTFullScanHypo Et;Et [MeV]; nevents", xbins=100, xmin=0., xmax=500000.) ]
        self.Histograms += [ defineHistogram('Et_acc', type='TH1F', title="TrigL2HTFullScanHypo Et acc jets;Et [MeV]; nevents", xbins=100, xmin=0., xmax=300000.) ]
        self.Histograms += [ defineHistogram('Eta_acc', type='TH1F', title="TrigL2HTFullScanHypo Eta acc jets;Eta; nevents", xbins=100, xmin=-5., xmax=5.) ]
        self.Histograms += [ defineHistogram('Phi_acc', type='TH1F', title="TrigL2HTFullScanHypo Phi acc jets;Phi; nevents", xbins=64, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta_acc,Phi_acc', type='TH2F', title="TrigL2HTFullScanHypo Phi vs. Eta acc jets;#eta;#phi", xbins=100, xmin=-5., xmax=5., ybins=64, ymin=-3.1416, ymax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta_acc,Et_acc', type='TH2F', title="TrigL2HTFullScanHypo Et vs. Eta acc jets;#eta;E_{T} [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=300000.) ]
        self.Histograms += [ defineHistogram('Phi_acc,Et_acc', type='TH2F', title="TrigL2HTFullScanHypo Et vs. Phi acc jets;#phi;E_{T} [MeV]", xbins=64, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=300000.) ]
        self.Histograms += [ defineHistogram('Et_rej', type='TH1F', title="TrigL2HTFullScanHypo Et rej jets;Et [MeV]; nevents", xbins=100, xmin=0., xmax=300000.) ]
        self.Histograms += [ defineHistogram('Eta_rej', type='TH1F', title="TrigL2HTFullScanHypo Eta rej jets;Eta; nevents", xbins=100, xmin=-5., xmax=5.) ]
        self.Histograms += [ defineHistogram('Phi_rej', type='TH1F', title="TrigL2HTFullScanHypo Phi rej jets;Phi; nevents", xbins=64, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta_rej,Phi_rej', type='TH2F', title="TrigL2HTFullScanHypo Phi vs. Eta rej jets;#eta;#phi", xbins=100, xmin=-5., xmax=5., ybins=64, ymin=-3.1416, ymax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta_rej,Et_rej', type='TH2F', title="TrigL2HTFullScanHypo Et vs. Eta rej jets;#eta;E_{T} [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=300000.) ]
        self.Histograms += [ defineHistogram('Phi_rej,Et_rej', type='TH2F', title="TrigL2HTFullScanHypo Et vs. Phi rej jets;#phi;E_{T} [MeV]", xbins=64, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=300000.) ]
        self.Histograms += [ defineHistogram('nLeadingTowers', type='TH1F', title="TrigL2HTFullScanHypo nLeadingTowers; nLeadingTowers; nevents", xbins=21, xmin=-0.5, xmax=20.5) ]


class TrigL2HTFullScanHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2HTFullScanHypo_Online"):
        """ defines histograms for online """
        super(TrigL2HTFullScanHypoOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")

        cutNames = ['No Jet obj', 'Rejected', 'Accepted']
        labelsDescription = ''
        for a in cutNames:
            labelsDescription += a+':'

        self.Histograms += [ defineHistogram('CutCounter', type='TH1F', title="TrigL2HTFullScanHypo cut counter", xbins=3, xmin=-1.5, xmax=1.5, labels=labelsDescription) ]
        self.Histograms += [ defineHistogram('n_tot_Jets', type='TH1F', title="TrigL2HTFullScanHypo total number of jets;N; nevents", xbins=21, xmin=-.5, xmax=20.5) ]
        self.Histograms += [ defineHistogram('n_acc_Jets', type='TH1F', title="TrigL2HTFullScanHypo number of accepted jets;N; nevents", xbins=21, xmin=-.5, xmax=20.5) ]
        self.Histograms += [ defineHistogram('n_rej_Jets', type='TH1F', title="TrigL2HTFullScanHypo number of rejected jets;N; nevents", xbins=21, xmin=-.5, xmax=20.5) ]
        self.Histograms += [ defineHistogram('HT', type='TH1F', title="TrigL2HTFullScanHypo HT; HT; nevents", xbins=100, xmin=0., xmax=500000.) ]
        self.Histograms += [ defineHistogram('E', type='TH1F', title="TrigL2HTFullScanHypo E;E [MeV]; nevents", xbins=100, xmin=0., xmax=500000.) ]
#        self.Histograms += [ defineHistogram('Et', type='TH1F', title="TrigL2HTFullScanHypo Et;Et [MeV]; nevents", xbins=100, xmin=0., xmax=500000.) ]
        self.Histograms += [ defineHistogram('Et_acc', type='TH1F', title="TrigL2HTFullScanHypo Et acc jets;Et [MeV]; nevents", xbins=100, xmin=0., xmax=300000.) ]
        self.Histograms += [ defineHistogram('Eta_acc', type='TH1F', title="TrigL2HTFullScanHypo Eta acc jets;Eta; nevents", xbins=100, xmin=-5., xmax=5.) ]
        self.Histograms += [ defineHistogram('Phi_acc', type='TH1F', title="TrigL2HTFullScanHypo Phi acc jets;Phi; nevents", xbins=64, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta_acc,Phi_acc', type='TH2F', title="TrigL2HTFullScanHypo Phi vs. Eta acc jets;#eta;#phi", xbins=100, xmin=-5., xmax=5., ybins=64, ymin=-3.1416, ymax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta_acc,Et_acc', type='TH2F', title="TrigL2HTFullScanHypo Et vs. Eta acc jets;#eta;E_{T} [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=300000.) ]
        self.Histograms += [ defineHistogram('Phi_acc,Et_acc', type='TH2F', title="TrigL2HTFullScanHypo Et vs. Phi acc jets;#phi;E_{T} [MeV]", xbins=64, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=300000.) ]
        self.Histograms += [ defineHistogram('Et_rej', type='TH1F', title="TrigL2HTFullScanHypo Et rej jets;Et [MeV]; nevents", xbins=100, xmin=0., xmax=300000.) ]
        self.Histograms += [ defineHistogram('Eta_rej', type='TH1F', title="TrigL2HTFullScanHypo Eta rej jets;Eta; nevents", xbins=100, xmin=-5., xmax=5.) ]
        self.Histograms += [ defineHistogram('Phi_rej', type='TH1F', title="TrigL2HTFullScanHypo Phi rej jets;Phi; nevents", xbins=64, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta_rej,Phi_rej', type='TH2F', title="TrigL2HTFullScanHypo Phi vs. Eta rej jets;#eta;#phi", xbins=100, xmin=-5., xmax=5., ybins=64, ymin=-3.1416, ymax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta_rej,Et_rej', type='TH2F', title="TrigL2HTFullScanHypo Et vs. Eta rej jets;#eta;E_{T} [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=300000.) ]
        self.Histograms += [ defineHistogram('Phi_rej,Et_rej', type='TH2F', title="TrigL2HTFullScanHypo Et vs. Phi rej jets;#phi;E_{T} [MeV]", xbins=64, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=300000.) ]
        self.Histograms += [ defineHistogram('nLeadingTowers', type='TH1F', title="TrigL2HTFullScanHypo nLeadingTowers; nLeadingTowers; nevents", xbins=21, xmin=-0.5, xmax=20.5) ]


class TrigL2HTFullScanHypoCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2HTFullScanHypo_Cosmic"):
        """ defines histograms for cosmic """
        super(TrigL2HTFullScanHypoCosmicMonitoring, self).__init__(name)

        self.defineTarget("Cosmic")

        cutNames = ['No Jet obj', 'Rejected', 'Accepted']
        labelsDescription = ''
        for a in cutNames:
            labelsDescription += a+':'
       
        self.Histograms += [ defineHistogram('CutCounter', type='TH1F', title="TrigL2HTFullScanHypo cut counter", xbins=3, xmin=-1.5, xmax=1.5, labels=labelsDescription) ]
        self.Histograms += [ defineHistogram('n_tot_Jets', type='TH1F', title="TrigL2HTFullScanHypo total number of jets;N; nevents", xbins=21, xmin=-.5, xmax=20.5) ]
        self.Histograms += [ defineHistogram('n_acc_Jets', type='TH1F', title="TrigL2HTFullScanHypo number of accepted jets;N; nevents", xbins=21, xmin=-.5, xmax=20.5) ]
        self.Histograms += [ defineHistogram('n_rej_Jets', type='TH1F', title="TrigL2HTFullScanHypo number of rejected jets;N; nevents", xbins=21, xmin=-.5, xmax=20.5) ]
        self.Histograms += [ defineHistogram('HT', type='TH1F', title="TrigL2HTFullScanHypo HT; HT; nevents", xbins=100, xmin=0., xmax=500000.) ]
        self.Histograms += [ defineHistogram('E', type='TH1F', title="TrigL2HTFullScanHypo E;E [MeV]; nevents", xbins=100, xmin=0., xmax=500000.) ]
#        self.Histograms += [ defineHistogram('Et', type='TH1F', title="TrigL2HTFullScanHypo Et;Et [MeV]; nevents", xbins=100, xmin=0., xmax=500000.) ]
        self.Histograms += [ defineHistogram('Et_acc', type='TH1F', title="TrigL2HTFullScanHypo Et acc jets;Et [MeV]; nevents", xbins=100, xmin=0., xmax=300000.) ]
        self.Histograms += [ defineHistogram('Eta_acc', type='TH1F', title="TrigL2HTFullScanHypo Eta acc jets;Eta; nevents", xbins=100, xmin=-5., xmax=5.) ]
        self.Histograms += [ defineHistogram('Phi_acc', type='TH1F', title="TrigL2HTFullScanHypo Phi acc jets;Phi; nevents", xbins=64, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta_acc,Phi_acc', type='TH2F', title="TrigL2HTFullScanHypo Phi vs. Eta acc jets;#eta;#phi", xbins=100, xmin=-5., xmax=5., ybins=64, ymin=-3.1416, ymax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta_acc,Et_acc', type='TH2F', title="TrigL2HTFullScanHypo Et vs. Eta acc jets;#eta;E_{T} [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=300000.) ]
        self.Histograms += [ defineHistogram('Phi_acc,Et_acc', type='TH2F', title="TrigL2HTFullScanHypo Et vs. Phi acc jets;#phi;E_{T} [MeV]", xbins=64, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=300000.) ]
        self.Histograms += [ defineHistogram('Et_rej', type='TH1F', title="TrigL2HTFullScanHypo Et rej jets;Et [MeV]; nevents", xbins=100, xmin=0., xmax=300000.) ]
        self.Histograms += [ defineHistogram('Eta_rej', type='TH1F', title="TrigL2HTFullScanHypo Eta rej jets;Eta; nevents", xbins=100, xmin=-5., xmax=5.) ]
        self.Histograms += [ defineHistogram('Phi_rej', type='TH1F', title="TrigL2HTFullScanHypo Phi rej jets;Phi; nevents", xbins=64, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta_rej,Phi_rej', type='TH2F', title="TrigL2HTFullScanHypo Phi vs. Eta rej jets;#eta;#phi", xbins=100, xmin=-5., xmax=5., ybins=64, ymin=-3.1416, ymax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta_rej,Et_rej', type='TH2F', title="TrigL2HTFullScanHypo Et vs. Eta rej jets;#eta;E_{T} [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=300000.) ]
        self.Histograms += [ defineHistogram('Phi_rej,Et_rej', type='TH2F', title="TrigL2HTFullScanHypo Et vs. Phi rej jets;#phi;E_{T} [MeV]", xbins=64, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=300000.) ]
        self.Histograms += [ defineHistogram('nLeadingTowers', type='TH1F', title="TrigL2HTFullScanHypo nLeadingTowers; nLeadingTowers; nevents", xbins=21, xmin=-0.5, xmax=20.5) ]

       

class TrigEFJetHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="EFJetHypo_Validation"):
        super(TrigEFJetHypoValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")

        cutNames = ['No Jet obj', 'Rejected', 'Accepted']
        labelsDescription = ''
        for a in cutNames:
            labelsDescription += a+':'

        self.Histograms = [ defineHistogram('CutCounter', type='TH1F', title="EFJetHypo cut counter", xbins=3, xmin=-1.5, xmax=1.5, labels=labelsDescription) ]
        self.Histograms += [ defineHistogram('NJet', type='TH1F', title="EFJetHypo NJet; Number of jets; nevents", xbins=20, xmin=0, xmax=20) ]
        self.Histograms += [ defineHistogram('Et', type='TH1F', title="EFJetHypo Et; Et [MeV]; nevents", xbins=100, xmin=0., xmax=500000.) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="EFJetHypo Eta;Eta; nevents", xbins=100, xmin=-5., xmax=5.) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="EFJetHypo Phi;Phi; nevents", xbins=64, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta, Phi', type='TH2F', title="EFJetHypo Phi vs. Eta;#eta;#phi", xbins=100, xmin=-5., xmax=5., ybins=64, ymin=-3.1416, ymax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta, Et', type='TH2F', title="EFJetHypo Et vs. Eta;#eta;E_{T} [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100, ymin=0, ymax=500000.) ]
        self.Histograms += [ defineHistogram('Phi, Et', type='TH2F', title="EFJetHypo Et vs. Phi;#phi;E_{T} [MeV]", xbins=64, xmin=-3.1416, xmax=3.1416, ybins=100, ymin=0, ymax=500000.) ]




#more histograms to be added later


class TrigEFJetHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="EFJetHypo_Online"):
        """ defines histograms for online """
        super(TrigEFJetHypoOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")

        cutNames = ['No Jet obj', 'Rejected', 'Accepted']
        labelsDescription = ''
        for a in cutNames:
            labelsDescription += a+':'

        self.Histograms = [ defineHistogram('CutCounter', type='TH1F', title="EFJetHypo cut counter", xbins=3, xmin=-1.5, xmax=1.5, labels=labelsDescription) ]
        self.Histograms += [ defineHistogram('NJet', type='TH1F', title="EFJetHypo NJet; Number of jets; nevents", xbins=20, xmin=0, xmax=20) ]
        self.Histograms += [ defineHistogram('Et', type='TH1F', title="EFJetHypo Et; Et [MeV]; nevents", xbins=100, xmin=0., xmax=500000.) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="EFJetHypo Eta;Eta; nevents", xbins=100, xmin=-5., xmax=5.) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="EFJetHypo Phi;Phi; nevents", xbins=64, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta, Phi', type='TH2F', title="EFJetHypo Phi vs. Eta;#eta;#phi", xbins=100, xmin=-5., xmax=5., ybins=64, ymin=-3.1416, ymax=3.1416) ]
        #self.Histograms += [ defineHistogram('Eta, Et', type='TH2F', title="EFJetHypo Et vs. Eta;#eta;E_{T} [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100, ymin=0, ymax=500000.) ]
        #self.Histograms += [ defineHistogram('Phi, Et', type='TH2F', title="EFJetHypo Et vs. Phi;#phi;E_{T} [MeV]", xbins=64, xmin=-3.1416, xmax=3.1416, ybins=100, ymin=0, ymax=500000.) ]


class TrigEFJetHypoCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="EFJetHypo_Cosmic"):
        """ defines histograms for cosmic """
        super(TrigEFJetHypoCosmicMonitoring, self).__init__(name)

        self.defineTarget("Cosmic")

        cutNames = ['No Jet obj', 'Rejected', 'Accepted']
        labelsDescription = ''
        for a in cutNames:
            labelsDescription += a+':'

        self.Histograms = [ defineHistogram('CutCounter', type='TH1F', title="EFJetHypo cut counter", xbins=3, xmin=-1.5, xmax=1.5, labels=labelsDescription) ]
        self.Histograms += [ defineHistogram('NJet', type='TH1F', title="EFJetHypo NJet; Number of jets; nevents", xbins=20, xmin=0, xmax=20) ]
        self.Histograms += [ defineHistogram('Et', type='TH1F', title="EFJetHypo Et; Et [MeV]; nevents", xbins=100, xmin=0., xmax=500000.) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="EFJetHypo Eta;Eta; nevents", xbins=100, xmin=-5., xmax=5.) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="EFJetHypo Phi;Phi; nevents", xbins=64, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta, Phi', type='TH2F', title="EFJetHypo Phi vs. Eta;#eta;#phi", xbins=100, xmin=-5., xmax=5., ybins=64, ymin=-3.1416, ymax=3.1416) ]
        #self.Histograms += [ defineHistogram('Eta, Et', type='TH2F', title="EFJetHypo Et vs. Eta;#eta;E_{T} [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100, ymin=0, ymax=300000.) ]
        #self.Histograms += [ defineHistogram('Phi, Et', type='TH2F', title="EFJetHypo Et vs. Phi;#phi;E_{T} [MeV]", xbins=64, xmin=-3.1416, xmax=3.1416, ybins=100, ymin=0, ymax=300000.) ]
