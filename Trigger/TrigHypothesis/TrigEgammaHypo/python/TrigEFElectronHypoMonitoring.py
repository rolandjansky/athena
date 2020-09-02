# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TrigEFElectronHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFElectronHypoValidationMonitoring"):
        super(TrigEFElectronHypoValidationMonitoring, self).__init__(name)
        self.defineTarget( "Validation" )

        
        cuts=['Input','PID','cluster E_{T}',
              'Abs. Etcone','Abs. Ptcone','Rel. Etcone','Rel. Ptcone','Passed',
              ]

        labelsDescription = ''
        for c in cuts:
            labelsDescription +=  c+':'

        self.Histograms = [ defineHistogram('CutCounter', 	type='TH1I', title="EFElectron Hypo Number of passed cuts; cut",
                                             xbins=8, xmin=-1.5, xmax=7.5, opt="kCumulative", labels=labelsDescription) ]


        #
        #----BitDefElectron for Offline 32-Bit isEM
        #
        cuts=['ClusterEtaRange ','ConversionMatch','ClusterHadronicLeakage ','ClusterMiddleEnergy ',
              'ClusterMiddleEratio37 ','ClusterMiddleEratio33 ','ClusterMiddleWidth ','f3',
              'ClusterStripsEratio ','ClusterStripsDeltaEmax2 ','ClusterStripsDeltaE ',
              'ClusterStripsWtot ','ClusterStripsFracm ','ClusterStripsWeta1c ','empty14',
              'ClusterStripsDEmaxs1 ','TrackBlayer ','TrackPixel ','TrackSi ',
              'TrackA0 ','TrackMatchEta ','TrackMatchPhi ','TrackMatchEoverP ','empty23',
              'TrackTRThits ','TrackTRTratio ','TrackTRTratio90 ','TrackA0Tight','TrackMatchEtaTight',
              'Isolation ','ClusterIsolation ','TrackIsolation ']
        labelsDescription = ''
        for c in cuts:
            labelsDescription +=  c+':'


        #isEMTrig Monitoring Histograms - Before/After Offline isEM cuts are applied        
        self.Histograms += [ defineHistogram('IsEMAfterCut', type='TH1I', title="EFElectron Hypo passed cuts from isEM flag; cut",
                                             xbins=32, xmin=-0.5, xmax=31.5, opt="kVec", labels=labelsDescription) ]
        self.Histograms += [ defineHistogram('IsEMBeforeCut', type='TH1I', title="EFElectron Hypo passed cuts from isEM flag; cut",
                                             xbins=32, xmin=-0.5, xmax=31.5, opt="kVec", labels=labelsDescription) ]        
        self.Histograms += [ defineHistogram('IsEMRequiredBitsBeforeCut', type='TH1I', title="Required Bits for isEM flag Before Cut; cut",
                                             xbins=32, xmin=-0.5, xmax=31.5, opt="kVec", labels=labelsDescription) ]
        self.Histograms += [ defineHistogram('IsEMRequiredBitsAfterCut', type='TH1I', title="Required Bits for isEM flag After Cut; cut",
                                             xbins=32, xmin=-0.5, xmax=31.5, opt="kVec", labels=labelsDescription) ]  
        #----------------------------------------------------------



        cuts=["loose","medium","tight"]

        labelsDescription = ''
        for c in cuts:
            labelsDescription +=  c+':'

        self.Histograms += [ defineHistogram('El_ClusterEt', 	type='TH1F', title="EFElectron Hypo Cluster E_{T}; E_{T}^{em} [MeV]",
                                            xbins=50, xmin=-2000, xmax=100000) ]
     
        self.Histograms += [ defineHistogram('El_F1', 	type='TH1F', title="EFElectron Hypo fraction of energy found in 1st em sampling;Fraction",
                                             xbins=50, xmin=-0.1, xmax=1.1 ) ]
        
        self.Histograms += [ defineHistogram('El_F3',	 type='TH1F', title="EFElectron Hypo Hypo F3; E3/(E0+E1+E2+E3)",xbins=96, xmin=-0.1, xmax=1.1) ]       

        self.Histograms += [ defineHistogram('El_E237', 	type='TH1F', title="EFElectron Hypo uncor energy in 3x7 cells in em sampling 2; E [MeV]",
                                             xbins=50, xmin=-15000, xmax=150000 ) ]

        self.Histograms += [ defineHistogram('El_E277', 	type='TH1F', title="EFElectron Hypo uncor energy in 7x7 cells in em sampling 2;E [MeV]",
                                             xbins=50, xmin=-15000, xmax=150000 ) ]
                                             
        self.Histograms += [ defineHistogram('El_Reta', 	type='TH1F', title="EFElectron Hypo Reta = E237/E277; Reta",
                                             xbins=130, xmin=-5.0, xmax=1.5 ) ]
        
        self.Histograms += [ defineHistogram('El_Rphi', 	type='TH1F', title="EFElectron Hypo Rphi = E233/E277; Rphi",
                                             xbins=130, xmin=-5.0, xmax=1.5 ) ]
                                             
        self.Histograms += [ defineHistogram('El_Rhad', 	type='TH1F', title="EFElectron Hypo Rhad = Ethad/et; Rhad",
                                             xbins=130, xmin=-0.25, xmax=0.25 ) ]
        
        self.Histograms += [ defineHistogram('El_Rhad1', 	type='TH1F', title="EFElectron Hypo Rhad1 = Ethad1/et; Rhad1",
                                             xbins=130, xmin=-0.25, xmax=0.25 ) ]
        
        
        self.Histograms += [ defineHistogram('El_F3core',	 type='TH1F', title="EFElectron Hypo F3; E3(3x3)/(E0+E1+E2+E3)",xbins=96, xmin=-0.1, xmax=1.1) ]       
        
        self.Histograms += [ defineHistogram('El_DeltaE',	 type='TH1F', title="EFElectron Hypo F3; e2tsts1-emins1 ",xbins=96, xmin=-0.1, xmax=1.1) ]       
        
        self.Histograms += [ defineHistogram('El_Eta',	 type='TH1F', title="EFElectron Hypo Eta; #eta ",xbins=100, xmin=-2.5, xmax=2.5) ]       

        self.Histograms += [ defineHistogram('El_Phi',	 type='TH1F', title="EFElectron Hypo Phi; #eta ",xbins=160, xmin=-3.2, xmax=3.2) ]       
        
        self.Histograms += [ defineHistogram('El_ClusterEt37', 	type='TH1F', title="EFElectron Hypo Cluster(3x7) E_{T}; E_{T}^{em} [MeV]",
                                            xbins=50, xmin=-2000, xmax=100000) ]
        
        self.Histograms += [ defineHistogram('El_EnergyBE0', 	type='TH1F', title="EFElectron Hypo Cluster E0; E_{em} [MeV]",
                                            xbins=50, xmin=-2000, xmax=100000) ]
        
        self.Histograms += [ defineHistogram('El_EnergyBE1', 	type='TH1F', title="EFElectron Hypo Cluster E1; E_{em} [MeV]",
                                            xbins=50, xmin=-2000, xmax=100000) ]
        
        self.Histograms += [ defineHistogram('El_EnergyBE2', 	type='TH1F', title="EFElectron Hypo Cluster E2; E_{em} [MeV]",
                                            xbins=50, xmin=-2000, xmax=100000) ]
        
        self.Histograms += [ defineHistogram('El_EnergyBE3', 	type='TH1F', title="EFElectron Hypo Cluster E3; E_{em} [MeV]",
                                            xbins=50, xmin=-2000, xmax=100000) ]
        
        self.Histograms += [ defineHistogram('El_Eaccordion', 	type='TH1F', title="EFElectron Hypo Cluster (E1+E2+E3); E_{T}^{em} [MeV]",
                                            xbins=50, xmin=-2000, xmax=100000) ]
        
        self.Histograms += [ defineHistogram('El_E0Eaccordion', 	type='TH1F', title="EFElectron Hypo Cluster E0/(E1+E2+E3); E_{T}^{em} [MeV]",
                                            xbins=50, xmin=-2000, xmax=100000) ]
        self.Histograms += [ defineHistogram('d0',	 type='TH1F', title="EFElectron Hypo d0; Impact Parameter",xbins=100, xmin=-1.0, xmax=1.0) ]        

        
        self.Histograms += [ defineHistogram('SigmaD0',	 type='TH1F', title="EFElectron Hypo #sigma(d0); #sigma(Impact Parameter) ",xbins=100, xmin=-1.0, xmax=1.0) ]        
        
        self.Histograms += [ defineHistogram('z0',	 type='TH1F', title="EFElectron Hypo z0; z0",xbins=100, xmin=-1.0, xmax=1.0) ]        
        
        
        self.Histograms += [ defineHistogram('dPhiRescaled', 	type='TH1F', title="EFElectron Hypo rescaled #Delta #phi between track and cluster in second layer; #Delta#phi",
                                             xbins=100, xmin=-0.150, xmax=0.150) ]
        
        self.Histograms += [ defineHistogram('El_Eratio', 	type='TH1F', title="EFElectron Hypo Eratio = (emax1 - emax2) / (emax1 + emax2) ; Eratio ",
                                             xbins=32, xmin=-0.1, xmax=1.5 ) ]

        self.Histograms += [ defineHistogram('El_EtHad1', 	type='TH1F', title="EFElectron Hypo E leakage into 1st sampling of had calo; E [MeV]",
                                             xbins=50, xmin=-15000, xmax=150000 ) ]

        self.Histograms += [ defineHistogram('El_WEta1', 	type='TH1F', title="EFElectron Hypo corrected width in 3 strips in the 1st samp. ; WEta1",
                                             xbins=20, xmin=0, xmax=1 ) ]

        self.Histograms += [ defineHistogram('El_WEta2', 	type='TH1F', title="EFElectron  Hypo corrected width in 3 strips in the 2st samp. ; WEta2",
                                             xbins=20, xmin=0, xmax=0.05 ) ]

        self.Histograms += [ defineHistogram('El_Emax2', 	type='TH1F', title="EFElectron Hypo 2nd maximum in strips; E [MeV]",
                                             xbins=50, xmin=-1000, xmax=5000 ) ]

        self.Histograms += [ defineHistogram('El_Emins1', 	type='TH1F', title="EFElectron Hypo energy of strip with minimum between max 1 & 2; E [MeV]",
                                             xbins=50, xmin=-1000, xmax=5000 ) ]

        self.Histograms += [ defineHistogram('El_Fracs1', 	type='TH1F', title="EFElectron Hypo energy outside core (E(+-7)-E(+-3))/E(+-7); E [MeV]",
                                             xbins=50, xmin=-5000, xmax=10000 ) ]

        self.Histograms += [ defineHistogram('El_wtots1', 	type='TH1F', title="EFElectron Hypo total width in em sampling 1 in 20 strips; WTots1",
                                             xbins=20, xmin=0, xmax=20 ) ]

        self.Histograms += [ defineHistogram('El_EtCone20', 	type='TH1F', title="EFElectron Hypo Et in a ring of DR<0.20 above noise (excluding electron ET); ET [MeV]",
                                             xbins=60, xmin=-10000, xmax=50000 ) ]

        self.Histograms += [ defineHistogram('El_PtCone20', 	type='TH1F', title="EFElectron Hypo Pt in a ring of DR<0.20 above noise (excluding electron PT); PT [MeV]",
                                             xbins=60, xmin=-10000, xmax=50000 ) ] 

        
        self.Histograms += [ defineHistogram('TrackPt', 	type='TH1F', title="EFElectron Hypo Track E_{T}; E_{T}^{track}} [MeV]",
                                            xbins=50, xmin=-2000, xmax=100000) ]
        self.Histograms += [ defineHistogram('dEta', 	type='TH1F', title="EFElectron Hypo #Delta #eta between track and cluster in first layer; #Delta#eta",
                                             xbins=100, xmin=-0.100, xmax=0.100) ]
        self.Histograms += [ defineHistogram('dPhi', 	type='TH1F', title="EFElectron Hypo #Delta #phi between track and cluster in second layer; #Delta#phi",
                                             xbins=100, xmin=-0.150, xmax=0.150) ]
        self.Histograms += [ defineHistogram('rTRT', 	type='TH1F', title="EFElectron Hypo ratio of TRT high threshold hits; rTRT",
                                             xbins=30, xmin=-0.1, xmax=1.1 ) ]
        self.Histograms += [ defineHistogram('nBLayerHits', 	type='TH1F', title="EFElectron Hypo number of B layer hits; Number of B layer hits",
                                             xbins=4, xmin=-0.5, xmax=3.5 ) ]

        self.Histograms += [ defineHistogram('nPixelHits', 	type='TH1F', title="EFElectron Hypo number of pixel hits; Number of pixel hits",
                                             xbins=11, xmin=-0.5, xmax=10.5 ) ]

        self.Histograms += [ defineHistogram('nSCTHits', 	type='TH1F', title="EFElectron Hypo number of SCT hits; Number of SCT hits",
                                             xbins=26, xmin=-0.5, xmax=25.5 ) ]

        self.Histograms += [ defineHistogram('nTRTHits', 	type='TH1F', title="EFElectron Hypo number of TRT hits; Number of TRT hits",
                                             xbins=46, xmin=-0.5, xmax=45.5 ) ]

        self.Histograms += [ defineHistogram('nTRTHitsHighTh', 	type='TH1F', title="EFElectron Hypo number of high threshold TRT hits; Number of TRT HighTh hits",
                                              xbins=26, xmin=-0.5, xmax=25.5 ) ]

        self.Histograms += [ defineHistogram('nTRTHitsHighThOutliers', 	type='TH1F', title="EFElectron Hypo number of outliers high threshold TRT hits; Number of TRT HighTh outlier hits",
                                              xbins=26, xmin=-0.5, xmax=25.5 ) ]

        self.Histograms += [ defineHistogram('nTRTHitsOutliers', 	type='TH1F', title="EFElectron Hypo number of outliers TRT hits; Number of TRT outlier hits",
                                             xbins=26, xmin=-0.5, xmax=25.5 ) ]
                                             

        self.Histograms += [ defineHistogram('mu',	 type='TH1F', title="EFElectron Hypo mu; Pileup",xbins=100, xmin=0., xmax=100.0) ]        

        self.Histograms += [ defineHistogram('LikelihoodRatio',	 type='TH1F', title="EFElectron Hypo Likelihood Ratio; LR",xbins=100, xmin=-1.0, xmax=1.0) ]        
class TrigEFElectronHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFElectronHypoOnlineMonitoring"):
        """ defines histograms for online """
        super(TrigEFElectronHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        cuts=['Input','PID','cluster E_{T}',
              'Abs. Etcone','Abs. Ptcone','Rel. Etcone','Rel. Ptcone','Passed',
              ]

        labelsDescription = ''
        for c in cuts:
            labelsDescription +=  c+':'

        self.Histograms = [ defineHistogram('CutCounter', 	type='TH1I', title="EFElectron Hypo Number of passed cuts; cut",
                                             xbins=8, xmin=-1.5, xmax=7.5, opt="kCumulative", labels=labelsDescription) ]




        #
        #----BitDefElectron for Offline 32-Bit isEM
        #
        cuts=['ClusterEtaRange ','ConversionMatch','ClusterHadronicLeakage ','ClusterMiddleEnergy ',
              'ClusterMiddleEratio37 ','ClusterMiddleEratio33 ','ClusterMiddleWidth ','f3',
              'ClusterStripsEratio ','ClusterStripsDeltaEmax2 ','ClusterStripsDeltaE ',
              'ClusterStripsWtot ','ClusterStripsFracm ','ClusterStripsWeta1c ','empty14',
              'ClusterStripsDEmaxs1 ','TrackBlayer ','TrackPixel ','TrackSi ',
              'TrackA0 ','TrackMatchEta ','TrackMatchPhi ','TrackMatchEoverP ','empty23',
              'TrackTRThits ','TrackTRTratio ','TrackTRTratio90 ','TrackA0Tight','TrackMatchEtaTight',
              'Isolation ','ClusterIsolation ','TrackIsolation ']
        labelsDescription = ''
        for c in cuts:
            labelsDescription +=  c+':'


        #isEMTrig Monitoring Histograms - Before/After Offline isEM cuts are applied        
        self.Histograms += [ defineHistogram('IsEMAfterCut', type='TH1I', title="EFElectron Hypo passed cuts from isEM flag; cut",
                                             xbins=32, xmin=-0.5, xmax=31.5, opt="kVec", labels=labelsDescription) ]

        self.Histograms += [ defineHistogram('IsEMBeforeCut', type='TH1I', title="EFElectron Hypo passed cuts from isEM flag; cut",
                                             xbins=32, xmin=-0.5, xmax=31.5, opt="kVec", labels=labelsDescription) ]

        self.Histograms += [ defineHistogram('IsEMRequiredBitsBeforeCut', type='TH1I', title="Required Bits for isEM flag Before Cut; cut",
                                             xbins=32, xmin=-0.5, xmax=31.5, opt="kVec", labels=labelsDescription) ]

        self.Histograms += [ defineHistogram('IsEMRequiredBitsAfterCut', type='TH1I', title="Required Bits for isEM flag After Cut; cut",
                                             xbins=32, xmin=-0.5, xmax=31.5, opt="kVec", labels=labelsDescription) ]      
        #----------------------------------------------------------




        cuts=["loose","medium","tight"]

        labelsDescription = ''
        for c in cuts:
            labelsDescription +=  c+':'


        self.Histograms += [ defineHistogram('El_ClusterEt', 	type='TH1F', title="EFElectron Hypo Cluster E_{T}; E_{T}^{em} [MeV]",
                                            xbins=50, xmin=-2000, xmax=100000) ]
                                             
        self.Histograms += [ defineHistogram('El_Eta',	 type='TH1F', title="EFElectron Hypo Eta; #eta ",xbins=100, xmin=-2.5, xmax=2.5) ]       

        self.Histograms += [ defineHistogram('El_Phi',	 type='TH1F', title="EFElectron Hypo Phi; #eta ",xbins=160, xmin=-3.2, xmax=3.2) ]       
        
        self.Histograms += [ defineHistogram('d0',	 type='TH1F', title="EFElectron Hypo d0; Impact Parameter",xbins=100, xmin=-1.0, xmax=1.0) ]        

        self.Histograms += [ defineHistogram('SigmaD0',	 type='TH1F', title="EFElectron Hypo #sigma(d0); #sigma(Impact Parameter) ",xbins=100, xmin=-1.0, xmax=1.0) ]        
        
        self.Histograms += [ defineHistogram('dEta', 	type='TH1F', title="EFElectron Hypo #Delta #eta between track and cluster in first layer; #Delta#eta",
                                             xbins=100, xmin=-0.100, xmax=0.100) ]
        self.Histograms += [ defineHistogram('dPhi', 	type='TH1F', title="EFElectron Hypo #Delta #phi between track and cluster in second layer; #Delta#phi",
                                             xbins=100, xmin=-0.150, xmax=0.150) ]

        self.Histograms += [ defineHistogram('mu',	 type='TH1F', title="EFElectron Hypo mu; Pileup",xbins=100, xmin=0., xmax=100.0) ]        

        self.Histograms += [ defineHistogram('LikelihoodRatio',	 type='TH1F', title="EFElectron Hypo Likelihood Ratio; LR",xbins=100, xmin=-1.0, xmax=1.0) ]        
        
        self.Histograms += [ defineHistogram('El_PtCone20', 	type='TH1F', title="EFElectron Hypo Pt in a ring of DR<0.20 above noise (excluding electron PT); PT [MeV]",
                                             xbins=60, xmin=-10000, xmax=50000 ) ] 


class TrigEFElectronHypoCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFElectronHypoCosmicMonitoring"):
        """ defines histograms for cosmic """
        super(TrigEFElectronHypoCosmicMonitoring, self).__init__(name)
        self.defineTarget("Cosmic")

      
        cuts=['Input','PID','cluster E_{T}',
              'Abs. Etcone','Abs. Ptcone','Rel. Etcone','Rel. Ptcone','Passed',
              ]

        labelsDescription = ''
        for c in cuts:
            labelsDescription +=  c+':'

        self.Histograms = [ defineHistogram('CutCounter', 	type='TH1I', title="EFElectron Hypo Number of passed cuts; cut",
                                             xbins=8, xmin=-1.5, xmax=7.5, opt="kCumulative", labels=labelsDescription) ]



        #
        #----BitDefElectron for Offline 32-Bit isEM
        #
        cuts=['ClusterEtaRange ','ConversionMatch','ClusterHadronicLeakage ','ClusterMiddleEnergy ',
              'ClusterMiddleEratio37 ','ClusterMiddleEratio33 ','ClusterMiddleWidth ','f3',
              'ClusterStripsEratio ','ClusterStripsDeltaEmax2 ','ClusterStripsDeltaE ',
              'ClusterStripsWtot ','ClusterStripsFracm ','ClusterStripsWeta1c ','empty14',
              'ClusterStripsDEmaxs1 ','TrackBlayer ','TrackPixel ','TrackSi ',
              'TrackA0 ','TrackMatchEta ','TrackMatchPhi ','TrackMatchEoverP ','empty23',
              'TrackTRThits ','TrackTRTratio ','TrackTRTratio90 ','TrackA0Tight','TrackMatchEtaTight',
              'Isolation ','ClusterIsolation ','TrackIsolation ']
        labelsDescription = ''
        for c in cuts:
            labelsDescription +=  c+':'


        #isEMTrig Monitoring Histograms - Before/After Offline isEM cuts are applied        
        self.Histograms += [ defineHistogram('IsEMAfterCut', type='TH1I', title="EFElectron Hypo passed cuts from isEM flag; cut",
                                             xbins=32, xmin=-0.5, xmax=31.5, opt="kVec", labels=labelsDescription) ]

        self.Histograms += [ defineHistogram('IsEMBeforeCut', type='TH1I', title="EFElectron Hypo passed cuts from isEM flag; cut",
                                             xbins=32, xmin=-0.5, xmax=31.5, opt="kVec", labels=labelsDescription) ]

        self.Histograms += [ defineHistogram('IsEMRequiredBitsBeforeCut', type='TH1I', title="Required Bits for isEM flag Before Cut; cut",
                                             xbins=32, xmin=-0.5, xmax=31.5, opt="kVec", labels=labelsDescription) ]

        self.Histograms += [ defineHistogram('IsEMRequiredBitsAfterCut', type='TH1I', title="Required Bits for isEM flag After Cut; cut",
                                             xbins=32, xmin=-0.5, xmax=31.5, opt="kVec", labels=labelsDescription) ]        
        #----------------------------------------------------------




        cuts=["loose","medium","tight"]

        labelsDescription = ''
        for c in cuts:
            labelsDescription +=  c+':'


        self.Histograms += [ defineHistogram('El_ClusterEt', 	type='TH1F', title="EFElectron Hypo Cluster E_{T}; E_{T}^{em} [MeV]",
                                            xbins=50, xmin=-2000, xmax=100000) ]
     
        self.Histograms += [ defineHistogram('El_F1', 	type='TH1F', title="EFElectron Hypo fraction of energy found in 1st em sampling;Fraction",
                                             xbins=50, xmin=-0.1, xmax=1.1 ) ]

        self.Histograms += [ defineHistogram('El_F3',	 type='TH1F', title="EFElectron Hypo Hypo F3; E3/(E0+E1+E2+E3)",xbins=96, xmin=-0.1, xmax=1.1) ]        
        
        self.Histograms += [ defineHistogram('El_E237', 	type='TH1F', title="EFElectron Hypo uncor energy in 3x7 cells in em sampling 2; E [MeV]",
                                             xbins=50, xmin=-15000, xmax=150000 ) ]

        self.Histograms += [ defineHistogram('El_E277', 	type='TH1F', title="EFElectron Hypo uncor energy in 7x7 cells in em sampling 2;E [MeV]",
                                             xbins=50, xmin=-15000, xmax=150000 ) ]
                                             
        self.Histograms += [ defineHistogram('El_Reta', 	type='TH1F', title="EFElectron Hypo Reta = E237/E277; Reta",
                                             xbins=130, xmin=-5.0, xmax=1.5 ) ]
        
        self.Histograms += [ defineHistogram('El_Rphi', 	type='TH1F', title="EFElectron Hypo Rphi = E233/E277; Rphi",
                                             xbins=130, xmin=-5.0, xmax=1.5 ) ]
                                             
        self.Histograms += [ defineHistogram('El_Rhad', 	type='TH1F', title="EFElectron Hypo Rhad = Ethad/et; Rhad",
                                             xbins=130, xmin=-0.25, xmax=0.25 ) ]
        
        self.Histograms += [ defineHistogram('El_Rhad1', 	type='TH1F', title="EFElectron Hypo Rhad1 = Ethad1/et; Rhad1",
                                             xbins=130, xmin=-0.25, xmax=0.25 ) ]
        
        
        self.Histograms += [ defineHistogram('El_F3core',	 type='TH1F', title="EFElectron Hypo F3; E3(3x3)/(E0+E1+E2+E3)",xbins=96, xmin=-0.1, xmax=1.1) ]       
        
        self.Histograms += [ defineHistogram('El_DeltaE',	 type='TH1F', title="EFElectron Hypo F3; e2tsts1-emins1 ",xbins=96, xmin=-0.1, xmax=1.1) ]       
        
        self.Histograms += [ defineHistogram('El_Eta',	 type='TH1F', title="EFElectron Hypo Eta; #eta ",xbins=100, xmin=-2.5, xmax=2.5) ]       

        self.Histograms += [ defineHistogram('El_Phi',	 type='TH1F', title="EFElectron Hypo Phi; #eta ",xbins=160, xmin=-3.2, xmax=3.2) ]       
        
        self.Histograms += [ defineHistogram('El_ClusterEt37', 	type='TH1F', title="EFElectron Hypo Cluster(3x7) E_{T}; E_{T}^{em} [MeV]",
                                            xbins=50, xmin=-2000, xmax=100000) ]
        
        self.Histograms += [ defineHistogram('El_EnergyBE0', 	type='TH1F', title="EFElectron Hypo Cluster E0; E_{em} [MeV]",
                                            xbins=50, xmin=-2000, xmax=100000) ]
        
        self.Histograms += [ defineHistogram('El_EnergyBE1', 	type='TH1F', title="EFElectron Hypo Cluster E1; E_{em} [MeV]",
                                            xbins=50, xmin=-2000, xmax=100000) ]
        
        self.Histograms += [ defineHistogram('El_EnergyBE2', 	type='TH1F', title="EFElectron Hypo Cluster E2; E_{em} [MeV]",
                                            xbins=50, xmin=-2000, xmax=100000) ]
        
        self.Histograms += [ defineHistogram('El_EnergyBE3', 	type='TH1F', title="EFElectron Hypo Cluster E3; E_{em} [MeV]",
                                            xbins=50, xmin=-2000, xmax=100000) ]
        
        self.Histograms += [ defineHistogram('El_Eaccordion', 	type='TH1F', title="EFElectron Hypo Cluster (E1+E2+E3); E_{T}^{em} [MeV]",
                                            xbins=50, xmin=-2000, xmax=100000) ]
        
        self.Histograms += [ defineHistogram('El_E0Eaccordion', 	type='TH1F', title="EFElectron Hypo Cluster E0/(E1+E2+E3); E_{T}^{em} [MeV]",
                                            xbins=50, xmin=-2000, xmax=100000) ]
        self.Histograms += [ defineHistogram('d0',	 type='TH1F', title="EFElectron Hypo d0; Impact Parameter",xbins=100, xmin=-1.0, xmax=1.0) ]        

        
        self.Histograms += [ defineHistogram('SigmaD0',	 type='TH1F', title="EFElectron Hypo #sigma(d0); #sigma(Impact Parameter) ",xbins=100, xmin=-1.0, xmax=1.0) ]        
        
        self.Histograms += [ defineHistogram('z0',	 type='TH1F', title="EFElectron Hypo z0; z0",xbins=100, xmin=-1.0, xmax=1.0) ]        
        
        
        self.Histograms += [ defineHistogram('dPhiRescaled', 	type='TH1F', title="EFElectron Hypo rescaled #Delta #phi between track and cluster in second layer; #Delta#phi",
                                             xbins=100, xmin=-0.150, xmax=0.150) ]
        self.Histograms += [ defineHistogram('El_Eratio', 	type='TH1F', title="EFElectron Hypo Eratio = (emax1 - emax2) / (emax1 + emax2) ; Eratio ",
                                             xbins=32, xmin=-0.1, xmax=1.5 ) ]

        self.Histograms += [ defineHistogram('El_EtHad1', 	type='TH1F', title="EFElectron Hypo E leakage into 1st sampling of had calo; E [MeV]",
                                             xbins=50, xmin=-15000, xmax=150000 ) ]

        self.Histograms += [ defineHistogram('El_WEta1', 	type='TH1F', title="EFElectron Hypo corrected width in 3 strips in the 1st samp. ; WEta1",
                                             xbins=20, xmin=0, xmax=1 ) ]

        self.Histograms += [ defineHistogram('El_WEta2', 	type='TH1F', title="EFElectron  Hypo corrected width in 3 strips in the 2st samp. ; WEta2",
                                             xbins=20, xmin=0, xmax=0.05 ) ]

        self.Histograms += [ defineHistogram('El_Emax2', 	type='TH1F', title="EFElectron Hypo 2nd maximum in strips; E [MeV]",
                                             xbins=50, xmin=-1000, xmax=5000 ) ]

        self.Histograms += [ defineHistogram('El_Emins1', 	type='TH1F', title="EFElectron Hypo energy of strip with minimum between max 1 & 2; E [MeV]",
                                             xbins=50, xmin=-1000, xmax=5000 ) ]

        self.Histograms += [ defineHistogram('El_Fracs1', 	type='TH1F', title="EFElectron Hypo energy outside core (E(+-7)-E(+-3))/E(+-7); E [MeV]",
                                             xbins=50, xmin=-5000, xmax=10000 ) ]

        self.Histograms += [ defineHistogram('El_wtots1', 	type='TH1F', title="EFElectron Hypo total width in em sampling 1 in 20 strips; WTots1",
                                             xbins=20, xmin=0, xmax=20 ) ]

        self.Histograms += [ defineHistogram('El_EtCone20', 	type='TH1F', title="EFElectron Hypo Et in a ring of DR<0.20 above noise (excluding electron ET); ET [MeV]",
                                             xbins=60, xmin=-10000, xmax=50000 ) ]

        self.Histograms += [ defineHistogram('El_PtCone20', 	type='TH1F', title="EFElectron Hypo Pt in a ring of DR<0.20 above noise (excluding electron PT); PT [MeV]",
                                             xbins=60, xmin=-10000, xmax=50000 ) ] 

        
        self.Histograms += [ defineHistogram('TrackPt', 	type='TH1F', title="EFElectron Hypo Track E_{T}; E_{T}^{track}} [MeV]",
                                            xbins=50, xmin=-2000, xmax=100000) ]
        self.Histograms += [ defineHistogram('dEta', 	type='TH1F', title="EFElectron Hypo #Delta #eta between track and cluster in first layer; #Delta#eta",
                                             xbins=100, xmin=-0.100, xmax=0.100) ]
        self.Histograms += [ defineHistogram('dPhi', 	type='TH1F', title="EFElectron Hypo #Delta #phi between track and cluster in second layer; #Delta#phi",
                                             xbins=100, xmin=-0.150, xmax=0.150) ]
        self.Histograms += [ defineHistogram('rTRT', 	type='TH1F', title="EFElectron Hypo ratio of TRT high threshold hits; rTRT",
                                             xbins=30, xmin=-0.1, xmax=1.1 ) ]
        self.Histograms += [ defineHistogram('nBLayerHits', 	type='TH1F', title="EFElectron Hypo number of B layer hits; Number of B layer hits",
                                             xbins=4, xmin=-0.5, xmax=3.5 ) ]

        self.Histograms += [ defineHistogram('nPixelHits', 	type='TH1F', title="EFElectron Hypo number of pixel hits; Number of pixel hits",
                                             xbins=11, xmin=-0.5, xmax=10.5 ) ]

        self.Histograms += [ defineHistogram('nSCTHits', 	type='TH1F', title="EFElectron Hypo number of SCT hits; Number of SCT hits",
                                             xbins=26, xmin=-0.5, xmax=25.5 ) ]

        self.Histograms += [ defineHistogram('nTRTHits', 	type='TH1F', title="EFElectron Hypo number of TRT hits; Number of TRT hits",
                                             xbins=46, xmin=-0.5, xmax=45.5 ) ]

        self.Histograms += [ defineHistogram('nTRTHitsHighTh', 	type='TH1F', title="EFElectron Hypo number of high threshold TRT hits; Number of TRT HighTh hits",
                                              xbins=26, xmin=-0.5, xmax=25.5 ) ]

        self.Histograms += [ defineHistogram('nTRTHitsHighThOutliers', 	type='TH1F', title="EFElectron Hypo number of outliers high threshold TRT hits; Number of TRT HighTh outlier hits",
                                              xbins=26, xmin=-0.5, xmax=25.5 ) ]

        self.Histograms += [ defineHistogram('nTRTHitsOutliers', 	type='TH1F', title="EFElectron Hypo number of outliers TRT hits; Number of TRT outlier hits",
                                             xbins=26, xmin=-0.5, xmax=25.5 ) ]
                                             

        self.Histograms += [ defineHistogram('mu',	 type='TH1F', title="EFElectron Hypo mu; Pileup",xbins=100, xmin=0., xmax=100.0) ]        

        self.Histograms += [ defineHistogram('LikelihoodRatio',	 type='TH1F', title="EFElectron Hypo Likelihood Ratio; LR",xbins=100, xmin=-1.0, xmax=1.0) ]        
