# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig


class TrigT2HistoPrmVtxAllTEValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigT2HistoPrmVtxAllTEValidationMonitoring"):
        super(TrigT2HistoPrmVtxAllTEValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")

        self.Histograms  = [ defineHistogram('PrmVtx', type='TH1F', title="Primary vertex",
                                             xbins=250, xmin=-200, xmax=200) ]
        self.Histograms += [ defineHistogram('TracksPerVtx', type='TH1F', title="Number of tracks for the primary vertex",
                                             xbins=15, xmin=0, xmax=15) ]
        self.Histograms += [ defineHistogram('PrmVtxSigmaAll', type='TH1F', title="Primary vertex sigma (all-track vertices)",
                                             xbins=50, xmin=0, xmax=0.5) ]
        self.Histograms += [ defineHistogram('PrmVtxSigma2Trk', type='TH1F', title="Primary vertex sigma (1-track vertices excluded)",
                                              xbins=50, xmin=0, xmax=0.5) ]
        self.Histograms += [ defineHistogram('NVtxFound', type='TH1F', title="Number of primary vertices found",
                                             xbins=6, xmin=0, xmax=6) ]
        self.Histograms += [ defineHistogram('Trk_selectedTracks', type='TH1F', title="Selected tracks (in percentage)",
                                             xbins=20, xmin=-0.2, xmax=1.1) ]



class TrigT2HistoPrmVtxAllTEOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigT2HistoPrmVtxAllTEOnlineMonitoring"):
        super(TrigT2HistoPrmVtxAllTEOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")

        self.Histograms  = [ defineHistogram('PrmVtx', type='TH1F', title="Primary vertex",
                                             xbins=250, xmin=-200, xmax=200) ]
        self.Histograms += [ defineHistogram('TracksPerVtx', type='TH1F', title="Number of tracks for the primary vertex",
                                             xbins=15, xmin=0, xmax=15) ]
        self.Histograms += [ defineHistogram('PrmVtxSigmaAll', type='TH1F', title="Primary vertex sigma (all-track vertices)",
                                             xbins=50, xmin=0, xmax=0.5) ]
        self.Histograms += [ defineHistogram('PrmVtxSigma2Trk', type='TH1F', title="Primary vertex sigma (1-track vertices excluded)",
                                             xbins=50, xmin=0, xmax=0.5) ]
        self.Histograms += [ defineHistogram('NVtxFound', type='TH1F', title="Number of primary vertices found",
                                             xbins=6, xmin=0, xmax=6) ]
        self.Histograms += [ defineHistogram('Trk_selectedTracks', type='TH1F', title="Selected tracks (in percentage)",
                                             xbins=20, xmin=-0.2, xmax=1.1) ]



class TrigT2HistoPrmVtxAllTEValidationMonitoring_L2(TrigT2HistoPrmVtxAllTEValidationMonitoring):
    def __init__ (self, name="TrigT2HistoPrmVtxAllTEValidationMonitoring_L2"):
        """ defines hisograms for validation """
        super(TrigT2HistoPrmVtxAllTEValidationMonitoring_L2, self).__init__(name)

        self.defineTarget("Validation")

        self.Histograms += [ defineHistogram('Trk_stepsToSelect', type='TH1F', title="Steps to select tracks",
                                             xbins=8, xmin=0.0, xmax=8,
                                             labels='eta matching:phi matching:pT cut:d0 cut:b-layer hit:silicon hits:chi2 cut:selected') ]



class TrigT2HistoPrmVtxAllTEValidationMonitoring_EF(TrigT2HistoPrmVtxAllTEValidationMonitoring):
    def __init__ (self, name="TrigT2HistoPrmVtxAllTEValidationMonitoring_EF"):
        """ defines hisograms for validation """
        super(TrigT2HistoPrmVtxAllTEValidationMonitoring_EF, self).__init__(name)

        self.defineTarget("Validation")

        self.Histograms += [ defineHistogram('Trk_stepsToSelect', type='TH1F', title="Steps to select tracks",
                                             xbins=9, xmin=0.0, xmax=9,
                                             labels='eta matching:phi matching:pT cut:d0 cut:b-layer hit:pixel hits:silicon hits:chi2 cut:selected') ]



class TrigT2HistoPrmVtxAllTEOnlineMonitoring_L2(TrigT2HistoPrmVtxAllTEOnlineMonitoring):
    def __init__ (self, name="TrigT2HistoPrmVtxAllTEOnlineMonitoring_L2"):
        """ defines hisograms for online """
        super(TrigT2HistoPrmVtxAllTEOnlineMonitoring_L2, self).__init__(name)

        self.defineTarget("Online")

        self.Histograms += [ defineHistogram('Trk_stepsToSelect', type='TH1F', title="Steps to select tracks",
                                             xbins=8, xmin=0.0, xmax=8,
                                             labels='eta matching:phi matching:pT cut:d0 cut:b-layer hit:silicon hits:chi2 cut:selected') ]



class TrigT2HistoPrmVtxAllTEOnlineMonitoring_EF(TrigT2HistoPrmVtxAllTEOnlineMonitoring):
    def __init__ (self, name="TrigT2HistoPrmVtxAllTEOnlineMonitoring_EF"):
        """ defines hisograms for online """
        super(TrigT2HistoPrmVtxAllTEOnlineMonitoring_EF, self).__init__(name)

        self.defineTarget("Online")

        self.Histograms += [ defineHistogram('Trk_stepsToSelect', type='TH1F', title="Steps to select tracks",
                                             xbins=9, xmin=0.0, xmax=9,
                                             labels='eta matching:phi matching:pT cut:d0 cut:b-layer hit:pixel hits:silicon hits:chi2 cut:selected') ]


                                             
        
