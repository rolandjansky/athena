# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig


class L1TopoSimulationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L1TopoSimulation"):
        super(L1TopoSimulationMonitoring, self).__init__(name)

        """ define monitoring histograms for L1Topo simulation"""

        self.defineTarget("Validation")

        self.Histograms += [ defineHistogram('DecisionModule1', type='TH1F', title='L1 Topo Decision (Module 1)', xbins=64, xmin=0, xmax=64) ]
        self.Histograms += [ defineHistogram('DecisionModule2', type='TH1F', title='L1 Topo Decision (Module 2)', xbins=64, xmin=0, xmax=64) ]
        self.Histograms += [ defineHistogram('DecisionModule3', type='TH1F', title='L1 Topo Decision (Module 3)', xbins=64, xmin=0, xmax=64) ]

