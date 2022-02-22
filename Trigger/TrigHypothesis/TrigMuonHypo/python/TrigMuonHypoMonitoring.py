# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

def TrigMufastHypoMonitoring(histPath):
    montool = GenericMonitoringTool(HistPath = histPath)
    montool.defineHistogram('Pt', type='TH1F', path='EXPERT', title="P_{T} reconstruction from #muFast; P_{T} (GeV)",
                            xbins=200, xmin=-100, xmax=100)
    montool.defineHistogram('PtFL', type='TH1F', path='EXPERT', title="P_{T} of not selected muons from #muFast; p_{T} (GeV)",
                            xbins=200, xmin=-100, xmax=100)
    montool.defineHistogram('Eta , Phi', type='TH2F', path='EXPERT', title="Eta vs Phi reconstruction of #muFast; Eta; Phi",
                            xbins=50, xmin=-3.2, xmax=3.2, ybins=25, ymin=-3.15, ymax=3.15)
    montool.defineHistogram('Eta', type='TH1F', path='EXPERT', title="Eta reconstruction from #muFast; Eta",
                            xbins=100, xmin=-3.2, xmax=3.2)
    montool.defineHistogram('Phi', type='TH1F', path='EXPERT', title="Phi reconstruction from #muFast; Phi",
                            xbins=100, xmin=-3.15, xmax=3.15)
    montool.defineHistogram('ZatSt, Phi', type='TH2F', path='EXPERT', title="Z vs Phi reconstructed in MIDDLE station; Z (cm); Phi (rad)",
                            xbins=50, xmin=-1200., xmax=1200., ybins=25, ymin=-3.2, ymax=3.2)
    montool.defineHistogram('XatSt , YatSt', type='TH2F', path='EXPERT', title="Y vs X reconstructed in MIDDLE station; X (cm); Y(cm)",
                            xbins=50, xmin=-1200., xmax=1200., ybins=50, ymin=-1200., ymax=1200.)
    montool.defineHistogram('ZatBe', type='TH1F', path='EXPERT', title="DCA along Z; Z (cm)",
                            xbins=100, xmin=-2100, xmax=2100)
    montool.defineHistogram('XatBe', type='TH1F', path='EXPERT', title="DCA along X; X (cm)",
                            xbins=100, xmin=-1000, xmax=1000)
    return montool


def TrigL2MuonOverlapRemoverMonitoringMufast(histPath):
    montool = TrigMufastHypoMonitoring(histPath)
    montool.defineHistogram('MufastError',  type='TH1F', path='EXPERT', title="error in #muFast based overlap removal; error code",
                         xbins=10, xmin=0, xmax=10)
    montool.defineHistogram('DR',  type='TH1F', path='EXPERT', title="#muFast objects dR; dR",
                         xbins=108, xmin=-0.1, xmax=3.5)
    montool.defineHistogram('Mass',type='TH1F', path='EXPERT', title="#muFast objects invMass; Mass (GeV)",
                         xbins=202, xmin=-1, xmax=100)
    montool.defineHistogram('DRLog10',  type='TH1F', path='EXPERT', title="#muFast objects Log10(dR); Log10(dR)",
                         xbins=102, xmin=-4.1, xmax=1.0)
    montool.defineHistogram('MassLog10',type='TH1F', path='EXPERT', title="#muFast objects Log10(invMass); Log10(Mass (GeV))",
                         xbins=142, xmin=-4.1, xmax=3.0)
    montool.defineHistogram('Mass , DR', type='TH2F', path='EXPERT', title="#muFast objects Mass vs DR; Mass; dR",
                         xbins=54, xmin=-0.1, xmax=3.5, ybins=101, ymin=-1, ymax=100)
    montool.defineHistogram('NrAllEVs',type='TH1F', path='EXPERT', title="nr of all EVs received for #muFast removal; nr",
                         xbins=10, xmin=0, xmax=10)
    montool.defineHistogram('NrActiveEVs',type='TH1F', path='EXPERT', title="nr of active EVs after #muFast removal ; nr",
                         xbins=10, xmin=0, xmax=10)
    montool.defineHistogram('NrOverlapped',type='TH1F', path='EXPERT', title="nr of #muFast overlapped; nr",
                         xbins=10, xmin=0, xmax=10)
    montool.defineHistogram('OverlappedEta , OverlappedPhi', type='TH2F', path='EXPERT', title="#muFast overlapped Eta vs Phi; Eta; Phi",
                         xbins=50, xmin=-3.2, xmax=3.2, ybins=25, ymin=-3.15, ymax=3.15)
    montool.defineHistogram('OverlappedPt', type='TH1F', path='EXPERT', title="#muFast overlapped P_{T}; P_{T} (GeV)",
                         xbins=200, xmin=-100, xmax=100)
    return montool


def TrigmuCombHypoMonitoring(histPath):
    montool = GenericMonitoringTool(HistPath = histPath)
    montool.defineHistogram('Pt', type='TH1F', path='EXPERT', title="p_{T} reconstruction from #muComb; p_{T} (GeV)",
                            xbins=210, xmin=-105, xmax=105)
    montool.defineHistogram('PtFL', type='TH1F', path='EXPERT', title="p_{T} of not selected muons from #muComb; p_{T} (GeV)",
                            xbins=210, xmin=-105., xmax=105.)
    montool.defineHistogram('StrategyFlag', type='TH1F', path='EXPERT', title="Combination Strategy from #muComb; Strategy Code",
                            xbins=12, xmin=-1.5, xmax=10.5)
    montool.defineHistogram('Eta', type='TH1F', path='EXPERT', title="Eta reconstruction from #muComb; Eta",
                            xbins=108, xmin=-2.7, xmax=2.7)
    montool.defineHistogram('Phi', type='TH1F', path='EXPERT', title="Phi reconstruction from #muComb; Phi (rad)",
                            xbins=96, xmin=-3.1416, xmax=3.1416)
    montool.defineHistogram('Z0', type='TH1F', path='EXPERT', title="PCA along Z from ID track from #muComb; PCA(Z0) (mm)",
                            xbins=100, xmin=-200, xmax=200)
    montool.defineHistogram('A0', type='TH1F', path='EXPERT', title="PCA along x-y from ID track from #muComb; PCA(A0) (mm)",
                            xbins=100, xmin=-0.6, xmax=0.6)
    return montool


def TrigL2MuonOverlapRemoverMonitoringMucomb(histPath):
    montool = TrigmuCombHypoMonitoring(histPath)
    montool.defineHistogram('MucombError',  type='TH1F', path='EXPERT', title="error in #muComb based overlap removal; error code",
                            xbins=10, xmin=0, xmax=10)
    montool.defineHistogram('DR',  type='TH1F', path='EXPERT', title="#muComb objects dR; dR",
                            xbins=108, xmin=-0.1, xmax=3.5)
    montool.defineHistogram('Mass',type='TH1F', path='EXPERT', title="#muComb objects invMass; Mass (GeV)",
                            xbins=202, xmin=-1, xmax=100)
    montool.defineHistogram('DRLog10',  type='TH1F', path='EXPERT', title="#muFast objects Log10(dR); Log10(dR)",
                            xbins=102, xmin=-4.1, xmax=1.0)
    montool.defineHistogram('MassLog10',type='TH1F', path='EXPERT', title="#muFast objects Log10(invMass); Log10(Mass (GeV))",
                            xbins=142, xmin=-4.1, xmax=3.0)
    montool.defineHistogram('Mass , DR', type='TH2F', path='EXPERT', title="#muComb objects Mass vs DR; Mass; dR",
                            xbins=54, xmin=-0.1, xmax=3.5, ybins=101, ymin=-1, ymax=100)
    montool.defineHistogram('NrAllEVs',type='TH1F', path='EXPERT', title="nr of all EVs received for #muComb removal; nr",
                            xbins=10, xmin=0, xmax=10)
    montool.defineHistogram('NrActiveEVs',type='TH1F', path='EXPERT', title="nr of active EVs after #muComb removal ; nr",
                            xbins=10, xmin=0, xmax=10)
    montool.defineHistogram('NrOverlapped',type='TH1F', path='EXPERT', title="nr of #muComb overlapped; nr",
                            xbins=10, xmin=0, xmax=10)
    montool.defineHistogram('OverlappedEta , OverlappedPhi', type='TH2F', path='EXPERT', title="#muComb overlapped Eta vs Phi; Eta; Phi",
                            xbins=50, xmin=-3.2, xmax=3.2, ybins=25, ymin=-3.15, ymax=3.15)
    montool.defineHistogram('OverlappedPt', type='TH1F', path='EXPERT', title="#muComb overlapped P_{T}; P_{T} (GeV)",
                            xbins=200, xmin=-100, xmax=100)
    return montool


def TrigMuonEFHypoMonitoring(histPath):
    montool = GenericMonitoringTool(HistPath = histPath)
    montool.defineHistogram('Pt', type='TH1F', path='EXPERT', title="P_{T} reconstruction from #TrigMuonEFHypo; P_{T} (GeV)",
                            xbins=200, xmin=-100, xmax=100)
    montool.defineHistogram('Eta', type='TH1F', path='EXPERT', title="Eta reconstruction from #TrigMuonEFHypo; Eta",
                            xbins=100, xmin=-3.2, xmax=3.2)
    montool.defineHistogram('Phi', type='TH1F', path='EXPERT', title="Phi reconstruction from #TrigMuonEFHypo; Phi",
                            xbins=100, xmin=-3.15, xmax=3.15)
    montool.defineHistogram('Pt_sel', type='TH1F', path='EXPERT', title="Selected P_{T} reconstruction from #TrigMuonEFHypo; P_{T} (GeV)",
                            xbins=200, xmin=-100, xmax=100)
    montool.defineHistogram('Eta_sel', type='TH1F', path='EXPERT', title="Selected Eta reconstruction from #TrigMuonEFHypo; Eta",
                            xbins=100, xmin=-3.2, xmax=3.2)
    montool.defineHistogram('Phi_sel', type='TH1F', path='EXPERT', title="Selected Phi reconstruction from #TrigMuonEFHypo; Phi",
                            xbins=100, xmin=-3.15, xmax=3.15)
    return montool


def TrigMuonEFInvMassHypoMonitoring(histPath):
    montool = GenericMonitoringTool(HistPath = histPath)
    montool.defineHistogram('Mass', type='TH1F', path='EXPERT', title="Dimuon mass from #TrigMuonEFInvMHypo; Mass (GeV)",
                            xbins=200, xmin=0, xmax=200)
    montool.defineHistogram('Mass_sel', type='TH1F', path='EXPERT', title="Dimuon mass for selected events from #TrigMuonEFInvMHypo; Mass (GeV)",
                            xbins=200, xmin=0, xmax=200)
    return montool


def TrigMuonTLAHypoMonitoring(histPath):
    montool = GenericMonitoringTool(HistPath = histPath)
    montool.defineHistogram('Pt', type='TH1F', path='EXPERT', title="P_{T} reconstruction from #TrigMuonTLAHypo; q*P_{T} (GeV)",
                            xbins=200, xmin=-100, xmax=100)
    montool.defineHistogram('Eta', type='TH1F', path='EXPERT', title="Eta reconstruction from #TrigMuonTLAHypo; Eta",
                            xbins=100, xmin=-3.2, xmax=3.2)
    montool.defineHistogram('Phi', type='TH1F', path='EXPERT', title="Phi reconstruction from #TrigMuonTLAHypo; Phi",
                            xbins=100, xmin=-3.15, xmax=3.15)
    montool.defineHistogram('Author', type='TH1F', path='EXPERT', title="Author reconstruction from #TrigMuonTLAHypo; Author",
                            xbins=7, xmin=0, xmax=7) # MuidCo(1), MuidSA(5), MuGirl(6)
    montool.defineHistogram('Nmuon', type='TH1F', path='EXPERT', title="Number of copied muons/event in #TrigMuonTLAHypo; N. muons/event",
                            xbins=50, xmin=0, xmax=50)
    return montool
