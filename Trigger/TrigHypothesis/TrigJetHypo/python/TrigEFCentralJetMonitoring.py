# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 


class TrigEFJetHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="EFJetHypo_Validation"):
        super(TrigEFJetHypoValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")

        self.Histograms =  [ defineHistogram('all_jet0_et', type='TH1F', title="EFCentralJet Et  before cut", xbins=100, xmin=-100, xmax=100) ]
        self.Histograms += [ defineHistogram('all_jet0_eta',type='TH1F', title="EFCentralJet Eta before cut", xbins=100, xmin=-4, xmax=4) ]
        self.Histograms += [ defineHistogram('passed_jet0_et', type='TH1F', title="EFCentralJet Et  after cut", xbins=100, xmin=-100, xmax=100) ]
        self.Histograms += [ defineHistogram('passed_jet0_eta',type='TH1F', title="EFCentralJet Eta after cut", xbins=100, xmin=-4, xmax=4) ]
        

class TrigEFJetHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="EFJetHypo_Online"):
        """ defines histograms for online """
        super(TrigEFJetHypoOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")


        self.Histograms =  [ defineHistogram('all_jet0_et', type='TH1F', title="EFCentralJet Et  before cut", xbins=100, xmin=-100, xmax=100) ]
        self.Histograms += [ defineHistogram('all_jet0_eta',type='TH1F', title="EFCentralJet Eta before cut", xbins=100, xmin=-4, xmax=4) ]
        self.Histograms += [ defineHistogram('passed_jet0_et', type='TH1F', title="EFCentralJet Et  after cut", xbins=100, xmin=-100, xmax=100) ]
        self.Histograms += [ defineHistogram('passed_jet0_eta',type='TH1F', title="EFCentralJet Eta after cut", xbins=100, xmin=-4, xmax=4) ]


class TrigEFJetHypoCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="EFJetHypo_Cosmic"):
        """ defines histograms for cosmic """
        super(TrigEFJetHypoCosmicMonitoring, self).__init__(name)

        self.defineTarget("Cosmic")


        self.Histograms =  [ defineHistogram('all_jet0_et', type='TH1F', title="EFCentralJet Et  before cut", xbins=100, xmin=-100, xmax=100) ]
        self.Histograms += [ defineHistogram('all_jet0_eta',type='TH1F', title="EFCentralJet Eta before cut", xbins=100, xmin=-4, xmax=4) ]
        self.Histograms += [ defineHistogram('passed_jet0_et', type='TH1F', title="EFCentralJet Et  after cut", xbins=100, xmin=-100, xmax=100) ]
        self.Histograms += [ defineHistogram('passed_jet0_eta',type='TH1F', title="EFCentralJet Eta after cut", xbins=100, xmin=-4, xmax=4) ]
