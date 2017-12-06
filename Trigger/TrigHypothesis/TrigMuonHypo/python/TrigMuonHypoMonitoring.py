# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig
from AthenaMonitoring.GenericMonitoringTool import GenericMonitoringTool, defineHistogram

class TrigMufastHypoMonitoring(GenericMonitoringTool):
    def __init__ (self, name="TrigMufastHypoMonitoring"):
        super(TrigMufastHypoMonitoring, self).__init__(name)

        self.HistPath = name  
        self.Histograms = [ defineHistogram('Pt', type='TH1F', title="P_{T} reconstruction from #muFast; P_{T} (GeV)", xbins=200, xmin=-100, xmax=100),
                            defineHistogram('Eta , Phi', type='TH2F', title="Eta vs Phi reconstruction of #muFast; Eta; Phi",xbins=50, xmin=-3.2, xmax=3.2, ybins=25, ymin=-3.15, ymax=3.15),
                            defineHistogram('Eta', type='TH1F', title="Eta reconstruction from #muFast; Eta",xbins=100, xmin=-3.2, xmax=3.2),
                            defineHistogram('Phi', type='TH1F', title="Phi reconstruction from #muFast; Phi",xbins=100, xmin=-3.15, xmax=3.15),
                            defineHistogram('ZatSt, Phi', type='TH2F', title="Z vs Phi reconstructed in MIDDLE station; Z (cm); Phi (rad)",xbins=50, xmin=-1200., xmax=1200., ybins=25, ymin=-3.2, ymax=3.2),
                            defineHistogram('XatSt , YatSt', type='TH2F', title="Y vs X reconstructed in MIDDLE station; X (cm); Y(cm)",xbins=50, xmin=-1200., xmax=1200., ybins=50, ymin=-1200., ymax=1200.),
                            defineHistogram('ZatBe', type='TH1F', title="DCA along Z; Z (cm)",xbins=100, xmin=-2100, xmax=2100),
                            defineHistogram('XatBe', type='TH1F', title="DCA along X; X (cm)",xbins=100, xmin=-1000, xmax=1000) ]


class MufastHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="MufastHypoValidationMonitoring"):
        super(MufastHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")

        self.Histograms  = [ defineHistogram('Pt', type='TH1F', title="P_{T} reconstruction from #muFast; P_{T} (GeV)",
                                             xbins=200, xmin=-100, xmax=100) ]
        self.Histograms += [ defineHistogram('Eta , Phi', type='TH2F', title="Eta vs Phi reconstruction of #muFast; Eta; Phi",
                                             xbins=50, xmin=-3.2, xmax=3.2, ybins=25, ymin=-3.15, ymax=3.15) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="Eta reconstruction from #muFast; Eta",
                                             xbins=100, xmin=-3.2, xmax=3.2) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="Phi reconstruction from #muFast; Phi",
                                             xbins=100, xmin=-3.15, xmax=3.15) ]
        self.Histograms += [ defineHistogram('ZatSt, Phi', type='TH2F', title="Z vs Phi reconstructed in MIDDLE station; Z (cm); Phi (rad)",
                                             xbins=50, xmin=-1200., xmax=1200., ybins=25, ymin=-3.2, ymax=3.2) ]
        self.Histograms += [ defineHistogram('XatSt , YatSt', type='TH2F', title="Y vs X reconstructed in MIDDLE station; X (cm); Y(cm)",
                                             xbins=50, xmin=-1200., xmax=1200., ybins=50, ymin=-1200., ymax=1200.) ]
        self.Histograms += [ defineHistogram('ZatBe', type='TH1F', title="DCA along Z; Z (cm)",
                                             xbins=100, xmin=-2100, xmax=2100) ]
        self.Histograms += [ defineHistogram('XatBe', type='TH1F', title="DCA along X; X (cm)",
                                             xbins=100, xmin=-1000, xmax=1000) ]

class MufastHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="MufastHypoOnlineMonitoring"):
        super(MufastHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms  = [ defineHistogram('Pt', type='TH1F', title="P_{T} reconstruction from #muFast; P_{T} (GeV)",
                                             xbins=200, xmin=-100, xmax=100) ]
        self.Histograms += [ defineHistogram('Eta , Phi', type='TH2F', title="Eta vs Phi reconstruction of #muFast; Eta; Phi",
                                             xbins=50, xmin=-3.2, xmax=3.2, ybins=25, ymin=-3.15, ymax=3.15) ] 
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="Eta reconstruction from #muFast; Eta",
                                             xbins=100, xmin=-3.2, xmax=3.2) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="Phi reconstruction from #muFast; Phi",
                                             xbins=100, xmin=-3.15, xmax=3.15) ]
        self.Histograms += [ defineHistogram('ZatSt, Phi', type='TH2F', title="Z vs Phi reconstructed in MIDDLE station; Z (cm); Phi (rad)",
                                             xbins=50, xmin=-1200., xmax=1200., ybins=25, ymin=-3.2, ymax=3.2) ]
        self.Histograms += [ defineHistogram('XatSt , YatSt', type='TH2F', title="Y vs X reconstructed in MIDDLE station; X (cm); Y(cm)",
                                             xbins=50, xmin=-1200., xmax=1200., ybins=50, ymin=-1200., ymax=1200.) ]
        self.Histograms += [ defineHistogram('ZatBe', type='TH1F', title="DCA along Z; Z (cm)",
                                             xbins=100, xmin=-2100, xmax=2100) ]
        self.Histograms += [ defineHistogram('XatBe', type='TH1F', title="DCA along X; X (cm)",
                                             xbins=100, xmin=-1000, xmax=1000) ]

class MufastHypoCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="MufastHypoCosmicMonitoring"):
        super(MufastHypoCosmicMonitoring, self).__init__(name)
        self.defineTarget("Cosmic")

        self.Histograms  = [ defineHistogram('Pt', type='TH1F', title="P_{T} reconstruction from #muFast; P_{T} (GeV)",
                                             xbins=200, xmin=-100, xmax=100) ]
        self.Histograms += [ defineHistogram('Eta , Phi', type='TH2F', title="Eta vs Phi reconstruction of #muFast; Eta; Phi",
                                             xbins=50, xmin=-3.2, xmax=3.2, ybins=25, ymin=-3.15, ymax=3.15) ]
        self.Histograms += [ defineHistogram('ZatSt, Phi', type='TH2F', title="Z vs Phi reconstructed in MIDDLE station; Z (cm); Phi (rad)",
                                             xbins=100, xmin=-1200., xmax=1200., ybins=50, ymin=-3.2, ymax=3.2) ]
        self.Histograms += [ defineHistogram('XatSt , YatSt', type='TH2F', title="Y vs X reconstructed in MIDDLE station; X (cm); Y(cm)",
                                             xbins=100, xmin=-1200., xmax=1200., ybins=100, ymin=-1200., ymax=1200.) ]
        self.Histograms += [ defineHistogram('ZatBe', type='TH1F', title="DCA along Z; Z (cm)",
                                             xbins=100, xmin=-2100, xmax=2100) ]
        self.Histograms += [ defineHistogram('XatBe', type='TH1F', title="DCA along X; X (cm)",
                                             xbins=100, xmin=-1000, xmax=1000) ]

class MufastPEBHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="MufastPEBHypoValidationMonitoring"):
        super(MufastPEBHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")

        self.Histograms  = [ defineHistogram('Pt', type='TH1F', title="P_{T} reconstruction from #muFast; P_{T} (GeV)",
                                             xbins=200, xmin=-100, xmax=100) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="Eta reconstruction from #muFast; Eta",
                                             xbins=100, xmin=-3.2, xmax=3.2) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="Phi reconstruction from #muFast; Phi",
                                             xbins=100, xmin=-3.15, xmax=3.15) ]

class MufastPEBHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="MufastPEBHypoOnlineMonitoring"):
        super(MufastPEBHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms  = [ defineHistogram('Pt', type='TH1F', title="P_{T} reconstruction from #muFast; P_{T} (GeV)",
                                             xbins=200, xmin=-100, xmax=100) ]
        self.Histograms += [ defineHistogram('Eta , Phi', type='TH2F', title="Eta vs Phi reconstruction of #muFast; Eta; Phi",
                                             xbins=50, xmin=-3.2, xmax=3.2, ybins=25, ymin=-3.15, ymax=3.15) ]

class MufastPEBHypoCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="MufastPEBHypoCosmicMonitoring"):
        super(MufastPEBHypoCosmicMonitoring, self).__init__(name)
        self.defineTarget("Cosmic")

        self.Histograms  = [ defineHistogram('Pt', type='TH1F', title="P_{T} reconstruction from #muFast; P_{T} (GeV)",
                                             xbins=200, xmin=-100, xmax=100) ]
        self.Histograms += [ defineHistogram('Eta , Phi', type='TH2F', title="Eta vs Phi reconstruction of #muFast; Eta; Phi",
                                             xbins=50, xmin=-3.2, xmax=3.2, ybins=25, ymin=-3.15, ymax=3.15) ]

class MufastCALHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="MufastCALHypoValidationMonitoring"):
        super(MufastCALHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")

        self.Histograms  = [ defineHistogram('Pt', type='TH1F', title="P_{T} reconstruction from #muFast; P_{T} (GeV)",
                                             xbins=200, xmin=-100, xmax=100) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="Eta reconstruction from #muFast; Eta",
                                             xbins=100, xmin=-3.2, xmax=3.2) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="Phi reconstruction from #muFast; Phi",
                                             xbins=100, xmin=-3.15, xmax=3.15) ]

class MufastCALHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="MufastCALHypoOnlineMonitoring"):
        super(MufastCALHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms  = [ defineHistogram('Pt', type='TH1F', title="P_{T} reconstruction from #muFast; P_{T} (GeV)",
                                             xbins=200, xmin=-100, xmax=100) ]
        self.Histograms += [ defineHistogram('Eta , Phi', type='TH2F', title="Eta vs Phi reconstruction of #muFast; Eta; Phi",
                                             xbins=50, xmin=-3.2, xmax=3.2, ybins=25, ymin=-3.15, ymax=3.15) ]

class MufastCALHypoCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="MufastCALHypoCosmicMonitoring"):
        super(MufastCALHypoCosmicMonitoring, self).__init__(name)
        self.defineTarget("Cosmic")

        self.Histograms  = [ defineHistogram('Pt', type='TH1F', title="P_{T} reconstruction from #muFast; P_{T} (MeV)",
                                             xbins=200, xmin=-100, xmax=100) ]
        self.Histograms += [ defineHistogram('Eta , Phi', type='TH2F', title="Eta vs Phi reconstruction of #muFast; Eta; Phi",
                                             xbins=50, xmin=-3.2, xmax=3.2, ybins=25, ymin=-3.15, ymax=3.15) ]

class MufastStauHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="MufastStauHypoValidationMonitoring"):
        super(MufastStauHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")

        self.Histograms  = [ defineHistogram('Pt', type='TH1F', title="P_{T} reconstruction from #muFast; P_{T} (GeV)",
                                             xbins=200, xmin=-100, xmax=100) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="Eta reconstruction from #muFast; Eta",
                                             xbins=100, xmin=-3.2, xmax=3.2) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="Phi reconstruction from #muFast; Phi",
                                             xbins=100, xmin=-3.15, xmax=3.15) ]
        self.Histograms += [ defineHistogram('Mass', type='TH1F', title="Mass reconstruction from #muFast; Mass (GeV)",
                                             xbins=250, xmin=0, xmax=1000) ]
        self.Histograms += [ defineHistogram('Beta', type='TH1F', title="Beta reconstruction from #muFast; Beta",
                                             xbins=100, xmin=0, xmax=1.2) ]

class MufastStauHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="MufastStauHypoOnlineMonitoring"):
        super(MufastStauHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms  = [ defineHistogram('Pt', type='TH1F', title="P_{T} reconstruction from #muFast; P_{T} (GeV)",
                                             xbins=200, xmin=-100, xmax=100) ]
        self.Histograms += [ defineHistogram('Eta , Phi', type='TH2F', title="Eta vs Phi reconstruction of #muFast; Eta; Phi",
                                             xbins=50, xmin=-3.2, xmax=3.2, ybins=25, ymin=-3.15, ymax=3.15) ]
        self.Histograms += [ defineHistogram('Mass', type='TH1F', title="Mass reconstruction from #muFast; Mass (GeV)",
                                             xbins=250, xmin=0, xmax=1000) ]
        self.Histograms += [ defineHistogram('Beta', type='TH1F', title="Beta reconstruction from #muFast; Beta",
                                             xbins=100, xmin=0, xmax=1.2) ]

class MufastStauHypoCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="MufastStauHypoCosmicMonitoring"):
        super(MufastStauHypoCosmicMonitoring, self).__init__(name)
        self.defineTarget("Cosmic")

        self.Histograms  = [ defineHistogram('Pt', type='TH1F', title="P_{T} reconstruction from #muFast; P_{T} (GeV)",
                                             xbins=200, xmin=-100, xmax=100) ]
        self.Histograms += [ defineHistogram('Eta , Phi', type='TH2F', title="Eta vs Phi reconstruction of #muFast; Eta; Phi",
                                             xbins=50, xmin=-3.2, xmax=3.2, ybins=25, ymin=-3.15, ymax=3.15) ]


class MufastOTRHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="MufastOTRHypoValidationMonitoring"):
        super(MufastOTRHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms  = [ defineHistogram('RpcOutOfTimeIn', type='TH1F', title="Distribution of the BCID difference for the Rpc RoI out of Time; (Muon RoI BCID - Event BCID)",
                                             xbins=21, xmin=-10.5, xmax=10.5) ]
        self.Histograms += [ defineHistogram('TgcOutOfTimeIn', type='TH1F', title="Distribution of the BCID difference for the Tgc RoI out of Time; (Muon RoI BCID - Event BCID)",
                                             xbins=21, xmin=-10.5, xmax=10.5) ]
        self.Histograms += [ defineHistogram('RpcOutOfTimeOut', type='TH1F', title="Distribution of the BCID difference for the Rpc RoI out of Time; (Muon RoI BCID - Event BCID)",
                                            xbins=21, xmin=-10.5, xmax=10.5) ]
        self.Histograms += [ defineHistogram('TgcOutOfTimeOut', type='TH1F', title="Distribution of the BCID difference for the Tgc RoI out of Time; (Muon RoI BCID - Event BCID)",
                                             xbins=21, xmin=-10.5, xmax=10.5) ]
        self.Histograms += [ defineHistogram('Pt', type='TH1F', title="P_{T} reconstruction from #muFast; P_{T} (MeV)",
                                             xbins=200, xmin=-100, xmax=100) ]
        self.Histograms += [ defineHistogram('Eta , Phi', type='TH2F', title="Eta vs Phi reconstruction of #muFast; Eta; Phi",
                                             xbins=50, xmin=-3.2, xmax=3.2, ybins=25, ymin=-3.15, ymax=3.15) ]

class MufastOTRHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="MufastOTRHypoOnlineMonitoring"):
        super(MufastOTRHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms  = [ defineHistogram('RpcOutOfTimeIn', type='TH1F', title="Distribution of the BCID difference for the Rpc RoI out of Time; (Muon RoI BCID - Event BCID)",
                                             xbins=21, xmin=-10.5, xmax=10.5) ]
        self.Histograms += [ defineHistogram('TgcOutOfTimeIn', type='TH1F', title="Distribution of the BCID difference for the Tgc RoI out of Time; (Muon RoI BCID - Event BCID)",
                                             xbins=21, xmin=-10.5, xmax=10.5) ]
        self.Histograms += [ defineHistogram('RpcOutOfTimeOut', type='TH1F', title="Distribution of the BCID difference for the Rpc RoI out of Time; (Muon RoI BCID - Event BCID)",
                                             xbins=21, xmin=-10.5, xmax=10.5) ]
        self.Histograms += [ defineHistogram('TgcOutOfTimeOut', type='TH1F', title="Distribution of the BCID difference for the Tgc RoI out of Time; (Muon RoI BCID - Event BCID)",
                                            xbins=21, xmin=-10.5, xmax=10.5) ]
        self.Histograms += [ defineHistogram('Pt', type='TH1F', title="P_{T} reconstruction from #muFast; P_{T} (MeV)",
                                             xbins=200, xmin=-100, xmax=100) ]
        self.Histograms += [ defineHistogram('Eta , Phi', type='TH2F', title="Eta vs Phi reconstruction of #muFast; Eta; Phi",
                                             xbins=50, xmin=-3.2, xmax=3.2, ybins=25, ymin=-3.15, ymax=3.15) ]

class MufastOTRHypoCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="MufastOTRHypoCosmicMonitoring"):
        super(MufastOTRHypoCosmicMonitoring, self).__init__(name)
        self.defineTarget("Cosmic")
        self.Histograms  = [ defineHistogram('RpcOutOfTimeIn', type='TH1F', title="Distribution of the BCID difference for the Rpc RoI out of Time; (Muon RoI BCID - Event BCID)",
                                             xbins=21, xmin=-10.5, xmax=10.5) ]
        self.Histograms += [ defineHistogram('TgcOutOfTimeIn', type='TH1F', title="Distribution of the BCID difference for the Tgc RoI out of Time; (Muon RoI BCID - Event BCID)",
                                             xbins=21, xmin=-10.5, xmax=10.5) ]
        self.Histograms += [ defineHistogram('RpcOutOfTimeOut', type='TH1F', title="Distribution of the BCID difference for the Rpc RoI out of Time; (Muon RoI BCID - Event BCID)",
                                             xbins=21, xmin=-10.5, xmax=10.5) ]
        self.Histograms += [ defineHistogram('TgcOutOfTimeOut', type='TH1F', title="Distribution of the BCID difference for the Tgc RoI out of Time; (Muon RoI BCID - Event BCID)",
                                             xbins=21, xmin=-10.5, xmax=10.5) ]
        self.Histograms += [ defineHistogram('Pt', type='TH1F', title="P_{T} reconstruction from #muFast; P_{T} (MeV)",
                                             xbins=200, xmin=-100, xmax=100) ]
        self.Histograms += [ defineHistogram('Eta , Phi', type='TH2F', title="Eta vs Phi reconstruction of #muFast; Eta; Phi",
                                             xbins=50, xmin=-3.2, xmax=3.2, ybins=25, ymin=-3.15, ymax=3.15) ]

class MucombHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="MucombHypoValidationMonitoring"):
        super(MucombHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")

        self.Histograms = [ defineHistogram('Pt', type='TH1F', title="p_{T} reconstruction from #muComb; p_{T} (GeV)",
                                            xbins=210, xmin=-105, xmax=105) ]
        self.Histograms += [ defineHistogram('PtFL', type='TH1F', title="p_{T} of not selected muons from #muComb; p_{T} (GeV)",
                                            xbins=210, xmin=-105., xmax=105.) ]
        self.Histograms += [ defineHistogram('StrategyFlag', type='TH1F', title="Combination Strategy from #muComb; Strategy Code",
                                            xbins=12, xmin=-1.5, xmax=10.5) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="Eta reconstruction from #muComb; Eta",
                                             xbins=108, xmin=-2.7, xmax=2.7) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="Phi reconstruction from #muComb; Phi (rad)",
                                             xbins=96, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('Z0', type='TH1F', title="PCA along Z from ID track from #muComb; PCA(Z0) (mm)",
                                             xbins=100, xmin=-200, xmax=200) ]
        self.Histograms += [ defineHistogram('A0', type='TH1F', title="PCA along x-y from ID track from #muComb; PCA(A0) (mm)",
                                             xbins=100, xmin=-0.6, xmax=0.6) ]

class MucombHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="MucombHypoOnlineMonitoring"):
        super(MucombHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        
        self.Histograms = [ defineHistogram('Pt', type='TH1F', title="p_{T} reconstruction from #muComb; p_{T} (GeV)",
                                            xbins=210, xmin=-105, xmax=105) ]
        self.Histograms += [ defineHistogram('StrategyFlag', type='TH1F', title="Combination Strategy from #muComb; Strategy Code",
                                            xbins=12, xmin=-1.5, xmax=10.5) ]

class MucombStauHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="MucombStauHypoOnlineMonitoring"):
        super(MucombStauHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms = [ defineHistogram('Pt', type='TH1F', title="P_{T} reconstruction from #muComb; P_{T} (MeV)",
                                            xbins=200, xmin=-100, xmax=100) ]
        self.Histograms += [ defineHistogram('Z0', type='TH1F', title="PCA along Z from ID track;PCA(Z0) (mm)",
                                            xbins=100, xmin=-20, xmax=20) ]
        self.Histograms += [ defineHistogram('A0', type='TH1F', title="PCA along x-y from ID track; PCA(A0) (mm)",
                                            xbins=100, xmin=-0.6, xmax=0.6) ]
        self.Histograms += [ defineHistogram('Mass', type='TH1F', title="Mass reconstruction from #muFast; Mass (GeV)",
                                             xbins=250, xmin=0, xmax=1000) ]
        self.Histograms += [ defineHistogram('IDMass', type='TH1F', title="Combined Mass reconstruction; Mass (GeV)",
                                             xbins=250, xmin=0, xmax=1000) ]
        self.Histograms += [ defineHistogram('Beta', type='TH1F', title="Beta reconstruction from #muFast; Beta",
                                             xbins=100, xmin=0, xmax=1.2) ]


class MucombStauHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="MucombStauHypoValidationMonitoring"):
        super(MucombStauHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")

        self.Histograms = [ defineHistogram('Pt', type='TH1F', title="P_{T} reconstruction from #muComb; P_{T} (MeV)",
                                            xbins=200, xmin=-100, xmax=100) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="Eta reconstruction from #muComb; Eta",
                                            xbins=100, xmin=-3.2, xmax=3.2) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="Phi reconstruction from #muComb; Phi",
                                    xbins=100, xmin=-3.15, xmax=3.15) ]
        self.Histograms += [ defineHistogram('Z0', type='TH1F', title="PCA along Z from ID track;PCA(Z0) (mm)",
                                            xbins=100, xmin=-60, xmax=60) ]
        self.Histograms += [ defineHistogram('A0', type='TH1F', title="PCA along x-y from ID track; PCA(A0) (mm)",
                                            xbins=100, xmin=-0.6, xmax=0.6) ]
        self.Histograms += [ defineHistogram('Mass', type='TH1F', title="Mass reconstruction from #muFast; Mass (GeV)",
                                             xbins=250, xmin=0, xmax=1000) ]
        self.Histograms += [ defineHistogram('IDMass', type='TH1F', title="Combined Mass reconstruction; Mass (GeV)",
                                             xbins=250, xmin=0, xmax=1000) ]
        self.Histograms += [ defineHistogram('Beta', type='TH1F', title="Beta reconstruction from #muFast; Beta",
                                             xbins=100, xmin=0, xmax=1.2) ]


# MuIsoHypo Monitoring
################ Validation, DQ checks
class MuisoHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="MuisoHypo_Online"):
        super(MuisoHypoOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")

        self.Histograms  = [ defineHistogram('CutCounter', type='TH1F', title="MuIsoHypo cut counter;cut; nevents", xbins=9, xmin=-1.5, xmax=7.5, opt="kCumulative") ]

##########   Create instance for validation (additional specific histograms)
class MuisoHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="MuisoHypo_Validation"):
        super(MuisoHypoValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")

        self.Histograms  = [ defineHistogram('CutCounter', type='TH1F', title="MuIsoHypo cut counter;cut; nevents", xbins=9, xmin=-1.5, xmax=7.5, opt="kCumulative") ]
        self.Histograms += [ defineHistogram('SumPtCone', type='TH1F', title="MuIsoHypo SumPt in cone around muon;E [GeV/c]; nevents", xbins=200, xmin=0., xmax=15.) ]

##########   Create instance for cosmic run
class MuisoHypoCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="MuisoHypo_Cosmic"):
        super(MuisoHypoCosmicMonitoring, self).__init__(name)

        self.defineTarget("Cosmic")

        self.Histograms  = [ defineHistogram('CutCounter', type='TH1F', title="MuIsoHypo cut counter;cut; nevents", xbins=9, xmin=-1.5, xmax=7.5, opt="kCumulative") ]


class TrigMooreHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigMooreHypoValidationMonitoring"):
        super(TrigMooreHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")

        self.Histograms = [ defineHistogram('Pt', type='TH1F', title="P_{T} reconstruction from #muFast; P_{T} (MeV)",
                                            xbins=200, xmin=-100, xmax=100) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="Eta reconstruction from #muFast; Eta",
                                            xbins=100, xmin=-3.2, xmax=3.2) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="Phi reconstruction from #muFast; Phi",
                                    xbins=100, xmin=-3.15, xmax=3.15) ]


class TrigMooreHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigMooreHypoOnlineMonitoring"):
        super(TrigMooreHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms = [ defineHistogram('Pt', type='TH1F', title="P_{T} reconstruction from #muFast; P_{T} (MeV)",
                                            xbins=200, xmin=-100, xmax=100) ]


class TrigMuonEFTrackBuilderHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigMuonEFTrackBuilderHypoValidationMonitoring"):
        super(TrigMuonEFTrackBuilderHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")

        self.Histograms = [ defineHistogram('Pt', type='TH1F', title="P_{T} reconstruction from #TrigMuonEFTrackBuilderHypo; P_{T} (MeV)",
                                            xbins=200, xmin=-100, xmax=100) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="Eta reconstruction from #TrigMuonEFTrackBuilderHypo; Eta",
                                            xbins=100, xmin=-3.2, xmax=3.2) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="Phi reconstruction from #TrigMuonEFTrackBuilderHypo; Phi",
                                    xbins=100, xmin=-3.15, xmax=3.15) ]


class TrigMuonEFTrackBuilderHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigMuonEFTrackBuilderHypoOnlineMonitoring"):
        super(TrigMuonEFTrackBuilderHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms = [ defineHistogram('Pt', type='TH1F', title="P_{T} reconstruction from #TrigMuonEFTrackBuilderHypo; P_{T} (MeV)",
                                            xbins=200, xmin=-100, xmax=100) ]


class TrigMuonEFExtrapolatorHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigMuonEFExtrapolatorHypoValidationMonitoring"):
        super(TrigMuonEFExtrapolatorHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")

        self.Histograms = [ defineHistogram('Pt', type='TH1F', title="P_{T} reconstruction from #TrigMuonEFExtrapolatorHypo; P_{T} (MeV)",
                                            xbins=200, xmin=-100, xmax=100) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="Eta reconstruction from #TrigMuonEFExtrapolatorHypo; Eta",
                                            xbins=100, xmin=-3.2, xmax=3.2) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="Phi reconstruction from #TrigMuonEFExtrapolatorHypo; Phi",
                                    xbins=100, xmin=-3.15, xmax=3.15) ]


class TrigMuonEFExtrapolatorHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigMuonEFExtrapolatorHypoOnlineMonitoring"):
        super(TrigMuonEFExtrapolatorHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms = [ defineHistogram('Pt', type='TH1F', title="P_{T} reconstruction from #TrigMuonEFExtrapolatorHypo; P_{T} (MeV)",
                                            xbins=200, xmin=-100, xmax=100) ]


class TrigMuonEFCombinerHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigMuonEFCombinerHypoValidationMonitoring"):
        super(TrigMuonEFCombinerHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")

        self.Histograms = [ defineHistogram('Pt', type='TH1F', title="P_{T} reconstruction from #TrigMuonEFCombinerHypo; P_{T} (GeV)",
                                            xbins=200, xmin=-100, xmax=100) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="Eta reconstruction from #TrigMuonEFCombinerHypo; Eta",
                                            xbins=100, xmin=-3.2, xmax=3.2) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="Phi reconstruction from #TrigMuonEFCombinerHypo; Phi",
                                             xbins=100, xmin=-3.15, xmax=3.15) ]

class TrigMuonEFCombinerHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigMuonEFCombinerHypoOnlineMonitoring"):
        super(TrigMuonEFCombinerHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms = [ defineHistogram('Pt', type='TH1F', title="P_{T} reconstruction from #TrigMuonEFCombinerHypo; P_{T} (GeV)",
                                            xbins=200, xmin=-100, xmax=100) ]



class TrigMuonEFCombinerMultiHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigMuonEFCombinerMultiHypoValidationMonitoring"):
        super(TrigMuonEFCombinerMultiHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")

        self.Histograms  = [ defineHistogram('Pt', type='TH1F', title="P_{T} reconstruction from #TrigMuonEFCombinerMultiHypo; P_{T} (GeV)",
                                            xbins=200, xmin=-100, xmax=100) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="Eta reconstruction from #TrigMuonEFCombinerMultiHypo; Eta",
                                            xbins=100, xmin=-3.2, xmax=3.2) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="Phi reconstruction from #TrigMuonEFCombinerMultiHypo; Phi",
                                             xbins=100, xmin=-3.15, xmax=3.15) ]
        self.Histograms += [ defineHistogram('Nmuons', type='TH1F', title="nr of muons from #TrigMuonEFCBSeededFSDiMuonHypo; N muons",
                                             xbins=10, xmin=0, xmax=10) ]


class TrigMuonEFCombinerMultiHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigMuonEFCombinerMultiHypoOnlineMonitoring"):
        super(TrigMuonEFCombinerMultiHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms  = [ defineHistogram('Pt', type='TH1F', title="P_{T} reconstruction from #TrigMuonEFCombinerMultiHypo; P_{T} (GeV)",
                                            xbins=200, xmin=-100, xmax=100) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="Eta reconstruction from #TrigMuonEFCombinerMultiHypo; Eta",
                                            xbins=100, xmin=-3.2, xmax=3.2) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="Phi reconstruction from #TrigMuonEFCombinerMultiHypo; Phi",
                                             xbins=100, xmin=-3.15, xmax=3.15) ]
        self.Histograms += [ defineHistogram('Nmuons', type='TH1F', title="nr of muons from #TrigMuonEFCBSeededFSDiMuonHypo; N muons",
                                             xbins=10, xmin=0, xmax=10) ]

class TrigMuonEFExtrapolatorMultiHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigMuonEFExtrapolatorMultiHypoValidationMonitoring"):
        super(TrigMuonEFExtrapolatorMultiHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")

        self.Histograms  = [ defineHistogram('Pt', type='TH1F', title="P_{T} reconstruction from #TrigMuonEFExtrapolatorMultiHypo; P_{T} (GeV)",
                                            xbins=200, xmin=-100, xmax=100) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="Eta reconstruction from #TrigMuonEFExtrapolatorMultiHypo; Eta",
                                            xbins=100, xmin=-3.2, xmax=3.2) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="Phi reconstruction from #TrigMuonEFExtrapolatorMultiHypo; Phi",
                                             xbins=100, xmin=-3.15, xmax=3.15) ]
        self.Histograms += [ defineHistogram('Nmuons', type='TH1F', title="nr of muons from #TrigMuonEFCBSeededFSDiMuonHypo; N muons",
                                             xbins=10, xmin=0, xmax=10) ]


class TrigMuonEFExtrapolatorMultiHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigMuonEFExtrapolatorMultiHypoOnlineMonitoring"):
        super(TrigMuonEFExtrapolatorMultiHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms  = [ defineHistogram('Pt', type='TH1F', title="P_{T} reconstruction from #TrigMuonEFExtrapolatorMultiHypo; P_{T} (GeV)",
                                            xbins=200, xmin=-100, xmax=100) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="Eta reconstruction from #TrigMuonEFExtrapolatorMultiHypo; Eta",
                                            xbins=100, xmin=-3.2, xmax=3.2) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="Phi reconstruction from #TrigMuonEFExtrapolatorMultiHypo; Phi",
                                             xbins=100, xmin=-3.15, xmax=3.15) ]
        self.Histograms += [ defineHistogram('Nmuons', type='TH1F', title="nr of muons from #TrigMuonEFCBSeededFSDiMuonHypo; N muons",
                                             xbins=10, xmin=0, xmax=10) ]

class TrigMuonEFTrackBuilderMultiHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigMuonEFTrackBuilderMultiHypoValidationMonitoring"):
        super(TrigMuonEFTrackBuilderMultiHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")

        self.Histograms  = [ defineHistogram('Pt', type='TH1F', title="P_{T} reconstruction from #TrigMuonEFTrackBuilderMultiHypo; P_{T} (GeV)",
                                            xbins=200, xmin=-100, xmax=100) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="Eta reconstruction from #TrigMuonEFTrackBuilderMultiHypo; Eta",
                                            xbins=100, xmin=-3.2, xmax=3.2) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="Phi reconstruction from #TrigMuonEFTrackBuilderMultiHypo; Phi",
                                             xbins=100, xmin=-3.15, xmax=3.15) ]
        self.Histograms += [ defineHistogram('Nmuons', type='TH1F', title="nr of muons from #TrigMuonEFCBSeededFSDiMuonHypo; N muons",
                                             xbins=10, xmin=0, xmax=10) ]

class TrigMuonEFTrackBuilderMultiHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigMuonEFTrackBuilderMultiHypoOnlineMonitoring"):
        super(TrigMuonEFTrackBuilderMultiHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms  = [ defineHistogram('Pt', type='TH1F', title="P_{T} reconstruction from #TrigMuonEFTrackBuilderMultiHypo; P_{T} (GeV)",
                                            xbins=200, xmin=-100, xmax=100) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="Eta reconstruction from #TrigMuonEFTrackBuilderMultiHypo; Eta",
                                            xbins=100, xmin=-3.2, xmax=3.2) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="Phi reconstruction from #TrigMuonEFTrackBuilderMultiHypo; Phi",
                                             xbins=100, xmin=-3.15, xmax=3.15) ]
        self.Histograms += [ defineHistogram('Nmuons', type='TH1F', title="nr of muons from #TrigMuonEFCBSeededFSDiMuonHypo; N muons",
                                             xbins=10, xmin=0, xmax=10) ]

# ------------ Added HL
class TileMuHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TileMuHypoValidationMonitoring"):
        super(TileMuHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")

        self.Histograms = [ defineHistogram('Eta', type='TH1F', title="Eta(TileMuFeature)", xbins=34, xmin=-1.7, xmax=1.7) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="Phi(TileMuFeature)", xbins=64, xmin=-3.14, xmax=3.14) ]
        self.Histograms += [ defineHistogram('EffwTrk', type='TH1F', title="Efficiency of Track-matching", xbins=2, xmin=-0.5, xmax=1.5) ]
        self.Histograms += [ defineHistogram('EtaTrk', type='TH1F', title="Eta(TileMuFeature) with Track", xbins=34, xmin=-1.7, xmax=1.7) ]
        self.Histograms += [ defineHistogram('PhiTrk', type='TH1F', title="Phi(TileMuFeature) with Track", xbins=64, xmin=-3.14, xmax=3.14) ]
        self.Histograms += [ defineHistogram('absPtTrk', type='TH1F', title="|P_{T}| from the combined track with TileMu; P_{T} (GeV)", xbins=200, xmin=0., xmax=20.)]          
        self.Histograms += [ defineHistogram('DelEtaTrk', type='TH1F', title="Eta(Tile)-Eta(Track)", xbins=200, xmin=-0.3, xmax=0.3) ]        
        self.Histograms += [ defineHistogram('DelEtaTrkTR', type='TH1F', title="Eta(Tile)-EtaTR(Track)", xbins=200, xmin=-0.3, xmax=0.3) ]        
        self.Histograms += [ defineHistogram('DelPhiTrk', type='TH1F', title="Phi(Tile)-Phi(Track)", xbins=200, xmin=-0.3, xmax=0.3) ]
        self.Histograms += [ defineHistogram('DelPhiTrkTR', type='TH1F', title="Phi(Tile)-PhiTR(Track)", xbins=200, xmin=-0.3, xmax=0.3) ]        
        self.Histograms += [ defineHistogram('absPtTrkPre, DelPhiTrk', type='TH2F', title="Phi(Tile)-Phi(Track) vs. P_{T} (GeV)", xbins=200, xmin=0., xmax=20., ybins=200, ymin=-0.3, ymax=0.3) ]        
        self.Histograms += [ defineHistogram('absPtTrkPre, DelPhiTrkTR', type='TH2F', title="Phi(Tile)-PhiTR(Track) vs. P_{T} (GeV)", xbins=200, xmin=0., xmax=20., ybins=200, ymin=-0.3, ymax=0.3) ]
        self.Histograms += [ defineHistogram('absPtTrkPre', type='TH1F', title="|P_{T}| from the combined track with TileMu before P_{T}-Cut; P_{T} (GeV)", xbins=200, xmin=0., xmax=20.)]        

class TileMuHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TileMuHypoOnlineMonitoring"):
        super(TileMuHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms = [ defineHistogram('Eta', type='TH1F', title="Eta(TileMuFeature)", xbins=34, xmin=-1.7, xmax=1.7) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="Phi(TiluMuFeature)", xbins=64, xmin=-3.14, xmax=3.14) ] 
        self.Histograms += [ defineHistogram('EffwTrk', type='TH1F', title="Efficiency of Track-matching", xbins=2, xmin=-0.5, xmax=1.5)]        
        self.Histograms += [ defineHistogram('EtaTrk', type='TH1F', title="Eta(TileMuFeature) with Track", xbins=34, xmin=-1.7, xmax=1.7) ]        
        self.Histograms += [ defineHistogram('PhiTrk', type='TH1F', title="Phi(TileMuFeature) with Track", xbins=64, xmin=-3.14, xmax=3.14) ]
        self.Histograms += [ defineHistogram('absPtTrk', type='TH1F', title="|P_{T}| from the combined track with TileMu; P_{T} (GeV)", xbins=200, xmin=0., xmax
=20.) ]        
        self.Histograms += [ defineHistogram('DelEtaTrk', type='TH1F', title="Eta(Tile)-Eta(Track)", xbins=200, xmin=-0.3, xmax=0.3) ]
        self.Histograms += [ defineHistogram('DelEtaTrkTR', type='TH1F', title="Eta(Tile)-EtaTR(Track)", xbins=200, xmin=-0.3, xmax=0.3) ]        
        self.Histograms += [ defineHistogram('DelPhiTrk', type='TH1F', title="Phi(Tile)-Phi(Track)", xbins=200, xmin=-0.3, xmax=0.3) ]
        self.Histograms += [ defineHistogram('DelPhiTrkTR', type='TH1F', title="Phi(Tile)-PhiTR(Track)", xbins=200, xmin=-0.3, xmax=0.3) ]
        self.Histograms += [ defineHistogram('absPtTrkPre, DelPhiTrk', type='TH2F', title="Phi(Tile)-Phi(Track) vs. P_{T} (GeV)", xbins=200, xmin=0., xmax=20., ybins=200, ymin=-0.3, ymax=0.3) ]
        self.Histograms += [ defineHistogram('absPtTrkPre, DelPhiTrkTR', type='TH2F', title="Phi(Tile)-PhiTR(Track) vs. P_{T} (GeV)", xbins=200, xmin=0., xmax=20., ybins=200, ymin=-0.3, ymax=0.3) ]
        self.Histograms += [ defineHistogram('absPtTrkPre', type='TH1F', title="|P_{T}| from the combined track with TileMu before P_{T}-Cut; P_{T} (GeV)", xbins=200, xmin=0., xmax=20.)]

class TileMuHypoCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TileMuHypoCosmicMonitoring"):
        super(TileMuHypoCosmicMonitoring, self).__init__(name)
        self.defineTarget("Cosmic")

        self.Histograms = [ defineHistogram('Eta', type='TH1F', title="Eta(TileMuFeature)", xbins=34, xmin=-1.7, xmax=1.7) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="Phi(TiluMuFeature)", xbins=64, xmin=-3.14, xmax=3.14) ] 
        self.Histograms += [ defineHistogram('EffwTrk', type='TH1F', title="Efficiency of Track-matching", xbins=2, xmin=-0.5, xmax=1.5)]        
        self.Histograms += [ defineHistogram('EtaTrk', type='TH1F', title="Eta(TileMuFeature) with Track", xbins=34, xmin=-1.7, xmax=1.7) ]        
        self.Histograms += [ defineHistogram('PhiTrk', type='TH1F', title="Phi(TileMuFeature) with Track", xbins=64, xmin=-3.14, xmax=3.14) ]
        self.Histograms += [ defineHistogram('absPtTrk', type='TH1F', title="|P_{T}| from the combined track with TileMu; P_{T} (GeV)", xbins=200, xmin=0., xmax
=20.) ]        
        self.Histograms += [ defineHistogram('DelEtaTrk', type='TH1F', title="Eta(Tile)-Eta(Track)", xbins=200, xmin=-0.3, xmax=0.3) ]
        self.Histograms += [ defineHistogram('DelEtaTrkTR', type='TH1F', title="Eta(Tile)-EtaTR(Track)", xbins=200, xmin=-0.3, xmax=0.3) ]        
        self.Histograms += [ defineHistogram('DelPhiTrk', type='TH1F', title="Phi(Tile)-Phi(Track)", xbins=200, xmin=-0.3, xmax=0.3) ]
        self.Histograms += [ defineHistogram('DelPhiTrkTR', type='TH1F', title="Phi(Tile)-PhiTR(Track)", xbins=200, xmin=-0.3, xmax=0.3) ]
        self.Histograms += [ defineHistogram('absPtTrkPre, DelPhiTrk', type='TH2F', title="Phi(Tile)-Phi(Track) vs. P_{T} (GeV)", xbins=200, xmin=0., xmax=20., ybins=200, ymin=-0.3, ymax=0.3) ]
        self.Histograms += [ defineHistogram('absPtTrkPre, DelPhiTrkTR', type='TH2F', title="Phi(Tile)-PhiTR(Track) vs. P_{T} (GeV)", xbins=200, xmin=0., xmax=20., ybins=200, ymin=-0.3, ymax=0.3) ]
        self.Histograms += [ defineHistogram('absPtTrkPre', type='TH1F', title="|P_{T}| from the combined track with TileMu before P_{T}-Cut; P_{T} (GeV)", xbins=200, xmin=0., xmax=20.)]


class TrigMuTagIMOHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigMuTagIMOHypoValidationMonitoring"):
        super(TrigMuTagIMOHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")

        self.Histograms  = [ defineHistogram('Pt', type='TH1F', title="P_{T} reconstruction from #TrigMuTagIMO; P_{T} (MeV)",
                                             xbins=200, xmin=-100, xmax=100) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="Eta reconstruction from #TrigMuTagIMO; Eta",
                                             xbins=100, xmin=-3.2, xmax=3.2) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="Phi reconstruction from #TrigMuTagIMO; Phi",
                                             xbins=100, xmin=-3.15, xmax=3.15) ]



class TrigMuTagIMOHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigMuTagIMOHypoOnlineMonitoring"):
        super(TrigMuTagIMOHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms  = [ defineHistogram('Pt', type='TH1F', title="P_{T} reconstruction from #TrigMuTagIMO; P_{T} (MeV)",
                                            xbins=200, xmin=-100, xmax=100) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="Eta reconstruction from #TrigMuTagIMO; Eta",
                                             xbins=100, xmin=-3.2, xmax=3.2) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="Phi reconstruction from #TrigMuTagIMO; Phi",
                                             xbins=100, xmin=-3.15, xmax=3.15) ]



class TrigMuTagIMOHypoCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigMuTagIMOHypoCosmicMonitoring"):
        super(TrigMuTagIMOHypoCosmicMonitoring, self).__init__(name)
        self.defineTarget("Cosmic")

        self.Histograms  = [ defineHistogram('Pt', type='TH1F', title="P_{T} reconstruction from #TrigMuTagIMO; P_{T} (MeV)",
                                             xbins=200, xmin=-100, xmax=100) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="Eta reconstruction from #TrigMuTagIMO; Eta",
                                             xbins=100, xmin=-3.2, xmax=3.2) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="Phi reconstruction from #TrigMuTagIMO; Phi",
                                             xbins=100, xmin=-3.15, xmax=3.15) ]
        
class TrigMuonEFTrackIsolationHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigMuonEFTrackIsolationHypoOnlineMonitoring"):
        super(TrigMuonEFTrackIsolationHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms  = [ defineHistogram('PtCone02', type='TH1F', title="Sum pT in 0.2 cone #TrigMuonEFTrackIsolationHypo; Sum p_{T} [GeV]",
                                            xbins=50, xmin=0.0, xmax=25.0) ]
        self.Histograms  += [ defineHistogram('PtCone03', type='TH1F', title="Sum pT in 0.3 cone #TrigMuonEFTrackIsolationHypo; Sum p_{T} [GeV]",
                                            xbins=50, xmin=0.0, xmax=25.0) ]
        pass


class TrigMuonEFTrackIsolationHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigMuonEFTrackIsolationHypoValidationMonitoring"):
        super(TrigMuonEFTrackIsolationHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")

        self.Histograms  = [ defineHistogram('PtCone02', type='TH1F', title="Sum pT in 0.2 cone #TrigMuonEFTrackIsolationHypo; P_{T} (GeV)",
                                            xbins=50, xmin=0.0, xmax=25.0) ]
        self.Histograms  += [ defineHistogram('PtCone03', type='TH1F', title="Sum pT in 0.3 cone #TrigMuonEFTrackIsolationHypo; P_{T} (GeV)",
                                            xbins=50, xmin=0.0, xmax=25.0) ]

class TrigMuonEFCombinerDiMuonMassHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigMuonEFCombinerDiMuonMassHypoOnlineMonitoring"):
        super(TrigMuonEFCombinerDiMuonMassHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms  = [ defineHistogram('nMuons', type='TH1F', title="number of Muons; nr of muons",
                                             xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('chargeMult', type='TH1F', title="charges multiplied between di-muons muon charge1*charge2",
                                             xbins=3, xmin=-1, xmax=2) ]
        self.Histograms += [ defineHistogram('invMassWide',   type='TH1F', title="invariant mass of di-muon pairs; inv mass [GeV]",
                                             xbins=220, xmin=0, xmax=110.0) ]
        self.Histograms += [ defineHistogram('invMassNarrow', type='TH1F', title="invariant mass of di-muon pairs; inv mass [GeV]",
                                             xbins=128, xmin=-1, xmax=15.0) ]
        self.Histograms += [ defineHistogram('muonPt', type='TH1F', title="pT of muons; muon pT [GeV]",
                                             xbins=100, xmin=0, xmax=100.0) ]
        self.Histograms += [ defineHistogram('muonEta', type='TH1F', title="#eta of muons; muon eta",
                                             xbins=100, xmin=-3.2, xmax=3.2) ]
        pass

#vvvvv pvn
class TrigMuonEFCombinerDiMuonMassPtImpactsHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigMuonEFCombinerDiMuonMassPtImpactsHypoOnlineMonitoring"):
        super(TrigMuonEFCombinerDiMuonMassPtImpactsHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms  = [ defineHistogram('nMuons', type='TH1F', title="number of Muons; nr of muons",
                                             xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('muonDeltaZ', type='TH1F', title="delta(z0) of muons; muons delta(z0)",
                                             xbins=100, xmin=0., xmax=50.) ]
        self.Histograms += [ defineHistogram('muonDeltaPhi', type='TH1F', title="delta(phi) of muons; muons delta(phi)",
                                             xbins=100, xmin=0., xmax=4.) ]
        self.Histograms += [ defineHistogram('dimuonPt', type='TH1F', title="pT of di-muons; di-muon pT [GeV]",
                                             xbins=200, xmin=0., xmax=20.0) ]
        self.Histograms += [ defineHistogram('invMassWide',   type='TH1F', title="invariant mass of di-muon pairs; inv mass [GeV]",
                                             xbins=200, xmin=0., xmax=100.0) ]
        pass
#^^^^^ pvn

#vvvvv masato
class TrigMuonIDTrackMultiHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigMuonIDTrackMultiHypoOnlineMonitoring"):
        super(TrigMuonIDTrackMultiHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Validation")

        self.Histograms  = [ defineHistogram('nTracks', type='TH1F', title="number of tracks; nr of tracks",
                                             xbins=20, xmin=0, xmax=20) ]
        self.Histograms  = [ defineHistogram('TrkPt', type='TH1F', title="track pt; track pt [GeV]",
                                             xbins=50, xmin=0, xmax=50) ]
        self.Histograms  = [ defineHistogram('MuRoiDr', type='TH1F', title="roi delta-R; RoI delta-R",
                                             xbins=50, xmin=0, xmax=5) ]
        # self.Histograms += [ defineHistogram('invMass', type='TH1F', title="invariant mass of tracks; inv mass [GeV]",
        #                                      xbins=100, xmin=0., xmax=50.) ]
        pass
#^^^^^ masato

class TrigMuonEFCombinerDiMuonMassHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigMuonEFCombinerDiMuonMassHypoValidationMonitoring"):
        super(TrigMuonEFCombinerDiMuonMassHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")

        self.Histograms  = [ defineHistogram('nMuons', type='TH1F', title="number of Muons; nr of muons",
                                             xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('chargeMult', type='TH1F', title="charges multiplied between di-muons muon charge1*charge2",
                                             xbins=3, xmin=-1, xmax=2) ]
        self.Histograms += [ defineHistogram('invMassWide',   type='TH1F', title="invariant mass of di-muon pairs; inv mass [GeV]",
                                             xbins=220, xmin=0, xmax=110.0) ]
        self.Histograms += [ defineHistogram('invMassNarrow', type='TH1F', title="invariant mass of di-muon pairs; inv mass [GeV]",
                                             xbins=128, xmin=-1, xmax=15.0) ]
        self.Histograms += [ defineHistogram('muonPt', type='TH1F', title="pT of muons; muon pT [GeV]",
                                             xbins=100, xmin=0, xmax=100.0) ]
        self.Histograms += [ defineHistogram('muonEta', type='TH1F', title="#eta of muons; muon eta",
                                             xbins=100, xmin=-3.2, xmax=3.2) ]
        pass

class MufastNSWHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="MufastNSWHypoValidationMonitoring"):
        super(MufastNSWHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")

        self.Histograms  = [ defineHistogram('Pt', type='TH1F', title="P_{T} reconstruction from #muFast; P_{T} (GeV)",
                                             xbins=200, xmin=-100, xmax=100) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="Eta reconstruction from #muFast; Eta",
                                             xbins=100, xmin=-3.2, xmax=3.2) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="Phi reconstruction from #muFast; Phi",
                                             xbins=100, xmin=-3.15, xmax=3.15) ]
        self.Histograms += [ defineHistogram('SegTgcInn', type='TH1F', title="Segment reconstruction from #muFast; SegTgcInn",
                                             xbins=2, xmin=0, xmax=2) ]
        self.Histograms += [ defineHistogram('SegInn', type='TH1F', title="Segment reconstruction from #muFast; SegInn",
                                             xbins=2, xmin=0, xmax=2) ]
        self.Histograms += [ defineHistogram('SegMid', type='TH1F', title="Segment reconstruction from #muFast; SegMid",
                                             xbins=2, xmin=0, xmax=2) ]
        self.Histograms += [ defineHistogram('SegOut', type='TH1F', title="Segment reconstruction from #muFast; SegOut",
                                             xbins=2, xmin=0, xmax=2) ]
        self.Histograms += [ defineHistogram('dTheta', type='TH1F', title="delta Theta from #muFast; dTheta",
                                             xbins=128, xmin=-0.04, xmax=0.04) ]
        self.Histograms += [ defineHistogram('dLEta', type='TH1F', title="dL eta from #muFast; dL Eta",
                                             xbins=120, xmin=0, xmax=0.30) ]
        self.Histograms += [ defineHistogram('dLPhi', type='TH1F', title="dL phi from #muFast; dL Phi",
                                             xbins=120, xmin=0, xmax=0.30) ]

class MufastNSWHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="MufastNSWHypoOnlineMonitoring"):
        super(MufastNSWHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms  = [ defineHistogram('Pt', type='TH1F', title="P_{T} reconstruction from #muFast; P_{T} (GeV)",
                                             xbins=200, xmin=-100, xmax=100) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="Eta reconstruction from #muFast; Eta",
                                             xbins=100, xmin=-3.2, xmax=3.2) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="Phi reconstruction from #muFast; Phi",
                                             xbins=100, xmin=-3.15, xmax=3.15) ]
        self.Histograms += [ defineHistogram('SegInn', type='TH1F', title="Segment reconstruction from #muFast; SegInn",
                                             xbins=2, xmin=0, xmax=2) ]
        self.Histograms += [ defineHistogram('SegMid', type='TH1F', title="Segment reconstruction from #muFast; SegMid",
                                             xbins=2, xmin=0, xmax=2) ]
        self.Histograms += [ defineHistogram('SegOut', type='TH1F', title="Segment reconstruction from #muFast; SegOut",
                                             xbins=2, xmin=0, xmax=2) ]
        self.Histograms += [ defineHistogram('dTheta', type='TH1F', title="delta Theta from #muFast; dTheta",
                                             xbins=128, xmin=-0.04, xmax=0.04) ]
        self.Histograms += [ defineHistogram('dLEta', type='TH1F', title="dL eta from #muFast; dL Eta",
                                             xbins=120, xmin=0, xmax=0.30) ]
        self.Histograms += [ defineHistogram('dLPhi', type='TH1F', title="dL phi from #muFast; dL Phi",
                                             xbins=120, xmin=0, xmax=0.30) ]

class TrigMuonEFExtrapolatorNSWHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigMuonEFExtrapolatorNSWHypoValidationMonitoring"):
        super(TrigMuonEFExtrapolatorNSWHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")

        self.Histograms  = [ defineHistogram('n_seg_passed', type='TH1F', title="nr of segments passed NSW cuts; nr of segments",
                                             xbins=5, xmin=0, xmax=5) ]
        self.Histograms += [ defineHistogram('n_seg_sameside', type='TH1F', title="nr of segments in same side NSW; nr of segments",
                                             xbins=5, xmin=0, xmax=5) ]
        self.Histograms += [ defineHistogram('dTheta', type='TH1F', title="dTheta by using TrigMuonEF; dTheta",
                                             xbins=128, xmin=-0.04, xmax=0.04) ]
        self.Histograms += [ defineHistogram('dLEta', type='TH1F', title="dL eta by using TrigMuonEF; dL Eta",
                                             xbins=120, xmin=0, xmax=0.30) ]
        self.Histograms += [ defineHistogram('dLPhi', type='TH1F', title="dL phi by using TrigMuonEF; dL Phi",
                                             xbins=120, xmin=0, xmax=0.30) ]
        self.Histograms += [ defineHistogram('failed_eta , failed_phi', type='TH2F', title="#eta vs #phi of NSW cut failed; #eta; #phi",
                                             xbins=50, xmin=-3.2, xmax=3.2, ybins=25, ymin=-3.15, ymax=3.15) ]
        self.Histograms += [ defineHistogram('failed_eta', type='TH1F', title="#eta of NSW cut failed; #eta",
                                             xbins=100, xmin=-3.2, xmax=3.2) ]
        self.Histograms += [ defineHistogram('failed_phi', type='TH1F', title="#phi of NSW cut failed; #phi",
                                             xbins=100, xmin=-3.15, xmax=3.15) ]

class TrigMuonEFExtrapolatorNSWHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigMuonEFExtrapolatorNSWHypoOnlineMonitoring"):
        super(TrigMuonEFExtrapolatorNSWHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms  = [ defineHistogram('n_seg_passed', type='TH1F', title="nr of segments passed NSW cuts; nr of segments",
                                             xbins=5, xmin=0, xmax=5) ]
        self.Histograms += [ defineHistogram('n_seg_sameside', type='TH1F', title="nr of segments in same side NSW; nr of segments",
                                             xbins=5, xmin=0, xmax=5) ]
        self.Histograms += [ defineHistogram('dTheta', type='TH1F', title="dTheta by using TrigMuonEF; dTheta",
                                             xbins=128, xmin=-0.04, xmax=0.04) ]
        self.Histograms += [ defineHistogram('dLEta', type='TH1F', title="dL eta by using TrigMuonEF; dL Eta",
                                             xbins=120, xmin=0, xmax=0.30) ]
        self.Histograms += [ defineHistogram('dLPhi', type='TH1F', title="dL phi by using TrigMuonEF; dL Phi",
                                             xbins=120, xmin=0, xmax=0.30) ]
        self.Histograms += [ defineHistogram('failed_eta , failed_phi', type='TH2F', title="#eta vs #phi of NSW cut failed; #eta; #phi",
                                             xbins=50, xmin=-3.2, xmax=3.2, ybins=25, ymin=-3.15, ymax=3.15) ]
        self.Histograms += [ defineHistogram('failed_eta', type='TH1F', title="#eta of NSW cut failed; #eta",
                                             xbins=100, xmin=-3.2, xmax=3.2) ]
        self.Histograms += [ defineHistogram('failed_phi', type='TH1F', title="#phi of NSW cut failed; #phi",
                                             xbins=100, xmin=-3.15, xmax=3.15) ]
