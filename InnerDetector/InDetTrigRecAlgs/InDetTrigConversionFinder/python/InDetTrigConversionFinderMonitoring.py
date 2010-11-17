# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class InDetTrigConversionFinderOnlineMonitor(TrigGenericMonitoringToolConfig):
    def __init__(self, name="InDetTrigConversionFinderOnlineMonitor"):
        super (InDetTrigConversionFinderOnlineMonitor, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms += [ defineHistogram('roi_nTrkPart',
                                             type='TH1F',
                                             title="Number of Input TrackParticles",
                                             xbins = 50, xmin=0., xmax=50.)]
        self.Histograms += [ defineHistogram('roi_nConVx',
                                             type='TH1F',
                                             title="Number of Conversion Vertices",
                                             xbins = 20, xmin=0., xmax=20.)]
        


class InDetTrigConversionFinderValidationMonitor(TrigGenericMonitoringToolConfig):
    def __init__(self, name="InDetTrigConversionFinderValidationMonitor"):
        super (InDetTrigConversionFinderValidationMonitor, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms += [ defineHistogram('roi_nTrkPart',
                                             type='TH1F',
                                             title="Number of Input TrackParticles",
                                             xbins = 50, xmin=0., xmax=50.)]
        self.Histograms += [ defineHistogram('roi_nConVx',
                                             type='TH1F',
                                             title="Number of Conversion Vertices",
                                             xbins = 20, xmin=0., xmax=20.)]
 
