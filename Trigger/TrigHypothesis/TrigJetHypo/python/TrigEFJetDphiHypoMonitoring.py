# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 


class TrigEFJetDphiHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFJetDphiHypoValidationMonitoring"):
        super(TrigEFJetDphiHypoValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")

        self.Histograms  = [ defineHistogram('passedJetCuts_minDphi',type='TH1F', title="EFJetDphi MIN Dphi of all considered jets after jet cuts", xbins=32, xmin=0., xmax=3.2) ]
        self.Histograms += [ defineHistogram('passedJetCuts_dphi',type='TH1F', title="EFJetDphi DeltaPhi of considered jets after jet cuts", xbins=32, xmin=0., xmax=3.2) ]
        self.Histograms += [ defineHistogram('passedJetCuts_et1',type='TH1F', title="EFJetDphi jet1 Et after jet cuts", xbins=198, xmin=10, xmax=1000) ]
        self.Histograms += [ defineHistogram('passedJetCuts_et2',type='TH1F', title="EFJetDphi jet2 Et after jet cuts", xbins=198, xmin=10, xmax=1000) ]
        self.Histograms += [ defineHistogram('passedJetCuts_eta1',type='TH1F', title="EFJetDphi jet1 eta after jet cuts", xbins=80, xmin=-4, xmax=4) ]
        self.Histograms += [ defineHistogram('passedJetCuts_eta2',type='TH1F', title="EFJetDphi jet2 eta after jet cuts", xbins=80, xmin=-4, xmax=4) ]
        self.Histograms += [ defineHistogram('passedJetCuts_phi1',type='TH1F', title="EFJetDphi jet1 phi after jet cuts", xbins=64, xmin=-3.2, xmax=3.2) ]
        self.Histograms += [ defineHistogram('passedJetCuts_phi2',type='TH1F', title="EFJetDphi jet2 phi after jet cuts", xbins=64, xmin=-3.2, xmax=3.2) ]
        
        self.Histograms += [ defineHistogram('passedDphiCut_dphi',type='TH1F', title="EFJetDphi DeltaPhi of considered jets after Dphi cut", xbins=32, xmin=0., xmax=3.2) ]
        self.Histograms += [ defineHistogram('passedDphiCut_minDphi',type='TH1F', title="EFJetDphi MIN Dphi of all considered jets after Dphi cut", xbins=32, xmin=0., xmax=3.2) ]
                                                                                                         
        self.Histograms += [ defineHistogram('nJet',type='TH1F', title="EFJetDphi no. of jets considered", xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('nComb',type='TH1F', title="EFJetDphi no. of jet pair combinations considered", xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('passednComb',type='TH1F', title="EFJetDphi no. of jet pair combinations passing Dphi cut", xbins=50, xmin=0, xmax=50) ]

        self.Histograms += [ defineHistogram('cutCounter',type='TH1F', title="EFJetDphi Cut Counter", xbins=10, xmin=0, xmax=10) ]

        

class TrigEFJetDphiHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFJetDphiHypoOnlineMonitoring"):
        """ defines histograms for online """
        super(TrigEFJetDphiHypoOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")

        self.Histograms  = [ defineHistogram('passedJetCuts_minDphi',type='TH1F', title="EFJetDphi MIN Dphi of all considered jets after jet cuts", xbins=32, xmin=0., xmax=3.2) ]
        self.Histograms += [ defineHistogram('passedJetCuts_dphi',type='TH1F', title="EFJetDphi DeltaPhi of considered jets after jet cuts", xbins=32, xmin=0., xmax=3.2) ]
        self.Histograms += [ defineHistogram('passedJetCuts_et1',type='TH1F', title="EFJetDphi jet1 Et after jet cuts", xbins=198, xmin=10, xmax=1000) ]
        self.Histograms += [ defineHistogram('passedJetCuts_et2',type='TH1F', title="EFJetDphi jet2 Et after jet cuts", xbins=198, xmin=10, xmax=1000) ]
        self.Histograms += [ defineHistogram('passedJetCuts_eta1',type='TH1F', title="EFJetDphi jet1 eta after jet cuts", xbins=80, xmin=-4, xmax=4) ]
        self.Histograms += [ defineHistogram('passedJetCuts_eta2',type='TH1F', title="EFJetDphi jet2 eta after jet cuts", xbins=80, xmin=-4, xmax=4) ]
        self.Histograms += [ defineHistogram('passedJetCuts_phi1',type='TH1F', title="EFJetDphi jet1 phi after jet cuts", xbins=64, xmin=-3.2, xmax=3.2) ]
        self.Histograms += [ defineHistogram('passedJetCuts_phi2',type='TH1F', title="EFJetDphi jet2 phi after jet cuts", xbins=64, xmin=-3.2, xmax=3.2) ]
        
        self.Histograms += [ defineHistogram('passedDphiCut_dphi',type='TH1F', title="EFJetDphi DeltaPhi of considered jets after Dphi cut", xbins=32, xmin=0., xmax=3.2) ]
        self.Histograms += [ defineHistogram('passedDphiCut_minDphi',type='TH1F', title="EFJetDphi MIN Dphi of all considered jets after Dphi cut", xbins=32, xmin=0., xmax=3.2) ]
                                                                                                         
        self.Histograms += [ defineHistogram('nJet',type='TH1F', title="EFJetDphi no. of jets considered", xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('nComb',type='TH1F', title="EFJetDphi no. of jet pair combinations considered", xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('passednComb',type='TH1F', title="EFJetDphi no. of jet pair combinations passing Dphi cut", xbins=50, xmin=0, xmax=50) ]

        self.Histograms += [ defineHistogram('cutCounter',type='TH1F', title="EFJetDphi Cut Counter", xbins=10, xmin=0, xmax=10) ]
