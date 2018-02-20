# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 

ACCEPTANCE_labels = ' HltExecute : PassNElectrons : Pass pt : pass isEM'
ACCEPTANCE_number = 4

class TrigBphysElectronCounterValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigBphysElectronCounterValidationMonitoring"):
        super(TrigBphysElectronCounterValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms += [ defineHistogram ( 'Acceptance', type = 'TH1F',
                                               title = 'BphysElectronCounter - Reached steps - acceptance ; # Events ; step',
                                               xbins = ACCEPTANCE_number , xmin = -0.5, xmax = ACCEPTANCE_number - 0.5,
                                               labels = ACCEPTANCE_labels ) ]
        self.Histograms += [ defineHistogram('nEFElectrons', type='TH1F', title="Number of EF electrons",
                                           xbins=100, xmin=0., xmax=13.) ]
        



class TrigBphysElectronCounterOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigBphysElectronCounterOnlineMonitoring"):
        super(TrigBphysElectronCounterOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms += [ defineHistogram ( 'Acceptance', type = 'TH1F',
                                               title = 'BphysElectronCounter - Reached steps - acceptance ; # Events ; step',
                                               xbins = ACCEPTANCE_number , xmin = -0.5, xmax = ACCEPTANCE_number - 0.5,
                                               labels = ACCEPTANCE_labels ) ]
        self.Histograms += [ defineHistogram('nEFElectrons', type='TH1F', title="Number of EF electrons",
                                           xbins=100, xmin=0., xmax=13.) ]
        
