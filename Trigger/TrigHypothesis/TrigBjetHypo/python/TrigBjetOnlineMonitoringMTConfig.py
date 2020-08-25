# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

class TrigBjetOnlineMonitoring(GenericMonitoringTool):
    def make_btag_hists(self, btag_name, btag_opts):
        self.defineHistogram('btag_'+btag_name+'_pb', title=btag_name+" PB", **btag_opts)
        self.defineHistogram('btag_'+btag_name+'_pc', title=btag_name+" PC", **btag_opts)
        self.defineHistogram('btag_'+btag_name+'_pu', title=btag_name+" PU", **btag_opts)

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


        # B-Tagging Histograms
        btag_options=TH1F_opts.copy()
        btag_options.update({'xbins':default_bin_count, 'xmin':0, 'xmax':1})
        self.make_btag_hists('IP2D', btag_options)
        self.make_btag_hists('IP3D', btag_options)
        self.make_btag_hists('SV1', btag_options)
        self.make_btag_hists('DL1r', btag_options)
        self.make_btag_hists('RNNIP', btag_options)
        self.defineHistogram('btag_MV2c10', title="MV2c10", xbins = default_bin_count, xmin=-1, xmax=1, **TH1F_opts)

        self.defineHistogram('btag_valD0wrtPVofTracks', title="Track d_{0} w/ Respect to PV of Tracks;d_{0} (mm)", xbins = default_bin_count, xmin=-2, xmax=2, **TH1F_opts)
        self.defineHistogram('btag_sigD0wrtPVofTracks', title="Track d_{0} Significance w/ Respect to PV of Tracks;d_{0} #sigma", xbins = default_bin_count, xmin=-100, xmax=100, **TH1F_opts)
        self.defineHistogram('btag_valZ0wrtPVofTracks', title="Track z_{0} w/ Respect to PV of Tracks;z_{0} (mm)", xbins = default_bin_count, xmin=-2, xmax=2, **TH1F_opts)
        self.defineHistogram('btag_sigZ0wrtPVofTracks', title="Track z_{0} Significance w/ Respect to PV of Tracks;z_{0} #sigma", xbins = default_bin_count, xmin=-100, xmax=100, **TH1F_opts)
