# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 

class TrigL2DsPhiPiHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2DsPhiPiHypoValidation"):
        super(TrigL2DsPhiPiHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms = [ defineHistogram('CutCounter', type='TH1F', title= "L2DsPhiPiHypo cut counter;cut; nevents", xbins=8, xmin=-1.5, xmax=6.5, opt="kCumulative") ]
        
class TrigL2DsPhiPiHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2DsPhiPiHypoOnline"):
        super(TrigL2DsPhiPiHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms = [ defineHistogram('CutCounter', type='TH1F', title= "L2DsPhiPiHypo cut counter;cut; nevents", xbins=8, xmin=-1.5, xmax=6.5, opt="kCumulative") ]
        
