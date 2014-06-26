# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TrigmuCombValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigmuCombValidationMonitoring"):
        super(TrigmuCombValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")

        self.Histograms = [ defineHistogram('Efficiency', type='TH1F', title="Monitored Track matching efficiency from #muComb",
                                            xbins=3, xmin=-1.5, xmax=1.5) ]
        self.Histograms += [ defineHistogram('ErrorFlagMC', type='TH1F', title="Error Flags during event processing from #muComb; Error Code",
                                            xbins=12, xmin=-1.5, xmax=10.5) ]
        self.Histograms += [ defineHistogram('MatchFlagMC', type='TH1F', title="Match Flags during matching algo from #muComb; Error Code",
                                            xbins=12, xmin=-1.5, xmax=10.5) ]
        self.Histograms += [ defineHistogram('StrategyMC', type='TH1F', title="Combination Strategy from #muComb; Strategy Code",
                                            xbins=12, xmin=-1.5, xmax=10.5) ]
        self.Histograms += [ defineHistogram('PtMS', type='TH1F', title="p_{T} MS from #muComb; p_{T} (GeV/c)",
                                            xbins=210, xmin=-105., xmax=105.) ]
        self.Histograms += [ defineHistogram('PtID', type='TH1F', title="p_{T} ID from #muComb; p_{T} (GeV/c)",
                                            xbins=210, xmin=-105., xmax=105.) ]
        self.Histograms += [ defineHistogram('PtMC', type='TH1F', title="p_{T} Combined from #muComb; p_{T} (GeV/c)",
                                            xbins=210, xmin=-105., xmax=105.) ]
        self.Histograms += [ defineHistogram('PtFL', type='TH1F', title="MS p_{T} failed matches from #muComb; p_{T} (GeV/c)",
                                            xbins=210, xmin=-105., xmax=105.) ]
        self.Histograms += [ defineHistogram('EtaFL', type='TH1F', title="MS Eta failed matchs from #muComb; Eta",
                                            xbins=108, xmin=-2.7, xmax=2.7) ]
        self.Histograms += [ defineHistogram('PhiFL', type='TH1F', title="MS Phi failed matches from #muComb; Phi (rad)",
                                            xbins=96, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('EtaID', type='TH1F', title="Eta ID from #muComb; Eta",
                                            xbins=108, xmin=-2.7, xmax=2.7) ]
        self.Histograms += [ defineHistogram('ZetaID', type='TH1F', title="z0 ID from #muComb; z0 (mm)",
                                            xbins=100, xmin=-200., xmax=200.) ]
        self.Histograms += [ defineHistogram('EtaMS', type='TH1F', title="Eta MS from #muComb; Eta",
                                            xbins=108, xmin=-2.7, xmax=2.7) ]
        self.Histograms += [ defineHistogram('ZetaMS', type='TH1F', title="z0 MS from #muComb; z0 (mm)",
                                            xbins=100, xmin=-200., xmax=200.) ]
        self.Histograms += [ defineHistogram('DEta', type='TH1F', title="Eta difference between muon and matched ID track from #muComb; DeltaEta",
                                            xbins=100, xmin=0.0, xmax=0.2) ]
        self.Histograms += [ defineHistogram('DPhi', type='TH1F', title="Phi difference between muon and matched ID track from #muComb; DeltaPhi (rad)",
                                            xbins=100, xmin=0.0, xmax=0.2) ]
        self.Histograms += [ defineHistogram('DZeta', type='TH1F', title="Zeta difference between muon and matched ID track from #muComb; DeltaZeta (cm)",
                                            xbins=100, xmin=-60, xmax=60) ]
        self.Histograms += [ defineHistogram('DeltaR', type='TH1F', title="DeltaR between muon and matched ID track from #muComb; DeltaR",
                                            xbins=100, xmin=0., xmax=0.5) ]
#  first x then y
        self.Histograms += [ defineHistogram('PtMS, PtID', type='TH2F', title="PtID vs PtMS from #muComb",
                                             xbins=105, xmin=-105.0, xmax=105.0, ybins=105, ymin=-105.0, ymax=105.0) ]
        self.Histograms += [ defineHistogram('EtaMS, DEta', type='TH2F', title="Eta diff (MS-ID) vs Eta(MS) from #muComb",
                                             xbins=100, xmin=-3, xmax=3, ybins=100, ymin=0.0, ymax=0.2) ]
        self.Histograms += [ defineHistogram('EtaMS, DPhi', type='TH2F', title="Phi diff (MS-ID) vs Eta(MS) from #muComb",
                                             xbins=100, xmin=-3, xmax=3, ybins=100, ymin=0.0, ymax=0.2) ]


class TrigmuCombOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigmuCombOnlineMonitoring"):
        super(TrigmuCombOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms  = [ defineHistogram('ErrorFlagMC', type='TH1F', title="Error Flags during event processing from #muComb; Error Code",
                                            xbins=12, xmin=-1.5, xmax=10.5) ]
        self.Histograms += [ defineHistogram('MatchFlagMC', type='TH1F', title="Match Flags during matching algo from #muComb; Error Code",
                                            xbins=12, xmin=-1.5, xmax=10.5) ]
        self.Histograms += [ defineHistogram('StrategyMC', type='TH1F', title="Combination Strategy from #muComb; Strategy Code",
                                            xbins=12, xmin=-1.5, xmax=10.5) ]
        self.Histograms += [ defineHistogram('PtMC', type='TH1F', title="p_{T} Combined from #muComb; p_{T} (GeV/c)",
                                            xbins=210, xmin=-105., xmax=105.) ]
        self.Histograms += [ defineHistogram('PtFL', type='TH1F', title="MS p_{T} failed matches from #muComb; p_{T} (GeV/c)",
                                            xbins=210, xmin=-105., xmax=105.) ]
        self.Histograms += [ defineHistogram('EtaFL', type='TH1F', title="MS Eta failed matchs from #muComb; Eta",
                                            xbins=108, xmin=-2.7, xmax=2.7) ]
        self.Histograms += [ defineHistogram('PhiFL', type='TH1F', title="MS Phi failed matches from #muComb; Phi (rad)",
                                            xbins=96, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('DEta', type='TH1F', title="Eta difference between muon and matched ID track from #muComb; DeltaEta",
                                            xbins=100, xmin=0.0, xmax=0.2) ]
        self.Histograms += [ defineHistogram('DPhi', type='TH1F', title="Phi difference between muon and matched ID track from #muComb; DeltaPhi (rad)",
                                            xbins=100, xmin=0.0, xmax=0.2) ]

class TrigmuCombCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigmuCombCosmicMonitoring"):
        super(TrigmuCombCosmicMonitoring, self).__init__(name)
        self.defineTarget("Cosmic")

        self.Histograms  = [ defineHistogram('ErrorFlagMC', type='TH1F', title="Error Flags during event processing from #muComb; Error Code",
                                            xbins=12, xmin=-1.5, xmax=10.5) ]
        self.Histograms += [ defineHistogram('MatchFlagMC', type='TH1F', title="Match Flags during matching algo from #muComb; Error Code",
                                            xbins=12, xmin=-1.5, xmax=10.5) ]
        self.Histograms += [ defineHistogram('StrategyMC', type='TH1F', title="Combination Strategy from #muComb; Strategy Code",
                                            xbins=12, xmin=-1.5, xmax=10.5) ]
        self.Histograms += [ defineHistogram('PtMC', type='TH1F', title="p_{T} Combined from #muComb; p_{T} (GeV/c)",
                                            xbins=210, xmin=-105., xmax=105.) ]
        self.Histograms += [ defineHistogram('PtFL', type='TH1F', title="MS p_{T} failed matches from #muComb; p_{T} (GeV/c)",
                                            xbins=210, xmin=-105., xmax=105.) ]
        self.Histograms += [ defineHistogram('EtaFL', type='TH1F', title="MS Eta failed matchs from #muComb; Eta",
                                            xbins=108, xmin=-2.7, xmax=2.7) ]
        self.Histograms += [ defineHistogram('PhiFL', type='TH1F', title="MS Phi failed matches from #muComb; Phi (rad)",
                                            xbins=96, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('DEta', type='TH1F', title="Eta difference between muon and matched ID track from #muComb; DeltaEta",
                                            xbins=100, xmin=0.0, xmax=0.2) ]
        self.Histograms += [ defineHistogram('DPhi', type='TH1F', title="Phi difference between muon and matched ID track from #muComb; DeltaPhi (rad)",
                                            xbins=100, xmin=0.0, xmax=0.2) ]

