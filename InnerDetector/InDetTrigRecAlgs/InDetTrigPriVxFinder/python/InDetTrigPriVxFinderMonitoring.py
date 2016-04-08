# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class InDetTrigPriVxFinderOnlineMonitor(TrigGenericMonitoringToolConfig):
    def __init__(self, name="InDetTrigPriVxFinderOnlineMonitor"):
        super (InDetTrigPriVxFinderOnlineMonitor, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms += [ defineHistogram('numTracks',
                                             type='TH1F',
                                             title="Number of Input Tracks",
                                             xbins = 50, xmin=0., xmax=50.)]
        self.Histograms += [ defineHistogram('numVertices',
                                             type='TH1F',
                                             title="Number of Vertices",
                                             xbins = 10, xmin=0., xmax=10.)]
        self.Histograms += [ defineHistogram('zOfPriVtx',
                                             type='TH1F',
                                             title="z for PriVtx Vertices",
                                             xbins = 200, xmin=-200., xmax=200.)]
        self.Histograms += [ defineHistogram('zOfPileUp',
                                             type='TH1F',
                                             title="z for PileUp Vertices",
                                             xbins = 200, xmin=-200., xmax=200.)]
        self.Histograms += [ defineHistogram('zOfNoVtx',
                                             type='TH1F',
                                             title="z for NoVtx Vertices",
                                             xbins = 200, xmin=-200., xmax=200.)]


class InDetTrigPriVxFinderValidationMonitor(TrigGenericMonitoringToolConfig):
    def __init__(self, name="InDetTrigPriVxFinderValidationMonitor"):
        super (InDetTrigPriVxFinderValidationMonitor, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms += [ defineHistogram('numTracks',
                                             type='TH1F',
                                             title="Number of Input Tracks",
                                             xbins = 50, xmin=0., xmax=50.)]
        self.Histograms += [ defineHistogram('numVertices',
                                             type='TH1F',
                                             title="Number of Vertices",
                                             xbins = 10, xmin=0., xmax=10.)]
        self.Histograms += [ defineHistogram('zOfPriVtx',
                                             type='TH1F',
                                             title="z for PrimaryVertices",
                                             xbins = 200, xmin=-200., xmax=200.)]
        self.Histograms += [ defineHistogram('zOfPileUp',
                                             type='TH1F',
                                             title="z for PileUp Vertices",
                                             xbins = 200, xmin=-200., xmax=200.)]
        self.Histograms += [ defineHistogram('zOfNoVtx',
                                             type='TH1F',
                                             title="z for NoVtx Vertices",
                                             xbins = 200, xmin=-200., xmax=200.)]
