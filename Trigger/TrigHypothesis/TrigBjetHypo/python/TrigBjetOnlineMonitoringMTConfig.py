# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

class TrigBjetOnlineMonitoring(GenericMonitoringTool):
    def make_flavor_hists(self, tagger):
      for probability in ['pb','pc','pu']:
        self.defineHistogram('btag_'+tagger+'_'+probability, title=tagger+' '+probability.capitalize(), type='TH1F', path='EXPERT', xbins=100, xmin=0, xmax=1)
      self.defineHistogram('btag_'+tagger+'_llr', title=tagger+' LLR', type='TH1F', path='EXPERT', xbins=100, xmin=-10, xmax=50)


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

        # Primary Vertex Histogram
        self.defineHistogram('primVtx_x', title="Primary Vertex X", xbins = default_bin_count, xmin=-2, xmax=2, **TH1F_opts)
        self.defineHistogram('primVtx_y', title="Primary Vertex Y", xbins = default_bin_count, xmin=-2, xmax=2, **TH1F_opts)
        self.defineHistogram('primVtx_z', title="Primary Vertex Z", xbins = default_bin_count, xmin=-300, xmax=300, **TH1F_opts)

        ## Track Histograms
        self.defineHistogram('track_Et', title="Track Transverse Energy;E_{T} (GeV)", xbins = default_bin_count, xmin=0, xmax=200, **TH1F_opts)
        self.defineHistogram('track_eta', title="Track #eta;#eta", xbins = default_bin_count, xmin=-5, xmax=5, **TH1F_opts)
        self.defineHistogram('track_phi', title="Track #phi;#phi", xbins = default_bin_count, xmin=-3.5, xmax=3.5, **TH1F_opts)
        self.defineHistogram('track_eta,track_phi', path='EXPERT', type='TH2F', title="Track #eta vs #phi;#eta;#phi",
                             xbins = default_bin_count, xmin=-5, xmax=5, ybins = default_bin_count, ymin=-4, ymax=4)
        self.defineHistogram('track_d0', title="Track d_{0};d_{0} (mm)", xbins = default_bin_count, xmin=-5, xmax=5, **TH1F_opts)
        self.defineHistogram('track_d0err', title="Track d_{0} Error;d_{0} Error (mm)", xbins = default_bin_count, xmin=0, xmax=10, **TH1F_opts)
        self.defineHistogram('track_d0sig', title="Track d_{0} Significance;d_{0} #sigma", xbins = default_bin_count, xmin=-100, xmax=100, **TH1F_opts)
        self.defineHistogram('track_z0', title="Track z_{0};z_{0} (mm)", xbins = default_bin_count, xmin=-200, xmax=200, **TH1F_opts)
        self.defineHistogram('track_z0err', title="Track z_{0} Error;z_{0} Error (mm)", xbins = default_bin_count, xmin=0, xmax=10, **TH1F_opts)
        self.defineHistogram('track_z0sig', title="Track z_{0} Significance;z_{0} #sigma", xbins = default_bin_count, xmin=-1000, xmax=1000, **TH1F_opts)

        # Jet Histograms
        self.defineHistogram('jet_pt', title="Jet Transverse Momentum;p_{T} (GeV)", xbins = default_bin_count, xmin=0, xmax=1000, **TH1F_opts)
        self.defineHistogram('jet_eta', title="Jet #eta;#eta", xbins = default_bin_count, xmin=-5, xmax=5, **TH1F_opts)
        self.defineHistogram('jet_phi', title="Jet #phi;#phi", xbins = default_bin_count, xmin=-3.5, xmax=3.5, **TH1F_opts)
        self.defineHistogram('jet_eta,jet_phi', path='EXPERT', type='TH2F', title="Jet #eta vs #phi;#eta;#phi",
                             xbins = default_bin_count, xmin=-5, xmax=5, ybins = default_bin_count, ymin=-4, ymax=4)


        # B-Tagging Histograms
        for tagger in ['IP2D', 'IP3D', 'DL1r', 'rnnip']: self.make_flavor_hists(tagger)

        self.defineHistogram('btag_MV2c10', title="MV2c10", xbins = default_bin_count, xmin=-1, xmax=1, **TH1F_opts)


        aux_variables_list = [ # title, xmin, xmax, xbins, plottype
          ('JetFitter_N2Tpair', 0, 100, 100, 'TH1I'),
          ('JetFitter_nVTX', 0, 50, 50, 'TH1I'),
          ('JetFitter_nSingleTracks', 0, 50, 50, 'TH1I'),
          ('JetFitter_nTracksAtVtx', 0, 50, 50, 'TH1I'),
          ('JetFitter_mass', 0, 20000, 1000, 'TH1F'),
          ('JetFitter_energyFraction', 0, 1.1, 100, 'TH1F'),
          ('JetFitter_significance3d', 0, 100, 100, 'TH1F'),
          ('JetFitter_deltaeta', 0, 2, 100, 'TH1F'),
          ('JetFitter_deltaphi', 0, 2, 100, 'TH1F'),
          ('JetFitter_isDefaults', 0, 2, 2, 'TH1I'),
          ('JetFitter_deltaR', 0, 2, 100, 'TH1F'),

          ('SV1_NGTinSvx', 0, 100, 100, 'TH1I'),
          ('SV1_masssvx', 0, 10000, 1000, 'TH1F'),
          ('SV1_isDefaults', 0, 2, 2, 'TH1I'),
          ('SV1_N2Tpair', 0, 50, 50, 'TH1I'),
          ('SV1_efracsvx', 0, 1.1, 100, 'TH1F'),
          ('SV1_deltaR', 0, 2, 100, 'TH1F'),
          ('SV1_Lxy', 0, 100, 100, 'TH1F'),
          ('SV1_L3d', 0, 100, 100, 'TH1F'),
          ('SV1_significance3d', 0, 50, 100, 'TH1F'),

          ('IP2D_isDefaults', 0, 2, 2, 'TH1I'),
          ('IP2D_bu', 0, 50, 100, 'TH1F'),
          ('IP2D_bc', 0, 50, 100, 'TH1F'),
          ('IP2D_cu', 0, 50, 100, 'TH1F'),
          ('IP3D_isDefaults', 0, 2, 2, 'TH1I'),
          ('IP3D_bu', 0, 50, 100, 'TH1F'),
          ('IP3D_bc', 0, 50, 100, 'TH1F'),
          ('IP3D_cu', 0, 50, 100, 'TH1F'),
          
          ('JetFitterSecondaryVertex_nTracks', 0, 50, 50, 'TH1I'),
          ('JetFitterSecondaryVertex_isDefaults', 0, 2, 2, 'TH1I'),
          ('JetFitterSecondaryVertex_mass', 0, 1e4, 100, 'TH1F'),
          ('JetFitterSecondaryVertex_energy', 0, 1e6, 100, 'TH1F'),
          ('JetFitterSecondaryVertex_energyFraction', 0, 1.1, 100, 'TH1F'),
          ('JetFitterSecondaryVertex_displacement3d', 0, 500, 100, 'TH1F'),
          ('JetFitterSecondaryVertex_displacement2d', 0, 500, 100, 'TH1F'),
          ('JetFitterSecondaryVertex_maximumTrackRelativeEta', 0, 8, 100, 'TH1F'),
          ('JetFitterSecondaryVertex_minimumTrackRelativeEta', 0, 8, 100, 'TH1F'),
          ('JetFitterSecondaryVertex_averageTrackRelativeEta', 0, 8, 100, 'TH1F'),

          ('maximumTrackRelativeEta', 0, 8, 100, 'TH1F'),
          ('minimumTrackRelativeEta', 0, 8, 100, 'TH1F'),
          ('averageTrackRelativeEta', 0, 8, 100, 'TH1F')
        ]

        for title, xmin, xmax, xbins, plottype in aux_variables_list:
            self.defineHistogram(title, title=title, xmin=xmin, xmax=xmax, xbins=xbins, type=plottype, path='EXPERT')

        self.defineHistogram('IP3D_valD0wrtPVofTracks', title="Track d_{0} w/ Respect to PV of Tracks of IP3D;d_{0} (mm)", xmin=-2, xmax=2, xbins = default_bin_count, **TH1F_opts)
        self.defineHistogram('IP3D_sigD0wrtPVofTracks', title="Track d_{0} Significance w/ Respect to PV of Tracks of IP3D;d_{0} #sigma", xmin=-100, xmax=100, xbins = default_bin_count, **TH1F_opts)
        self.defineHistogram('IP3D_valZ0wrtPVofTracks', title="Track z_{0} w/ Respect to PV of Tracks of IP3D;z_{0} (mm)", xmin=-2, xmax=2, xbins = default_bin_count, **TH1F_opts)
        self.defineHistogram('IP3D_sigZ0wrtPVofTracks', title="Track z_{0} Significance w/ Respect to PV of Tracks of IP3D;z_{0} #sigma", xmin=-100, xmax=100, xbins = default_bin_count, **TH1F_opts)

