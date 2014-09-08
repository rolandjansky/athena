# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TrigEFPhotonHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFPhotonHypoValidationMonitoring"):
        super(TrigEFPhotonHypoValidationMonitoring, self).__init__(name)
        self.defineTarget( "Validation" )

        cuts=['Input','has egammaContainer','isEM','isEMTrig', 'has caloCluster', 'Cluster Et', 'has EMShower', 
              'OldSet1','OldSet2','OldSet3']

        labelsDescription = ''
        for c in cuts:
            labelsDescription +=  c+':'

        self.Histograms  = [ defineHistogram('CutCounter', type='TH1F', title="EFPhoton Hypo cut counter; Cut",
                                             xbins=10, xmin=0.5, xmax=10.5, opt="kCumulative", labels=labelsDescription) ]    
        self.Histograms += [ defineHistogram('Et', 	type='TH1F', title="EFPhoton Hypo E_{T}; E_{T}^{em} [MeV]",    xbins=50, xmin=0., xmax=70000.) ]
        self.Histograms += [ defineHistogram('Eta', 	type='TH1F', title="EFPhoton Hypo #eta; #eta",             xbins=50, xmin=-3.5, xmax=3.5) ]
        self.Histograms += [ defineHistogram('Phi', 	type='TH1F', title="EFPhoton Hypo #phi; #phi",             xbins=64, xmin=-3.2, xmax=3.2) ]
        #self.Histograms += [ defineHistogram('rE37E77',	type='TH1F', title="EFPhoton Hypo e237/e277; e237/e277", xbins=50, xmin=0., xmax=2.) ]
        #self.Histograms += [ defineHistogram('rE33E77',	type='TH1F', title="EFPhoton Hypo e233/e277; e233/e277", xbins=50, xmin=0., xmax=2.) ]
        #self.Histograms += [ defineHistogram('Eratio',	type='TH1F', title="EFPhoton Hypo Eratio; Eratio", xbins=40, xmin=-0.1, xmax=1.5) ]


        #----------------------------------------------------------
        #AT Jan2010
        #----BitDef for Trigger-coded 28-Bit isEMTrig 
        cuts=['ClusterEtaRange','ClusterHadronicLeakage','empty2','empty3','ClusterMiddleEnergy','ClusterMiddleEratio37 ',
              'ClusterMiddleEratio33','ClusterMiddleWidth','ClusterStripsEratio','ClusterStripsDeltaEmax2',
              'ClusterStripsDeltaE ','ClusterStripsWtot','ClusterStripsFracm','ClusterStripsWeta1c',
              'ClusterIsolation','ClusterStripsDEmaxs1','TrackBlayer','TrackPixel','TrackSi','TrackA0','TrackMatchEta',
              'TrackMatchPhi','TrackMatchEoverP','empty23','TrackTRThits','TrackTRTratio','TrackTRTratio90', 'TrackIsolation']        
        labelsDescription = ''
        for c in cuts:
            labelsDescription +=  c+':'
 

        #
        #----BitDefElectron for Offline 32-Bit isEM (valid for LowLumi, at HighLumi must use BitDefPhoton)
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

	#AT Aug2011: deactivate histogram egIsEM - outdated
        #self.Histograms += [ defineHistogram('egIsEM', 	type='TH1I', title="EFEgammaHypo isEM; Cut",
        #                                     xbins=3, xmin=0.5, xmax=3.5, labels=labelsDescription)]
       


class TrigEFPhotonHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFPhotonHypoOnlineMonitoring"):
        super(TrigEFPhotonHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        cuts=['Input','has egammaContainer','isEM','isEMTrig', 'has caloCluster', 'Cluster Et', 'has EMShower', 
              'OldSet1','OldSet2','OldSet3']

        labelsDescription = ''
        for c in cuts:
            labelsDescription +=  c+':'
            
        self.Histograms  = [ defineHistogram('CutCounter', type='TH1F', title="EFPhoton Hypo cut counter; Cut",
                                             xbins=10, xmin=0.5, xmax=10.5, opt="kCumulative", labels=labelsDescription) ]    
        self.Histograms += [ defineHistogram('Et', 	type='TH1F', title="EFPhoton Hypo E_{T}; E_{T}^{em} [MeV]",    xbins=50, xmin=0., xmax=70000.) ]
        self.Histograms += [ defineHistogram('Eta', 	type='TH1F', title="EFPhoton Hypo #eta; #eta",             xbins=50, xmin=-3.5, xmax=3.5) ]
        self.Histograms += [ defineHistogram('Phi', 	type='TH1F', title="EFPhoton Hypo #phi; #phi",             xbins=64, xmin=-3.2, xmax=3.2) ]
        #self.Histograms += [ defineHistogram('rE37E77',	type='TH1F', title="EFPhoton Hypo e237/e277; e237/e277", xbins=50, xmin=0., xmax=2.) ]
        #self.Histograms += [ defineHistogram('rE33E77',	type='TH1F', title="EFPhoton Hypo e233/e277; e233/e277", xbins=50, xmin=0., xmax=2.) ]
        #self.Histograms += [ defineHistogram('Eratio',	type='TH1F', title="EFPhoton Hypo Eratio; Eratio", xbins=40, xmin=-0.1, xmax=1.5) ]

        #----------------------------------------------------------
        #AT Jan2010
        #----BitDef for Trigger-coded 28-Bit isEMTrig 
        cuts=['ClusterEtaRange','ClusterHadronicLeakage','empty2','empty3','ClusterMiddleEnergy','ClusterMiddleEratio37 ',
              'ClusterMiddleEratio33','ClusterMiddleWidth','ClusterStripsEratio','ClusterStripsDeltaEmax2',
              'ClusterStripsDeltaE ','ClusterStripsWtot','ClusterStripsFracm','ClusterStripsWeta1c',
              'ClusterIsolation','ClusterStripsDEmaxs1','TrackBlayer','TrackPixel','TrackSi','TrackA0','TrackMatchEta',
              'TrackMatchPhi','TrackMatchEoverP','empty23','TrackTRThits','TrackTRTratio','TrackTRTratio90', 'TrackIsolation']        
        labelsDescription = ''
        for c in cuts:
            labelsDescription +=  c+':'


        #
        #----BitDefElectron for Offline 32-Bit isEM (valid for LowLumi, at HighLumi must use BitDefPhoton)
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

	#AT Aug2011: deactivate histogram egIsEM - outdated
        #self.Histograms += [ defineHistogram('egIsEM', 	type='TH1I', title="EFEgammaHypo isEM; Cut",
        #                                     xbins=3, xmin=0.5, xmax=3.5, labels=labelsDescription)]


class TrigEFPhotonHypoCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFPhotonHypoCosmicMonitoring"):
        super(TrigEFPhotonHypoCosmicMonitoring, self).__init__(name)
        self.defineTarget("Cosmic")

        cuts=['Input','has egammaContainer','isEM','isEMTrig', 'has caloCluster', 'Cluster Et', 'has EMShower', 
              'OldSet1','OldSet2','OldSet3']

        labelsDescription = ''
        for c in cuts:
            labelsDescription +=  c+':'
            
       
        self.Histograms  = [ defineHistogram('CutCounter', type='TH1F', title="EFPhoton Hypo cut counter; Cut",
                                             xbins=10, xmin=0.5, xmax=10.5, opt="kCumulative", labels=labelsDescription) ] 
        self.Histograms += [ defineHistogram('Et', 	type='TH1F', title="EFPhoton Hypo E_{T}; E_{T}^{em} [MeV]",    xbins=50, xmin=0., xmax=70000.) ]
        self.Histograms += [ defineHistogram('Eta', 	type='TH1F', title="EFPhoton Hypo #eta; #eta",             xbins=50, xmin=-3.5, xmax=3.5) ]
        self.Histograms += [ defineHistogram('Phi', 	type='TH1F', title="EFPhoton Hypo #phi; #phi",             xbins=64, xmin=-3.2, xmax=3.2) ]
        #self.Histograms += [ defineHistogram('rE37E77',	type='TH1F', title="EFPhoton Hypo e237/e277; e237/e277", xbins=50, xmin=0., xmax=2.) ]
        #self.Histograms += [ defineHistogram('rE33E77',	type='TH1F', title="EFPhoton Hypo e233/e277; e233/e277", xbins=50, xmin=0., xmax=2.) ]
        #self.Histograms += [ defineHistogram('Eratio',	type='TH1F', title="EFPhoton Hypo Eratio; Eratio", xbins=50, xmin=-0.1, xmax=1.5) ]


        #----------------------------------------------------------
        #AT Jan2010
        #----BitDef for Trigger-coded 28-Bit isEMTrig 
        cuts=['ClusterEtaRange','ClusterHadronicLeakage','empty2','empty3','ClusterMiddleEnergy','ClusterMiddleEratio37 ',
              'ClusterMiddleEratio33','ClusterMiddleWidth','ClusterStripsEratio','ClusterStripsDeltaEmax2',
              'ClusterStripsDeltaE ','ClusterStripsWtot','ClusterStripsFracm','ClusterStripsWeta1c',
              'ClusterIsolation','ClusterStripsDEmaxs1','TrackBlayer','TrackPixel','TrackSi','TrackA0','TrackMatchEta',
              'TrackMatchPhi','TrackMatchEoverP','empty23','TrackTRThits','TrackTRTratio','TrackTRTratio90', 'TrackIsolation']        
        labelsDescription = ''
        for c in cuts:
            labelsDescription +=  c+':'


        #
        #----BitDefElectron for Offline 32-Bit isEM (valid for LowLumi, at HighLumi must use BitDefPhoton)
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

	#AT Aug2011: deactivate histogram egIsEM - outdated
        #self.Histograms += [ defineHistogram('egIsEM', 	type='TH1I', title="EFEgammaHypo isEM; Cut",
        #                                     xbins=3, xmin=0.5, xmax=3.5, labels=labelsDescription)]
       
