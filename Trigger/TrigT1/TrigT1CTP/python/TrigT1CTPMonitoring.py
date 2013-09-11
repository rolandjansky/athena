# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TrigT1CTPValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="CTPValidation"):
        super(TrigT1CTPValidationMonitoring, self).__init__(name)

        """ define monitoring histograms for CTP validation(RTT) """

        self.defineTarget("Validation")

        self.Histograms += [ defineHistogram('PIT', type='TH1F', title='CTP PIT', xbins=192, xmin=0., xmax=192.) ]
        self.Histograms += [ defineHistogram('TBP', type='TH1F', title='CTP TBP', xbins=256, xmin=0., xmax=256.) ]
        self.Histograms += [ defineHistogram('TAP', type='TH1F', title='CTP TAP', xbins=256, xmin=0., xmax=256.) ]
        self.Histograms += [ defineHistogram('TAV', type='TH1F', title='CTP TAV', xbins=256, xmin=0., xmax=256.) ]
