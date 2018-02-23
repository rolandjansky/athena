# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TrigMuSuperEFMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigMuSuperEFMonitoring"):
        super(TrigMuSuperEFMonitoring, self).__init__(name)
        self.defineTarget( ["Online", "Validation"] )

        # overall histograms for full algorithm
        self.Histograms += [ defineHistogram('EF_trks_n', type='TH1F', title="Number of tracks; MuEF Number of Tracks",
                                             xbins=51, xmin=-0.5, xmax=50.5) ]
        self.Histograms += [ defineHistogram('EF_trks_pt', type='TH1F', title="pt ; MuEF Track pt [GeV]",
                                             xbins=100, xmin=0, xmax=200) ]
        self.Histograms += [ defineHistogram('EF_trks_phi', type='TH1F', title="phi ; MuEF Track Phi",
                                             xbins=100, xmin=-5, xmax=5) ]
        self.Histograms += [ defineHistogram('EF_trks_eta', type='TH1F', title="eta ; MuEF Track Eta",
                                             xbins=100, xmin=-5.5, xmax=5.5) ]
        self.Histograms += [ defineHistogram('EF_IDtrks_n', type='TH1F', title="Number of ID tracks ; n MuEF ID tracks",
                                             xbins=51, xmin=-0.5, xmax=50.5) ]
        self.Histograms += [ defineHistogram('EF_IDtrks_phi', type='TH1F', title="ID track phi; MuEF ID track phi",
                                             xbins=100, xmin=-5, xmax=5) ]
        self.Histograms += [ defineHistogram('EF_IDtrks_eta', type='TH1F', title="ID track eta ; MuEF ID track eta",
                                             xbins=100, xmin=-5.5, xmax=5.5) ]
        self.Histograms += [ defineHistogram('EF_IDtrks_pT', type='TH1F', title="ID track pT; MuEF ID track pT",
                                             xbins=100, xmin=0.0, xmax=200.0) ]
        self.Histograms += [ defineHistogram('EF_IDtrks_pT,EF_IDtrks_phi', type='TH2F', title="ID track phi; MuEF ID track pT; MuEF ID track phi",
                                             xbins=100, xmin=0.0, xmax=200.0, ybins=100, ymin=-5, ymax=5) ]
        self.Histograms += [ defineHistogram('EF_IDtrks_pT,EF_IDtrks_eta', type='TH2F', title="ID track eta; MuEF ID track pT; MuEF ID track eta",
                                             xbins=100, xmin=0.0, xmax=200.0, ybins=100, ymin=-5.5, ymax=5.5) ]


class TrigMuSuperEFValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigMuSuperEFValidationMonitoring"):
        super(TrigMuSuperEFValidationMonitoring, self).__init__(name)
        self.defineTarget( ["Validation"] )

        # overall histograms for full algorithm
        self.Histograms += [ defineHistogram('EF_trks_type', type='TH1F', title="author;1=TrigMuonEF 2=TrigMuGirl 3=TrigMuGirlTag",
                                             xbins=3, xmin=0.5, xmax=3.5) ]
        self.Histograms += [ defineHistogram('EF_trks_chi2', type='TH1F', title="chi2; MuEF Track chi2",
                                             xbins=100, xmin=0, xmax=30) ]
        self.Histograms += [ defineHistogram('EF_trks_chi2Prob', type='TH1F', title="chi2Prob; MuEF Track chi2 probability",
                                             xbins=50, xmin=0, xmax=1) ]
        self.Histograms += [ defineHistogram('EF_trks_match_chi2', type='TH1F', title="match_chi2; MuEF Track matching chi2",
                                             xbins=100, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('EF_trks_charge', type='TH1F', title="charge;Muon charge",
                                             xbins=3, xmin=-1.5, xmax=1.5) ]
        self.Histograms += [ defineHistogram('EF_trks_d0', type='TH1F', title="d0 ; MuEF Track d0",
                                             xbins=100, xmin=-10, xmax=10) ]
        self.Histograms += [ defineHistogram('EF_trks_z0', type='TH1F', title="z0 ; MuEF Track z0 [mm]",
                                             xbins=100, xmin=-250, xmax=250) ]
        self.Histograms += [ defineHistogram('EF_trks_nMdt', type='TH1F', title="Number of Mdt ; MuEF Mdt hits",
                                             xbins=51, xmin=-0.5, xmax=50.5) ]
        self.Histograms += [ defineHistogram('EF_trks_nRpcEta', type='TH1F', title="Number of Rpc eta; MuEF Rpc Eta hits",
                                             xbins=51, xmin=-0.5, xmax=50.5) ]
        self.Histograms += [ defineHistogram('EF_trks_nRpcPhi', type='TH1F', title="Number of Rpc phi ; MuEF Rpc Phi hits",
                                             xbins=51, xmin=-0.5, xmax=50.5) ]
        self.Histograms += [ defineHistogram('EF_trks_nTgcEta', type='TH1F', title="Number of Tgc eta; MuEF Tgc Eta hits",
                                             xbins=51, xmin=-0.5, xmax=50.5) ]
        self.Histograms += [ defineHistogram('EF_trks_nTgcPhi', type='TH1F', title="Number of Tgc phi; MuEF Tgc Phi hits",
                                             xbins=51, xmin=-0.5, xmax=50.5) ]
        self.Histograms += [ defineHistogram('EF_trks_nCscEta', type='TH1F', title="Number of Csc eta; MuEF Csc Eta hits",
                                             xbins=51, xmin=-0.5, xmax=50.5) ]
        self.Histograms += [ defineHistogram('EF_trks_nCscPhi', type='TH1F', title="Number of Csc phi; MuEF Csc Phi hits",
                                             xbins=51, xmin=-0.5, xmax=50.5) ]
        self.Histograms += [ defineHistogram('EF_trks_nHit', type='TH1F', title="Number of Hit on track ; MuEF Total hits",
                                             xbins=151, xmin=-0.5, xmax=150.5) ]
        self.Histograms += [ defineHistogram('EF_trks_nIdPixelHit', type='TH1F', title="Number of IdPixelHit on track ; MuEF Pixel hits",
                                             xbins=11, xmin=-0.5, xmax=10.5) ]
        self.Histograms += [ defineHistogram('EF_trks_nIdSctHit', type='TH1F', title="Number of IdSctHit on track ; MuEF Sct hits",
                                             xbins=21, xmin=-0.5, xmax=20.5) ]
        self.Histograms += [ defineHistogram('EF_trks_nIdTrtHit', type='TH1F', title="Number of IdTrtHit on track ; MuEF Trt hits",
                                             xbins=51, xmin=-0.5, xmax=50.5) ]
        
        #CT
        self.Histograms += [ defineHistogram('EF_cttrks_n', type='TH1F', title="Number of tracks; MuEF Number of Tracks",
                                             xbins=51, xmin=-0.5, xmax=50.5) ]
        self.Histograms += [ defineHistogram('EF_cttrks_pT', type='TH1F', title="pt ; MuEF Track pt [GeV]",
                                             xbins=100, xmin=0, xmax=200) ]
        self.Histograms += [ defineHistogram('EF_cttrks_phi', type='TH1F', title="phi ; MuEF Track Phi",
                                             xbins=100, xmin=-5, xmax=5) ]
        self.Histograms += [ defineHistogram('EF_cttrks_eta', type='TH1F', title="eta ; MuEF Track Eta",
                                             xbins=100, xmin=-5.5, xmax=5.5) ]
        self.Histograms += [ defineHistogram('EF_cttrks_d0', type='TH1F', title="d0 ; MuEF Track d0",
                                             xbins=100, xmin=-10, xmax=10) ]
        self.Histograms += [ defineHistogram('EF_cttrks_z0', type='TH1F', title="z0 ; MuEF Track z0 [mm]",
                                             xbins=100, xmin=-250, xmax=250) ]
        self.Histograms += [ defineHistogram('EF_cttrks_nHit', type='TH1F', title="Total Hits ; Total Hits",
                                             xbins=51, xmin=-0.5, xmax=50.5) ]
        self.Histograms += [ defineHistogram('EF_cttrks_nIdSctHit', type='TH1F', title="Sct Hits ; Sct Hits",
                                             xbins=51, xmin=-0.5, xmax=50.5) ]
        self.Histograms += [ defineHistogram('EF_cttrks_nIdPixelHit', type='TH1F', title="Pixel Hits ; Pixel Hits",
                                             xbins=51, xmin=-0.5, xmax=50.5) ]
        self.Histograms += [ defineHistogram('EF_cttrks_nIdTrtHit', type='TH1F', title="TRT Hits ; TRT Hits",
                                             xbins=51, xmin=-0.5, xmax=50.5) ]
        self.Histograms += [ defineHistogram('EF_cttrks_caloTag', type='TH1F', title="Calotag ; Calotag",
                                             xbins=51, xmin=-0.5, xmax=50.5) ]
        self.Histograms += [ defineHistogram('EF_cttrks_caloLH', type='TH1F', title="Calotag Likelihood ' Calo LH",
                                             xbins=51, xmin=-0.05, xmax=1.05) ]
        self.Histograms += [ defineHistogram('EF_cttrks_pT,EF_cttrks_caloTag', type='TH2F', title="CT Track Pt; MuEF ID track pT; CaloTag",
                                             xbins=100, xmin=0.0, xmax=200.0, ybins=51, ymin=-0.5, ymax=50.5)]
        self.Histograms += [ defineHistogram('EF_cttrks_eta,EF_cttrks_caloTag', type='TH2F', title="CT Track Eta; MuEF ID track eta; CaloTag",
                                             xbins=100, xmin=-5.5, xmax=5.5, ybins=51, ymin=-0.5, ymax=50.5)]
        
