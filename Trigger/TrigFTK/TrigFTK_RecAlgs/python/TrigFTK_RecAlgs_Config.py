# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigFTK_RecAlgs.TrigFTK_RecAlgsConf import TrigFTK_VxPrimary

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TrigFTK_PriVxFinderOnlineMonitor(TrigGenericMonitoringToolConfig):
    def __init__(self, name="TrigFTK_PriVxFinderOnlineMonitor"):
        super (TrigFTK_PriVxFinderOnlineMonitor, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms += [ defineHistogram('numVertices',
                                             type='TH1F',
                                             title="Number of Vertices",
                                             xbins = 10, xmin=0., xmax=10.)]
        self.Histograms += [ defineHistogram('VertexType',
                                             type='TH1F',
                                             title="Type of Primary Vertex",
                                             xbins = 10, xmin=0., xmax=10.)]
        self.Histograms += [ defineHistogram('numTracksPriVtx',
                                             type='TH1F',
                                             title="Number of Tracks in Primary Vertex",
                                             xbins = 80, xmin=0., xmax=80.)]
        self.Histograms += [ defineHistogram('numTracksPileUp',
                                             type='TH1F',
                                             title="Number of Tracks in PileUp Vertex",
                                             xbins = 50, xmin=0., xmax=50.)]
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


class TrigFTK_PriVxFinderValidationMonitor(TrigGenericMonitoringToolConfig):
    def __init__(self, name="TrigFTK_PriVxFinderValidationMonitor"):
        super (TrigFTK_PriVxFinderValidationMonitor, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms += [ defineHistogram('numVertices',
                                             type='TH1F',
                                             title="Number of Vertices",
                                             xbins = 10, xmin=0., xmax=10.)]
        self.Histograms += [ defineHistogram('VertexType',
                                             type='TH1F',
                                             title="Type of Primary Vertex",
                                             xbins = 10, xmin=0., xmax=10.)]
        self.Histograms += [ defineHistogram('numTracksPriVtx',
                                             type='TH1F',
                                             title="Number Tracks in Primary Vertex",
                                             xbins = 80, xmin=0., xmax=80.)]
        self.Histograms += [ defineHistogram('numTracksPileUp',
                                             type='TH1F',
                                             title="Number of Tracks in PileUp Vertex",
                                             xbins = 50, xmin=0., xmax=50.)]
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

class TrigFTK_VxPrimary_EF(TrigFTK_VxPrimary):
    __slots__ = []
    def __init__(self, name, type):
        TrigFTK_VxPrimary.__init__(self, name)
        from TrigFTK_RecExample.TrigFTKLoadTools import theFTK_DataProviderSvc
        self.FTK_DataProvider = theFTK_DataProviderSvc
        #monitoring
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        vxtime = TrigTimeHistToolConfig("VxTime")
        vxtime.TimerHistLimits = [0,100]
        self.AthenaMonTools = [TrigFTK_PriVxFinderOnlineMonitor(),
                               TrigFTK_PriVxFinderValidationMonitor(),
                               vxtime]
