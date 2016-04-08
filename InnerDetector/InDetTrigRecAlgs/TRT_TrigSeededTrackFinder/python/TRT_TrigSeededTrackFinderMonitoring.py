# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TRT_TrigSeededTrackFinderOnlineMonitor(TrigGenericMonitoringToolConfig):
    def __init__(self, name="TRT_TrigSeededTrackFinderOnlineMonitor"):
        super (TRT_TrigSeededTrackFinderOnlineMonitor, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms += [ defineHistogram('numTrtSeg',
                                             type='TH1F',
                                             title="Number of TRT Segments",
                                             xbins = 50, xmin=0., xmax=50.)]
        self.Histograms += [ defineHistogram('numTrtSegGood',
                                             type='TH1F',
                                             title="Number of Good TRT Segments",
                                             xbins = 50, xmin=0., xmax=50.)]
        self.Histograms += [ defineHistogram('numBckTrk',
                                             type='TH1F',
                                             title="Number of Back Tracks Found",
                                             xbins = 50, xmin=0., xmax=50.)]
        self.Histograms += [ defineHistogram('numBckTrkTrt',
                                             type='TH1F',
                                             title="Number of Back Tracks Found no Si Extension",
                                             xbins = 50, xmin=0., xmax=50.)]
        self.Histograms += [ defineHistogram('numBckTrkSi',
                                             type='TH1F',
                                             title="Number of Back Tracks Found with Si Extension",
                                             xbins = 50, xmin=0., xmax=50.)]

class TRT_TrigSeededTrackFinderValidationMonitor(TrigGenericMonitoringToolConfig):
    def __init__(self, name="TRT_TrigSeededTrackFinderValidationMonitor"):
        super (TRT_TrigSeededTrackFinderValidationMonitor, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms += [ defineHistogram('numTrtSeg',
                                             type='TH1F',
                                             title="Number of TRT Segments",
                                             xbins = 50, xmin=0., xmax=50.)]
        self.Histograms += [ defineHistogram('numTrtSegGood',
                                             type='TH1F',
                                             title="Number of Good TRT Segments",
                                             xbins = 50, xmin=0., xmax=50.)]
        self.Histograms += [ defineHistogram('numBckTrk',
                                             type='TH1F',
                                             title="Number of Back Tracks Found",
                                             xbins = 50, xmin=0., xmax=50.)]
        self.Histograms += [ defineHistogram('numBckTrkTrt',
                                             type='TH1F',
                                             title="Number of Back Tracks Found no Si Extension",
                                             xbins = 50, xmin=0., xmax=50.)]
        self.Histograms += [ defineHistogram('numBckTrkSi',
                                             type='TH1F',
                                             title="Number of Back Tracks Found with Si Extension",
                                             xbins = 50, xmin=0., xmax=50.)]
        
        
