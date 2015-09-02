# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig


class TrigEFMtAllTEValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFMtAllTEValidationMonitoring"):
        """ defines histograms for validation """
        super(TrigEFMtAllTEValidationMonitoring, self).__init__(name)
        self.defineTarget( "Validation" )
        
        self.Histograms += [ defineHistogram('mt_electron1',      type='TH1F', title="Transverse mass after cut", xbins=100, xmin=0, xmax=100) ]
        self.Histograms += [ defineHistogram('mt_electron1_pass', type='TH1F', title="Transverse mass after cut", xbins=100, xmin=0, xmax=100) ]


class TrigEFMtAllTEOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFMtAllTEOnlineMonitoring"):
        """ defines histograms for online """
        super(TrigEFElectronHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms += [ defineHistogram('mt_electron1',      type='TH1F', title="Transverse mass after cut", xbins=100, xmin=0, xmax=100) ]
        self.Histograms += [ defineHistogram('mt_electron1_pass', type='TH1F', title="Transverse mass after cut", xbins=100, xmin=0, xmax=100) ]
