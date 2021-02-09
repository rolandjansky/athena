# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

class TrigBjetOnlineMonitoring(GenericMonitoringTool):
    def make_flavor_hists(self, tagger):
        self.defineHistogram('btag_'+tagger+'_pb', title=tagger+': Probability jets are B-jets', type='TH1F', path='EXPERT', xbins=100, xmin=0, xmax=1)
        self.defineHistogram('btag_'+tagger+'_pc', title=tagger+': Probability jets are Charm-jets', type='TH1F', path='EXPERT', xbins=100, xmin=0, xmax=1)
        self.defineHistogram('btag_'+tagger+'_pu', title=tagger+': Probability jets are Light-jets', type='TH1F', path='EXPERT', xbins=100, xmin=0, xmax=1)
        self.defineHistogram('btag_'+tagger+'_llr', title=tagger+': Log(P_{b}/P_{light}), Likelihood Ratio between the B-jet and Light-flavour Jet Hypotheses', type='TH1F', path='EXPERT', xbins=100, xmin=-10, xmax=50)


    def __init__ (self, name="TrigBjetOnlineMonitoring"):
        super(TrigBjetOnlineMonitoring, self).__init__(name)
        self.name = "TrigBjetOnlineMonitoring"
        self.HistPath = self.name
        default_bin_count = 100

        # Event Histograms
        self.defineHistogram('track_count', title="Number of Tracks per Trigger Decision", xbins = 100, xmin=0, xmax=100, path='EXPERT', type='TH1I')
        self.defineHistogram('jet_count', title="Number of Jets Considered for B-Tagging", xbins = 20, xmin=0, xmax=20, path='EXPERT', type='TH1I')
        self.defineHistogram('vertex_count', title="Number of Primary Vertex Candidates per Event", xbins = 200, xmin=0, xmax=200, path='EXPERT', type='TH1I')

        # Primary Vertex Histogram
        self.defineHistogram('primVtx_x', title="Primary Vertex X", xbins = default_bin_count, xmin=-2, xmax=2, path='EXPERT', type='TH1F')
        self.defineHistogram('primVtx_y', title="Primary Vertex Y", xbins = default_bin_count, xmin=-2, xmax=2, path='EXPERT', type='TH1F')
        self.defineHistogram('primVtx_z', title="Primary Vertex Z", xbins = default_bin_count, xmin=-300, xmax=300, path='EXPERT', type='TH1F')

        ## Track Histograms
        self.defineHistogram('track_Et', title="Track Transverse Energy;E_{T} (GeV)", xbins = default_bin_count, xmin=0, xmax=200, path='EXPERT', type='TH1F')
        self.defineHistogram('track_eta', title="Track #eta;#eta", xbins = default_bin_count, xmin=-5, xmax=5, path='EXPERT', type='TH1F')
        self.defineHistogram('track_phi', title="Track #phi;#phi", xbins = default_bin_count, xmin=-3.5, xmax=3.5, path='EXPERT', type='TH1F')
        self.defineHistogram('track_eta,track_phi', path='EXPERT', type='TH2F', title="Track #eta vs #phi;#eta;#phi",
                             xbins = default_bin_count, xmin=-5, xmax=5, ybins = default_bin_count, ymin=-4, ymax=4)
        self.defineHistogram('track_d0', title="Track d_{0};d_{0} (mm)", xbins = default_bin_count, xmin=-5, xmax=5, path='EXPERT', type='TH1F')
        self.defineHistogram('track_d0err', title="Track d_{0} Error;d_{0} Error (mm)", xbins = default_bin_count, xmin=0, xmax=10, path='EXPERT', type='TH1F')
        self.defineHistogram('track_d0sig', title="Track d_{0} Significance;d_{0} #sigma", xbins = default_bin_count, xmin=-100, xmax=100, path='EXPERT', type='TH1F')
        self.defineHistogram('track_z0', title="Track z_{0};z_{0} (mm)", xbins = default_bin_count, xmin=-200, xmax=200, path='EXPERT', type='TH1F')
        self.defineHistogram('track_z0err', title="Track z_{0} Error;z_{0} Error (mm)", xbins = default_bin_count, xmin=0, xmax=10, path='EXPERT', type='TH1F')
        self.defineHistogram('track_z0sig', title="Track z_{0} Significance;z_{0} #sigma", xbins = default_bin_count, xmin=-1000, xmax=1000, path='EXPERT', type='TH1F')

        # Jet Histograms
        self.defineHistogram('jet_pt', title="Jet Transverse Momentum;p_{T} (GeV)", xbins = default_bin_count, xmin=0, xmax=1000, path='EXPERT', type='TH1F')
        self.defineHistogram('jet_eta', title="Jet #eta;#eta", xbins = default_bin_count, xmin=-5, xmax=5, path='EXPERT', type='TH1F')
        self.defineHistogram('jet_phi', title="Jet #phi;#phi", xbins = default_bin_count, xmin=-3.5, xmax=3.5, path='EXPERT', type='TH1F')
        self.defineHistogram('jet_eta,jet_phi', path='EXPERT', type='TH2F', title="Jet #eta vs #phi;#eta;#phi",
                             xbins = default_bin_count, xmin=-5, xmax=5, ybins = default_bin_count, ymin=-4, ymax=4)

        self.defineHistogram('Bjet_pt', title="B-Tagged Jet Transverse Momentum;p_{T} (GeV)", xbins = default_bin_count, xmin=0, xmax=1000, path='EXPERT', type='TH1F')
        self.defineHistogram('Bjet_eta', title="B-Tagged Jet #eta;#eta", xbins = default_bin_count, xmin=-5, xmax=5, path='EXPERT', type='TH1F')
        self.defineHistogram('Bjet_phi', title="B-Tagged Jet #phi;#phi", xbins = default_bin_count, xmin=-3.5, xmax=3.5, path='EXPERT', type='TH1F')
        self.defineHistogram('Bjet_eta,Bjet_phi', path='EXPERT', type='TH2F', title="B-Tagged Jet #eta vs #phi;#eta;#phi",
                             xbins = default_bin_count, xmin=-5, xmax=5, ybins = default_bin_count, ymin=-4, ymax=4)

        self.defineHistogram('jet_bjet_delta_pt', title="Transverse Momentum Difference Between Jet and B-Tagged Jet;#Delta p_{T} (GeV)", xbins = default_bin_count, xmin=0, xmax=1000, path='EXPERT', type='TH1F')
        self.defineHistogram('jet_bjet_delta_eta', title="#eta Difference Between Jet and B-Tagged Jet;#Delta #eta", xbins = default_bin_count, xmin=-5, xmax=5, path='EXPERT', type='TH1F')
        self.defineHistogram('jet_bjet_delta_phi', title="#phi Difference Between Jet and B-Tagged Jet;#phi", xbins = default_bin_count, xmin=-3.5, xmax=3.5, path='EXPERT', type='TH1F')
        self.defineHistogram('jet_bjet_delta_eta,jet_phi', path='EXPERT', type='TH2F', title="#Delta #eta vs #Delta #phi for Jet and B-Tagged Jet;#Delta #eta;#Delta #phi",
                             xbins = default_bin_count, xmin=-5, xmax=5, ybins = default_bin_count, ymin=-4, ymax=4)


        # B-Tagging Histograms
        for tagger in ['IP2D', 'IP3D', 'DL1r', 'rnnip']: self.make_flavor_hists(tagger)

        self.defineHistogram('MV2c10_discriminant', title="MV2c10 Score", xbins = default_bin_count, xmin=-1, xmax=1, path='EXPERT', type='TH1F')


        self.defineHistogram('JetFitter_N2Tpair',
            title='JetFitter: Number of 2-Track Pairs',
            path='EXPERT', type='TH1I', xbins=100, xmin=0, xmax=100)
        self.defineHistogram('JetFitter_nVTX',
            title='JetFitter: Number of Vertices Used',
            path='EXPERT', type='TH1I', xbins=50, xmin=0, xmax=50)
        self.defineHistogram('JetFitter_nSingleTracks',
            title='JetFitter: Number of Single Tracks',
            path='EXPERT', type='TH1I', xbins=50, xmin=0, xmax=50)
        self.defineHistogram('JetFitter_nTracksAtVtx',
            title='JetFitter: Number of Tracks Associated with Vertex',
            path='EXPERT', type='TH1I', xbins=50, xmin=0, xmax=50)
        self.defineHistogram('JetFitter_mass',
            title='JetFitter: Invariant Mass of All Tracks Associated to Vertex',
            path='EXPERT', type='TH1F', xbins=1000, xmin=0, xmax=20000)
        self.defineHistogram('JetFitter_energyFraction',
            title='JetFitter: Fraction of Charged Jet Energy in Secondary Vertices',
            path='EXPERT', type='TH1F', xbins=100, xmin=0, xmax=1.1)
        self.defineHistogram('JetFitter_significance3d',
            title='JetFitter: Significance-3D',
            path='EXPERT', type='TH1F', xbins=100, xmin=0, xmax=100)
        self.defineHistogram('JetFitter_deltaeta',
            title='JetFitter: #Delta #eta Between Jet and Track Momentum Sum',
            path='EXPERT', type='TH1F', xbins=100, xmin=0, xmax=2)
        self.defineHistogram('JetFitter_deltaphi',
            title='JetFitter: #Delta #phi Between Jet and Track Momentum Sum',
            path='EXPERT', type='TH1F', xbins=100, xmin=0, xmax=2)
        self.defineHistogram('JetFitter_isDefaults',
            title='JetFitter: Fraction of Defaulted Jets; 1 -> Jet Has Defaulted, 0 -> Jet is Valid',
            path='EXPERT', type='TH1I', xbins=2, xmin=0, xmax=2)
        self.defineHistogram('JetFitter_deltaR',
            title='JetFitter: #Delta R Between Jet and Track Momentum Sum',
            path='EXPERT', type='TH1F', xbins=100, xmin=0, xmax=2)

        self.defineHistogram('SV1_NGTinSvx',
            title='SV1: Number of "Good" Tracks in Vertex',
            path='EXPERT', type='TH1I', xbins=100, xmin=0, xmax=100)
        self.defineHistogram('SV1_masssvx',
            title='SV1: Invariant Mass of All Tracks Associated to Vertex',
            path='EXPERT', type='TH1F', xbins=1000, xmin=0, xmax=10000)
        self.defineHistogram('SV1_isDefaults',
            title='SV1: Fraction of Defaulted Jets; 1 -> Jet Has Defaulted, 0 -> Jet is Valid',
            path='EXPERT', type='TH1I', xbins=2, xmin=0, xmax=2)
        self.defineHistogram('SV1_N2Tpair',
            title='SV1: Number of 2-Track Pairs',
            path='EXPERT', type='TH1I', xbins=50, xmin=0, xmax=50)
        self.defineHistogram('SV1_efracsvx',
            title='SV1: Ratio of Energy in Vertex Tracks to All Tracks in Jet ',
            path='EXPERT', type='TH1F', xbins=100, xmin=0, xmax=1.1)
        self.defineHistogram('SV1_deltaR',
            title='SV1: #Delta R Between Jet and Track Momentum Sum',
            path='EXPERT', type='TH1F', xbins=100, xmin=0, xmax=2)
        self.defineHistogram('SV1_Lxy',
            title='SV1: Transverse Distance Between Primary and Secondary Vertices',
            path='EXPERT', type='TH1F', xbins=100, xmin=0, xmax=100)
        self.defineHistogram('SV1_L3d',
            title='SV1: Total Distance Between Primary and Secondary Vertices',
            path='EXPERT', type='TH1F', xbins=100, xmin=0, xmax=100)
        self.defineHistogram('SV1_significance3d',
            title='SV1: Significance-3D',
            path='EXPERT', type='TH1F', xbins=100, xmin=0, xmax=50)

        self.defineHistogram('IP2D_isDefaults',
            title='IP2D_isDefaults',
            path='EXPERT', type='TH1I', xbins=2, xmin=0, xmax=2)
        self.defineHistogram('IP2D_bu',
            title='IP2D: Log(P_{b}/P_{light}), Likelihood ratio between the b-jet and light-flavour jet hypotheses',
            path='EXPERT', type='TH1F', xbins=100, xmin=-10, xmax=50)
        self.defineHistogram('IP2D_bc',
            title='IP2D: Log(P_{b}/P_{c}), Likelihood ratio between the b-jet and c-jet hypotheses',
            path='EXPERT', type='TH1F', xbins=100, xmin=-10, xmax=50)
        self.defineHistogram('IP2D_cu',
            title='IP2D: Log(P_{c}/P_{light}), Likelihood ratio between the c-jet and light-flavour jet hypotheses',
            path='EXPERT', type='TH1F', xbins=100, xmin=-10, xmax=50)

        self.defineHistogram('IP3D_isDefaults',
            title='IP3D_isDefaults',
            path='EXPERT', type='TH1I', xbins=2, xmin=0, xmax=2)
        self.defineHistogram('IP3D_bu',
            title='IP3D: Log(P_{b}/P_{light}), Likelihood ratio between the b-jet and light-flavour jet hypotheses',
            path='EXPERT', type='TH1F', xbins=100, xmin=-10, xmax=50)
        self.defineHistogram('IP3D_bc',
            title='IP3D: Log(P_{b}/P_{c}), Likelihood ratio between the b-jet and c-jet hypotheses',
            path='EXPERT', type='TH1F', xbins=100, xmin=-10, xmax=50)
        self.defineHistogram('IP3D_cu',
            title='IP3D: Log(P_{c}/P_{light}), Likelihood ratio between the c-jet and light-flavour jet hypotheses',
            path='EXPERT', type='TH1F', xbins=100, xmin=-10, xmax=50)
                  
        self.defineHistogram('JetFitterSecondaryVertex_nTracks',
            title='JFSV: Number of Tracks',
            path='EXPERT', type='TH1I', xbins=50, xmin=0, xmax=50)
        self.defineHistogram('JetFitterSecondaryVertex_isDefaults',
            title='JFSV: Fraction of Defaulted Jets; 1 -> Jet Has Defaulted, 0 -> Jet is Valid',
            path='EXPERT', type='TH1I', xbins=2, xmin=0, xmax=2)
        self.defineHistogram('JetFitterSecondaryVertex_mass',
            title='JFSV: Invariant Mass of All Tracks Associated to Vertex',
            path='EXPERT', type='TH1F', xbins=100, xmin=0, xmax=1e4)
        self.defineHistogram('JetFitterSecondaryVertex_energy',
            title='JFSV: Energy of All Tracks Associated to Vertex',
            path='EXPERT', type='TH1F', xbins=100, xmin=0, xmax=1e6)
        self.defineHistogram('JetFitterSecondaryVertex_energyFraction',
            title='JFSV: Fraction of Charged Jet Energy in Secondary Vertices',
            path='EXPERT', type='TH1F', xbins=100, xmin=0, xmax=1.1)
        self.defineHistogram('JetFitterSecondaryVertex_displacement3d',
            title='JFSV: Total Distance Between Primary and Secondary Vertices',
            path='EXPERT', type='TH1F', xbins=100, xmin=0, xmax=500)
        self.defineHistogram('JetFitterSecondaryVertex_displacement2d',
            title='JFSV: Transverse Distance Between Primary and Secondary Vertices',
            path='EXPERT', type='TH1F', xbins=100, xmin=0, xmax=500)
        self.defineHistogram('JetFitterSecondaryVertex_maximumTrackRelativeEta',
            title='JFSV: Max #eta Between Track and Jet for Tracks from the SV',
            path='EXPERT', type='TH1F', xbins=100, xmin=0, xmax=8)
        self.defineHistogram('JetFitterSecondaryVertex_minimumTrackRelativeEta',
            title='JFSV: Min #eta Between Track and Jet Vector for Tracks from the SV',
            path='EXPERT', type='TH1F', xbins=100, xmin=0, xmax=8)
        self.defineHistogram('JetFitterSecondaryVertex_averageTrackRelativeEta',
            title='JFSV: Average #eta Between Track and Jet Vector for Tracks from the SV',
            path='EXPERT', type='TH1F', xbins=100, xmin=0, xmax=8)
        self.defineHistogram('JetFitterSecondaryVertex_maximumAllJetTrackRelativeEta',
            title='JFSV: Maximum #eta Between Track and Jet Vector for All Tracks from the Jet',
            path='EXPERT', type='TH1F', xbins=100, xmin=0, xmax=8)
        self.defineHistogram('JetFitterSecondaryVertex_minimumAllJetTrackRelativeEta',
            title='JFSV: Minimum #eta Between Track and Jet Vector for All Tracks from the Jet',
            path='EXPERT', type='TH1F', xbins=100, xmin=0, xmax=8)
        self.defineHistogram('JetFitterSecondaryVertex_averageAllJetTrackRelativeEta',
            title='JFSV: Average #eta Between Track and Jet Vector for All Tracks from the Jet',
            path='EXPERT', type='TH1F', xbins=100, xmin=0, xmax=8)


        self.defineHistogram('IP3D_valD0wrtPVofTracks', title="Track d_{0} w/ Respect to PV of Tracks of IP3D;d_{0} (mm)", xmin=-2, xmax=2, xbins = default_bin_count, path='EXPERT', type='TH1F')
        self.defineHistogram('IP3D_sigD0wrtPVofTracks', title="Track d_{0} Significance w/ Respect to PV of Tracks of IP3D;d_{0} #sigma", xmin=-100, xmax=100, xbins = default_bin_count, path='EXPERT', type='TH1F')
        self.defineHistogram('IP3D_valZ0wrtPVofTracks', title="Track z_{0} w/ Respect to PV of Tracks of IP3D;z_{0} (mm)", xmin=-2, xmax=2, xbins = default_bin_count, path='EXPERT', type='TH1F')
        self.defineHistogram('IP3D_sigZ0wrtPVofTracks', title="Track z_{0} Significance w/ Respect to PV of Tracks of IP3D;z_{0} #sigma", xmin=-100, xmax=100, xbins = default_bin_count, path='EXPERT', type='TH1F')

