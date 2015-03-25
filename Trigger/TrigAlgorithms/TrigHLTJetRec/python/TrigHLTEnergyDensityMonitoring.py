# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 
        

class TrigHLTEnergyDensityValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigHLTEnergyDensityMonitoring_Validation"):
        """ defines histograms for online """
        super(TrigHLTEnergyDensityValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")

        self.Histograms += [
            defineHistogram('energyDensity',
                            type='TH1F',
                            title="EnergyDensity",
                            xbins=100,
                            xmin=0,
                            xmax=50000) ]

        

class TrigHLTEnergyDensityOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigHLTEnergyDensityMonitoring_Online"):
        """ defines histograms for online """
        super(TrigHLTEnergyDensityOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")

        self.Histograms += [
            defineHistogram('energyDensity',
                            type='TH1F',
                            title="EnergyDensity",
                            xbins=100,
                            xmin=0,
                            xmax=50000) ]
        

class TrigHLTEnergyDensityCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigHLTEnergyDensityMonitoring_Cosmic"):
        """ defines histograms for online """
        super(TrigHLTEnergyDensityCosmicMonitoring, self).__init__(name)

        self.defineTarget("Cosmic")

        self.Histograms += [
            defineHistogram('energyDensity',
                            type='TH1F',
                            title="EnergyDensity",
                            xbins=100,
                            xmin=0,
                            xmax=50000) ]
