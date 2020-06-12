# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig
from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

class TrigmuRoIValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigmuRoIValidationMonitoring"):
        super(TrigmuRoIValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")

        self.Histograms = [ defineHistogram('RpcOutOfTime', type='TH1F', title="Distribution of the BCID difference for the Rpc RoI out of Time; (Muon RoI BCID - Event BCID)",
                                            xbins=21, xmin=-10.5, xmax=10.5) ]
        self.Histograms += [ defineHistogram('TgcOutOfTime', type='TH1F', title="Distribution of the BCID difference for the Tgc RoI out of Time; (Muon RoI BCID - Event BCID)",
                                            xbins=21, xmin=-10.5, xmax=10.5) ]
        self.Histograms += [ defineHistogram('EtaOutOfTime, PhiOutOfTime', type='TH2F', title="Eta vs Phi of the Mupon RoI out of time; Eta; Phi",
                                            xbins=108, xmin=-2.7, xmax=2.7, ybins=96, ymin=-3.1416, ymax=3.1416 ) ]


				    
class TrigmuRoIOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigmuRoIOnlineMonitoring"):
        super(TrigmuRoIOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms = [ defineHistogram('RpcOutOfTime', type='TH1F', title="Distribution of the BCID difference for the Rpc RoI out of Time; (Muon RoI BCID - Event BCID)",
                                            xbins=21, xmin=-10.5, xmax=10.5) ]
        self.Histograms += [ defineHistogram('TgcOutOfTime', type='TH1F', title="Distribution of the BCID difference for the Tgc RoI out of Time; (Muon RoI BCID - Event BCID)",
                                            xbins=21, xmin=-10.5, xmax=10.5) ]
        self.Histograms += [ defineHistogram('EtaOutOfTime, PhiOutOfTime', type='TH2F', title="Eta vs Phi of the Mupon RoI out of time; Eta; Phi",
                                            xbins=108, xmin=-2.7, xmax=2.7, ybins=96, ymin=-3.1416, ymax=3.1416 ) ]



class TrigmuRoICosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigmuFastCosmicMonitoring"):
        super(TrigmuRoICosmicMonitoring, self).__init__(name)
        self.defineTarget("Cosmic")

        self.Histograms = [ defineHistogram('RpcOutOfTime', type='TH1F', title="Distribution of the BCID difference for the Rpc RoI out of Time; (Muon RoI BCID - Event BCID)",
                                            xbins=21, xmin=-10.5, xmax=10.5) ]
        self.Histograms += [ defineHistogram('TgcOutOfTime', type='TH1F', title="Distribution of the BCID difference for the Tgc RoI out of Time; (Muon RoI BCID - Event BCID)",
                                            xbins=21, xmin=-10.5, xmax=10.5) ]
        self.Histograms += [ defineHistogram('EtaOutOfTime, PhiOutOfTime', type='TH2F', title="Eta vs Phi of the Mupon RoI out of time; Eta; Phi",
                                            xbins=108, xmin=-2.7, xmax=2.7, ybins=96, ymin=-3.1416, ymax=3.1416 ) ]

class TrigmuRoIMTMonitoring(GenericMonitoringTool):
    def __init__ (self, name = "TrigmuRoIMTMonitoring"):
        super(TrigmuRoIMTMonitoring, self).__init__( name )

        self.HistPath = name
        self.defineHistogram('RpcOutOfTime', type='TH1F', path='EXPERT', title="Distribution of the BCID difference for the Rpc RoI out of Time; (Muon RoI BCID - Event BCID)",xbins=21, xmin=-10.5, xmax=10.5)
        self.defineHistogram('TgcOutOfTime', type='TH1F', path='EXPERT', title="Distribution of the BCID difference for the Tgc RoI out of Time; (Muon RoI BCID - Event BCID)",xbins=21, xmin=-10.5, xmax=10.5)
        self.defineHistogram('EtaOutOfTime, PhiOutOfTime', type='TH2F', path='EXPERT', title="Eta vs Phi of the Mupon RoI out of time; Eta; Phi", xbins=108, xmin=-2.7, xmax=2.7, ybins=96, ymin=-3.1416, ymax=3.1416 )
