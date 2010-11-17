# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class InDetTrigDetailedTruthOnlineMonitor(TrigGenericMonitoringToolConfig):
    def __init__(self, name="InDetTrigDetailedTruthOnlineMonitor"):
        super (InDetTrigDetailedTruthOnlineMonitor, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms += [ defineHistogram('numPRDs',
                                             type='TH1F',
                                             title="Number PRD collections",
                                             xbins = 10, xmin=0., xmax=10.)]
        self.Histograms += [ defineHistogram('numTruthTracks',
                                             type='TH1F',
                                             title="Size of input track collection",
                                             xbins = 50, xmin=0., xmax=50.)]
        self.Histograms += [ defineHistogram('numPRDClusters',
                                             type='TH1F',
                                             title="Number clusters per PRD collection",
                                             xbins = 100, xmin=0., xmax=1000.)]


class InDetTrigDetailedTruthValidationMonitor(TrigGenericMonitoringToolConfig):
    def __init__(self, name="InDetTrigDetailedTruthValidationMonitor"):
        super (InDetTrigDetailedTruthValidationMonitor, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms += [ defineHistogram('numPRDs',
                                             type='TH1F',
                                             title="Number PRD collections",
                                             xbins = 10, xmin=0., xmax=10.)]
        self.Histograms += [ defineHistogram('numTruthTracks',
                                             type='TH1F',
                                             title="Size of input track collection",
                                             xbins = 50, xmin=0., xmax=50.)]
        self.Histograms += [ defineHistogram('numPRDClusters',
                                             type='TH1F',
                                             title="Number clusters per PRD collection",
                                             xbins = 100, xmin=0., xmax=1000.)]
