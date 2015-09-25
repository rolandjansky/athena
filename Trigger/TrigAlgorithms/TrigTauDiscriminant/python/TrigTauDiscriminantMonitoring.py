# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

################# Validation, DQ checks
from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 

class TrigTauDiscriminantOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigTauDiscriminantOnlineMonitoring"):
        super(TrigTauDiscriminantOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        
        #self.Histograms += [ defineHistogram('EF_LLHScore', type='TH1F', title="EF LLHScore; LLH score; a.u.", xbins=100, xmin=-30., xmax=30.) ]
        self.Histograms += [ defineHistogram('EF_BDTScore', type='TH1F', title="EF BDTScore; BDT score; a.u.", xbins=50, xmin=-0.1, xmax=1.) ]
        
########## add validation specific histograms 
class TrigTauDiscriminantValidationMonitoring(TrigTauDiscriminantOnlineMonitoring):
    def __init__ (self, name="TrigTauDiscriminantValidationMonitoring"):
        super(TrigTauDiscriminantValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")

        




