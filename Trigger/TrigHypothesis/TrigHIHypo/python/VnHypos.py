# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigHIHypo.TrigHIHypoConf import VnHypo

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig
class VnMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="VnMonitoring", harmonic=1):
        super(VnMonitoring, self).__init__(name)
        self.defineTarget(['Online', 'Validation'])
        self.Histograms += [ defineHistogram('TotalEt',
                                             type='TH1F',
                                             title="Total ET; ET [MeV]",
                                             xbins = 100, xmin=0.0, xmax=4500000)]
        self.Histograms += [ defineHistogram('TotalEtPassing',
                                             type='TH1F',
                                             title="Total ET for passing events; ET [MeV]",
                                             xbins = 100, xmin=0.0, xmax=4500000)]


        self.Histograms += [ defineHistogram('icent',
                                             type='TH1F',
                                             title="Centrality bin if checked events;centrality[%]",
                                             xbins = 101, xmin=-0.5, xmax=100.5)]


        self.Histograms += [ defineHistogram('icentPassing',
                                             type='TH1F',
                                             title="Centrality bin of passing events;centrality[%]",
                                             xbins = 101, xmin=-0.5, xmax=100.5)]


        self.Histograms += [ defineHistogram('q',
                                             type='TH1F',
                                             title="q%d of checked events" % harmonic,
                                             xbins = 50, xmin=0.0, xmax=0.25)]

        self.Histograms += [ defineHistogram('qPassing',
                                             type='TH1F',
                                             title="q%d of passing events" % harmonic,
                                             xbins = 50, xmin=0.0, xmax=0.25)]
        

        self.Histograms += [ defineHistogram('qPassing, icentPassing',
                                             type='TH2F',
                                             title="q%d vs centrality of passing events" % harmonic,
                                             xbins=50, xmin=0.0, xmax=0.25,
                                             ybins=101, ymin=-0.5, ymax=100.5) ]


class V2(VnHypo):
    __slots__ = []
    def __init__(self, name, bin):
        super( VnHypo, self ).__init__( name )
        self.ThresholdVnBin = bin
        self.FlowHarmonic = 2
        self.AthenaMonTools += [VnMonitoring(name="VnMonitoring", harmonic=2)]


class V3(VnHypo):
    __slots__ = []
    def __init__(self, name, bin):
        super( VnHypo, self ).__init__( name )
        self.ThresholdVnBin = bin
        self.FlowHarmonic = 3
        self.AthenaMonTools += [VnMonitoring(name="VnMonitoring", harmonic=3)]
