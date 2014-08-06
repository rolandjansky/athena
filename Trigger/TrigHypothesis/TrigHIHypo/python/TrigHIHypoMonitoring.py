# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 

class TrigHIJetBaseSubtractionValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="HIJetBaseSubtraction_Validation"):
        """ defines histograms for validation """
        super(TrigHIJetBaseSubtractionValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")
        
        self.Histograms += [ defineHistogram('Jet_Background', type='TH1F', title="HIJetBaseSubtraction Jet_Background;Et [MeV]; nevents", xbins=150, xmin=0., xmax=150000.) ]
        self.Histograms += [ defineHistogram('JetEt', type='TH1F', title="HIJetBaseSubtraction JetEt;Et [MeV]; njets", xbins=100, xmin=0., xmax=1000000.) ]
        self.Histograms += [ defineHistogram ('L1_SumEt, JetEt' ,type='TH2F',title='HIJetBaseSubtraction L1_SumEt vs Jet Et; L1_SumEt [MeV] ; Jet Et [MeV]' ,ybins=100,ymin=0.,ymax=1000000,xbins=150,xmin=0.,xmax=15000000)]
        self.Histograms += [ defineHistogram ('JetEta, JetEt' ,type='TH2F',title='HIJetBaseSubtraction Jet Eta vs JetEt; Jet Eta ; Jet Et [MeV]' ,ybins=100,ymin=0.,ymax=1000000,xbins=100,xmin=-5.,xmax=5.)]


class TrigHIJetBaseSubtractionOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="HIJetBaseSubtraction_Online"):
        """ defines histograms for online """
        super(TrigHIJetBaseSubtractionOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")
               
        self.Histograms += [ defineHistogram('Jet_Background', type='TH1F', title="HIJetBaseSubtraction Jet_Background;Et [MeV]; nevents", xbins=150, xmin=0., xmax=150000.) ]
        self.Histograms += [ defineHistogram('JetEt', type='TH1F', title="HIJetBaseSubtraction JetEt;Et [MeV]; njets", xbins=100, xmin=0., xmax=1000000.) ]
        self.Histograms += [ defineHistogram ('L1_SumEt, JetEt' ,type='TH2F',title='HIJetBaseSubtraction L1_SumEt vs Jet Et; L1_SumEt [MeV] ; Jet Et [MeV]' ,ybins=100,ymin=0.,ymax=1000000,xbins=150,xmin=0.,xmax=15000000)]
        self.Histograms += [ defineHistogram ('JetEta, JetEt' ,type='TH2F',title='HIJetBaseSubtraction Jet Eta vs JetEt; Jet Eta ; Jet Et [MeV]' ,ybins=100,ymin=0.,ymax=1000000,xbins=100,xmin=-5.,xmax=5.)]
   

class TrigHIJetBaseSubtractionCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="HIJetBaseSubtraction_Cosmic"):
        """ defines histograms for cosmic """
        super(TrigHIJetBaseSubtractionCosmicMonitoring, self).__init__(name)

        self.defineTarget("Cosmic")

        self.Histograms += [ defineHistogram('Jet_Background', type='TH1F', title="HIJetBaseSubtraction Jet_Background;Et [MeV]; nevents", xbins=150, xmin=0., xmax=150000.) ]
        self.Histograms += [ defineHistogram('JetEt', type='TH1F', title="HIJetBaseSubtraction JetEt;Et [MeV]; njets", xbins=100, xmin=0., xmax=1000000.) ]
        self.Histograms += [ defineHistogram ('L1_SumEt, JetEt' ,type='TH2F',title='HIJetBaseSubtraction L1_SumEt vs Jet Et; L1_SumEt [MeV] ; Jet Et [MeV]' ,ybins=100,ymin=0.,ymax=1000000,xbins=150,xmin=0.,xmax=15000000)]
        self.Histograms += [ defineHistogram ('JetEta, JetEt' ,type='TH2F',title='HIJetBaseSubtraction Jet Eta vs JetEt; Jet Eta ; Jet Et [MeV]' ,ybins=100,ymin=0.,ymax=1000000,xbins=100,xmin=-5.,xmax=5.)]
       
