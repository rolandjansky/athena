# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TRT_TrigTrackSegmentsFinderOnlineMonitor(TrigGenericMonitoringToolConfig):
    def __init__(self, name="TRT_TrigTrackSegmentsFinderOnlineMonitor"):
        super (TRT_TrigTrackSegmentsFinderOnlineMonitor, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms += [ defineHistogram('numTRTSeg',
                                             type='TH1F',
                                             title="Number of TRT Segments",
                                             xbins = 50, xmin=0., xmax=50.)]
        self.Histograms += [ defineHistogram('RoIEta',
                                             type='TH1F',
                                             title="Eta of RoI",
                                             xbins = 100, xmin=-3., xmax=3.)]
        self.Histograms += [ defineHistogram('RoIPhi',
                                             type='TH1F',
                                             title="Phi of RoI",
                                             xbins = 100, xmin=-3.2, xmax=3.2)]
class TRT_TrigTrackSegmentsFinderValidationMonitor(TrigGenericMonitoringToolConfig):
    def __init__(self, name="TRT_TrigTrackSegmentsFinderValidationMonitor"):
        super (TRT_TrigTrackSegmentsFinderValidationMonitor, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms += [ defineHistogram('numTRTSeg',
                                             type='TH1F',
                                             title="Number of TRT Segments",
                                             xbins = 50, xmin=0., xmax=50.)]
        self.Histograms += [ defineHistogram('RoIEta',
                                             type='TH1F',
                                             title="Eta of RoI",
                                             xbins = 100, xmin=-3., xmax=3.)]
        self.Histograms += [ defineHistogram('RoIPhi',
                                             type='TH1F',
                                             title="Phi of RoI",
                                             xbins = 100, xmin=-3.2, xmax=3.2)]
        
        
