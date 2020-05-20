# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

class TrigBjetOnlineMonitoring(GenericMonitoringTool):
    def __init__ (self, name="TrigBjetOnlineMonitoring"):
        super(TrigBjetOnlineMonitoring, self).__init__(name)
        self.name = "TrigBjetOnlineMonitoring"
        self.HistPath = self.name
        default_bin_count = 20

        # Event Histograms
        self.defineHistogram('track_count', path='EXPERT', type='TH1F', title="Number of Tracks", xbins = 50, xmin=0, xmax=50) 
        self.defineHistogram('jet_count', path='EXPERT', type='TH1F', title="Number of Jets", xbins = 20, xmin=0, xmax=20) 
        self.defineHistogram('vertex_count', path='EXPERT', type='TH1F', title="Number of Vertices", xbins = 50, xmin=0, xmax=50)

        ## Track Histograms
        self.defineHistogram('track_Et', path='EXPERT', type='TH1F', title="Track Transverse Energy", xbins = default_bin_count, xmin=0, xmax=800000) 
        self.defineHistogram('track_eta', path='EXPERT', type='TH1F', title="Track Eta", xbins = default_bin_count, xmin=-5, xmax=5) 
        self.defineHistogram('track_phi', path='EXPERT', type='TH1F', title="Track Phi", xbins = default_bin_count, xmin=-4, xmax=4) 
        self.defineHistogram('track_eta,track_phi', path='EXPERT', type='TH2F', title="Track Eta vs Phi",
                             xbins = default_bin_count, xmin=-5, xmax=5, ybins = default_bin_count, ymin=-4, ymax=4) 
        self.defineHistogram('track_d0', path='EXPERT', type='TH1F', title="Track D0", xbins = default_bin_count, xmin=0, xmax=10) 
        self.defineHistogram('track_d0err', path='EXPERT', type='TH1F', title="Track D0 Error", xbins = default_bin_count, xmin=0, xmax=10) 
        self.defineHistogram('track_d0sig', path='EXPERT', type='TH1F', title="Track D0 Significance", xbins = default_bin_count, xmin=0, xmax=1000) 
        self.defineHistogram('track_z0', path='EXPERT', type='TH1F', title="Track Z0", xbins = default_bin_count, xmin=0, xmax=100) 
        self.defineHistogram('track_z0err', path='EXPERT', type='TH1F', title="Track Z0 Error", xbins = default_bin_count, xmin=0, xmax=10) 
        self.defineHistogram('track_z0sig', path='EXPERT', type='TH1F', title="Track Z0 Significance", xbins = default_bin_count, xmin=0, xmax=10000) 

        # Jet Histograms
        self.defineHistogram('jet_pt', path='EXPERT', type='TH1F', title="Jet PT", xbins = default_bin_count, xmin=0, xmax=800000) 
        self.defineHistogram('jet_eta', path='EXPERT', type='TH1F', title="Jet Eta", xbins = default_bin_count, xmin=-5, xmax=5) 
        self.defineHistogram('jet_phi', path='EXPERT', type='TH1F', title="Jet Phi", xbins = default_bin_count, xmin=-3.5, xmax=3.5) 
