# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TrigGSCFexMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigGSCFexMonitoring"):
        super(TrigGSCFexMonitoring, self).__init__(name)

        # Run-2 histograms
        #self.Histograms += [ defineHistogram('sv_mass', type='TH1F', title="GSCFex SV Mass",
        #                                     xbins=200, xmin=0.0, xmax=6000.0) ]
        #self.Histograms += [ defineHistogram('sv_evtx', type='TH1F', title="GSCFex SV Energy Fraction",
        #                                     xbins=200, xmin=0.0, xmax=1.0) ]
        #self.Histograms += [ defineHistogram('sv_nvtx', type='TH1F', title="GSCFex SV Two-Track Vertex Number",
        #                                     xbins=21, xmin=-0.5, xmax=20.5) ]
        #
        #self.Histograms += [ defineHistogram('tag_IP2D', type='TH1F', title="GSCFex IP2D Likelihood Ratio",
        #                                     xbins=200, xmin=-50.0, xmax=50.0) ]
        #self.Histograms += [ defineHistogram('tag_IP3D', type='TH1F', title="GSCFex IP3D Likelihood Ratio",
        #                                     xbins=200, xmin=-50.0, xmax=50.0) ]
        #self.Histograms += [ defineHistogram('tag_SV1', type='TH1F', title="GSCFex SV1 Likelihood Ratio",
        #                                     xbins=200, xmin=-50.0, xmax=50.0) ]
        #self.Histograms += [ defineHistogram('tag_IP3DSV1', type='TH1F', title="GSCFex IP3D+SV1 Discriminant",
        #                                     xbins=200, xmin=-50.0, xmax=50.0) ]
        #self.Histograms += [ defineHistogram('tag_MV2c00', type='TH1F', title="GSCFex MV2c00 Discriminant",
        #                                     xbins=200, xmin=-50.0, xmax=50.0) ]
        #self.Histograms += [ defineHistogram('tag_MV2c10', type='TH1F', title="GSCFex MV2c10 Discriminant",
        #                                     xbins=200, xmin=-50.0, xmax=50.0) ]
        #self.Histograms += [ defineHistogram('tag_MV2c20', type='TH1F', title="GSCFex MV2c20 Discriminant",
        #                                     xbins=200, xmin=-50.0, xmax=50.0) ]
        self.Histograms += [ defineHistogram('gsc_ntrk', type='TH1I', title="GSCFex Number of tracks",
                                             xbins=21, xmin=-0.5, xmax=30.5) ]
        self.Histograms += [ defineHistogram('gsc_width', type='TH1F', title="GSCFex Track width",
                                             xbins=200, xmin=-2.0, xmax=2.0) ]
        self.Histograms += [ defineHistogram('gsc_ptsum', type='TH1F', title="GSCFex Sum of transverse momentum of tracks",
                                             xbins=200, xmin=-100.0, xmax=600000.0) ]
        self.Histograms += [ defineHistogram('gsc_ptdiff', type='TH1F', title="GSCFex PT difference between uncal jet and cal jets",
                                             xbins=200, xmin=-100000.0, xmax=100000.0) ]
        self.Histograms += [ defineHistogram('gsc_ptratio', type='TH1F', title="GSCFex PT ratio",
                                             xbins=200, xmin=-1.0, xmax=1.0) ]

class TrigEFGSCFexValidationMonitoring(TrigGSCFexMonitoring):
    def __init__ (self, name="TrigEFGSCFexValidationMonitoring"):
        super(TrigEFGSCFexValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")
        
#        self.Histograms += [ defineHistogram('trk_a0', type='TH1F', title="transverse IP",
#                                             xbins=200, xmin=-10, xmax=10) ]
#        self.Histograms += [ defineHistogram('trk_z0', type='TH1F', title="longitudinal IP",
#                                             xbins=200, xmin=-400, xmax=400) ]
#        
#        self.Histograms += [ defineHistogram('roi_stepsToSelect', type='TH1F', title="Steps to select tracks for CHI2 tagger",
#                                             xbins=12, xmin=0.0, xmax=12,
#                                             labels='BS flag status:BS width:eta matching:phi matching:pT cut:d0 cut:z0 cut:b-layer hit cut:pixel hit cut:silicon hit cut:chi2 cut:selected') ]



class TrigEFGSCFexOnlineMonitoring(TrigGSCFexMonitoring):
    def __init__ (self, name="TrigEFGSCFexOnlineMonitoring"):
        super(TrigEFGSCFexOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")
        
#        self.Histograms += [ defineHistogram('roi_stepsToSelect', type='TH1F', title="Steps to select tracks for CHI2 tagger",
#                                             xbins=12, xmin=0.0, xmax=12,
#                                             labels='BS flag status:BS width:eta matching:phi matching:pT cut:d0 cut:z0 cut:b-layer hit cut:pixel hit cut:silicon hit cut:chi2 cut:selected') ]


