# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 

class TrigEFDsPhiPiHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="EFHypoValidation"):
        super(TrigEFDsPhiPiHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms = [ defineHistogram('CutCounter', type='TH1F', title= "EFDsPhiPiHypo cut counter;cut; nevents", xbins=8, xmin=-1.5, xmax=6.5, opt="kCumulative") ]
        
class TrigEFDsPhiPiHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="EFHypoOnline"):
        super(TrigEFDsPhiPiHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms = [ defineHistogram('CutCounter', type='TH1F', title= "EFDsPhiPiHypo cut counter;cut; nevents", xbins=8, xmin=-1.5, xmax=6.5, opt="kCumulative") ]
        
