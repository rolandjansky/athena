# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class PRD_TrigMultiTruthOnlineMonitor(TrigGenericMonitoringToolConfig):
    def __init__(self, name="PRD_TrigMultiTruthOnlineMonitor"):
        super (PRD_TrigMultiTruthOnlineMonitor, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms += [ defineHistogram('prdPIXsize',
                                             type='TH1F',
                                             title="Size of PIXEL truth structure",
                                             xbins = 100, xmin=0., xmax=500.)]
        self.Histograms += [ defineHistogram('prdSCTsize',
                                             type='TH1F',
                                             title="Size of SCT truth structure",
                                             xbins = 100, xmin=0., xmax=500.)]
        self.Histograms += [ defineHistogram('prdTRTsize',
                                             type='TH1F',
                                             title="Size of TRT truth structure",
                                             xbins = 100, xmin=0., xmax=3000.)]
        


class PRD_TrigMultiTruthValidationMonitor(TrigGenericMonitoringToolConfig):
    def __init__(self, name="PRD_TrigMultiTruthValidationMonitor"):
        super (PRD_TrigMultiTruthValidationMonitor, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms += [ defineHistogram('prdPIXsize',
                                             type='TH1F',
                                             title="Size of PIXEL truth structure",
                                             xbins = 100, xmin=0., xmax=500.)]
        self.Histograms += [ defineHistogram('prdSCTsize',
                                             type='TH1F',
                                             title="Size of SCT truth structure",
                                             xbins = 100, xmin=0., xmax=500.)]
        self.Histograms += [ defineHistogram('prdTRTsize',
                                             type='TH1F',
                                             title="Size of TRT truth structure",
                                             xbins = 100, xmin=0., xmax=3000.)]
  
