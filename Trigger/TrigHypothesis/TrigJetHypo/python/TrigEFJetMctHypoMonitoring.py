# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 


class TrigEFJetMctHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFJetMctHypoValidationMonitoring"):
        super(TrigEFJetMctHypoValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")

        self.Histograms += [ defineHistogram('passedJetCuts_maxmct', type='TH1F', title="EFJetMct MAX MCT of jets after jet cuts", xbins=200, xmin=0, xmax=1000) ]
        self.Histograms += [ defineHistogram('passedJetCuts_mct', type='TH1F', title="EFJetMct MCT of jets after jet cuts", xbins=200, xmin=0, xmax=1000) ]
        self.Histograms += [ defineHistogram('passedJetCuts_dphi',type='TH1F', title="EFJetMct DeltaPhi of jets after jet cuts", xbins=32, xmin=0., xmax=3.2) ]
        self.Histograms += [ defineHistogram('passedJetCuts_et1',type='TH1F', title="EFJetMct jet1 Et after jet cuts", xbins=198, xmin=10, xmax=1000) ]
        self.Histograms += [ defineHistogram('passedJetCuts_et2',type='TH1F', title="EFJetMct jet2 Et after jet cuts", xbins=198, xmin=10, xmax=1000) ]
        self.Histograms += [ defineHistogram('passedJetCuts_eta1',type='TH1F', title="EFJetMct jet1 eta after jet cuts", xbins=80, xmin=-4, xmax=4) ]
        self.Histograms += [ defineHistogram('passedJetCuts_eta2',type='TH1F', title="EFJetMct jet2 eta after jet cuts", xbins=80, xmin=-4, xmax=4) ]
        self.Histograms += [ defineHistogram('passedJetCuts_phi1',type='TH1F', title="EFJetMct jet1 phi after jet cuts", xbins=64, xmin=-3.2, xmax=3.2) ]
        self.Histograms += [ defineHistogram('passedJetCuts_phi2',type='TH1F', title="EFJetMct jet2 phi after jet cuts", xbins=64, xmin=-3.2, xmax=3.2) ]

                                                                                                         
        self.Histograms += [ defineHistogram('passedMctCut_mct', type='TH1F', title="EFJetMct MCT of jets after Mct cut", xbins=200, xmin=0, xmax=1000) ]
        self.Histograms += [ defineHistogram('passedMctCut_maxmct', type='TH1F', title="EFJetMct MAX MCT of jets after Mct cut", xbins=200, xmin=0, xmax=1000) ]
        self.Histograms += [ defineHistogram('passedMctCut_dphi',type='TH1F', title="EFJetMct DeltaPhi of jets after Mct cut", xbins=64, xmin=0., xmax=3.2) ]
                                                                                                         
        self.Histograms += [ defineHistogram('nJet',type='TH1F', title="EFJetMct no. of jets considered", xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('nComb',type='TH1F', title="EFJetMct no. of jet pair combinations considered", xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('passednComb',type='TH1F', title="EFJetMct no. of jet pair combinations passing Mct cut", xbins=50, xmin=0, xmax=50) ]

        self.Histograms += [ defineHistogram('cutCounter',type='TH1F', title="EFJetMct Cut Counter", xbins=10, xmin=0, xmax=10) ]
        

class TrigEFJetMctHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFJetMctHypoOnlineMonitoring"):
        """ defines histograms for online """
        super(TrigEFJetMctHypoOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")

        self.Histograms += [ defineHistogram('passedJetCuts_maxmct', type='TH1F', title="EFJetMct MAX MCT of jets after jet cuts", xbins=200, xmin=0, xmax=1000) ]
        self.Histograms += [ defineHistogram('passedJetCuts_mct', type='TH1F', title="EFJetMct MCT of jets after jet cuts", xbins=200, xmin=0, xmax=1000) ]
        self.Histograms += [ defineHistogram('passedJetCuts_dphi',type='TH1F', title="EFJetMct DeltaPhi of jets after jet cuts", xbins=32, xmin=0., xmax=3.2) ]
        self.Histograms += [ defineHistogram('passedJetCuts_et1',type='TH1F', title="EFJetMct jet1 Et after jet cuts", xbins=198, xmin=10, xmax=1000) ]
        self.Histograms += [ defineHistogram('passedJetCuts_et2',type='TH1F', title="EFJetMct jet2 Et after jet cuts", xbins=198, xmin=10, xmax=1000) ]
        self.Histograms += [ defineHistogram('passedJetCuts_eta1',type='TH1F', title="EFJetMct jet1 eta after jet cuts", xbins=80, xmin=-4, xmax=4) ]
        self.Histograms += [ defineHistogram('passedJetCuts_eta2',type='TH1F', title="EFJetMct jet2 eta after jet cuts", xbins=80, xmin=-4, xmax=4) ]
        self.Histograms += [ defineHistogram('passedJetCuts_phi1',type='TH1F', title="EFJetMct jet1 phi after jet cuts", xbins=64, xmin=-3.2, xmax=3.2) ]
        self.Histograms += [ defineHistogram('passedJetCuts_phi2',type='TH1F', title="EFJetMct jet2 phi after jet cuts", xbins=64, xmin=-3.2, xmax=3.2) ]

                                                                                                         
        self.Histograms += [ defineHistogram('passedMctCut_mct', type='TH1F', title="EFJetMct MCT of jets after Mct cut", xbins=200, xmin=0, xmax=1000) ]
        self.Histograms += [ defineHistogram('passedMctCut_maxmct', type='TH1F', title="EFJetMct MAX MCT of jets after Mct cut", xbins=200, xmin=0, xmax=1000) ]
        self.Histograms += [ defineHistogram('passedMctCut_dphi',type='TH1F', title="EFJetMct DeltaPhi of jets after Mct cut", xbins=64, xmin=0., xmax=3.2) ]
                                                                                                         
        self.Histograms += [ defineHistogram('nJet',type='TH1F', title="EFJetMct no. of jets considered", xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('nComb',type='TH1F', title="EFJetMct no. of jet pair combinations considered", xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('passednComb',type='TH1F', title="EFJetMct no. of jet pair combinations passing Mct cut", xbins=50, xmin=0, xmax=50) ]

        self.Histograms += [ defineHistogram('cutCounter',type='TH1F', title="EFJetMct Cut Counter", xbins=10, xmin=0, xmax=10) ]
        
