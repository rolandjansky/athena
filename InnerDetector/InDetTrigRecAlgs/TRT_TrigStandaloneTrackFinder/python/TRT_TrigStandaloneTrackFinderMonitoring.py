# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TRT_TrigStandaloneTrackFinderOnlineMonitor(TrigGenericMonitoringToolConfig):
    def __init__(self, name="TRT_TrigStandaloneTrackFinderOnlineMonitor"):
        super (TRT_TrigStandaloneTrackFinderOnlineMonitor, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms += [ defineHistogram('numTRTSeg',
                                             type='TH1F',
                                             title="Number of TRT Segments",
                                             xbins = 50, xmin=0., xmax=50.)]
        self.Histograms += [ defineHistogram('numTRTTracks',
                                             type='TH1F',
                                             title="Number of TRT standalone tracks",
                                             xbins = 50, xmin=0., xmax=50.)]
class TRT_TrigStandaloneTrackFinderValidationMonitor(TrigGenericMonitoringToolConfig):
    def __init__(self, name="TRT_TrigStandaloneTrackFinderValidationMonitor"):
        super (TRT_TrigStandaloneTrackFinderValidationMonitor, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms += [ defineHistogram('numTRTSeg',
                                             type='TH1F',
                                             title="Number of TRT Segments",
                                             xbins = 50, xmin=0., xmax=50.)]
        self.Histograms += [ defineHistogram('numTRTTracks',
                                             type='TH1F',
                                             title="Number of TRT standalone tracks",
                                             xbins = 50, xmin=0., xmax=50.)]
