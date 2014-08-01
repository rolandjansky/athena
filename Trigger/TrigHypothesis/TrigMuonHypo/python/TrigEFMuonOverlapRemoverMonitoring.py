# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TrigEFMuonOverlapRemoverValidationMonitoringMuExtr(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFMuonOverlapRemoverValidationMonitoringMuExtr"):
        super(TrigEFMuonOverlapRemoverValidationMonitoringMuExtr, self).__init__(name)
        self.defineTarget("Validation")

        self.Histograms  = [ defineHistogram('MuExtrError',  type='TH1F', title="error in #muExtr based overlap removal; error code",
                                             xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('MuExtrDR',  type='TH1F', title="#muExtr objects dR; dR",
                                             xbins=108, xmin=-0.1, xmax=3.5) ]
        self.Histograms += [ defineHistogram('MuextrMass',type='TH1F', title="#muExtr objects invMass; Mass (GeV)",
                                             xbins=202, xmin=-1, xmax=100) ]
        self.Histograms += [ defineHistogram('MuExtrDRLog10',  type='TH1F', title="#muFast objects Log10(dR); Log10(dR)",
                                             xbins=102, xmin=-4.1, xmax=1.0) ]
        self.Histograms += [ defineHistogram('MuextrMassLog10',type='TH1F', title="#muFast objects Log10(invMass); Log10(Mass (GeV))",
                                             xbins=142, xmin=-4.1, xmax=3.0) ]
        self.Histograms += [ defineHistogram('MuextrMass , MuExtrDR', type='TH2F', title="#muExtr objects Mass vs DR; Mass; dR",
                                             xbins=54, xmin=-0.1, xmax=3.5, ybins=101, ymin=-1, ymax=100) ]
        self.Histograms += [ defineHistogram('MuExtrNrAllTEs',type='TH1F', title="nr of all TEs received for #muExtr removal; nr",
                                             xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('MuExtrNrActiveTEs',type='TH1F', title="nr of active TEs after #muExtr removal ; nr",
                                             xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('MuExtrNrOverlapped',type='TH1F', title="nr of #muExtr overlapped; nr",
                                             xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('MuExtrOverlappedEta , MuExtrOverlappedPhi', type='TH2F',
                                             title="#muExtr overlapped Eta vs Phi; Eta; Phi",
                                             xbins=50, xmin=-3.2, xmax=3.2, ybins=25, ymin=-3.15, ymax=3.15) ]
        self.Histograms += [ defineHistogram('MuExtrOverlappedPt', type='TH1F', title="#muExtr overlapped P_{T}; P_{T} (GeV)",
                                             xbins=200, xmin=-100, xmax=100) ]

# -----------------------------------------------------------------------------------------
# -----------------------------------------------------------------------------------------

class TrigEFMuonOverlapRemoverOnlineMonitoringMuExtr(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFMuonOverlapRemoverOnlineMonitoringMuExtr"):
        super(TrigEFMuonOverlapRemoverOnlineMonitoringMuExtr, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms  = [ defineHistogram('MuExtrError',  type='TH1F', title="error in #muExtr based overlap removal; error code",
                                             xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('MuExtrDRLog10',  type='TH1F', title="#muFast objects Log10(dR); Log10(dR)",
                                             xbins=102, xmin=-4.1, xmax=1.0) ]
        self.Histograms += [ defineHistogram('MuextrMassLog10',type='TH1F', title="#muFast objects Log10(invMass); Log10(Mass (GeV))",
                                             xbins=142, xmin=-4.1, xmax=3.0) ]
        self.Histograms += [ defineHistogram('MuExtrNrAllTEs',type='TH1F', title="nr of all TEs received for #muExtr removal; nr",
                                             xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('MuExtrNrActiveTEs',type='TH1F', title="nr of active TEs after #muExtr removal ; nr",
                                             xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('MuExtrNrOverlapped',type='TH1F', title="nr of #muExtr overlapped; nr",
                                             xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('MuExtrOverlappedEta , MuExtrOverlappedPhi', type='TH2F',
                                             title="#muExtr overlapped Eta vs Phi; Eta; Phi",
                                             xbins=50, xmin=-3.2, xmax=3.2, ybins=25, ymin=-3.15, ymax=3.15) ]
        self.Histograms += [ defineHistogram('MuExtrOverlappedPt', type='TH1F', title="#muExtr overlapped P_{T}; P_{T} (GeV)",
                                             xbins=200, xmin=-100, xmax=100) ]

# -----------------------------------------------------------------------------------------
# -----------------------------------------------------------------------------------------

class TrigEFMuonOverlapRemoverCosmicMonitoringMuExtr(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFMuonOverlapRemoverCosmicMonitoringMuExtr"):
        super(TrigEFMuonOverlapRemoverCosmicMonitoringMuExtr, self).__init__(name)
        self.defineTarget("Cosmic")

        self.Histograms  = [ defineHistogram('MuExtrError',  type='TH1F', title="error in #muExtr based overlap removal; error code",
                                             xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('MuExtrDRLog10',  type='TH1F', title="#muFast objects Log10(dR); Log10(dR)",
                                             xbins=102, xmin=-4.1, xmax=1.0) ]
        self.Histograms += [ defineHistogram('MuextrMassLog10',type='TH1F', title="#muFast objects Log10(invMass); Log10(Mass (GeV))",
                                             xbins=142, xmin=-4.1, xmax=3.0) ]
        self.Histograms += [ defineHistogram('MuExtrNrAllTEs',type='TH1F', title="nr of all TEs received for #muExtr removal; nr",
                                             xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('MuExtrNrActiveTEs',type='TH1F', title="nr of active TEs after #muExtr removal ; nr",
                                             xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('MuExtrNrOverlapped',type='TH1F', title="nr of #muExtr overlapped; nr",
                                             xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('MuExtrOverlappedEta , MuExtrOverlappedPhi', type='TH2F',
                                             title="#muExtr overlapped Eta vs Phi; Eta; Phi",
                                             xbins=50, xmin=-3.2, xmax=3.2, ybins=25, ymin=-3.15, ymax=3.15) ]
        self.Histograms += [ defineHistogram('MuExtrOverlappedPt', type='TH1F', title="#muExtr overlapped P_{T}; P_{T} (GeV)",
                                             xbins=200, xmin=-100, xmax=100) ]

# -----------------------------------------------------------------------------------------
# -----------------------------------------------------------------------------------------
