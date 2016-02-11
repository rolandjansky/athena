# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TrigL2CaloHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2CaloHypoValidationMonitoring"):
        super(TrigL2CaloHypoValidationMonitoring, self).__init__(name)
        self.defineTarget( "Validation" )

        self.Histograms = [ defineHistogram('dEta', type='TH1F', title="L2Calo Hypo #Delta#eta_{L2 L1}; #Delta#eta_{L2 L1}",
                                            xbins=80, xmin=-0.01, xmax=0.01) ]
        self.Histograms += [ defineHistogram('dPhi', type='TH1F', title="L2Calo Hypo #Delta#phi_{L2 L1}; #Delta#phi_{L2 L1}",
                                            xbins=80, xmin=-0.01, xmax=0.01) ]
        self.Histograms += [ defineHistogram('Et_em', type='TH1F', title="L2Calo Hypo cluster E_{T}^{EM};E_{T}^{EM} [MeV]",
                                    xbins=50, xmin=-2000, xmax=100000) ]
        self.Histograms += [ defineHistogram('Et_had', type='TH1F', title="L2Calo Hypo E_{T}^{had} in first layer;E_{T}^{had} [MeV]",
                                    xbins=50, xmin=-2000, xmax=100000) ]
        self.Histograms += [ defineHistogram('Rcore', type='TH1F', title="L2Calo Hypo R_{core};E^{3x3}/E^{3x7} in sampling 2",
                                    xbins=48, xmin=-0.1, xmax=1.1) ]
        self.Histograms += [ defineHistogram('Eratio', type='TH1F',
                                    title="L2Calo Hypo E_{ratio};E^{max1}-E^{max2}/E^{max1}+E^{max2} in sampling 1 (excl.crack)",
                                    xbins=64, xmin=-0.1, xmax=1.5) ]
        self.Histograms += [ defineHistogram('EtaBin', type='TH1I', title="L2Calo Hypo entries per Eta bin;Eta bin no.",
                                    xbins=11, xmin=-0.5, xmax=10.5) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="L2Calo Hypo entries per Eta;Eta",
                                    xbins=100, xmin=-2.5, xmax=2.5) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="L2Calo Hypo entries per Phi;Phi",
                                    xbins=128, xmin=-3.2, xmax=3.2) ]

        self.Histograms += [ defineHistogram('F1', type='TH1F', title="L2Calo Hypo f_{1};f_{1}",
                                             xbins=34, xmin=-0.5, xmax=1.2) ] 
                                           
        self.Histograms += [ defineHistogram('Weta2', type='TH1F', title="L2Calo Hypo Weta2; E Width in sampling 2",xbins=96, xmin=-0.1, xmax=0.61) ]     
        self.Histograms += [ defineHistogram('Wstot', type='TH1F', title="L2Calo Hypo Wstot; E Width in sampling 1",xbins=48, xmin=-0.1, xmax=11.) ]
        self.Histograms += [ defineHistogram('F3', type='TH1F', title="L2Calo Hypo F3; E3/(E0+E1+E2+E3)",xbins=96, xmin=-0.1, xmax=1.1) ]        
 
        cuts=['Input','has one TrigEMCluster', '#Delta #eta L2-L1', '#Delta #phi L2-L1','eta','rCore',
              'eRatio','E_{T}^{EM}', 'E_{T}^{Had}','f_{1}','Weta2','Wstot','F3']

        labelsDescription = ''
        for c in cuts:
            labelsDescription +=  c+':'
            
        self.Histograms += [ defineHistogram('CutCounter', type='TH1I', title="L2Calo Hypo Passed Cuts;Cut",
                                    xbins=13, xmin=-1.5, xmax=12.5,  opt="kCumulative", labels=labelsDescription) ]


class TrigL2CaloHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2CaloHypoOnlineMonitoring"):
        """ defines hisograms for online """
        super(TrigL2CaloHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms = [ defineHistogram('dEta', type='TH1F', title="L2Calo Hypo #Delta#eta_{L2 L1}; #Delta#eta_{L2 L1}",
                                            xbins=80, xmin=-0.01, xmax=0.01) ]
        self.Histograms += [ defineHistogram('dPhi', type='TH1F', title="L2Calo Hypo #Delta#phi_{L2 L1}; #Delta#phi_{L2 L1}",
                                            xbins=80, xmin=-0.01, xmax=0.01) ]
        self.Histograms += [ defineHistogram('Et_em', type='TH1F', title="L2Calo Hypo cluster E_{T}^{EM};E_{T}^{EM} [MeV]",
                                    xbins=50, xmin=-2000, xmax=100000) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="L2Calo Hypo entries per Eta;Eta",
                                    xbins=100, xmin=-2.5, xmax=2.5) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="L2Calo Hypo entries per Phi;Phi",
                                    xbins=128, xmin=-3.2, xmax=3.2) ]

        cuts=['Input','has one TrigEMCluster', '#Delta #eta L2-L1', '#Delta #phi L2-L1','eta','rCore',
              'eRatio','E_{T}^{EM}', 'E_{T}^{Had}','f_{1}','Weta2','Wstot','F3']

        labelsDescription = ''
        for c in cuts:
            labelsDescription +=  c+':'
            
        self.Histograms += [ defineHistogram('CutCounter', type='TH1I', title="L2Calo Hypo Passed Cuts;Cut",
                                             xbins=13, xmin=-1.5, xmax=12.5,  opt="kCumulative", labels=labelsDescription) ]

class TrigL2CaloLayersHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2CaloLayersHypoOnlineMonitoring"):
        """ defines hisograms for online """
        super(TrigL2CaloLayersHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms += [ defineHistogram('Energy', type='TH1F', title="RoI Energy [MeV]",
                                    xbins=50, xmin=0, xmax=50000) ]
        self.Histograms += [ defineHistogram('PreSampler_Energy', type='TH1F', title="Energy in the PreSampler [MeV]",
                                    xbins=50, xmin=0, xmax=20000) ]
        self.Histograms += [ defineHistogram('PreSampler_fracEnergy', type='TH1F', title="Fraction of energy in the PreSampler [MeV]",
                                    xbins=50, xmin=0, xmax=1) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="L2Calo Hypo entries per Eta;Eta",
                                    xbins=50, xmin=-2.5, xmax=2.5) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="L2Calo Hypo entries per Phi;Phi",
                                    xbins=64, xmin=-3.2, xmax=3.2) ]

        cuts=['Input','has one TrigEMCluster', 'Fraction of Energy in PreSampler', 'Absolute Energy in Presampler']

        labelsDescription = ''
        for c in cuts:
            labelsDescription +=  c+':'

        self.Histograms += [ defineHistogram('CutCounter', type='TH1I', title="RoI Layers Hypo Passed Cuts;Cut",
                                             xbins=4, xmin=-0.5, xmax=3.5,  opt="kCumulative", labels=labelsDescription) ]
        
class TrigL2CaloLayersHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2CaloLayersHypoValidationMonitoring"):
        super(TrigL2CaloLayersHypoValidationMonitoring, self).__init__(name)
        self.defineTarget( "Validation" )


        self.Histograms += [ defineHistogram('Energy', type='TH1F', title="RoI Energy [MeV]",
                                    xbins=50, xmin=0, xmax=50000) ]
        self.Histograms += [ defineHistogram('PreSampler_Energy', type='TH1F', title="Energy in the PreSampler [MeV]",
                                    xbins=50, xmin=0, xmax=20000) ]
        self.Histograms += [ defineHistogram('PreSampler_fracEnergy', type='TH1F', title="Fraction of energy in the PreSampler [MeV]",
                                    xbins=50, xmin=0, xmax=1) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="L2Calo Hypo entries per Eta;Eta",
                                    xbins=50, xmin=-2.5, xmax=2.5) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="L2Calo Hypo entries per Phi;Phi",
                                    xbins=64, xmin=-3.2, xmax=3.2) ]

        cuts=['Input','has one TrigEMCluster', 'Fraction of Energy in PreSampler', 'Absolute Energy in Presampler']

        labelsDescription = ''
        for c in cuts:
            labelsDescription +=  c+':'

        self.Histograms += [ defineHistogram('CutCounter', type='TH1I', title="RoI Layers Hypo Passed Cuts;Cut",
                                             xbins=4, xmin=-0.5, xmax=3.5,  opt="kCumulative", labels=labelsDescription) ]
