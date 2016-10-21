# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig
from AthenaCommon.SystemOfUnits import GeV

class TrigEFDielectronMassOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFDielectronMassOnlineMonitoring"):
        """ defines hisograms for online """
        super(TrigEFDielectronMassOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms = [ defineHistogram('cut', type='TH1F', title="Cut counter",
                                            xbins=6, xmin=-0.5, xmax=5.5, opt="kCumulative") ]
        self.Histograms += [ defineHistogram('massOfAccepted', type='TH1F', title="Mass in accepted combinations",
                                             xbins=50, xmin=0.0*GeV, xmax=250.0*GeV) ]
        
class TrigEFDielectronMassValidationMonitoring_Zee(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFDielectronMassValidationMonitoring_Zee"):
        """ defines hisograms for online (to be used in Fex and Hypo)"""
        super(TrigEFDielectronMassValidationMonitoring_Zee, self).__init__(name)
        self.defineTarget("Validation")

        self.Histograms += [ defineHistogram('cut', type='TH1F', title="Cut counter",
                                             xbins=6, xmin=-0.5, xmax=5.5, opt="kCumulative") ]
        self.Histograms += [ defineHistogram('massOfAccepted', type='TH1F', title="Mass in accepted combinations",
                                             xbins=50, xmin=0.0*GeV, xmax=250.0*GeV) ]
        
class TrigEFDielectronMassOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFDielectronMassOnlineMonitoring"):
        """ defines hisograms for online """
        super(TrigEFDielectronMassOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms = [ defineHistogram('cut', type='TH1F', title="Cut counter",
                                            xbins=6, xmin=-0.5, xmax=5.5, opt="kCumulative") ]
        self.Histograms += [ defineHistogram('massOfAccepted', type='TH1F', title="Mass in accepted combinations",
                                             xbins=75, xmin=0.0*GeV, xmax=15.0*GeV) ]
        
class TrigEFDielectronMassValidationMonitoring_Jpsi(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFDielectronMassValidationMonitoring_Jpsi"):
        """ defines hisograms for online (to be used in Fex and Hypo)"""
        super(TrigEFDielectronMassValidationMonitoring_Jpsi, self).__init__(name)
        self.defineTarget("Validation")

        self.Histograms += [ defineHistogram('cut', type='TH1F', title="Cut counter",
                                             xbins=7, xmin=-0.5, xmax=6.5, opt="kCumulative") ]
        self.Histograms += [ defineHistogram('massOfAccepted', type='TH1F', title="Mass in accepted combinations",
                                             xbins=75, xmin=0.0*GeV, xmax=15.0*GeV) ]

class TrigEFDielectronMassOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFDielectronMassOnlineMonitoring"):
        """ defines hisograms for online """
        super(TrigEFDielectronMassOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms = [ defineHistogram('cut', type='TH1F', title="Cut counter",
                                            xbins=6, xmin=-0.5, xmax=5.5, opt="kCumulative") ]
        self.Histograms += [ defineHistogram('massOfAccepted', type='TH1F', title="Mass in accepted combinations",
                                             xbins=60, xmin=0.0*GeV, xmax=30.0*GeV) ]

class TrigEFDielectronMassValidationMonitoring_Upsi(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFDielectronMassValidationMonitoring_Upsi"):
        """ defines hisograms for online (to be used in Fex and Hypo)"""
        super(TrigEFDielectronMassValidationMonitoring_Upsi, self).__init__(name)
        self.defineTarget("Validation")

        self.Histograms += [ defineHistogram('cut', type='TH1F', title="Cut counter",
                                             xbins=6, xmin=-0.5, xmax=5.5, opt="kCumulative") ]
        self.Histograms += [ defineHistogram('massOfAccepted', type='TH1F', title="Mass in accepted combinations",
                                             xbins=60, xmin=0.0*GeV, xmax=30.0*GeV) ]        
