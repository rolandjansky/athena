# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

class TrigMuCombMonitoring(GenericMonitoringTool):
    def __init__ (self, name = "TrigMuCombMonitoring"):
        super(TrigMuCombMonitoring, self).__init__( name )

        self.HistPath = name
        self.defineHistogram('Efficiency', path='EXPERT', type='TH1F', title="Monitored Track matching efficiency from #muComb", xbins=3, xmin=-1.5, xmax=1.5) 
        self.defineHistogram('ErrorFlagMC', path='EXPERT', type='TH1F', title="Error Flags during event processing from #muComb; Error Code", xbins=12, xmin=-1.5, xmax=10.5) 
        self.defineHistogram('MatchFlagMC', path='EXPERT', type='TH1F', title="Match Flags during matching algo from #muComb; Error Code", xbins=12, xmin=-1.5, xmax=10.5) 
        self.defineHistogram('StrategyMC', path='EXPERT', type='TH1F', title="Combination Strategy from #muComb; Strategy Code", xbins=12, xmin=-1.5, xmax=10.5) 
        self.defineHistogram('PtMS', path='EXPERT', type='TH1F', title="p_{T} MS from #muComb; p_{T} (GeV/c)", xbins=210, xmin=-105., xmax=105.) 
        self.defineHistogram('PtID', path='EXPERT', type='TH1F', title="p_{T} ID from #muComb; p_{T} (GeV/c)", xbins=210, xmin=-105., xmax=105.) 
        self.defineHistogram('PtMC', path='EXPERT', type='TH1F', title="p_{T} Combined from #muComb; p_{T} (GeV/c)", xbins=210, xmin=-105., xmax=105.) 
        self.defineHistogram('PtFL', path='EXPERT', type='TH1F', title="MS p_{T} failed matches from #muComb; p_{T} (GeV/c)", xbins=210, xmin=-105., xmax=105.) 
        self.defineHistogram('EtaFL', path='EXPERT', type='TH1F', title="MS Eta failed matchs from #muComb; Eta", xbins=108, xmin=-2.7, xmax=2.7) 
        self.defineHistogram('PhiFL', path='EXPERT', type='TH1F', title="MS Phi failed matches from #muComb; Phi (rad)", xbins=96, xmin=-3.1416, xmax=3.1416) 
        self.defineHistogram('EtaID', path='EXPERT', type='TH1F', title="Eta ID from #muComb; Eta", xbins=108, xmin=-2.7, xmax=2.7) 
        self.defineHistogram('ZetaID', path='EXPERT', type='TH1F', title="z0 ID from #muComb; z0 (mm)", xbins=100, xmin=-200., xmax=200.) 
        self.defineHistogram('EtaMS', path='EXPERT', type='TH1F', title="Eta MS from #muComb; Eta", xbins=108, xmin=-2.7, xmax=2.7) 
        self.defineHistogram('ZetaMS', path='EXPERT', type='TH1F', title="z0 MS from #muComb; z0 (mm)", xbins=100, xmin=-200., xmax=200.) 
        self.defineHistogram('DEta', path='EXPERT', type='TH1F', title="Eta difference between muon and matched ID track from #muComb; DeltaEta", xbins=100, xmin=0.0, xmax=0.2) 
        self.defineHistogram('DPhi', path='EXPERT', type='TH1F', title="Phi difference between muon and matched ID track from #muComb; DeltaPhi (rad)", xbins=100, xmin=0.0, xmax=0.2) 
        self.defineHistogram('DZeta', path='EXPERT', type='TH1F', title="Zeta difference between muon and matched ID track from #muComb; DeltaZeta (cm)", xbins=100, xmin=-60, xmax=60) 
        self.defineHistogram('DeltaR', path='EXPERT', type='TH1F', title="DeltaR between muon and matched ID track from #muComb; DeltaR", xbins=100, xmin=0., xmax=0.5) 
        self.defineHistogram('PtMS, PtID', path='EXPERT', type='TH2F', title="PtID vs PtMS from #muComb", xbins=105, xmin=-105.0, xmax=105.0, ybins=105, ymin=-105.0, ymax=105.0) 
        self.defineHistogram('EtaMS, DEta', path='EXPERT', type='TH2F', title="Eta diff (MS-ID) vs Eta(MS) from #muComb", xbins=100, xmin=-3, xmax=3, ybins=100, ymin=0.0, ymax=0.2) 
        self.defineHistogram('EtaMS, DPhi', path='EXPERT', type='TH2F', title="Phi diff (MS-ID) vs Eta(MS) from #muComb", xbins=100, xmin=-3, xmax=3, ybins=100, ymin=0.0, ymax=0.2) 

