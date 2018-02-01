# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TrigT1NSWValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="NSWL1Validation"):
        super(TrigT1NSWValidationMonitoring, self).__init__(name)

        """ define monitoring histograms for NSW L1 validation(RTT) """

        self.defineTarget("Validation")

        self.Histograms += [ defineHistogram('COUNTERS', type='TH1F', title='Counters; ; events', xbins=1, xmin=0.5, xmax=1.5,
                              labels='input_events') ]
