# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TrigBjetFexMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigBjetFexMonitoring"):
        super(TrigBjetFexMonitoring, self).__init__(name)

        self.Histograms += [ defineHistogram('X(IP1D)', type='TH1F', title="Bjet Fex X(IP1D)",
                                             xbins=200, xmin=-50.0, xmax=50.0) ]
        self.Histograms += [ defineHistogram('X(IP2D)', type='TH1F', title="Bjet Fex X(IP2D)",
                                             xbins=200, xmin=-50.0, xmax=50.0) ]
        self.Histograms += [ defineHistogram('X(IP3D)', type='TH1F', title="Bjet Fex X(IP3D)",
                                             xbins=200, xmin=-50.0, xmax=50.0) ]
        self.Histograms += [ defineHistogram('X(CHI2)', type='TH1F', title="Bjet Fex X(CHI2)",
                                             xbins=60, xmin=-0.5, xmax=1.0) ]
        self.Histograms += [ defineHistogram('X(SVTX)', type='TH1F', title="Bjet Fex X(SVTX)",
                                             xbins=200, xmin=-50.0, xmax=50.0) ]
        self.Histograms += [ defineHistogram('X(COMB)', type='TH1F', title="Bjet Fex X(COMB)",
                                             xbins=200, xmin=-50.0, xmax=50.0) ]
        
        self.Histograms += [ defineHistogram('trk_a0_sel', type='TH1F', title="transverse IP for selected tracks",
                                             xbins=200, xmin=-10, xmax=10) ]
        self.Histograms += [ defineHistogram('trk_S(a0)_sel', type='TH1F', title="transverse IP significance for selected tracks",
                                             xbins=400, xmin=-20.0, xmax=20.0) ]
        
        self.Histograms += [ defineHistogram('trk_z0_sel', type='TH1F', title="longitudinal IP for selected tracks",
                                             xbins=200, xmin=-400, xmax=400) ]
        self.Histograms += [ defineHistogram('trk_z0_sel_PV', type='TH1F', title="longitudinal IP w.r.t. primary vertex for selected tracks",
                                             xbins=200, xmin=-20, xmax=20) ]
        self.Histograms += [ defineHistogram('trk_S(z0)_sel', type='TH1F', title="longitudinal IP significance for selected tracks",
                                             xbins=400, xmin=-20.0, xmax=20.0) ]
        
        self.Histograms += [ defineHistogram('trk_prob', type='TH1F', title="track probability estimated by JetProb",
                                             xbins=40, xmin=0.0, xmax=1.0) ]
        
        self.Histograms += [ defineHistogram('roi_nTracks', type='TH1F', title="Number of tracks per RoI before track selection",
                                             xbins=50, xmin=-0.5, xmax=49.5) ]
        self.Histograms += [ defineHistogram('roi_nTracks_sel', type='TH1F', title="Number of tracks per RoI after track selection",
                                             xbins=50, xmin=-0.5, xmax=49.5) ]
        self.Histograms += [ defineHistogram('roi_selectedTracks', type='TH1F', title="Selected tracks for CHI2 tagger (in percentage)",
                                             xbins=20, xmin=-0.2, xmax=1.1) ]
        
        self.Histograms += [ defineHistogram('roi_deltaEtaJet', type='TH1F', title="Delta eta between the LVL1 jet RoI and the HLT jet",
                                             xbins=40, xmin=-1.0, xmax=1.0) ]
        self.Histograms += [ defineHistogram('roi_deltaPhiJet', type='TH1F', title="Delta phi between the LVL1 jet RoI and the HLT jet",
                                             xbins=40, xmin=-1.0, xmax=1.0) ]
        self.Histograms += [ defineHistogram('roi_deltaEtaTrkJet', type='TH1F', title="Delta eta between the LVL1 jet RoI and the HLT track-jet",
                                             xbins=40, xmin=-1.0, xmax=1.0) ]
        self.Histograms += [ defineHistogram('roi_deltaPhiTrkJet', type='TH1F', title="Delta phi between the LVL1 jet RoI and the HLT track-jet",
                                             xbins=40, xmin=-1.0, xmax=1.0) ]
        self.Histograms += [ defineHistogram('roi_deltaEtaJetTrkJet', type='TH1F', title="Delta eta between the HLT jet and the HLT track-jet",
                                             xbins=40, xmin=-1.0, xmax=1.0) ]
        self.Histograms += [ defineHistogram('roi_deltaPhiJetTrkJet', type='TH1F', title="Delta phi between the HLT jet and the HLT track-jet",
                                             xbins=40, xmin=-1.0, xmax=1.0) ]

        # Run-2 histograms
        self.Histograms += [ defineHistogram('sv_mass', type='TH1F', title="BjetFex SV Mass",
                                             xbins=200, xmin=0.0, xmax=6000.0) ]
        self.Histograms += [ defineHistogram('sv_evtx', type='TH1F', title="BjetFex SV Energy Fraction",
                                             xbins=200, xmin=0.0, xmax=1.0) ]
        self.Histograms += [ defineHistogram('sv_nvtx', type='TH1F', title="BjetFex SV Two-Track Vertex Number",
                                             xbins=21, xmin=-0.5, xmax=20.5) ]

        self.Histograms += [ defineHistogram('tag_IP2D', type='TH1F', title="BjetFex IP2D Likelihood Ratio",
                                             xbins=200, xmin=-50.0, xmax=50.0) ]
        self.Histograms += [ defineHistogram('tag_IP3D', type='TH1F', title="BjetFex IP3D Likelihood Ratio",
                                             xbins=200, xmin=-50.0, xmax=50.0) ]
        self.Histograms += [ defineHistogram('tag_SV1', type='TH1F', title="BjetFex SV1 Likelihood Ratio",
                                             xbins=200, xmin=-50.0, xmax=50.0) ]
        self.Histograms += [ defineHistogram('tag_IP3DSV1', type='TH1F', title="BjetFex IP3D+SV1 Discriminant",
                                             xbins=200, xmin=-50.0, xmax=50.0) ]
        self.Histograms += [ defineHistogram('tag_MV2c00', type='TH1F', title="BjetFex MV2c00 Discriminant",
                                             xbins=200, xmin=-50.0, xmax=50.0) ]
        self.Histograms += [ defineHistogram('tag_MV2c10', type='TH1F', title="BjetFex MV2c10 Discriminant",
                                             xbins=200, xmin=-50.0, xmax=50.0) ]
        self.Histograms += [ defineHistogram('tag_MV2c20', type='TH1F', title="BjetFex MV2c20 Discriminant",
                                             xbins=200, xmin=-50.0, xmax=50.0) ]


class TrigEFBjetFexValidationMonitoring(TrigBjetFexMonitoring):
    def __init__ (self, name="TrigEFBjetFexValidationMonitoring"):
        super(TrigEFBjetFexValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")
        
        self.Histograms += [ defineHistogram('trk_a0', type='TH1F', title="transverse IP",
                                             xbins=200, xmin=-10, xmax=10) ]
        self.Histograms += [ defineHistogram('trk_z0', type='TH1F', title="longitudinal IP",
                                             xbins=200, xmin=-400, xmax=400) ]
        
        self.Histograms += [ defineHistogram('roi_stepsToSelect', type='TH1F', title="Steps to select tracks for CHI2 tagger",
                                             xbins=12, xmin=0.0, xmax=12,
                                             labels='BS flag status:BS width:eta matching:phi matching:pT cut:d0 cut:z0 cut:b-layer hit cut:pixel hit cut:silicon hit cut:chi2 cut:selected') ]



class TrigEFBjetFexOnlineMonitoring(TrigBjetFexMonitoring):
    def __init__ (self, name="TrigEFBjetFexOnlineMonitoring"):
        super(TrigEFBjetFexOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")
        
        self.Histograms += [ defineHistogram('roi_stepsToSelect', type='TH1F', title="Steps to select tracks for CHI2 tagger",
                                             xbins=12, xmin=0.0, xmax=12,
                                             labels='BS flag status:BS width:eta matching:phi matching:pT cut:d0 cut:z0 cut:b-layer hit cut:pixel hit cut:silicon hit cut:chi2 cut:selected') ]


