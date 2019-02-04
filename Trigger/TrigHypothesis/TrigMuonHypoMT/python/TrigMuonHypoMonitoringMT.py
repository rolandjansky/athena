# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# PURPOSE: AthenaMT Migration
#

from AthenaMonitoring.GenericMonitoringTool import GenericMonitoringTool, defineHistogram

class TrigMufastHypoMonitoring(GenericMonitoringTool):
    def __init__ (self, name):
        super(TrigMufastHypoMonitoring, self).__init__(name)

        self.HistPath = name  
        self.Histograms = [ defineHistogram('Pt', type='TH1F', path='EXPERT', title="P_{T} reconstruction from #muFast; P_{T} (GeV)", xbins=200, xmin=-100, xmax=100),
                            defineHistogram('PtFL', type='TH1F', path='EXPERT', title="P_{T} of not selected muons from #muFast; p_{T} (GeV)", xbins=200, xmin=-100, xmax=100),
                            defineHistogram('Eta , Phi', type='TH2F', path='EXPERT', title="Eta vs Phi reconstruction of #muFast; Eta; Phi",xbins=50, xmin=-3.2, xmax=3.2, ybins=25, ymin=-3.15, ymax=3.15),
                            defineHistogram('Eta', type='TH1F', path='EXPERT', title="Eta reconstruction from #muFast; Eta",xbins=100, xmin=-3.2, xmax=3.2),
                            defineHistogram('Phi', type='TH1F', path='EXPERT', title="Phi reconstruction from #muFast; Phi",xbins=100, xmin=-3.15, xmax=3.15),
                            defineHistogram('ZatSt, Phi', type='TH2F', path='EXPERT', title="Z vs Phi reconstructed in MIDDLE station; Z (cm); Phi (rad)",xbins=50, xmin=-1200., xmax=1200., ybins=25, ymin=-3.2, ymax=3.2),
                            defineHistogram('XatSt , YatSt', type='TH2F', path='EXPERT', title="Y vs X reconstructed in MIDDLE station; X (cm); Y(cm)",xbins=50, xmin=-1200., xmax=1200., ybins=50, ymin=-1200., ymax=1200.),
                            defineHistogram('ZatBe', type='TH1F', path='EXPERT', title="DCA along Z; Z (cm)",xbins=100, xmin=-2100, xmax=2100),
                            defineHistogram('XatBe', type='TH1F', path='EXPERT', title="DCA along X; X (cm)",xbins=100, xmin=-1000, xmax=1000) ]


class TrigmuCombHypoMonitoring(GenericMonitoringTool):
    def __init__ (self, name):
        super(TrigmuCombHypoMonitoring, self).__init__(name)

        self.HistPath = name  
        self.Histograms = [ defineHistogram('Pt', type='TH1F', path='EXPERT', title="p_{T} reconstruction from #muComb; p_{T} (GeV)",
                                            xbins=210, xmin=-105, xmax=105) ]
        self.Histograms += [ defineHistogram('PtFL', type='TH1F', path='EXPERT', title="p_{T} of not selected muons from #muComb; p_{T} (GeV)",
                                            xbins=210, xmin=-105., xmax=105.) ]
        self.Histograms += [ defineHistogram('StrategyFlag', type='TH1F', path='EXPERT', title="Combination Strategy from #muComb; Strategy Code",
                                            xbins=12, xmin=-1.5, xmax=10.5) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', path='EXPERT', title="Eta reconstruction from #muComb; Eta",
                                             xbins=108, xmin=-2.7, xmax=2.7) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', path='EXPERT', title="Phi reconstruction from #muComb; Phi (rad)",
                                             xbins=96, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('Z0', type='TH1F', path='EXPERT', title="PCA along Z from ID track from #muComb; PCA(Z0) (mm)",
                                             xbins=100, xmin=-200, xmax=200) ]
        self.Histograms += [ defineHistogram('A0', type='TH1F', path='EXPERT', title="PCA along x-y from ID track from #muComb; PCA(A0) (mm)",
                                             xbins=100, xmin=-0.6, xmax=0.6) ]

class TrigMuisoHypoMonitoring(GenericMonitoringTool):

    def __init__ (self, name):
        super(TrigMuisoHypoMonitoring, self).__init__(name)

        self.HistPath = name  
        self.Histograms  = [ defineHistogram('CutCounter', type='TH1F', path='EXPERT', title="MuIsoHypo cut counter;cut; nevents", xbins=9, xmin=-1.5, xmax=7.5, opt="kCumulative") ]
        self.Histograms += [ defineHistogram('SumPtCone', type='TH1F', path='EXPERT', title="MuIsoHypo SumPt in cone around muon;E [GeV/c]; nevents", xbins=200, xmin=0., xmax=15.) ]


class TrigMuonEFMSonlyHypoMonitoring(GenericMonitoringTool):

    def __init__ (self, name="TrigMuonEFMSonlyHypoMonitoring"):
        super(TrigMuonEFMSonlyHypoMonitoring, self).__init__(name)
        self.HistPath = name  

        self.Histograms = [ defineHistogram('Pt', type='TH1F', path='EXPERT', title="P_{T} reconstruction from #TrigMuonEFMSonlyHypo; P_{T} (MeV)",
                                            xbins=200, xmin=-100, xmax=100) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', path='EXPERT', title="Eta reconstruction from #TrigMuonEFMSonlyHypo; Eta",
                                            xbins=100, xmin=-3.2, xmax=3.2) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', path='EXPERT', title="Phi reconstruction from #TrigMuonEFMSonlyHypo; Phi",
                                    xbins=100, xmin=-3.15, xmax=3.15) ]
        self.Histograms += [ defineHistogram('Pt_sel', type='TH1F', path='EXPERT', title="Selected P_{T} reconstruction from #TrigMuonEFMSonlyHypo; P_{T} (MeV)",
                                            xbins=200, xmin=-100, xmax=100) ]
        self.Histograms += [ defineHistogram('Eta_sel', type='TH1F', path='EXPERT', title="Selected Eta reconstruction from #TrigMuonEFMSonlyHypo; Eta",
                                            xbins=100, xmin=-3.2, xmax=3.2) ]
        self.Histograms += [ defineHistogram('Phi_sel', type='TH1F', path='EXPERT', title="Selected Phi reconstruction from #TrigMuonEFMSonlyHypo; Phi",
                                    xbins=100, xmin=-3.15, xmax=3.15) ]

class TrigMuonEFCombinerHypoMonitoring(GenericMonitoringTool):

    def __init__ (self, name="TrigMuonEFCombinerHypoMonitoring"):
        super(TrigMuonEFCombinerHypoMonitoring, self).__init__(name)
        self.HistPath = name  

        self.Histograms = [ defineHistogram('Pt', type='TH1F', path='EXPERT', title="P_{T} reconstruction from #TrigMuonEFCombinerHypo; P_{T} (MeV)",
                                            xbins=200, xmin=-100, xmax=100) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', path='EXPERT', title="Eta reconstruction from #TrigMuonEFCombinerHypo; Eta",
                                            xbins=100, xmin=-3.2, xmax=3.2) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', path='EXPERT', title="Phi reconstruction from #TrigMuonEFCombinerHypo; Phi",
                                    xbins=100, xmin=-3.15, xmax=3.15) ]
        self.Histograms += [ defineHistogram('Pt_sel', type='TH1F', path='EXPERT', title="Selected P_{T} reconstruction from #TrigMuonEFCombinerHypo; P_{T} (MeV)",
                                            xbins=200, xmin=-100, xmax=100) ]
        self.Histograms += [ defineHistogram('Eta_sel', type='TH1F', path='EXPERT', title="Selected Eta reconstruction from #TrigMuonEFCombinerHypo; Eta",
                                            xbins=100, xmin=-3.2, xmax=3.2) ]
        self.Histograms += [ defineHistogram('Phi_sel', type='TH1F', path='EXPERT', title="Selected Phi reconstruction from #TrigMuonEFCombinerHypo; Phi",
                                    xbins=100, xmin=-3.15, xmax=3.15) ]
