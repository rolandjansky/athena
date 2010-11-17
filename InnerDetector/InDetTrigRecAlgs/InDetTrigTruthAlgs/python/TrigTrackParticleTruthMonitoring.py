# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TrigTrackParticleTruthOnlineMonitor(TrigGenericMonitoringToolConfig):
    def __init__(self, name="TrigTrackParticleTruthOnlineMonitor"):
        super (TrigTrackParticleTruthOnlineMonitor, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms += [ defineHistogram('numTracks',
                                             type='TH1F',
                                             title="Number input tracks",
                                             xbins = 50, xmin=0., xmax=50.)]
        self.Histograms += [ defineHistogram('numTracksTruth',
                                             type='TH1F',
                                             title="Number of input truth tracks",
                                             xbins = 50, xmin=0., xmax=50.)]
        self.Histograms += [ defineHistogram('numParticleTracks',
                                             type='TH1F',
                                             title="Size of input track particle container",
                                             xbins = 50, xmin=0., xmax=50.)]
        self.Histograms += [ defineHistogram('numParticleTracksTruth',
                                             type='TH1F',
                                             title="Size of TrackParticleTruth container",
                                             xbins = 50, xmin=0., xmax=50.)]


class TrigTrackParticleTruthValidationMonitor(TrigGenericMonitoringToolConfig):
    def __init__(self, name="TrigTrackParticleTruthValidationMonitor"):
        super (TrigTrackParticleTruthValidationMonitor, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms += [ defineHistogram('numTracks',
                                             type='TH1F',
                                             title="Number input tracks",
                                             xbins = 50, xmin=0., xmax=50.)]
        self.Histograms += [ defineHistogram('numTracksTruth',
                                             type='TH1F',
                                             title="Number of input truth tracks",
                                             xbins = 50, xmin=0., xmax=50.)]
        self.Histograms += [ defineHistogram('numParticleTracks',
                                             type='TH1F',
                                             title="Size of input track particle container",
                                             xbins = 50, xmin=0., xmax=50.)]
        self.Histograms += [ defineHistogram('numParticleTracksTruth',
                                             type='TH1F',
                                             title="Size of TrackParticleTruth container",
                                             xbins = 50, xmin=0., xmax=50.)]
