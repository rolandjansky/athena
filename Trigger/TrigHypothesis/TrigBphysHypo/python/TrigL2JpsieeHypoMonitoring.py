# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 

class TrigL2JpsieeHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2HypoValidation"):
        super(TrigL2JpsieeHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms  = [ defineHistogram('CutCounter', type='TH1F', title= "L2JpsieeHypo cut counter;cut; nevents", xbins=8, xmin=-1.5, xmax=6.5, opt="kCumulative") ]
        self.Histograms += [ defineHistogram('Mass', type='TH1F', title="L2JpsieeHypo Mass; Mass[GeV]", xbins=100, xmin=1.4, xmax=4.) ]        
class TrigL2JpsieeHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2HypoOnline"):
        super(TrigL2JpsieeHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms  = [ defineHistogram('CutCounter', type='TH1F', title= "L2JpsieeHypo cut counter;cut; nevents", xbins=8, xmin=-1.5, xmax=6.5, opt="kCumulative") ]
        self.Histograms += [ defineHistogram('Mass', type='TH1F', title="L2JpsieeHypo Mass; Mass[GeV]", xbins=100, xmin=1.4, xmax=4.) ]        
