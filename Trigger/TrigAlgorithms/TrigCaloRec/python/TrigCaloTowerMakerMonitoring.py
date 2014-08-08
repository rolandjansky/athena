# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#Definitions of the monitoring classes

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TrigCaloTowerMakerValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigCaloTowerMaker_Validation"):
        super(TrigCaloTowerMakerValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")

        self.Histograms += [ defineHistogram('TowerContainerSize', type='TH1F', title="TrigCaloTowerMaker Tower container size; Size; nevents", xbins=100, xmin=0., xmax=10000.) ]


class TrigCaloTowerMakerOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigCaloTowerMaker_Online"):
        super(TrigCaloTowerMakerOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")

        self.Histograms += [ defineHistogram('TowerContainerSize', type='TH1F', title="TrigCaloTowerMaker Tower container size; Size; nevents", xbins=100, xmin=0., xmax=10000.) ]

class TrigCaloTowerMakerCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigCaloTowerMaker_Cosmic"):
        super(TrigCaloTowerMakerCosmicMonitoring, self).__init__(name)

        self.defineTarget("Cosmic")

        self.Histograms += [ defineHistogram('TowerContainerSize', type='TH1F', title="TrigCaloTowerMaker Tower container size; Size; nevents", xbins=100, xmin=0., xmax=5000.) ]
