# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TrigLeptonJetFexAllTEMonitoring_CloseBy(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigLeptonJetFexAllTEMonitoring_CloseBy"):
        super(TrigLeptonJetFexAllTEMonitoring_CloseBy, self).__init__(name)

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



class TrigLeptonJetFexAllTEMonitoring_FarOff(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigLeptonJetFexAllTEMonitoring_FarOff"):
        super(TrigLeptonJetFexAllTEMonitoring_FarOff, self).__init__(name)

        self.Histograms += [ defineHistogram('CutCounter', type='TH1F', title="Muon-jet hypo cut counter",
                                             xbins=3, xmin=-1.5, xmax=1.5,
                                             labels='No obj:Rejected:Accepted') ]

        self.Histograms += [ defineHistogram('DeltaEtaPass', type='TH1F', title="Delta eta between EF muon and EF jet in far-off working mode",
                                             xbins=600, xmin=0.0, xmax=6.0) ]
        self.Histograms += [ defineHistogram('DeltaPhiPass', type='TH1F', title="Delta phi between EF muon and EF jet in far-off working mode",
                                             xbins=320, xmin=0, xmax=3.2) ]
        self.Histograms += [ defineHistogram('MuonEFEta', type='TH1F', title="Eta of EF muons for successful mu-jet candidates",
                                             xbins=100, xmin=-5, xmax=5) ]
        self.Histograms += [ defineHistogram('MuonEFPhi', type='TH1F', title="Phi of EF muons for successful mu-jet candidates",
                                             xbins=100, xmin=-3.2, xmax=3.2) ]



class TrigEFLeptonJetFexAllTEValidationMonitoring_CloseBy(TrigLeptonJetFexAllTEMonitoring_CloseBy):
    def __init__ (self, name="TrigEFLeptonJetFexAllTEValidationMonitoring_CloseBy"):
        super(TrigEFLeptonJetFexAllTEValidationMonitoring_CloseBy, self).__init__(name)

        self.defineTarget("Validation")



class TrigEFLeptonJetFexAllTEOnlineMonitoring_CloseBy(TrigLeptonJetFexAllTEMonitoring_CloseBy):
    def __init__ (self, name="TrigEFLeptonJetFexAllTEOnlineMonitoring_CloseBy"):
        super(TrigEFLeptonJetFexAllTEOnlineMonitoring_CloseBy, self).__init__(name)

        self.defineTarget("Online")



class TrigEFLeptonJetFexAllTEValidationMonitoring_FarOff(TrigLeptonJetFexAllTEMonitoring_FarOff):
    def __init__ (self, name="TrigEFLeptonJetFexAllTEValidationMonitoring_FarOff"):
        super(TrigEFLeptonJetFexAllTEValidationMonitoring_FarOff, self).__init__(name)

        self.defineTarget("Validation")



class TrigEFLeptonJetFexAllTEOnlineMonitoring_FarOff(TrigLeptonJetFexAllTEMonitoring_FarOff):
    def __init__ (self, name="TrigEFLeptonJetFexAllTEOnlineMonitoring_FarOff"):
        super(TrigEFLeptonJetFexAllTEOnlineMonitoring_FarOff, self).__init__(name)

        self.defineTarget("Online")
