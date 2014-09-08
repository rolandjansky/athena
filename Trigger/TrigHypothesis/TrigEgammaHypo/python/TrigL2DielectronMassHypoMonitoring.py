# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig
from AthenaCommon.SystemOfUnits import GeV

class TrigL2DielectronMassOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2DielectronMassOnlineMonitoring"):
        """ defines hisograms for online """
        super(TrigL2DielectronMassOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms = [ defineHistogram('cut', type='TH1F', title="Cut counter",
                                            xbins=7, xmin=-0.5, xmax=6.5, opt="kCumulative") ]
        

class TrigL2DielectronMassValidationMonitoring_Jpsiee(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2DielectronMassValidationMonitoring_Jpsiee"):
        """ defines hisograms for online """
        super(TrigL2DielectronMassValidationMonitoring_Jpsiee, self).__init__(name)
        self.defineTarget("Validation")

        self.Histograms += [ defineHistogram('cut', type='TH1F', title="Cut counter",
                                             xbins=7, xmin=-0.5, xmax=6.5, opt="kCumulative") ]
        self.Histograms += [ defineHistogram('massOfAccepted', type='TH1F', title="Mass in accepted combinations",
                                             xbins=50, xmin=0.*GeV, xmax=20.*GeV) ]

class TrigL2DielectronMassValidationMonitoring_Upsiee(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2DielectronMassValidationMonitoring_Upsiee"):
        """ defines hisograms for online """
        super(TrigL2DielectronMassValidationMonitoring_Upsiee, self).__init__(name)
        self.defineTarget("Validation")

        self.Histograms += [ defineHistogram('cut', type='TH1F', title="Cut counter",
                                             xbins=7, xmin=-0.5, xmax=6.5, opt="kCumulative") ]
        self.Histograms += [ defineHistogram('massOfAccepted', type='TH1F', title="Mass in accepted combinations",
                                             xbins=50, xmin=0.*GeV, xmax=50.*GeV) ]

class TrigL2DielectronMassValidationMonitoring_Zee(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2DielectronMassValidationMonitoring_Zee"):
        """ defines hisograms for online (to be used in Fex and Hypo)"""
        super(TrigL2DielectronMassValidationMonitoring_Zee, self).__init__(name)
        self.defineTarget("Validation")

        self.Histograms += [ defineHistogram('cut', type='TH1F', title="Cut counter",
                                             xbins=7, xmin=-0.5, xmax=6.5, opt="kCumulative") ]
        self.Histograms += [ defineHistogram('massOfAccepted', type='TH1F', title="Mass in accepted combinations",
                                             xbins=50, xmin=0.0*GeV, xmax=250.0*GeV) ]
        
