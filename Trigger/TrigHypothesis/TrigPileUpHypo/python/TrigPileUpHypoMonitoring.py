# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

### histograms

cutNames = ['Rejected', 'FJ', 'FJ_PV', 'PV']

labelsDescription = ''
for a in cutNames:
    labelsDescription += a+':'

hCuts    = defineHistogram('Hypo_PileUp_cuts',  type='TH1F', title="Num Forward Jets", xbins=4, xmin=-.5, xmax=3.5, labels=labelsDescription)

class TrigPileUpHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigPileUpHypoOnlineMonitoring"):
        super(TrigPileUpHypoMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms  = [ hCuts ]

