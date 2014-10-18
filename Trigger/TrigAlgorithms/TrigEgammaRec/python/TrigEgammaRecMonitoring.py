# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TrigEgammaRecValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEgammaRecValidationMonitoring"):
        super(TrigEgammaRecValidationMonitoring, self).__init__(name)
        self.defineTarget( "Validation" )

        self.Histograms = [defineHistogram('El_Eta', type='TH1F', title="#eta of Clusters; #eta; number of RoIs", xbins=50,xmin=-2.5,xmax=2.5) ]
        self.Histograms += [defineHistogram('El_Phi',type='TH1F', title="#phi of Clusters; #phi; number of RoIs", xbins=64,xmin=-3.2,xmax=3.2) ]
        self.Histograms += [defineHistogram('El_Eta,El_Phi',type='TH2F',title="#eta vs #phi of Clusters; #eta; #phi", xbins=50,xmin=-2.5,xmax=2.5, ybins=64,ymin=-3.2,ymax=3.2) ]                

        self.Histograms += [ defineHistogram('El_ClusterEt', 	type='TH1F', title="EFEgamma Hypo Cluster E_{T}; E_{T}^{em} [MeV]",
                                            xbins=50, xmin=-2000, xmax=100000) ]
     
        self.Histograms += [ defineHistogram('El_F1', 	type='TH1F', title="EFEgamma Hypo fraction of energy found in 1st em sampling;Fraction",
                                             xbins=50, xmin=-0.1, xmax=1.1 ) ]

        self.Histograms += [ defineHistogram('El_E237', 	type='TH1F', title="EFEgamma Hypo uncor energy in 3x7 cells in em sampling 2; E [MeV]",
                                             xbins=50, xmin=-15000, xmax=150000 ) ]

        self.Histograms += [ defineHistogram('El_E277', 	type='TH1F', title="EFEgamma Hypo uncor energy in 7x7 cells in em sampling 2;E [MeV]",
                                             xbins=50, xmin=-15000, xmax=150000 ) ]

        self.Histograms += [ defineHistogram('El_EtHad1', 	type='TH1F', title="EFEgamma Hypo E leakage into 1st sampling of had calo; E [MeV]",
                                             xbins=50, xmin=-15000, xmax=150000 ) ]

        self.Histograms += [ defineHistogram('El_WEta1', 	type='TH1F', title="EFEgamma Hypo corrected width in 3 strips in the 1st samp. ; Width",
                                             xbins=20, xmin=0, xmax=1 ) ]

        self.Histograms += [ defineHistogram('El_WEta2', 	type='TH1F', title="EFEgamma  Hypo corrected width in 3 strips in the 2st samp. ; Width",
                                             xbins=20, xmin=0, xmax=0.05 ) ]

        self.Histograms += [ defineHistogram('El_Emax2', 	type='TH1F', title="EFEgamma Hypo 2nd maximum in strips; E [MeV]",
                                             xbins=50, xmin=-1000, xmax=5000 ) ]

        self.Histograms += [ defineHistogram('El_Emins1', 	type='TH1F', title="EFEgamma Hypo energy of strip with minimum between max 1 & 2; E [MeV]",
                                             xbins=50, xmin=-1000, xmax=5000 ) ]

        self.Histograms += [ defineHistogram('El_Fracs1', 	type='TH1F', title="EFEgamma Hypo energy outside core (E(+-7)-E(+-3))/E(+-7); E [MeV]",
                                             xbins=50, xmin=-5000, xmax=10000 ) ]

        self.Histograms += [ defineHistogram('El_wtots1', 	type='TH1F', title="EFEgamma Hypo total width in em sampling 1 in 20 strips; Width",
                                             xbins=20, xmin=0, xmax=20 ) ]

        self.Histograms += [ defineHistogram('El_EtCone20', 	type='TH1F', title="EFEgamma Hypo fraction of Et in a ring of 0.1<DR<0.20 above noise to total Et; Fraction",
                                             xbins=50, xmin=-1, xmax=3 ) ]

        self.Histograms += [ defineHistogram('TrackPt', 	type='TH1F', title="EFEgamma Hypo Track E_{T}; E_{T}^{track}} [MeV]",
                                            xbins=50, xmin=-2000, xmax=100000) ]    

        self.Histograms += [ defineHistogram('dEta', 	type='TH1F', title="EFEgamma Hypo; #Delta#eta  between track and cluster in first layer; #Delta#eta",
                                             xbins=100, xmin=-0.1, xmax=0.1) ]

        self.Histograms += [ defineHistogram('dPhi', 	type='TH1F', title="EFEgamma Hypo #Delta #phi between track and cluster in second layer; #Delta#phi",
                                             xbins=100, xmin=-0.150, xmax=0.150) ]
        self.Histograms += [ defineHistogram('rTRT', 	type='TH1F', title="EFEgamma Hypo ratio of TRT high threshold hits; rTRT",
                                             xbins=30, xmin=-0.1, xmax=1.1 ) ]
        self.Histograms += [ defineHistogram('nBLayerHits', 	type='TH1F', title="EFEgamma Hypo number of B layer hits;Number of hits",
                                             xbins=4, xmin=-0.5, xmax=3.5 ) ]

        self.Histograms += [ defineHistogram('nPixelHits', 	type='TH1F', title="EFEgamma Hypo number of pixel hits; Number of hits",
                                             xbins=11, xmin=-0.5, xmax=10.5 ) ]

        self.Histograms += [ defineHistogram('nSCTHits', 	type='TH1F', title="EFEgamma Hypo number of SCT hits; Number of hits",
                                             xbins=26, xmin=-0.5, xmax=25.5 ) ]

        self.Histograms += [ defineHistogram('nTRTHits', 	type='TH1F', title="EFEgamma Hypo number of TRT hits; Number of hits",
                                             xbins=46, xmin=-0.5, xmax=45.5 ) ]

        self.Histograms += [ defineHistogram('nTRTHitsHighTh', 	type='TH1F', title="EFEgamma Hypo number of high threshold TRT hits; Number of hits",
                                              xbins=26, xmin=-0.5, xmax=25.5 ) ]

        self.Histograms += [ defineHistogram('nTRTHitsHighThOutliers', 	type='TH1F', title="EFEgamma Hypo number of outliers high threshold TRT hits; Number of hits",
                                              xbins=26, xmin=-0.5, xmax=25.5 ) ]

        self.Histograms += [ defineHistogram('nTRTHitsOutliers', 	type='TH1F', title="EFEgamma Hypo number of outliers TRT hits; Number of hits",
                                             xbins=26, xmin=-0.5, xmax=25.5 ) ]

        # Photon Collection
        self.Histograms += [ defineHistogram('Ph_e237', 	type='TH1F', title="EF Photon Hypo uncor energy in 3x7 cells in em sampling 2; E [MeV]",
                                             xbins=50, xmin=-15000, xmax=150000 ) ]

        self.Histograms += [ defineHistogram('Ph_e277', 	type='TH1F', title="EF Photon Hypo uncor energy in 7x7 cells in em sampling 2;E [MeV]",
                                             xbins=50, xmin=-15000, xmax=150000 ) ]

        self.Histograms += [ defineHistogram('Ph_ethad1', 	type='TH1F', title="EF Photon Hypo E leakage into 1st sampling of had calo; E [MeV]",
                                             xbins=50, xmin=-15000, xmax=150000 ) ]

        self.Histograms += [ defineHistogram('Ph_weta1', 	type='TH1F', title="EF Photon Hypo corrected width in 3 strips in the 1st samp. ; Width",
                                             xbins=20, xmin=0, xmax=1 ) ]

        self.Histograms += [ defineHistogram('Ph_weta2', 	type='TH1F', title="EF Photon  Hypo corrected width in 3 strips in the 2st samp. ; Width",
                                             xbins=20, xmin=0, xmax=0.05 ) ]

        self.Histograms += [ defineHistogram('Ph_Emax2', 	type='TH1F', title="EF Photon Hypo 2nd maximum in strips; E [MeV]",
                                             xbins=50, xmin=-1000, xmax=5000 ) ]

        self.Histograms += [ defineHistogram('Ph_Emins1', 	type='TH1F', title="EF Photon Hypo energy of strip with minimum between max 1 & 2; E [MeV]",
                                             xbins=50, xmin=-1000, xmax=5000 ) ]

        self.Histograms += [ defineHistogram('Ph_fracs1', 	type='TH1F', title="EF Photon Hypo energy outside core (E(+-7)-E(+-3))/E(+-7); E [MeV]",
                                             xbins=50, xmin=-5000, xmax=10000 ) ]

        self.Histograms += [ defineHistogram('Ph_wtots1', 	type='TH1F', title="EF Photon Hypo total width in em sampling 1 in 20 strips; Width",
                                             xbins=20, xmin=0, xmax=20 ) ]
        cuts=["loose","medium","tight"]


        labelsDescription = ''
        for c in cuts:
            labelsDescription +=  c+':'


        #self.Histograms += [ defineHistogram('egIsEM', 	type='TH1I', title="EFEgammaRec isEM; Cut",
        #                                     xbins=3, xmin=0.5, xmax=3.5, labels=labelsDescription)]

  



class TrigEgammaRecOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEgammaRecOnlineMonitoring"):
        """ defines hisograms for online """
        super(TrigEgammaRecOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms = [defineHistogram('El_Eta', type='TH1F', title="#eta of Clusters; #eta; number of RoIs", xbins=50,xmin=-2.5,xmax=2.5) ]
        self.Histograms += [defineHistogram('El_Phi',type='TH1F', title="#phi of Clusters; #phi; number of RoIs", xbins=64,xmin=-3.2,xmax=3.2) ]
        self.Histograms += [defineHistogram('El_Eta,El_Phi',type='TH2F',title="#eta vs #phi of Clusters; #eta; #phi", xbins=50,xmin=-2.5,xmax=2.5, ybins=64,ymin=-3.2,ymax=3.2) ]                

        self.Histograms += [ defineHistogram('El_ClusterEt', 	type='TH1F', title="EFEgamma Hypo Cluster E_{T}; E_{T}^{em} [MeV]",
                                            xbins=50, xmin=-2000, xmax=100000) ]
     
        self.Histograms += [ defineHistogram('El_F1', 	type='TH1F', title="EFEgamma Hypo fraction of energy found in 1st em sampling;Fraction",
                                             xbins=50, xmin=-0.1, xmax=1.1 ) ]

        self.Histograms += [ defineHistogram('El_E237', 	type='TH1F', title="EFEgamma Hypo uncor energy in 3x7 cells in em sampling 2; E [MeV]",
                                             xbins=50, xmin=-15000, xmax=150000 ) ]

        self.Histograms += [ defineHistogram('El_E277', 	type='TH1F', title="EFEgamma Hypo uncor energy in 7x7 cells in em sampling 2;E [MeV]",
                                             xbins=50, xmin=-15000, xmax=150000 ) ]

        self.Histograms += [ defineHistogram('El_EtHad1', 	type='TH1F', title="EFEgamma Hypo E leakage into 1st sampling of had calo; E [MeV]",
                                             xbins=50, xmin=-15000, xmax=150000 ) ]

        self.Histograms += [ defineHistogram('El_WEta1', 	type='TH1F', title="EFEgamma Hypo corrected width in 3 strips in the 1st samp. ; Width",
                                             xbins=20, xmin=0, xmax=1 ) ]

        self.Histograms += [ defineHistogram('El_WEta2', 	type='TH1F', title="EFEgamma  Hypo corrected width in 3 strips in the 2st samp. ; Width",
                                             xbins=20, xmin=0, xmax=0.05 ) ]

        self.Histograms += [ defineHistogram('El_Emax2', 	type='TH1F', title="EFEgamma Hypo 2nd maximum in strips; E [MeV]",
                                             xbins=50, xmin=-1000, xmax=5000 ) ]

        self.Histograms += [ defineHistogram('El_Emins1', 	type='TH1F', title="EFEgamma Hypo energy of strip with minimum between max 1 & 2; E [MeV]",
                                             xbins=50, xmin=-1000, xmax=5000 ) ]

        self.Histograms += [ defineHistogram('El_Fracs1', 	type='TH1F', title="EFEgamma Hypo energy outside core (E(+-7)-E(+-3))/E(+-7); E [MeV]",
                                             xbins=50, xmin=-5000, xmax=10000 ) ]

        self.Histograms += [ defineHistogram('El_wtots1', 	type='TH1F', title="EFEgamma Hypo total width in em sampling 1 in 20 strips; Width",
                                             xbins=20, xmin=0, xmax=20 ) ]

        self.Histograms += [ defineHistogram('El_EtCone20', 	type='TH1F', title="EFEgamma Hypo fraction of Et in a ring of 0.1<DR<0.20 above noise to total Et; Fraction",
                                             xbins=50, xmin=-1, xmax=3 ) ]

        self.Histograms += [ defineHistogram('TrackPt', 	type='TH1F', title="EFEgamma Hypo Track E_{T}; E_{T}^{track}} [MeV]",
                                            xbins=50, xmin=-2000, xmax=100000) ]    

        self.Histograms += [ defineHistogram('dEta', 	type='TH1F', title="EFEgamma Hypo; #Delta#eta  between track and cluster in first layer; #Delta#eta",
                                             xbins=100, xmin=-0.1, xmax=0.1) ]

        self.Histograms += [ defineHistogram('dPhi', 	type='TH1F', title="EFEgamma Hypo #Delta #phi between track and cluster in second layer; #Delta#phi",
                                             xbins=100, xmin=-0.150, xmax=0.150) ]
        self.Histograms += [ defineHistogram('rTRT', 	type='TH1F', title="EFEgamma Hypo ratio of TRT high threshold hits; rTRT",
                                             xbins=30, xmin=-0.1, xmax=1.1 ) ]
        self.Histograms += [ defineHistogram('nBLayerHits', 	type='TH1F', title="EFEgamma Hypo number of B layer hits;Number of hits",
                                             xbins=4, xmin=-0.5, xmax=3.5 ) ]

        self.Histograms += [ defineHistogram('nPixelHits', 	type='TH1F', title="EFEgamma Hypo number of pixel hits; Number of hits",
                                             xbins=11, xmin=-0.5, xmax=10.5 ) ]

        self.Histograms += [ defineHistogram('nSCTHits', 	type='TH1F', title="EFEgamma Hypo number of SCT hits; Number of hits",
                                             xbins=26, xmin=-0.5, xmax=25.5 ) ]

        self.Histograms += [ defineHistogram('nTRTHits', 	type='TH1F', title="EFEgamma Hypo number of TRT hits; Number of hits",
                                             xbins=46, xmin=-0.5, xmax=45.5 ) ]

        self.Histograms += [ defineHistogram('nTRTHitsHighTh', 	type='TH1F', title="EFEgamma Hypo number of high threshold TRT hits; Number of hits",
                                              xbins=26, xmin=-0.5, xmax=25.5 ) ]

        self.Histograms += [ defineHistogram('nTRTHitsHighThOutliers', 	type='TH1F', title="EFEgamma Hypo number of outliers high threshold TRT hits; Number of hits",
                                              xbins=26, xmin=-0.5, xmax=25.5 ) ]

        self.Histograms += [ defineHistogram('nTRTHitsOutliers', 	type='TH1F', title="EFEgamma Hypo number of outliers TRT hits; Number of hits",
                                             xbins=26, xmin=-0.5, xmax=25.5 ) ]

        # Photon Collection
        self.Histograms += [ defineHistogram('Ph_e237', 	type='TH1F', title="EF Photon Hypo uncor energy in 3x7 cells in em sampling 2; E [MeV]",
                                             xbins=50, xmin=-15000, xmax=150000 ) ]

        self.Histograms += [ defineHistogram('Ph_e277', 	type='TH1F', title="EF Photon Hypo uncor energy in 7x7 cells in em sampling 2;E [MeV]",
                                             xbins=50, xmin=-15000, xmax=150000 ) ]

        self.Histograms += [ defineHistogram('Ph_ethad1', 	type='TH1F', title="EF Photon Hypo E leakage into 1st sampling of had calo; E [MeV]",
                                             xbins=50, xmin=-15000, xmax=150000 ) ]

        self.Histograms += [ defineHistogram('Ph_weta1', 	type='TH1F', title="EF Photon Hypo corrected width in 3 strips in the 1st samp. ; Width",
                                             xbins=20, xmin=0, xmax=1 ) ]

        self.Histograms += [ defineHistogram('Ph_weta2', 	type='TH1F', title="EF Photon  Hypo corrected width in 3 strips in the 2st samp. ; Width",
                                             xbins=20, xmin=0, xmax=0.05 ) ]

        self.Histograms += [ defineHistogram('Ph_Emax2', 	type='TH1F', title="EF Photon Hypo 2nd maximum in strips; E [MeV]",
                                             xbins=50, xmin=-1000, xmax=5000 ) ]

        self.Histograms += [ defineHistogram('Ph_Emins1', 	type='TH1F', title="EF Photon Hypo energy of strip with minimum between max 1 & 2; E [MeV]",
                                             xbins=50, xmin=-1000, xmax=5000 ) ]

        self.Histograms += [ defineHistogram('Ph_fracs1', 	type='TH1F', title="EF Photon Hypo energy outside core (E(+-7)-E(+-3))/E(+-7); E [MeV]",
                                             xbins=50, xmin=-5000, xmax=10000 ) ]

        self.Histograms += [ defineHistogram('Ph_wtots1', 	type='TH1F', title="EF Photon Hypo total width in em sampling 1 in 20 strips; Width",
                                             xbins=20, xmin=0, xmax=20 ) ]


        cuts=["loose","medium","tight"]


        labelsDescription = ''
        for c in cuts:
            labelsDescription +=  c+':'


        #self.Histograms += [ defineHistogram('egIsEM', 	type='TH1I', title="EFEgammaRec isEM; Cut",
        #                                     xbins=3, xmin=0.5, xmax=3.5, labels=labelsDescription)]
      

class TrigEgammaRecCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEgammaRecCosmicMonitoring"):
        """ defines hisograms for online """
        super(TrigEgammaRecCosmicMonitoring, self).__init__(name)
        self.defineTarget("Cosmic")

        self.Histograms = [defineHistogram('El_Eta', type='TH1F', title="#eta of Clusters; #eta; number of RoIs", xbins=50,xmin=-2.5,xmax=2.5) ]
        self.Histograms += [defineHistogram('El_Phi',type='TH1F', title="#phi of Clusters; #phi; number of RoIs", xbins=64,xmin=-3.2,xmax=3.2) ]
        self.Histograms += [defineHistogram('El_Eta,El_Phi',type='TH2F',title="#eta vs #phi of Clusters; #eta; #phi", xbins=50,xmin=-2.5,xmax=2.5, ybins=64,ymin=-3.2,ymax=3.2) ]                

        self.Histograms += [ defineHistogram('El_ClusterEt', 	type='TH1F', title="EFEgamma Hypo Cluster E_{T}; E_{T}^{em} [MeV]",
                                            xbins=50, xmin=-2000, xmax=100000) ]
     
        self.Histograms += [ defineHistogram('El_F1', 	type='TH1F', title="EFEgamma Hypo fraction of energy found in 1st em sampling;Fraction",
                                             xbins=50, xmin=-0.1, xmax=1.1 ) ]

        self.Histograms += [ defineHistogram('El_E237', 	type='TH1F', title="EFEgamma Hypo uncor energy in 3x7 cells in em sampling 2; E [MeV]",
                                             xbins=50, xmin=-15000, xmax=150000 ) ]

        self.Histograms += [ defineHistogram('El_E277', 	type='TH1F', title="EFEgamma Hypo uncor energy in 7x7 cells in em sampling 2;E [MeV]",
                                             xbins=50, xmin=-15000, xmax=150000 ) ]

        self.Histograms += [ defineHistogram('El_EtHad1', 	type='TH1F', title="EFEgamma Hypo E leakage into 1st sampling of had calo; E [MeV]",
                                             xbins=50, xmin=-15000, xmax=150000 ) ]

        self.Histograms += [ defineHistogram('El_WEta1', 	type='TH1F', title="EFEgamma Hypo corrected width in 3 strips in the 1st samp. ; Width",
                                             xbins=20, xmin=0, xmax=1 ) ]

        self.Histograms += [ defineHistogram('El_WEta2', 	type='TH1F', title="EFEgamma  Hypo corrected width in 3 strips in the 2st samp. ; Width",
                                             xbins=20, xmin=0, xmax=0.05 ) ]

        self.Histograms += [ defineHistogram('El_Emax2', 	type='TH1F', title="EFEgamma Hypo 2nd maximum in strips; E [MeV]",
                                             xbins=50, xmin=-1000, xmax=5000 ) ]

        self.Histograms += [ defineHistogram('El_Emins1', 	type='TH1F', title="EFEgamma Hypo energy of strip with minimum between max 1 & 2; E [MeV]",
                                             xbins=50, xmin=-1000, xmax=5000 ) ]

        self.Histograms += [ defineHistogram('El_Fracs1', 	type='TH1F', title="EFEgamma Hypo energy outside core (E(+-7)-E(+-3))/E(+-7); E [MeV]",
                                             xbins=50, xmin=-5000, xmax=10000 ) ]

        self.Histograms += [ defineHistogram('El_wtots1', 	type='TH1F', title="EFEgamma Hypo total width in em sampling 1 in 20 strips; Width",
                                             xbins=20, xmin=0, xmax=20 ) ]

        self.Histograms += [ defineHistogram('El_EtCone20', 	type='TH1F', title="EFEgamma Hypo fraction of Et in a ring of 0.1<DR<0.20 above noise to total Et; Fraction",
                                             xbins=50, xmin=-1, xmax=3 ) ]
        self.Histograms += [ defineHistogram('El_PtCone20', 	type='TH1F', title="EFEgamma Hypo fraction of Et in a ring of 0.1<DR<0.20 above noise to total Et; Fraction",
                                             xbins=50, xmin=-1, xmax=3 ) ]

        self.Histograms += [ defineHistogram('TrackPt', 	type='TH1F', title="EFEgamma Hypo Track E_{T}; E_{T}^{track}} [MeV]",
                                            xbins=50, xmin=-2000, xmax=100000) ]    

        self.Histograms += [ defineHistogram('dEta', 	type='TH1F', title="EFEgamma Hypo; #Delta#eta  between track and cluster in first layer; #Delta#eta",
                                             xbins=100, xmin=-0.1, xmax=0.1) ]

        self.Histograms += [ defineHistogram('dPhi', 	type='TH1F', title="EFEgamma Hypo #Delta #phi between track and cluster in second layer; #Delta#phi",
                                             xbins=100, xmin=-0.150, xmax=0.150) ]
        self.Histograms += [ defineHistogram('rTRT', 	type='TH1F', title="EFEgamma Hypo ratio of TRT high threshold hits; rTRT",
                                             xbins=30, xmin=-0.1, xmax=1.1 ) ]
        self.Histograms += [ defineHistogram('nBLayerHits', 	type='TH1F', title="EFEgamma Hypo number of B layer hits;Number of hits",
                                             xbins=4, xmin=-0.5, xmax=3.5 ) ]

        self.Histograms += [ defineHistogram('nPixelHits', 	type='TH1F', title="EFEgamma Hypo number of pixel hits; Number of hits",
                                             xbins=11, xmin=-0.5, xmax=10.5 ) ]

        self.Histograms += [ defineHistogram('nSCTHits', 	type='TH1F', title="EFEgamma Hypo number of SCT hits; Number of hits",
                                             xbins=26, xmin=-0.5, xmax=25.5 ) ]

        self.Histograms += [ defineHistogram('nTRTHits', 	type='TH1F', title="EFEgamma Hypo number of TRT hits; Number of hits",
                                             xbins=46, xmin=-0.5, xmax=45.5 ) ]

        self.Histograms += [ defineHistogram('nTRTHitsHighTh', 	type='TH1F', title="EFEgamma Hypo number of high threshold TRT hits; Number of hits",
                                              xbins=26, xmin=-0.5, xmax=25.5 ) ]

        self.Histograms += [ defineHistogram('nTRTHitsHighThOutliers', 	type='TH1F', title="EFEgamma Hypo number of outliers high threshold TRT hits; Number of hits",
                                              xbins=26, xmin=-0.5, xmax=25.5 ) ]

        self.Histograms += [ defineHistogram('nTRTHitsOutliers', 	type='TH1F', title="EFEgamma Hypo number of outliers TRT hits; Number of hits",
                                             xbins=26, xmin=-0.5, xmax=25.5 ) ]

        # Photon Collection
        self.Histograms += [ defineHistogram('Ph_e237', 	type='TH1F', title="EF Photon Hypo uncor energy in 3x7 cells in em sampling 2; E [MeV]",
                                             xbins=50, xmin=-15000, xmax=150000 ) ]

        self.Histograms += [ defineHistogram('Ph_e277', 	type='TH1F', title="EF Photon Hypo uncor energy in 7x7 cells in em sampling 2;E [MeV]",
                                             xbins=50, xmin=-15000, xmax=150000 ) ]

        self.Histograms += [ defineHistogram('Ph_ethad1', 	type='TH1F', title="EF Photon Hypo E leakage into 1st sampling of had calo; E [MeV]",
                                             xbins=50, xmin=-15000, xmax=150000 ) ]

        self.Histograms += [ defineHistogram('Ph_weta1', 	type='TH1F', title="EF Photon Hypo corrected width in 3 strips in the 1st samp. ; Width",
                                             xbins=20, xmin=0, xmax=1 ) ]

        self.Histograms += [ defineHistogram('Ph_weta2', 	type='TH1F', title="EF Photon  Hypo corrected width in 3 strips in the 2st samp. ; Width",
                                             xbins=20, xmin=0, xmax=0.05 ) ]

        self.Histograms += [ defineHistogram('Ph_Emax2', 	type='TH1F', title="EF Photon Hypo 2nd maximum in strips; E [MeV]",
                                             xbins=50, xmin=-1000, xmax=5000 ) ]

        self.Histograms += [ defineHistogram('Ph_Emins1', 	type='TH1F', title="EF Photon Hypo energy of strip with minimum between max 1 & 2; E [MeV]",
                                             xbins=50, xmin=-1000, xmax=5000 ) ]

        self.Histograms += [ defineHistogram('Ph_fracs1', 	type='TH1F', title="EF Photon Hypo energy outside core (E(+-7)-E(+-3))/E(+-7); E [MeV]",
                                             xbins=50, xmin=-5000, xmax=10000 ) ]

        self.Histograms += [ defineHistogram('Ph_wtots1', 	type='TH1F', title="EF Photon Hypo total width in em sampling 1 in 20 strips; Width",
                                             xbins=20, xmin=0, xmax=20 ) ]


        cuts=["loose","medium","tight"]


        labelsDescription = ''
        for c in cuts:
            labelsDescription +=  c+':'


        #self.Histograms += [ defineHistogram('egIsEM', 	type='TH1I', title="EFEgammaRec isEM; Cut",
        #                                     xbins=3, xmin=0.5, xmax=3.5, labels=labelsDescription)]
