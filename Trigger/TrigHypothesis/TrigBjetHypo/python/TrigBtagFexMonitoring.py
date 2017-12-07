# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TrigBtagFexMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigBtagFexMonitoring"):
        super(TrigBtagFexMonitoring, self).__init__(name)

        # Run-2 histograms
        self.Histograms += [ defineHistogram('sv1_mass', type='TH1F', title="BtagFex SV Mass",
                                             xbins=200, xmin=0.0, xmax=6000.0) ] 
        self.Histograms += [ defineHistogram('sv1_evtx', type='TH1F', title="BtagFex SV Energy Fraction",
                                             xbins=200, xmin=0.0, xmax=1.0) ]
        self.Histograms += [ defineHistogram('sv1_nvtx', type='TH1F', title="BtagFex SV Two-Track Vertex Number",
                                             xbins=31, xmin=-0.5, xmax=30.5) ]
        self.Histograms += [ defineHistogram('sv1_ntkv', type='TH1F', title="BtagFex SV Number of tracks used in the SV",
                                             xbins=21, xmin=-0.5, xmax=20.5) ]
        self.Histograms += [ defineHistogram('sv1_sig3', type='TH1F', title="BtagFex SV 3D Vertex Significance",
                                             xbins=210, xmin=0.0, xmax=210.0) ]
        self.Histograms += [ defineHistogram('sv1_Lxy', type='TH1F', title="BtagFex SV Transverse distance between PV and SV",
                                             xbins=220, xmin=0.0, xmax=110.0) ]
        self.Histograms += [ defineHistogram('sv1_L3d', type='TH1F', title="BtagFex SV Distance between PV and SV",
                                             xbins=220, xmin=0.0, xmax=110.0) ]
        self.Histograms += [ defineHistogram('sv1_dR', type='TH1F', title="BtagFex SV delta R",
                                             xbins=220, xmin=0.0, xmax=1.1) ]

        self.Histograms += [ defineHistogram('tag_IP2D', type='TH1F', title="BtagFex IP2D Likelihood Ratio",
                                             xbins=200, xmin=-30.0, xmax=40.0) ]
        self.Histograms += [ defineHistogram('tag_IP3D', type='TH1F', title="BtagFex IP3D Likelihood Ratio",
                                             xbins=200, xmin=-30.0, xmax=40.0) ]
        self.Histograms += [ defineHistogram('tag_SV1', type='TH1F', title="BtagFex SV1 Likelihood Ratio",
                                             xbins=200, xmin=-20.0, xmax=30.0) ]
        self.Histograms += [ defineHistogram('tag_IP3DSV1', type='TH1F', title="BtagFex IP3D+SV1 Discriminant",
                                             xbins=200, xmin=-20.0, xmax=50.0) ] 
        #   self.Histograms += [ defineHistogram('tag_MV2c00', type='TH1F', title="BtagFex MV2c00 Discriminant",
        #                                        xbins=200, xmin=-1., xmax=1.) ]
        self.Histograms += [ defineHistogram('tag_MV2c10', type='TH1F', title="BtagFex MV2c10 Discriminant",
                                             xbins=200, xmin=-1., xmax=1.) ]
        self.Histograms += [ defineHistogram('tag_MV2c20', type='TH1F', title="BtagFex MV2c20 Discriminant",
                                             xbins=200, xmin=-1., xmax=1.) ]  
        self.Histograms += [ defineHistogram('tag_IP2_c', type='TH1F', title="BtagFex IP2D Likelihood Ratio between b- and c-jet",
                                             xbins=200, xmin=-30.0, xmax=30.0) ]
        self.Histograms += [ defineHistogram('tag_IP2_cu', type='TH1F', title="BtagFex IP2D Likelihood Ratio between c- and light jet",
                                             xbins=200, xmin=-30.0, xmax=30.0) ]
        self.Histograms += [ defineHistogram('tag_IP3_c', type='TH1F', title="BtagFex IP3D Likelihood Ratio between b- and c-jet",
                                             xbins=200, xmin=-30.0, xmax=30.0) ]
        self.Histograms += [ defineHistogram('tag_IP3_cu', type='TH1F', title="BtagFex IP3D Likelihood Ratio between c- and light jet",
                                             xbins=200, xmin=-30.0, xmax=30.0) ]

        #MV2 taggers 
        self.Histograms += [ defineHistogram('jf_n2tv', type='TH1F', title="BtagFex JetFitter Number of 2-track vertex",
                                             xbins=21, xmin=-0.5, xmax=20.5) ]
        self.Histograms += [ defineHistogram('jf_ntrkv', type='TH1F', title="BtagFex JetFitter Number of tracks from displaced vertices",
                                             xbins=16, xmin=-0.5, xmax=15.5) ]
        self.Histograms += [ defineHistogram('jf_nvtx', type='TH1F', title="BtagFex JetFitter Number of displaced vertices ",
                                             xbins=11, xmin=-0.5, xmax=10.5) ]
        self.Histograms += [ defineHistogram('jf_nvtx1t', type='TH1F', title="BtagFex JetFitter Number of displaced vertices wih 1-track",
                                             xbins=11, xmin=-0.5, xmax=10.5) ]
        self.Histograms += [ defineHistogram('jf_mass', type='TH1F', title="BtagFex JetFitter Jet mass",
                                             xbins=200, xmin=0.0, xmax=9100.0) ]
        self.Histograms += [ defineHistogram('jf_efrc', type='TH1F', title="BtagFex JetFitter Jet efrac ",
                                             xbins=220, xmin=0.0, xmax=1.1) ]  
        self.Histograms += [ defineHistogram('jf_dR', type='TH1F', title="BtagFex JetFitter Delta R ",
                                             xbins=200, xmin=-1.0, xmax=1.0) ]
        self.Histograms += [ defineHistogram('jf_sig3', type='TH1F', title="BtagFex JetFitter 3D vertex significance ",
                                             xbins=200, xmin=-0.0, xmax=100.0) ]
        self.Histograms += [ defineHistogram('jet_pt', type='TH1F', title="BtagFex Jet PT",
                                             xbins=200, xmin=0.0, xmax=800000.0) ]
        self.Histograms += [ defineHistogram('jet_eta', type='TH1F', title="BtagFex Jet Eta ",
                                             xbins=200, xmin=-4.9, xmax=4.9) ]
        self.Histograms += [ defineHistogram('sv_bool', type='TH1F', title="BtagFex Boolean if SV is found",
                                             xbins=3, xmin=-0.5, xmax=2.5,
                                             labels='sv_ntkv == -1:sv_ntkv == 1:sv_ntkv > 1') ]
        self.Histograms += [ defineHistogram('jf_bool', type='TH1F', title="BtagFex Boolean of jf_dR",
                                             xbins=2, xmin=-0.5, xmax=1.5,
                                             labels='jf_dR == -1:jf_dR != -1') ]

class TrigEFBtagFexValidationMonitoring(TrigBtagFexMonitoring):
    def __init__ (self, name="TrigEFBtagFexValidationMonitoring"):
        super(TrigEFBtagFexValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")
        
#        self.Histograms += [ defineHistogram('trk_a0', type='TH1F', title="transverse IP",
#                                             xbins=200, xmin=-10, xmax=10) ]
#        self.Histograms += [ defineHistogram('trk_z0', type='TH1F', title="longitudinal IP",
#                                             xbins=200, xmin=-400, xmax=400) ]
#        
#        self.Histograms += [ defineHistogram('roi_stepsToSelect', type='TH1F', title="Steps to select tracks for CHI2 tagger",
#                                             xbins=12, xmin=0.0, xmax=12,
#                                             labels='BS flag status:BS width:eta matching:phi matching:pT cut:d0 cut:z0 cut:b-layer hit cut:pixel hit cut:silicon hit cut:chi2 cut:selected') ]



class TrigEFBtagFexOnlineMonitoring(TrigBtagFexMonitoring):
    def __init__ (self, name="TrigEFBtagFexOnlineMonitoring"):
        super(TrigEFBtagFexOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")
        
#        self.Histograms += [ defineHistogram('roi_stepsToSelect', type='TH1F', title="Steps to select tracks for CHI2 tagger",
#                                             xbins=12, xmin=0.0, xmax=12,
#                                             labels='BS flag status:BS width:eta matching:phi matching:pT cut:d0 cut:z0 cut:b-layer hit cut:pixel hit cut:silicon hit cut:chi2 cut:selected') ]


