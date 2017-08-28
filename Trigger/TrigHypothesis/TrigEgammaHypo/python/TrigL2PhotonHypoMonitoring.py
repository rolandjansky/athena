# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TrigL2PhotonHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2PhotonHypoValidationMonitoring"):
        super(TrigL2PhotonHypoValidationMonitoring, self).__init__(name)
        self.defineTarget( "Validation" )


        self.Histograms = [ defineHistogram('PhEt', 	type='TH1F', title="L2Photon Hypo E_{T}; E_{T}^{EM} [MeV]",
                                                                                                                xbins=50, xmin=-2000, xmax=100000) ]
        self.Histograms += [ defineHistogram('PhEta', 	type='TH1F', title="L2Photon Hypo #eta;#eta",
                                            									xbins=100, xmin=-2.5, xmax=2.5) ]
        self.Histograms += [ defineHistogram('PhPhi', 	type='TH1F', title="L2Photon Hypo #phi;#phi",
                                            									xbins=128, xmin=-3.2, xmax=3.2) ]
        self.Histograms += [ defineHistogram('PhEratio', 	type='TH1F', title="L2Photon Hypo Eratio;Eratio",
                                    										xbins=64, xmin=-0.1, xmax=1.5) ]
        self.Histograms += [ defineHistogram('PhRcore', 	type='TH1F', title="L2Photon Hypo Rcore; Rcore",
                                             									xbins=48, xmin=-0.1, xmax=1.1 ) ]

        self.Histograms += [ defineHistogram('dEta', 	type='TH1F', title="L2Photon Hypo #Delta #eta; #Delta #eta",
                                             xbins=80, xmin=-0.2, xmax=0.2 ) ]

        self.Histograms += [ defineHistogram('dPhi', 	type='TH1F', title="L2Photon Hypo #Delta #phi; #Delta #phi",
                                             xbins=80, xmin=-0.2, xmax=0.2 ) ]

        self.Histograms += [ defineHistogram('PhHadEt', 	type='TH1F', title="L2Photon Hypo E_{T}^{had} [MeV]; E_{T}^{Had} [MeV]}",
                                             xbins=50, xmin=-2000, xmax=100000 ) ]

        self.Histograms += [ defineHistogram('PhF1', type='TH1F', title="L2Photon Hypo f_{1};f_{1}",
                                             xbins=34, xmin=-0.5, xmax=1.2) ] 



        cuts=['Input','has TrigPhotonContainer', '#Delta #eta', '#Delta #phi', '#eta','Reta','eRatio','E_{T}^{EM}', 'E_{T}^{Had}','f_{1}']

        labelsDescription = ''
        for c in cuts:
            labelsDescription +=  c+':'

        self.Histograms += [ defineHistogram('CutCounter', 	type='TH1F', title="L2Photon Hypo Passed Cuts;Cut",
                                             xbins=10, xmin=-1.5, xmax=10.5, opt="kCumulative", labels=labelsDescription) ]        


class TrigL2PhotonHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2PhotonHypoOnlineMonitoring"):
        """ defines histograms for online """
        super(TrigL2PhotonHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")


        self.Histograms = [ defineHistogram('PhEt', 	type='TH1F', title="L2Photon Hypo E_{T}; E_{T}^{EM} [MeV]",
                                                                                                                xbins=50, xmin=-2000, xmax=100000) ]
        self.Histograms += [ defineHistogram('PhEta', 	type='TH1F', title="L2Photon Hypo #eta;#eta",
                                            									xbins=100, xmin=-2.5, xmax=2.5) ]
        self.Histograms += [ defineHistogram('PhPhi', 	type='TH1F', title="L2Photon Hypo #phi;#phi",
                                            									xbins=128, xmin=-3.2, xmax=3.2) ]
        cuts=['Input','has TrigPhotonContainer', '#Delta #eta', '#Delta #phi', '#eta','Reta','eRatio','E_{T}^{EM}', 'E_{T}^{Had}','f_{1}']
        
        labelsDescription = ''
        for c in cuts:
            labelsDescription +=  c+':'

        self.Histograms += [ defineHistogram('CutCounter', 	type='TH1F', title="L2Photon Hypo Passed Cuts;Cut",
                                             xbins=10, xmin=-1.5, xmax=10.5, opt="kCumulative", labels=labelsDescription) ]   
            
      

class TrigL2PhotonHypoCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2PhotonHypoCosmicMonitoring"):
        """ defines histograms for cosmic """
        super(TrigL2PhotonHypoCosmicMonitoring, self).__init__(name)
        self.defineTarget("Cosmic")

        self.Histograms = [ defineHistogram('PhEt', 	type='TH1F', title="L2Photon Hypo E_{T}; E_{T}^{EM} [MeV]",
                                                                                                                xbins=50, xmin=-2000, xmax=100000) ]
        self.Histograms += [ defineHistogram('PhEta', 	type='TH1F', title="L2Photon Hypo #eta;#eta",
                                            									xbins=100, xmin=-2.5, xmax=2.5) ]
        self.Histograms += [ defineHistogram('PhPhi', 	type='TH1F', title="L2Photon Hypo #phi;#phi",
                                            									xbins=128, xmin=-3.2, xmax=3.2) ]
        self.Histograms += [ defineHistogram('PhEratio', 	type='TH1F', title="L2Photon Hypo Eratio;Eratio",
                                    										xbins=64, xmin=-0.1, xmax=1.5) ]
        self.Histograms += [ defineHistogram('PhRcore', 	type='TH1F', title="L2Photon Hypo Rcore; Rcore",
                                             									xbins=48, xmin=-0.1, xmax=1.1 ) ]

        self.Histograms += [ defineHistogram('dEta', 	type='TH1F', title="L2Photon Hypo #Delta #eta; #Delta #eta",
                                             xbins=80, xmin=-0.2, xmax=0.2 ) ]

        self.Histograms += [ defineHistogram('dPhi', 	type='TH1F', title="L2Photon Hypo #Delta #phi; #Delta #phi",
                                             xbins=80, xmin=-0.2, xmax=0.2 ) ]

        self.Histograms += [ defineHistogram('PhHadEt', 	type='TH1F', title="L2Photon Hypo E_{T}^{had} [MeV]; E_{T}^{Had} [MeV]}",
                                             xbins=50, xmin=-2000, xmax=100000 ) ]

        self.Histograms += [ defineHistogram('PhF1', type='TH1F', title="L2Photon Hypo f_{1};f_{1}",
                                             xbins=34, xmin=-0.5, xmax=1.2) ] 
        cuts=['Input','has TrigPhotonContainer', '#Delta #eta', '#Delta #phi', '#eta','Reta','eRatio','E_{T}^{EM}', 'E_{T}^{Had}','f_{1}']

        labelsDescription = ''
        for c in cuts:
            labelsDescription +=  c+':'

        self.Histograms += [ defineHistogram('CutCounter', 	type='TH1F', title="L2Photon Hypo Passed Cuts;Cut",
                                             xbins=10, xmin=-1.5, xmax=10.5, opt="kCumulative", labels=labelsDescription) ]   
       
      

