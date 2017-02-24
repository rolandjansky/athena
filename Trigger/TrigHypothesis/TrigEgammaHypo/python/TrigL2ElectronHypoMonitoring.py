# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TrigL2ElectronHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2ElectronHypoOnlineMonitoring"):
        super(TrigL2ElectronHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget( "Online" )

        cuts=['Input','has TrigElectronContainer', 'p_{T} calo cut', '#Delta #eta tk-cl', '#Delta #phi tk-cl','E/p low','E/p hi','TRT ratio']

        labelsDescription = ''
        for c in cuts:
            labelsDescription +=  c+':'
        
        self.Histograms += [ defineHistogram('CutCounter', type='TH1I', title="L2Electron Hypo Cut Counter;Cut Counter", xbins=8, xmin=-1.5, xmax=7.5, opt="kCumulative", labels=labelsDescription) ]
        self.Histograms += [ defineHistogram('CaloTrackdEta', type='TH1F', title="L2Electron Hypo #Delta #eta between cluster and track;#Delta #eta;Nevents", xbins=80, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('CaloTrackdPhi', type='TH1F', title="L2Electron Hypo #Delta #phi between cluster and track;#Delta #phi;Nevents", xbins=80, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('CaloTrackEoverP', type='TH1F', title="L2Electron Hypo E/p;E/p;Nevents", xbins=120, xmin=0, xmax=12) ]
        self.Histograms += [ defineHistogram('PtTrack', type='TH1F', title="L2Electron Hypo p_{T}^{track} [MeV];p_{T}^{track} [MeV];Nevents", xbins=50, xmin=0, xmax=100000) ]
        self.Histograms += [ defineHistogram('PtCalo', type='TH1F', title="L2Electron Hypo p_{T}^{calo} [MeV];p_{T}^{calo} [MeV];Nevents", xbins=50, xmin=0, xmax=100000) ]
        self.Histograms += [ defineHistogram('CaloEta', type='TH1F', title="L2Electron Hypo #eta^{calo} ; #eta^{calo};Nevents", xbins=200, xmin=-2.5, xmax=2.5) ]
        self.Histograms += [ defineHistogram('CaloPhi', type='TH1F', title="L2Electron Hypo #phi^{calo} ; #phi^{calo};Nevents", xbins=320, xmin=-3.2, xmax=3.2) ]


##########   Create instance for validation  - same as online here
class TrigL2ElectronHypoValidationMonitoring(TrigL2ElectronHypoOnlineMonitoring):
    def __init__ (self, name="TrigL2ElectronHypoValidationMonitoring"):
        super(TrigL2ElectronHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Validation")

        cuts=['Input','has TrigElectronContainer', 'p_{T} calo cut', '#Delta #eta tk-cl', '#Delta #phi tk-cl','E/p low','E/p hi','TRT ratio']
        labelsDescription = ''
        for c in cuts:
            labelsDescription +=  c+':'

        self.Histograms += [ defineHistogram('CutCounter', type='TH1I', title="L2Electron Hypo Cut Counter;Cut Counter", xbins=8, xmin=-1.5, xmax=7.5, opt="kCumulative", labels=labelsDescription) ]
        self.Histograms += [ defineHistogram('CaloTrackdEta', type='TH1F', title="L2Electron Hypo #Delta #eta between cluster and track;#Delta #eta;Nevents", xbins=80, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('CaloTrackdPhi', type='TH1F', title="L2Electron Hypo #Delta #phi between cluster and track;#Delta #phi;Nevents", xbins=80, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('CaloTrackEoverP', type='TH1F', title="L2Electron Hypo E/p;E/p;Nevents", xbins=120, xmin=0, xmax=12) ]
        self.Histograms += [ defineHistogram('PtTrack', type='TH1F', title="L2Electron Hypo p_{T}^{track} [MeV];p_{T}^{track} [MeV];Nevents", xbins=50, xmin=0, xmax=100000) ]
        self.Histograms += [ defineHistogram('PtCalo', type='TH1F', title="L2Electron Hypo p_{T}^{calo} [MeV];p_{T}^{calo} [MeV];Nevents", xbins=50, xmin=0, xmax=100000) ]
        self.Histograms += [ defineHistogram('CaloEta', type='TH1F', title="L2Electron Hypo #eta^{calo} ; #eta^{calo};Nevents", xbins=200, xmin=-2.5, xmax=2.5) ]
        self.Histograms += [ defineHistogram('CaloPhi', type='TH1F', title="L2Electron Hypo #phi^{calo} ; #phi^{calo};Nevents", xbins=320, xmin=-3.2, xmax=3.2) ]



class TrigL2ElectronFexOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2ElectronFexOnlineMonitoring"):
        super(TrigL2ElectronFexOnlineMonitoring, self).__init__(name)
        self.defineTarget( "Online" )
        self.Histograms += [ defineHistogram('CaloTrackdEtaNoExtrapMon', type='TH1F', title="L2Electron Fex #Delta #eta between cluster and track;#Delta #eta;Nevents", xbins=80, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('CaloTrackdEta', type='TH1F', title="L2Electron Fex #Delta #eta between cluster and track;#Delta #eta;Nevents", xbins=80, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('CaloTrackdPhi', type='TH1F', title="L2Electron Fex #Delta #phi between cluster and track;#Delta #phi;Nevents", xbins=80, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('CaloTrackEoverP', type='TH1F', title="L2Electron Fex E/p;E/p;Nevents", xbins=120, xmin=0, xmax=12) ]
        self.Histograms += [ defineHistogram('PtTrack', type='TH1F', title="L2Electron Fex p_{T}^{track} [MeV];p_{T}^{track} [MeV];Nevents", xbins=50, xmin=0, xmax=100000) ]
        self.Histograms += [ defineHistogram('PtCalo', type='TH1F', title="L2Electron Fex p_{T}^{calo} [MeV];p_{T}^{calo} [MeV];Nevents", xbins=50, xmin=0, xmax=100000) ]


class TrigL2ElectronFexValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2ElectronFexValidationMonitoring"):
        super(TrigL2ElectronFexValidationMonitoring, self).__init__(name)
        self.defineTarget( "Validation" )
        self.Histograms += [ defineHistogram('CaloTrackdEtaNoExtrapMon', type='TH1F', title="L2Electron Fex #Delta #eta between cluster and track;#Delta #eta;Nevents", xbins=80, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('CaloTrackdEta', type='TH1F', title="L2Electron Fex #Delta #eta between cluster and track;#Delta #eta;Nevents", xbins=80, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('CaloTrackdPhi', type='TH1F', title="L2Electron Fex #Delta #phi between cluster and track;#Delta #phi;Nevents", xbins=80, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('CaloTrackEoverP', type='TH1F', title="L2Electron Fex E/p;E/p;Nevents", xbins=120, xmin=0, xmax=12) ]
        self.Histograms += [ defineHistogram('PtTrack', type='TH1F', title="L2Electron Fex p_{T}^{track} [MeV];p_{T}^{track} [MeV];Nevents", xbins=50, xmin=0, xmax=100000) ]
        self.Histograms += [ defineHistogram('PtCalo', type='TH1F', title="L2Electron Fex p_{T}^{calo} [MeV];p_{T}^{calo} [MeV];Nevents", xbins=50, xmin=0, xmax=100000) ]

