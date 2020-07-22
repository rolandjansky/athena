# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

class TrigBjetOnlineMonitoring(GenericMonitoringTool):
    def __init__ (self, name="TrigBjetOnlineMonitoring"):
        super(TrigBjetOnlineMonitoring, self).__init__(name)
        self.name = "TrigBjetOnlineMonitoring"
        self.HistPath = self.name
        default_bin_count = 100
        TH1F_opts = { 'type':'TH1F', 'path':'EXPERT' }

        # Event Histograms
        self.defineHistogram('track_count', title="Number of Tracks", xbins = 100, xmin=0, xmax=100, **TH1F_opts)
        self.defineHistogram('jet_count', title="Number of Jets", xbins = 20, xmin=0, xmax=20, **TH1F_opts)
        self.defineHistogram('vertex_count', title="Number of Vertices", xbins = 200, xmin=0, xmax=200, **TH1F_opts)

        ## Track Histograms
        self.defineHistogram('track_Et', title="Track Transverse Energy;E_{T} (GeV)", xbins = default_bin_count, xmin=0, xmax=200, **TH1F_opts)
        self.defineHistogram('track_eta', title="Track #eta;#eta", xbins = default_bin_count, xmin=-5, xmax=5, **TH1F_opts)
        self.defineHistogram('track_phi', title="Track #phi;#phi", xbins = default_bin_count, xmin=-3.5, xmax=3.5, **TH1F_opts)
        self.defineHistogram('track_eta,track_phi', path='EXPERT', type='TH2F', title="Track #eta vs #phi;#eta;#phi",
                             xbins = default_bin_count, xmin=-5, xmax=5, ybins = default_bin_count, ymin=-4, ymax=4)
        self.defineHistogram('track_d0', title="Track d_{0};d_{0} (mm)", xbins = default_bin_count, xmin=-5, xmax=5, **TH1F_opts)
        self.defineHistogram('track_d0err', title="Track d_{0} Error;d_{0} Error (mm)", xbins = default_bin_count, xmin=0, xmax=10, **TH1F_opts)
        self.defineHistogram('track_d0sig', title="Track d_{0} Significance;d_{0} #sigma", xbins = default_bin_count, xmin=-100, xmax=100, **TH1F_opts)
        self.defineHistogram('track_z0', title="Track z_{0};z_{0} (mm)", xbins = default_bin_count, xmin=-100, xmax=100, **TH1F_opts)
        self.defineHistogram('track_z0err', title="Track z_{0} Error;z_{0} Error (mm)", xbins = default_bin_count, xmin=0, xmax=10, **TH1F_opts)
        self.defineHistogram('track_z0sig', title="Track z_{0} Significance;z_{0} #sigma", xbins = default_bin_count, xmin=-1000, xmax=1000, **TH1F_opts)

        # Jet Histograms
        self.defineHistogram('jet_pt', title="Jet Transverse Momentum;p_{T} (GeV)", xbins = default_bin_count, xmin=0, xmax=200, **TH1F_opts)
        self.defineHistogram('jet_eta', title="Jet #eta;#eta", xbins = default_bin_count, xmin=-5, xmax=5, **TH1F_opts)
        self.defineHistogram('jet_phi', title="Jet #phi;#phi", xbins = default_bin_count, xmin=-3.5, xmax=3.5, **TH1F_opts)
        self.defineHistogram('jet_eta,jet_phi', path='EXPERT', type='TH2F', title="Jet #eta vs #phi;#eta;#phi",
                             xbins = default_bin_count, xmin=-5, xmax=5, ybins = default_bin_count, ymin=-4, ymax=4)
