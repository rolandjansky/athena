# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TRT_TrigTrackExtensionAlgOnlineMonitor(TrigGenericMonitoringToolConfig):
    def __init__(self, name="TRT_TrigTrackExtensionAlgOnlineMonitor"):
        super (TRT_TrigTrackExtensionAlgOnlineMonitor, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms += [ defineHistogram('numTracks',
                                             type='TH1F',
                                             title="Number of input tracks",
                                             xbins = 50, xmin=0., xmax=50.)]
        self.Histograms += [ defineHistogram('numTracksExtended',
                                             type='TH1F',
                                             title="Number of extended tracks",
                                             xbins = 50, xmin=0., xmax=50.)]


class TRT_TrigTrackExtensionAlgValidationMonitor(TrigGenericMonitoringToolConfig):
    def __init__(self, name="TRT_TrigTrackExtensionAlgValidationMonitor"):
        super (TRT_TrigTrackExtensionAlgValidationMonitor, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms += [ defineHistogram('numTracks',
                                             type='TH1F',
                                             title="Number of input tracks",
                                             xbins = 50, xmin=0., xmax=50.)]
        self.Histograms += [ defineHistogram('numTracksExtended',
                                             type='TH1F',
                                             title="Number of extended tracks",
                                             xbins = 50, xmin=0., xmax=50.)]        
