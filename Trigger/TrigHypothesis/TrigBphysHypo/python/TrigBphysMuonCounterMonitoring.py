# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 

ACCEPTANCE_labels = ' HltExecute : PassNEFMuons'
ACCEPTANCE_number = 2

class TrigBphysMuonCounterValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigBphysMuonCounterValidationMonitoring"):
        super(TrigBphysMuonCounterValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms += [ defineHistogram ( 'Acceptance', type = 'TH1F',
                                               title = 'BphysMuonCounter - Reached steps - acceptance ; # Events ; step',
                                               xbins = ACCEPTANCE_number , xmin = -0.5, xmax = ACCEPTANCE_number - 0.5,
                                               labels = ACCEPTANCE_labels ) ]
        self.Histograms += [ defineHistogram('nEFMuons', type='TH1F', title="Number of EF muons",
                                           xbins=100, xmin=0., xmax=13.) ]
        



class TrigBphysMuonCounterOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigBphysMuonCounterOnlineMonitoring"):
        super(TrigBphysMuonCounterOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms += [ defineHistogram ( 'Acceptance', type = 'TH1F',
                                               title = 'BphysMuonCounter - Reached steps - acceptance ; # Events ; step',
                                               xbins = ACCEPTANCE_number , xmin = -0.5, xmax = ACCEPTANCE_number - 0.5,
                                               labels = ACCEPTANCE_labels ) ]
        self.Histograms += [ defineHistogram('nEFMuons', type='TH1F', title="Number of EF muons",
                                           xbins=100, xmin=0., xmax=13.) ]
        
