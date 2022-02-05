# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

def TrigMuCombMonitoring(name = "TrigMuCombMonitoring"):

    montool = GenericMonitoringTool(name, HistPath = name)

    montool.defineHistogram('Efficiency', path='EXPERT', type='TH1F', title="Monitored Track matching efficiency from #muComb", xbins=3, xmin=-1.5, xmax=1.5)
    montool.defineHistogram('ErrorFlagMC', path='EXPERT', type='TH1F', title="Error Flags during event processing from #muComb; Error Code", xbins=12, xmin=-1.5, xmax=10.5)
    montool.defineHistogram('MatchFlagMC', path='EXPERT', type='TH1F', title="Match Flags during matching algo from #muComb; Error Code", xbins=12, xmin=-1.5, xmax=10.5)
    montool.defineHistogram('StrategyMC', path='EXPERT', type='TH1F', title="Combination Strategy from #muComb; Strategy Code", xbins=12, xmin=-1.5, xmax=10.5)
    montool.defineHistogram('PtMS', path='EXPERT', type='TH1F', title="p_{T} MS from #muComb; p_{T} (GeV/c)", xbins=210, xmin=-105., xmax=105.)
    montool.defineHistogram('PtID', path='EXPERT', type='TH1F', title="p_{T} ID from #muComb; p_{T} (GeV/c)", xbins=210, xmin=-105., xmax=105.)
    montool.defineHistogram('PtMC', path='EXPERT', type='TH1F', title="p_{T} Combined from #muComb; p_{T} (GeV/c)", xbins=210, xmin=-105., xmax=105.)
    montool.defineHistogram('PtFL', path='EXPERT', type='TH1F', title="MS p_{T} failed matches from #muComb; p_{T} (GeV/c)", xbins=210, xmin=-105., xmax=105.)
    montool.defineHistogram('EtaFL', path='EXPERT', type='TH1F', title="MS Eta failed matchs from #muComb; Eta", xbins=108, xmin=-2.7, xmax=2.7)
    montool.defineHistogram('PhiFL', path='EXPERT', type='TH1F', title="MS Phi failed matches from #muComb; Phi (rad)", xbins=96, xmin=-3.1416, xmax=3.1416)
    montool.defineHistogram('EtaID', path='EXPERT', type='TH1F', title="Eta ID from #muComb; Eta", xbins=108, xmin=-2.7, xmax=2.7)
    montool.defineHistogram('ZetaID', path='EXPERT', type='TH1F', title="z0 ID from #muComb; z0 (mm)", xbins=100, xmin=-200., xmax=200.)
    montool.defineHistogram('EtaMS', path='EXPERT', type='TH1F', title="Eta MS from #muComb; Eta", xbins=108, xmin=-2.7, xmax=2.7)
    montool.defineHistogram('ZetaMS', path='EXPERT', type='TH1F', title="z0 MS from #muComb; z0 (mm)", xbins=100, xmin=-200., xmax=200.)
    montool.defineHistogram('DEta', path='EXPERT', type='TH1F', title="Eta difference between muon and matched ID track from #muComb; DeltaEta", xbins=100, xmin=0.0, xmax=0.2)
    montool.defineHistogram('DPhi', path='EXPERT', type='TH1F', title="Phi difference between muon and matched ID track from #muComb; DeltaPhi (rad)", xbins=100, xmin=0.0, xmax=0.2)
    montool.defineHistogram('DZeta', path='EXPERT', type='TH1F', title="Zeta difference between muon and matched ID track from #muComb; DeltaZeta (cm)", xbins=100, xmin=-60, xmax=60)
    montool.defineHistogram('DeltaR', path='EXPERT', type='TH1F', title="DeltaR between muon and matched ID track from #muComb; DeltaR", xbins=100, xmin=0., xmax=0.5)
    montool.defineHistogram('PtMS, PtID', path='EXPERT', type='TH2F', title="PtID vs PtMS from #muComb", xbins=105, xmin=-105.0, xmax=105.0, ybins=105, ymin=-105.0, ymax=105.0)
    montool.defineHistogram('EtaMS, DEta', path='EXPERT', type='TH2F', title="Eta diff (MS-ID) vs Eta(MS) from #muComb", xbins=100, xmin=-3, xmax=3, ybins=100, ymin=0.0, ymax=0.2)
    montool.defineHistogram('EtaMS, DPhi', path='EXPERT', type='TH2F', title="Phi diff (MS-ID) vs Eta(MS) from #muComb", xbins=100, xmin=-3, xmax=3, ybins=100, ymin=0.0, ymax=0.2)

    return montool
