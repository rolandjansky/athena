# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TrigLeptonJetMatchAllTEMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigLeptonJetMatchAllTEMonitoring"):
        super(TrigLeptonJetMatchAllTEMonitoring, self).__init__(name)

        self.Histograms += [ defineHistogram('CutCounter', type='TH1F', title="Muon-jet hypo cut counter",
                                             xbins=3, xmin=-1.5, xmax=1.5,
                                             labels='No obj:Rejected:Accepted') ]

        self.Histograms += [ defineHistogram('DeltaEtaPass', type='TH1F', title="Delta eta between EF muon and EF jet in close-by working mode",
                                             xbins=160, xmin=-0.8, xmax=0.8) ]
        self.Histograms += [ defineHistogram('DeltaPhiPass', type='TH1F', title="Delta phi between EF muon and EF jet in close-by working mode",
                                             xbins=160, xmin=-0.8, xmax=0.8) ]
        self.Histograms += [ defineHistogram('MuonEFEta', type='TH1F', title="Eta of EF muons for successful mu-jet candidates",
                                             xbins=100, xmin=-5.0, xmax=5.0) ]
        self.Histograms += [ defineHistogram('MuonEFPhi', type='TH1F', title="Phi of EF muons for successful mu-jet candidates",
                                             xbins=100, xmin=-3.2, xmax=3.2) ]
        self.Histograms += [ defineHistogram('DeltaRPass', type='TH1F', title="DeltaR between mu-jet candidates",
                                             xbins=100, xmin=0., xmax=2.) ]
        self.Histograms += [ defineHistogram('DeltaRAll', type='TH1F', title="DeltaR between muon and any jet",
                                             xbins=100, xmin=0., xmax=6.) ]
        self.Histograms += [ defineHistogram('DeltaZPass', type='TH1F', title="DeltaZ between mu-jet candidates",
                                             xbins=100, xmin=0., xmax=10.) ]
        self.Histograms += [ defineHistogram('DeltaZAll', type='TH1F', title="DeltaZ between muon and any jet",
                                             xbins=100, xmin=0., xmax=10.) ]


class TrigEFLeptonJetMatchAllTEValidationMonitoring(TrigLeptonJetMatchAllTEMonitoring):
    def __init__ (self, name="TrigEFLeptonJetMatchAllTEValidationMonitoring"):
        super(TrigEFLeptonJetMatchAllTEValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")



class TrigEFLeptonJetMatchAllTEOnlineMonitoring(TrigLeptonJetMatchAllTEMonitoring):
    def __init__ (self, name="TrigEFLeptonJetMatchAllTEOnlineMonitoring"):
        super(TrigEFLeptonJetMatchAllTEOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")

