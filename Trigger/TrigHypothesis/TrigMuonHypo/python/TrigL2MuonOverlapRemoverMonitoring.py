# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TrigL2MuonOverlapRemoverValidationMonitoringMufast(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2MuonOverlapRemoverValidationMonitoringMufast"):
        super(TrigL2MuonOverlapRemoverValidationMonitoringMufast, self).__init__(name)
        self.defineTarget("Validation")

        self.Histograms  = [ defineHistogram('MufastError',  type='TH1F', title="error in #muFast based overlap removal; error code",
                                             xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('MufastDR',  type='TH1F', title="#muFast objects dR; dR",
                                             xbins=108, xmin=-0.1, xmax=3.5) ]
        self.Histograms += [ defineHistogram('MufastMass',type='TH1F', title="#muFast objects invMass; Mass (GeV)",
                                             xbins=202, xmin=-1, xmax=100) ]
        self.Histograms += [ defineHistogram('MufastDRLog10',  type='TH1F', title="#muFast objects Log10(dR); Log10(dR)",
                                             xbins=102, xmin=-4.1, xmax=1.0) ]
        self.Histograms += [ defineHistogram('MufastMassLog10',type='TH1F', title="#muFast objects Log10(invMass); Log10(Mass (GeV))",
                                             xbins=142, xmin=-4.1, xmax=3.0) ]
        self.Histograms += [ defineHistogram('MufastMass , MufastDR', type='TH2F', title="#muFast objects Mass vs DR; Mass; dR",
                                             xbins=54, xmin=-0.1, xmax=3.5, ybins=101, ymin=-1, ymax=100) ]
        self.Histograms += [ defineHistogram('MufastNrAllTEs',type='TH1F', title="nr of all TEs received for #muFast removal; nr",
                                             xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('MufastNrActiveTEs',type='TH1F', title="nr of active TEs after #muFast removal ; nr",
                                             xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('MufastNrOverlapped',type='TH1F', title="nr of #muFast overlapped; nr",
                                             xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('MufastOverlappedEta , MufastOverlappedPhi', type='TH2F',
                                             title="#muFast overlapped Eta vs Phi; Eta; Phi",
                                             xbins=50, xmin=-3.2, xmax=3.2, ybins=25, ymin=-3.15, ymax=3.15) ]
        self.Histograms += [ defineHistogram('MufastOverlappedPt', type='TH1F', title="#muFast overlapped P_{T}; P_{T} (GeV)",
                                             xbins=200, xmin=-100, xmax=100) ]

class TrigL2MuonOverlapRemoverValidationMonitoringMucomb(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2MuonOverlapRemoverValidationMonitoringMucomb"):
        super(TrigL2MuonOverlapRemoverValidationMonitoringMucomb, self).__init__(name)
        self.defineTarget("Validation")

        self.Histograms  = [ defineHistogram('MucombError',  type='TH1F', title="error in #muComb based overlap removal; error code",
                                             xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('MucombDR',  type='TH1F', title="#muComb objects dR; dR",
                                             xbins=108, xmin=-0.1, xmax=3.5) ]
        self.Histograms += [ defineHistogram('MucombMass',type='TH1F', title="#muComb objects invMass; Mass (GeV)",
                                             xbins=202, xmin=-1, xmax=100) ]
        self.Histograms += [ defineHistogram('MucombDRLog10',  type='TH1F', title="#muFast objects Log10(dR); Log10(dR)",
                                             xbins=102, xmin=-4.1, xmax=1.0) ]
        self.Histograms += [ defineHistogram('MucombMassLog10',type='TH1F', title="#muFast objects Log10(invMass); Log10(Mass (GeV))",
                                             xbins=142, xmin=-4.1, xmax=3.0) ]
        self.Histograms += [ defineHistogram('MucombMass , MucombDR', type='TH2F', title="#muComb objects Mass vs DR; Mass; dR",
                                             xbins=54, xmin=-0.1, xmax=3.5, ybins=101, ymin=-1, ymax=100) ]
        self.Histograms += [ defineHistogram('MucombNrAllTEs',type='TH1F', title="nr of all TEs received for #muComb removal; nr",
                                             xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('MucombNrActiveTEs',type='TH1F', title="nr of active TEs after #muComb removal ; nr",
                                             xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('MucombNrOverlapped',type='TH1F', title="nr of #muComb overlapped; nr",
                                             xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('MucombOverlappedEta , MucombOverlappedPhi', type='TH2F',
                                             title="#muComb overlapped Eta vs Phi; Eta; Phi",
                                             xbins=50, xmin=-3.2, xmax=3.2, ybins=25, ymin=-3.15, ymax=3.15) ]
        self.Histograms += [ defineHistogram('MucombOverlappedPt', type='TH1F', title="#muComb overlapped P_{T}; P_{T} (GeV)",
                                             xbins=200, xmin=-100, xmax=100) ]

# -----------------------------------------------------------------------------------------
# -----------------------------------------------------------------------------------------

class TrigL2MuonOverlapRemoverOnlineMonitoringMufast(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2MuonOverlapRemoverOnlineMonitoringMufast"):
        super(TrigL2MuonOverlapRemoverOnlineMonitoringMufast, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms  = [ defineHistogram('MufastError',  type='TH1F', title="error in #muFast based overlap removal; error code",
                                             xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('MufastDRLog10',  type='TH1F', title="#muFast objects Log10(dR); Log10(dR)",
                                             xbins=102, xmin=-4.1, xmax=1.0) ]
        self.Histograms += [ defineHistogram('MufastMassLog10',type='TH1F', title="#muFast objects Log10(invMass); Log10(Mass (GeV))",
                                             xbins=142, xmin=-4.1, xmax=3.0) ]
        self.Histograms += [ defineHistogram('MufastNrAllTEs',type='TH1F', title="nr of all TEs received for #muFast removal; nr",
                                             xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('MufastNrActiveTEs',type='TH1F', title="nr of active TEs after #muFast removal ; nr",
                                             xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('MufastNrOverlapped',type='TH1F', title="nr of #muFast overlapped; nr",
                                             xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('MufastOverlappedEta , MufastOverlappedPhi', type='TH2F',
                                             title="#muFast overlapped Eta vs Phi; Eta; Phi",
                                             xbins=50, xmin=-3.2, xmax=3.2, ybins=25, ymin=-3.15, ymax=3.15) ]
        self.Histograms += [ defineHistogram('MufastOverlappedPt', type='TH1F', title="#muFast overlapped P_{T}; P_{T} (GeV)",
                                             xbins=200, xmin=-100, xmax=100) ]

class TrigL2MuonOverlapRemoverOnlineMonitoringMucomb(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2MuonOverlapRemoverOnlineMonitoringMucomb"):
        super(TrigL2MuonOverlapRemoverOnlineMonitoringMucomb, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms  = [ defineHistogram('MucombError',  type='TH1F', title="error in #muComb based overlap removal; error code",
                                             xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('MucombDRLog10',  type='TH1F', title="#muFast objects Log10(dR); Log10(dR)",
                                             xbins=102, xmin=-4.1, xmax=1.0) ]
        self.Histograms += [ defineHistogram('MucombMassLog10',type='TH1F', title="#muFast objects Log10(invMass); Log10(Mass (GeV))",
                                             xbins=142, xmin=-4.1, xmax=3.0) ]
        self.Histograms += [ defineHistogram('MucombNrAllTEs',type='TH1F', title="nr of all TEs received for #muComb removal; nr",
                                             xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('MucombNrActiveTEs',type='TH1F', title="nr of active TEs after #muComb removal ; nr",
                                             xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('MucombNrOverlapped',type='TH1F', title="nr of #muComb overlapped; nr",
                                             xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('MucombOverlappedEta , MucombOverlappedPhi', type='TH2F',
                                             title="#muComb overlapped Eta vs Phi; Eta; Phi",
                                             xbins=50, xmin=-3.2, xmax=3.2, ybins=25, ymin=-3.15, ymax=3.15) ]
        self.Histograms += [ defineHistogram('MucombOverlappedPt', type='TH1F', title="#muComb overlapped P_{T}; P_{T} (GeV)",
                                             xbins=200, xmin=-100, xmax=100) ]

# -----------------------------------------------------------------------------------------
# -----------------------------------------------------------------------------------------

class TrigL2MuonOverlapRemoverCosmicMonitoringMufast(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2MuonOverlapRemoverCosmicMonitoringMufast"):
        super(TrigL2MuonOverlapRemoverCosmicMonitoringMufast, self).__init__(name)
        self.defineTarget("Cosmic")

        self.Histograms  = [ defineHistogram('MufastError',  type='TH1F', title="error in #muFast based overlap removal; error code",
                                             xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('MufastDRLog10',  type='TH1F', title="#muFast objects Log10(dR); Log10(dR)",
                                             xbins=102, xmin=-4.1, xmax=1.0) ]
        self.Histograms += [ defineHistogram('MufastMassLog10',type='TH1F', title="#muFast objects Log10(invMass); Log10(Mass (GeV))",
                                             xbins=142, xmin=-4.1, xmax=3.0) ]
        self.Histograms += [ defineHistogram('MufastNrAllTEs',type='TH1F', title="nr of all TEs received for #muFast removal; nr",
                                             xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('MufastNrActiveTEs',type='TH1F', title="nr of active TEs after #muFast removal ; nr",
                                             xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('MufastNrOverlapped',type='TH1F', title="nr of #muFast overlapped; nr",
                                             xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('MufastOverlappedEta , MufastOverlappedPhi', type='TH2F',
                                             title="#muFast overlapped Eta vs Phi; Eta; Phi",
                                             xbins=50, xmin=-3.2, xmax=3.2, ybins=25, ymin=-3.15, ymax=3.15) ]
        self.Histograms += [ defineHistogram('MufastOverlappedPt', type='TH1F', title="#muFast overlapped P_{T}; P_{T} (GeV)",
                                             xbins=200, xmin=-100, xmax=100) ]

class TrigL2MuonOverlapRemoverCosmicMonitoringMucomb(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2MuonOverlapRemoverCosmicMonitoringMucomb"):
        super(TrigL2MuonOverlapRemoverCosmicMonitoringMucomb, self).__init__(name)
        self.defineTarget("Cosmic")

        self.Histograms  = [ defineHistogram('MucombError',  type='TH1F', title="error in #muComb based overlap removal; error code",
                                             xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('MucombDRLog10',  type='TH1F', title="#muFast objects Log10(dR); Log10(dR)",
                                             xbins=102, xmin=-4.1, xmax=1.0) ]
        self.Histograms += [ defineHistogram('MucombMassLog10',type='TH1F', title="#muFast objects Log10(invMass); Log10(Mass (GeV))",
                                             xbins=142, xmin=-4.1, xmax=3.0) ]
        self.Histograms += [ defineHistogram('MucombNrAllTEs',type='TH1F', title="nr of all TEs received for #muComb removal; nr",
                                             xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('MucombNrActiveTEs',type='TH1F', title="nr of active TEs after #muComb removal ; nr",
                                             xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('MucombNrOverlapped',type='TH1F', title="nr of #muComb overlapped; nr",
                                             xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('MucombOverlappedEta , MucombOverlappedPhi', type='TH2F',
                                             title="#muComb overlapped Eta vs Phi; Eta; Phi",
                                             xbins=50, xmin=-3.2, xmax=3.2, ybins=25, ymin=-3.15, ymax=3.15) ]
        self.Histograms += [ defineHistogram('MucombOverlappedPt', type='TH1F', title="#muComb overlapped P_{T}; P_{T} (GeV)",
                                             xbins=200, xmin=-100, xmax=100) ]

# -----------------------------------------------------------------------------------------
# -----------------------------------------------------------------------------------------
