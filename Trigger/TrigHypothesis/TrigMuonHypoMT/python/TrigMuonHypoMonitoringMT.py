# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

class TrigMufastHypoMonitoring(GenericMonitoringTool):
    def __init__ (self, name):
        super(TrigMufastHypoMonitoring, self).__init__(name)

        self.defineHistogram('Pt', type='TH1F', path='EXPERT', title="P_{T} reconstruction from #muFast; P_{T} (GeV)",
                             xbins=200, xmin=-100, xmax=100)
        self.defineHistogram('PtFL', type='TH1F', path='EXPERT', title="P_{T} of not selected muons from #muFast; p_{T} (GeV)",
                             xbins=200, xmin=-100, xmax=100)
        self.defineHistogram('Eta , Phi', type='TH2F', path='EXPERT', title="Eta vs Phi reconstruction of #muFast; Eta; Phi",
                             xbins=50, xmin=-3.2, xmax=3.2, ybins=25, ymin=-3.15, ymax=3.15)
        self.defineHistogram('Eta', type='TH1F', path='EXPERT', title="Eta reconstruction from #muFast; Eta",
                             xbins=100, xmin=-3.2, xmax=3.2)
        self.defineHistogram('Phi', type='TH1F', path='EXPERT', title="Phi reconstruction from #muFast; Phi",
                             xbins=100, xmin=-3.15, xmax=3.15)
        self.defineHistogram('ZatSt, Phi', type='TH2F', path='EXPERT', title="Z vs Phi reconstructed in MIDDLE station; Z (cm); Phi (rad)",
                             xbins=50, xmin=-1200., xmax=1200., ybins=25, ymin=-3.2, ymax=3.2)
        self.defineHistogram('XatSt , YatSt', type='TH2F', path='EXPERT', title="Y vs X reconstructed in MIDDLE station; X (cm); Y(cm)",
                             xbins=50, xmin=-1200., xmax=1200., ybins=50, ymin=-1200., ymax=1200.)
        self.defineHistogram('ZatBe', type='TH1F', path='EXPERT', title="DCA along Z; Z (cm)",
                             xbins=100, xmin=-2100, xmax=2100)
        self.defineHistogram('XatBe', type='TH1F', path='EXPERT', title="DCA along X; X (cm)",
                             xbins=100, xmin=-1000, xmax=1000)

class TrigL2MuonOverlapRemoverMonitoringMufast(TrigMufastHypoMonitoring):
    def __init__ (self, name):
        super(TrigL2MuonOverlapRemoverMonitoringMufast, self).__init__(name)

        self.defineHistogram('MufastError',  type='TH1F', path='EXPERT', title="error in #muFast based overlap removal; error code",
                             xbins=10, xmin=0, xmax=10)
        self.defineHistogram('DR',  type='TH1F', path='EXPERT', title="#muFast objects dR; dR",
                             xbins=108, xmin=-0.1, xmax=3.5)
        self.defineHistogram('Mass',type='TH1F', path='EXPERT', title="#muFast objects invMass; Mass (GeV)",
                             xbins=202, xmin=-1, xmax=100)
        self.defineHistogram('DRLog10',  type='TH1F', path='EXPERT', title="#muFast objects Log10(dR); Log10(dR)",
                             xbins=102, xmin=-4.1, xmax=1.0)
        self.defineHistogram('MassLog10',type='TH1F', path='EXPERT', title="#muFast objects Log10(invMass); Log10(Mass (GeV))",
                             xbins=142, xmin=-4.1, xmax=3.0)
        self.defineHistogram('Mass , DR', type='TH2F', path='EXPERT', title="#muFast objects Mass vs DR; Mass; dR",
                             xbins=54, xmin=-0.1, xmax=3.5, ybins=101, ymin=-1, ymax=100)
        self.defineHistogram('NrAllEVs',type='TH1F', path='EXPERT', title="nr of all EVs received for #muFast removal; nr",
                             xbins=10, xmin=0, xmax=10)
        self.defineHistogram('NrActiveEVs',type='TH1F', path='EXPERT', title="nr of active EVs after #muFast removal ; nr",
                             xbins=10, xmin=0, xmax=10)
        self.defineHistogram('NrOverlapped',type='TH1F', path='EXPERT', title="nr of #muFast overlapped; nr",
                             xbins=10, xmin=0, xmax=10)
        self.defineHistogram('OverlappedEta , OverlappedPhi', type='TH2F', path='EXPERT', title="#muFast overlapped Eta vs Phi; Eta; Phi",
                             xbins=50, xmin=-3.2, xmax=3.2, ybins=25, ymin=-3.15, ymax=3.15)
        self.defineHistogram('OverlappedPt', type='TH1F', path='EXPERT', title="#muFast overlapped P_{T}; P_{T} (GeV)",
                             xbins=200, xmin=-100, xmax=100)

class TrigmuCombHypoMonitoring(GenericMonitoringTool):
    def __init__ (self, name):
        super(TrigmuCombHypoMonitoring, self).__init__(name)

        self.defineHistogram('Pt', type='TH1F', path='EXPERT', title="p_{T} reconstruction from #muComb; p_{T} (GeV)",
                             xbins=210, xmin=-105, xmax=105)
        self.defineHistogram('PtFL', type='TH1F', path='EXPERT', title="p_{T} of not selected muons from #muComb; p_{T} (GeV)",
                             xbins=210, xmin=-105., xmax=105.)
        self.defineHistogram('StrategyFlag', type='TH1F', path='EXPERT', title="Combination Strategy from #muComb; Strategy Code",
                             xbins=12, xmin=-1.5, xmax=10.5)
        self.defineHistogram('Eta', type='TH1F', path='EXPERT', title="Eta reconstruction from #muComb; Eta",
                             xbins=108, xmin=-2.7, xmax=2.7)
        self.defineHistogram('Phi', type='TH1F', path='EXPERT', title="Phi reconstruction from #muComb; Phi (rad)",
                             xbins=96, xmin=-3.1416, xmax=3.1416)
        self.defineHistogram('Z0', type='TH1F', path='EXPERT', title="PCA along Z from ID track from #muComb; PCA(Z0) (mm)",
                             xbins=100, xmin=-200, xmax=200)
        self.defineHistogram('A0', type='TH1F', path='EXPERT', title="PCA along x-y from ID track from #muComb; PCA(A0) (mm)",
                             xbins=100, xmin=-0.6, xmax=0.6)

class TrigL2MuonOverlapRemoverMonitoringMucomb(TrigmuCombHypoMonitoring):
    def __init__ (self, name):
        super(TrigL2MuonOverlapRemoverMonitoringMucomb, self).__init__(name)

        self.defineHistogram('MucombError',  type='TH1F', path='EXPERT', title="error in #muComb based overlap removal; error code",
                             xbins=10, xmin=0, xmax=10)
        self.defineHistogram('DR',  type='TH1F', path='EXPERT', title="#muComb objects dR; dR",
                             xbins=108, xmin=-0.1, xmax=3.5)
        self.defineHistogram('Mass',type='TH1F', path='EXPERT', title="#muComb objects invMass; Mass (GeV)",
                             xbins=202, xmin=-1, xmax=100)
        self.defineHistogram('DRLog10',  type='TH1F', path='EXPERT', title="#muFast objects Log10(dR); Log10(dR)",
                             xbins=102, xmin=-4.1, xmax=1.0)
        self.defineHistogram('MassLog10',type='TH1F', path='EXPERT', title="#muFast objects Log10(invMass); Log10(Mass (GeV))",
                             xbins=142, xmin=-4.1, xmax=3.0)
        self.defineHistogram('Mass , DR', type='TH2F', path='EXPERT', title="#muComb objects Mass vs DR; Mass; dR",
                             xbins=54, xmin=-0.1, xmax=3.5, ybins=101, ymin=-1, ymax=100)
        self.defineHistogram('NrAllEVs',type='TH1F', path='EXPERT', title="nr of all EVs received for #muComb removal; nr",
                             xbins=10, xmin=0, xmax=10)
        self.defineHistogram('NrActiveEVs',type='TH1F', path='EXPERT', title="nr of active EVs after #muComb removal ; nr",
                             xbins=10, xmin=0, xmax=10)
        self.defineHistogram('NrOverlapped',type='TH1F', path='EXPERT', title="nr of #muComb overlapped; nr",
                             xbins=10, xmin=0, xmax=10)
        self.defineHistogram('OverlappedEta , OverlappedPhi', type='TH2F', path='EXPERT', title="#muComb overlapped Eta vs Phi; Eta; Phi",
                             xbins=50, xmin=-3.2, xmax=3.2, ybins=25, ymin=-3.15, ymax=3.15)
        self.defineHistogram('OverlappedPt', type='TH1F', path='EXPERT', title="#muComb overlapped P_{T}; P_{T} (GeV)",
                             xbins=200, xmin=-100, xmax=100)


class TrigMuisoHypoMonitoring(GenericMonitoringTool):

    def __init__ (self, name):
        super(TrigMuisoHypoMonitoring, self).__init__(name)

        self.defineHistogram('CutCounter', type='TH1F', path='EXPERT', title="MuIsoHypo cut counter;cut; nevents",
                             xbins=9, xmin=-1.5, xmax=7.5, opt="kCumulative")
        self.defineHistogram('SumPtCone', type='TH1F', path='EXPERT', title="MuIsoHypo SumPt in cone around muon;E [GeV/c]; nevents",
                             xbins=200, xmin=0., xmax=15.)


class TrigMuonEFHypoMonitoring(GenericMonitoringTool):

    def __init__ (self, name="TrigMuonEFHypoMonitoring"):
        super(TrigMuonEFHypoMonitoring, self).__init__(name)

        self.defineHistogram('Pt', type='TH1F', path='EXPERT', title="P_{T} reconstruction from #TrigMuonEFHypo; P_{T} (MeV)",
                             xbins=200, xmin=-100, xmax=100)
        self.defineHistogram('Eta', type='TH1F', path='EXPERT', title="Eta reconstruction from #TrigMuonEFHypo; Eta",
                             xbins=100, xmin=-3.2, xmax=3.2)
        self.defineHistogram('Phi', type='TH1F', path='EXPERT', title="Phi reconstruction from #TrigMuonEFHypo; Phi",
                             xbins=100, xmin=-3.15, xmax=3.15)
        self.defineHistogram('Pt_sel', type='TH1F', path='EXPERT', title="Selected P_{T} reconstruction from #TrigMuonEFHypo; P_{T} (MeV)",
                             xbins=200, xmin=-100, xmax=100)
        self.defineHistogram('Eta_sel', type='TH1F', path='EXPERT', title="Selected Eta reconstruction from #TrigMuonEFHypo; Eta",
                             xbins=100, xmin=-3.2, xmax=3.2)
        self.defineHistogram('Phi_sel', type='TH1F', path='EXPERT', title="Selected Phi reconstruction from #TrigMuonEFHypo; Phi",
                             xbins=100, xmin=-3.15, xmax=3.15)


class TrigMuonEFInvMassHypoMonitoring(GenericMonitoringTool):

    def __init__ (self, name="TrigMuonEFInvMassHypoMonitoring"):
        super(TrigMuonEFInvMassHypoMonitoring, self).__init__(name)

        self.defineHistogram('Mass', type='TH1F', path='EXPERT', title="Dimuon mass from #TrigMuonEFInvMHypo; Mass (GeV)",
                             xbins=200, xmin=0, xmax=200)
        self.defineHistogram('Mass_sel', type='TH1F', path='EXPERT', title="Dimuon mass for selected events from #TrigMuonEFInvMHypo; Mass (GeV)",
                             xbins=200, xmin=0, xmax=200)
