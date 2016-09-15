# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

from math import pi


class TrigCaloClusterMakerValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigCaloClusterMaker_Validation"):
        super(TrigCaloClusterMakerValidationMonitoring, self).__init__(name)
        
        self.defineTarget("Validation")

        self.Histograms += [ defineHistogram('container_size', type='TH1D', title="Container Size;     Number of Clusters; Entries", xbins=2001, xmin=-0.5, xmax=2000.5) ]
        self.Histograms += [ defineHistogram('algorithm_time', type='TH1D', title="Algorithm Time;     Time [ms];          Entries", xbins=1000, xmin= 0.0, xmax=1000.0) ]
        self.Histograms += [ defineHistogram('et',             type='TH1D', title="E_{T};              E_{T} [GeV];        Entries", xbins= 150, xmin= 0.0, xmax= 150.0) ]
        self.Histograms += [ defineHistogram('eta',            type='TH1D', title="#eta;               #eta;               Entries", xbins=  50, xmin=-5.0, xmax=   5.0) ]
        self.Histograms += [ defineHistogram('phi',            type='TH1D', title="#phi;               #phi;               Entries", xbins=  64, xmin= -pi, xmax=    pi) ]
        self.Histograms += [ defineHistogram('eta, phi',       type='TH2D', title="Number of Clusters; #eta;               #phi",    xbins=  50, xmin=-5.0, xmax=   5.0, 
                                                                                                                                     ybins=  64, ymin= -pi, ymax=    pi) ]
        self.Histograms += [ defineHistogram('clusterSize',    type='TH1D', title="Cluster Size;       Type;               Entries", xbins=  13, xmin= 0.5, xmax=  13.5) ]
        self.Histograms += [ defineHistogram('signalState',    type='TH1D', title="Signal State;       Signal State;       Entries", xbins=   4, xmin=-1.5, xmax=   2.5) ]
        self.Histograms += [ defineHistogram('size',           type='TH1D', title="Size;               Size [Cells];       Entries", xbins= 250, xmin= 0.0, xmax= 250.0) ]
        self.Histograms += [ defineHistogram('N_BAD_CELLS',    type='TH1D', title="N_BAD_CELLS;        N_BAD_CELLS;        Entries", xbins= 250, xmin= 0.0, xmax= 250.0) ]
        self.Histograms += [ defineHistogram('ENG_FRAC_MAX',   type='TH1D', title="ENG_FRAC_MAX;       ENG_FRAC_MAX;       Entries", xbins=  50, xmin= 0.0, xmax=   1.1) ]

                
class TrigCaloClusterMakerOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigCaloClusterMaker_Online"):
        super(TrigCaloClusterMakerOnlineMonitoring, self).__init__(name)
        
        self.defineTarget("Online")
        
        self.Histograms += [ defineHistogram('container_size', type='TH1D', title="Container Size;     Number of Clusters; Entries", xbins=2001, xmin=-0.5, xmax=2000.5) ]
        self.Histograms += [ defineHistogram('algorithm_time', type='TH1D', title="Algorithm Time;     Time [ms];          Entries", xbins=1000, xmin= 0.0, xmax=1000.0) ]
        self.Histograms += [ defineHistogram('et',             type='TH1D', title="E_{T};              E_{T} [GeV];        Entries", xbins= 150, xmin= 0.0, xmax= 150.0) ]
        self.Histograms += [ defineHistogram('eta',            type='TH1D', title="#eta;               #eta;               Entries", xbins=  50, xmin=-5.0, xmax=   5.0) ]
        self.Histograms += [ defineHistogram('phi',            type='TH1D', title="#phi;               #phi;               Entries", xbins=  64, xmin= -pi, xmax=    pi) ]
        self.Histograms += [ defineHistogram('eta, phi',       type='TH2D', title="Number of Clusters; #eta;               #phi",    xbins=  50, xmin=-5.0, xmax=   5.0, 
                                                                                                                                     ybins=  64, ymin= -pi, ymax=    pi) ]
        self.Histograms += [ defineHistogram('clusterSize',    type='TH1D', title="Cluster Size;       Type;               Entries", xbins=  13, xmin= 0.5, xmax=  13.5) ]
        self.Histograms += [ defineHistogram('signalState',    type='TH1D', title="Signal State;       Signal State;       Entries", xbins=   4, xmin=-1.5, xmax=   2.5) ]
        self.Histograms += [ defineHistogram('size',           type='TH1D', title="Size;               Size [Cells];       Entries", xbins= 250, xmin= 0.0, xmax= 250.0) ]
        self.Histograms += [ defineHistogram('N_BAD_CELLS',    type='TH1D', title="N_BAD_CELLS;        N_BAD_CELLS;        Entries", xbins= 250, xmin= 0.0, xmax= 250.0) ]
        self.Histograms += [ defineHistogram('ENG_FRAC_MAX',   type='TH1D', title="ENG_FRAC_MAX;       ENG_FRAC_MAX;       Entries", xbins=  50, xmin= 0.0, xmax=   1.1) ]
        
                        
class TrigCaloClusterMakerCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigCaloClusterMaker_Cosmic"):
        super(TrigCaloClusterMakerCosmicMonitoring, self).__init__(name)

        self.defineTarget("Cosmic")

        self.Histograms += [ defineHistogram('container_size', type='TH1D', title="Container Size;     Number of Clusters; Entries", xbins=2001, xmin=-0.5, xmax=2000.5) ]
        self.Histograms += [ defineHistogram('algorithm_time', type='TH1D', title="Algorithm Time;     Time [ms];          Entries", xbins=1000, xmin= 0.0, xmax=1000.0) ]
        self.Histograms += [ defineHistogram('et',             type='TH1D', title="E_{T};              E_{T} [GeV];        Entries", xbins= 150, xmin= 0.0, xmax= 150.0) ]
        self.Histograms += [ defineHistogram('eta',            type='TH1D', title="#eta;               #eta;               Entries", xbins=  50, xmin=-5.0, xmax=   5.0) ]
        self.Histograms += [ defineHistogram('phi',            type='TH1D', title="#phi;               #phi;               Entries", xbins=  64, xmin= -pi, xmax=    pi) ]
        self.Histograms += [ defineHistogram('eta, phi',       type='TH2D', title="Number of Clusters; #eta;               #phi",    xbins=  50, xmin=-5.0, xmax=   5.0, 
                                                                                                                                     ybins=  64, ymin= -pi, ymax=    pi) ]
        self.Histograms += [ defineHistogram('clusterSize',    type='TH1D', title="Cluster Size;       Type;               Entries", xbins=  13, xmin= 0.5, xmax=  13.5) ]
        self.Histograms += [ defineHistogram('signalState',    type='TH1D', title="Signal State;       Signal State;       Entries", xbins=   4, xmin=-1.5, xmax=   2.5) ]
        self.Histograms += [ defineHistogram('size',           type='TH1D', title="Size;               Size [Cells];       Entries", xbins= 250, xmin= 0.0, xmax= 250.0) ]
        self.Histograms += [ defineHistogram('N_BAD_CELLS',    type='TH1D', title="N_BAD_CELLS;        N_BAD_CELLS;        Entries", xbins= 250, xmin= 0.0, xmax= 250.0) ]
        self.Histograms += [ defineHistogram('ENG_FRAC_MAX',   type='TH1D', title="ENG_FRAC_MAX;       ENG_FRAC_MAX;       Entries", xbins=  50, xmin= 0.0, xmax=   1.1) ]

        
class TrigFullCaloClusterMakerValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigFullCaloClusterMaker_Validation"):
        super(TrigFullCaloClusterMakerValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")

        self.Histograms += [ defineHistogram('ClusterContainerSize', type='TH1F', title="TrigCaloClusterMaker Cluster container size; Size; nevents", xbins=100, xmin=0., xmax=25.) ]
        self.Histograms += [ defineHistogram('Eta',                  type='TH1F', title="TrigCaloClusterMaker Cluster #eta; #eta; nevents",           xbins=80, xmin=-4., xmax=4.)]
        self.Histograms += [ defineHistogram('Phi',                  type='TH1F', title="TrigCaloClusterMaker Cluster #phi; #phi; nevents",           xbins=64, xmin=-3.2,xmax=3.2)]
        self.Histograms += [ defineHistogram('Et',                   type='TH1F', title="TrigCaloClusterMaker Cluster Et; Et; nevents",               xbins=100,xmin=0.,  xmax=150000.)]
        self.Histograms += [ defineHistogram('L2Eta',                type='TH1F', title="TrigCaloClusterMaker Cluster L2 #eta; #eta; nevents",        xbins=80, xmin=-4., xmax=4.)]
        self.Histograms += [ defineHistogram('L2Phi',                type='TH1F', title="TrigCaloClusterMaker Cluster L2 #phi; #phi; nevents",        xbins=64, xmin=-3.2,xmax=3.2)]
        self.Histograms += [ defineHistogram('Eta, Phi',             type='TH2F', title="Eta vs Phi", xbins=60, xmin=-3., xmax=3., ybins=64, ymin=-3.2, ymax=3.2) ]

class TrigFullCaloClusterMakerOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigFullCaloClusterMaker_Online"):
        super(TrigFullCaloClusterMakerOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")

        self.Histograms += [ defineHistogram('ClusterContainerSize', type='TH1F', title="TrigCaloClusterMaker Cluster container size; Size; nevents", xbins=100, xmin=0., xmax=25.) ]
        self.Histograms += [ defineHistogram('Eta',                  type='TH1F', title="TrigCaloClusterMaker Cluster #eta; #eta; nevents",           xbins=80, xmin=-4., xmax=4.)]
        self.Histograms += [ defineHistogram('Phi',                  type='TH1F', title="TrigCaloClusterMaker Cluster #phi; #phi; nevents",           xbins=64, xmin=-3.2,xmax=3.2)]
        self.Histograms += [ defineHistogram('Et',                   type='TH1F', title="TrigCaloClusterMaker Cluster Et; Et; nevents",               xbins=100,xmin=0.,  xmax=150000.)]
        self.Histograms += [ defineHistogram('L2Eta',                type='TH1F', title="TrigCaloClusterMaker Cluster L2 #eta; #eta; nevents",        xbins=80, xmin=-4., xmax=4.)]
        self.Histograms += [ defineHistogram('L2Phi',                type='TH1F', title="TrigCaloClusterMaker Cluster L2 #phi; #phi; nevents",        xbins=64, xmin=-3.2,xmax=3.2)]
        self.Histograms += [ defineHistogram('Eta, Phi',             type='TH2F', title="Eta vs Phi", xbins=60, xmin=-3., xmax=3., ybins=64, ymin=-3.2, ymax=3.2) ]
        
class TrigFullCaloClusterMakerCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigFullCaloClusterMaker_Cosmic"):
        super(TrigFullCaloClusterMakerCosmicMonitoring, self).__init__(name)

        self.defineTarget("Cosmic")

        self.Histograms += [ defineHistogram('ClusterContainerSize', type='TH1F', title="TrigCaloClusterMaker Cluster container size; Size; nevents", xbins=100, xmin=0., xmax=25.) ]
        self.Histograms += [ defineHistogram('Eta',                  type='TH1F', title="TrigCaloClusterMaker Cluster #eta; #eta; nevents",           xbins=80, xmin=-4., xmax=4.)]
        self.Histograms += [ defineHistogram('Phi',                  type='TH1F', title="TrigCaloClusterMaker Cluster #phi; #phi; nevents",           xbins=64, xmin=-3.2,xmax=3.2)]
        self.Histograms += [ defineHistogram('Et',                   type='TH1F', title="TrigCaloClusterMaker Cluster Et; Et; nevents",               xbins=100,xmin=0.,  xmax=150000.)]
        self.Histograms += [ defineHistogram('L2Eta',                type='TH1F', title="TrigCaloClusterMaker Cluster L2 #eta; #eta; nevents",        xbins=80, xmin=-4., xmax=4.)]
        self.Histograms += [ defineHistogram('L2Phi',                type='TH1F', title="TrigCaloClusterMaker Cluster L2 #phi; #phi; nevents",        xbins=64, xmin=-3.2,xmax=3.2)]
        self.Histograms += [ defineHistogram('Eta, Phi',             type='TH2F', title="Eta vs Phi", xbins=60, xmin=-3., xmax=3., ybins=64, ymin=-3.2, ymax=3.2) ]

class TrigL1BSTowerHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL1BSTowerHypo_Online"):
        super(TrigL1BSTowerHypoOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")

        self.Histograms += [ defineHistogram('EtaEM',                type='TH1F', title="TrigL1BSTowerHypo EM #eta; #eta; nevents",           xbins=80, xmin=-4., xmax=4.)]
        self.Histograms += [ defineHistogram('PhiEM',                type='TH1F', title="TrigL1BSTowerHypo EM #phi; #phi; nevents",           xbins=64, xmin=0.0,xmax=6.283)]
        self.Histograms += [ defineHistogram('EtEM',                 type='TH1F', title="TrigL1BSTowerHypo EM Et; Et; nevents",              xbins=150,xmin=0.,  xmax=300.)]
        self.Histograms += [ defineHistogram('MaxAdcEM',             type='TH1F', title="TrigL1BSTowerHypo EM maxADC; maxADC; nevents",              xbins=125,xmin=0.,  xmax=1250.)]
        self.Histograms += [ defineHistogram('EtaEM, PhiEM',         type='TH2F', title="Eta vs Phi for EM Towers", xbins=60, xmin=-3., xmax=3., ybins=64, ymin=0.0, ymax=6.283) ]
        self.Histograms += [ defineHistogram('EtaHAD',               type='TH1F', title="TrigL1BSTowerHypo HAD #eta; #eta; nevents",           xbins=80, xmin=-4., xmax=4.)]
        self.Histograms += [ defineHistogram('PhiHAD',               type='TH1F', title="TrigL1BSTowerHypo HAD #phi; #phi; nevents",           xbins=64, xmin=0.0,xmax=6.283)]
        self.Histograms += [ defineHistogram('EtHAD',                type='TH1F', title="TrigL1BSTowerHypo HAD Et; Et; nevents",              xbins=150,xmin=0.,  xmax=300.)]
        self.Histograms += [ defineHistogram('MaxAdcHAD',            type='TH1F', title="TrigL1BSTowerHypo HAD maxADC; maxADC; nevents",              xbins=125,xmin=0.,  xmax=1250.)]
        self.Histograms += [ defineHistogram('EtaHAD, PhiHAD',       type='TH2F', title="Eta vs Phi for HAD Towers", xbins=60, xmin=-3., xmax=3., ybins=64, ymin=0.0, ymax=6.283) ]
