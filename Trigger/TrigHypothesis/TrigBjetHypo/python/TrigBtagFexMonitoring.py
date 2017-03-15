# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TrigBtagFexMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigBtagFexMonitoring"):
        super(TrigBtagFexMonitoring, self).__init__(name)

        # Run-2 histograms
        self.Histograms += [ defineHistogram('sv_mass', type='TH1F', title="BtagFex SV Mass",
                                             xbins=200, xmin=0.0, xmax=6000.0) ]
        self.Histograms += [ defineHistogram('sv_evtx', type='TH1F', title="BtagFex SV Energy Fraction",
                                             xbins=200, xmin=0.0, xmax=1.0) ]
        self.Histograms += [ defineHistogram('sv_nvtx', type='TH1F', title="BtagFex SV Two-Track Vertex Number",
                                             xbins=21, xmin=-0.5, xmax=20.5) ]

        self.Histograms += [ defineHistogram('tag_IP2D', type='TH1F', title="BtagFex IP2D Likelihood Ratio",
                                             xbins=200, xmin=-50.0, xmax=50.0) ]
        self.Histograms += [ defineHistogram('tag_IP3D', type='TH1F', title="BtagFex IP3D Likelihood Ratio",
                                             xbins=200, xmin=-50.0, xmax=50.0) ]
        self.Histograms += [ defineHistogram('tag_SV1', type='TH1F', title="BtagFex SV1 Likelihood Ratio",
                                             xbins=200, xmin=-50.0, xmax=50.0) ]
        self.Histograms += [ defineHistogram('tag_IP3DSV1', type='TH1F', title="BtagFex IP3D+SV1 Discriminant",
                                             xbins=200, xmin=-50.0, xmax=50.0) ]
        self.Histograms += [ defineHistogram('tag_MV2c00', type='TH1F', title="BtagFex MV2c00 Discriminant",
                                             xbins=200, xmin=-1., xmax=1.) ]
        self.Histograms += [ defineHistogram('tag_MV2c10', type='TH1F', title="BtagFex MV2c10 Discriminant",
                                             xbins=200, xmin=--1., xmax=1.) ]
        self.Histograms += [ defineHistogram('tag_MV2c20', type='TH1F', title="BtagFex MV2c20 Discriminant",
                                             xbins=200, xmin=-1., xmax=1.) ]



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


