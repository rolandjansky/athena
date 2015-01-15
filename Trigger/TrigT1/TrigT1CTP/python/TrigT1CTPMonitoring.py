# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TrigT1CTPValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="CTPValidation"):
        super(TrigT1CTPValidationMonitoring, self).__init__(name)

        """ define monitoring histograms for CTP validation(RTT) """

        self.defineTarget("Validation")

        self.Histograms += [ defineHistogram('TIP', type='TH1F', title='CTP TIP', xbins=352, xmin=0., xmax=352.) ]
        #self.Histograms += [ defineHistogram('TBP', type='TH1F', title='CTP TBP', xbins=512, xmin=0., xmax=512.) ]
        #self.Histograms += [ defineHistogram('TAP', type='TH1F', title='CTP TAP', xbins=512, xmin=0., xmax=512.) ]
        #self.Histograms += [ defineHistogram('TAV', type='TH1F', title='CTP TAV', xbins=512, xmin=0., xmax=512.) ]
