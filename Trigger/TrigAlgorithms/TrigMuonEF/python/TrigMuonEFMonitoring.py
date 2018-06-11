# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import math

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig




class TrigMuonEFTrackIsolationOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigMuonEFTrackIsolationOnlineMonitoring"):
        super(TrigMuonEFTrackIsolationOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms += [ defineHistogram('trks_trkptiso_cone2', type='TH1F', title="Track Isolation cone 0.2; MuonEF CB Trk Isolation cone 0.2 [GeV] (ID)",
                                             xbins=35, xmin=-10, xmax=25) ]
        self.Histograms += [ defineHistogram('trks_trkptiso_cone3', type='TH1F', title="Track Isolation cone 0.3; MuonEF CB Trk Isolation cone 0.3 [GeV] (ID)",
                                             xbins=35, xmin=-10, xmax=25) ]
        self.Histograms += [ defineHistogram('trks_trkdz', type='TH1F', title="dz to trks; dz to trks",
                                             xbins=200, xmin=-10, xmax=10.0) ]
        self.Histograms += [ defineHistogram('trks_trkdr', type='TH1F', title="dr to trks; dr to trks",
                                             xbins=60, xmin=0.0, xmax=3.0) ]

class TrigMuonEFTrackIsolationValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigMuonEFTrackIsolationValidationMonitoring"):
        super(TrigMuonEFTrackIsolationValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")

        self.Histograms += [ defineHistogram('trks_trkptiso_cone2', type='TH1F', title="Track Isolation cone 0.2; MuonEF CB Trk Isolation cone 0.2 [GeV] (ID)",
                                             xbins=35, xmin=-10, xmax=25) ]
        self.Histograms += [ defineHistogram('trks_trkptiso_cone3', type='TH1F', title="Track Isolation cone 0.3; MuonEF CB Trk Isolation cone 0.3 [GeV] (ID)",
                                             xbins=35, xmin=-10, xmax=25) ]
        self.Histograms += [ defineHistogram('trks_trkdz', type='TH1F', title="dz to trks; dz to trks",
                                             xbins=200, xmin=-10, xmax=10.0) ]
        self.Histograms += [ defineHistogram('trks_trkdr', type='TH1F', title="dr to trks; dr to trks",
                                             xbins=60, xmin=0.0, xmax=3.0) ]
         
        self.Histograms += [ defineHistogram('n_trks', type='TH1F', title="N_tracks passed to algorithm",
                                             xbins=11, xmin=-0.5, xmax=10.5) ]
       
        self.Histograms += [ defineHistogram('muon_selfpt', type='TH1F', title="Muon ID Track pT (ID Tracks)",
                                             xbins=100, xmin=0, xmax=100) ]

        self.Histograms += [ defineHistogram('muon_removedpt', type='TH1F', title="Muon Removed pT (ID Tracks)",
                                             xbins=100, xmin=0, xmax=100) ]
        self.Histograms += [ defineHistogram('muon_combinedpt', type='TH1F', title="Muon Combined pT (ID Tracks)",
                                             xbins=100, xmin=0, xmax=100) ]
         

        
        
class TrigMuonEFCaloIsolationValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigMuonEFCaloIsolationValidationMonitoring"):
        super(TrigMuonEFCaloIsolationValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms += [ defineHistogram('EF_etiso_cone2', type='TH1F', title="Calo Isolation cone 0.2; MuonEF Calo Isolation cone 0.2 [GeV]",
                                             xbins=40, xmin=-10, xmax=30) ]
        self.Histograms += [ defineHistogram('EF_etiso_cone3', type='TH1F', title="Calo Isolation cone 0.3; MuonEF Calo Isolation cone 0.3 [GeV]",
                                             xbins=40, xmin=-10, xmax=30) ]
        self.Histograms += [ defineHistogram('EF_etiso_cone4', type='TH1F', title="Calo Isolation cone 0.4; MuonEF Calo Isolation cone 0.4 [GeV]",
                                             xbins=40, xmin=-10, xmax=30) ]


class TrigMuonEFStandaloneToolMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigMuonEFStandaloneToolMonitoring"):
        super(TrigMuonEFStandaloneToolMonitoring, self).__init__(name)
        self.defineTarget( ["Online", "Validation"] )
 
        # segment finding
        self.Histograms += [ defineHistogram('EF_segs_nsegs', type='TH1F', title="Number of segments (TMEF seg); Muon EF SF segments",
                                             xbins=100, xmin=0, xmax=100)]
        self.Histograms += [ defineHistogram('EF_segs_phi', type='TH1F', title="EF Segment Phi (TMEF seg); Muon EF SF Phi",
                                             xbins=100, xmin=-5, xmax=5)]                              
        self.Histograms += [ defineHistogram('EF_segs_eta', type='TH1F', title="EF Segment Eta (TMEF seg); Muon EF SF Eta",
                                             xbins=100, xmin=-5, xmax=5)]
        self.Histograms += [ defineHistogram('EF_segs_eta, EF_segs_phi', type='TH2F', title="EF Segment Eta vs Phi (TMEF seg); Muon EF SF Eta ; Muon EF SF Phi",
                                             xbins=60, xmin=-3., xmax=3., ybins=48, ymin=-3.142, ymax=3.142) ]
        
        # TMEF MS track
        self.Histograms += [ defineHistogram('EF_mstrks_n', type='TH1F', title="Number of tracks (TMEF MS track); Muon EF TB Number of Tracks",
                                             xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('EF_mstrks_pt', type='TH1F', title="pt (TMEF MS track); Muon EF TB Track pt [GeV]",
                                             xbins=100, xmin=0, xmax=200) ]
        self.Histograms += [ defineHistogram('EF_mstrks_phi', type='TH1F', title="phi (TMEF MS track); Muon EF TB Track Phi",
                                             xbins=100, xmin=-5, xmax=5) ]
        self.Histograms += [ defineHistogram('EF_mstrks_eta', type='TH1F', title="eta (TMEF MS track); Muon EF TB Track Eta",
                                             xbins=100, xmin=-5.5, xmax=5.5) ]
        self.Histograms += [ defineHistogram('EF_mstrks_eta, EF_mstrks_phi', type='TH2F', title="EF_mstrks_eta_vs_EF_mstrks_phi (TMEF MS track); Muon EF TB Eta-Phi",
                                             xbins=60, xmin=-3, xmax=3, ybins=48, ymin=-3.142, ymax=3.142) ]

        
        # TMEF SA track
        self.Histograms += [ defineHistogram('EF_satrks_n', type='TH1F', title="Number of tracks (TMEF SA track); Muon EF SA Number of Tracks",
                                             xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('EF_satrks_pt', type='TH1F', title="pt (TMEF SA track); Muon EF SA Track pt [GeV]",
                                             xbins=100, xmin=0, xmax=200) ]
        self.Histograms += [ defineHistogram('EF_satrks_phi', type='TH1F', title="phi (TMEF SA track); Muon EF SA Track Phi",
                                             xbins=100, xmin=-5, xmax=5) ]
        self.Histograms += [ defineHistogram('EF_satrks_eta', type='TH1F', title="eta (TMEF SA track); Muon EF SA Track Eta",
                                             xbins=100, xmin=-5.5, xmax=5.5) ]

class TrigMuonEFStandaloneToolValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigMuonEFStandaloneToolValidationMonitoring"):
        super(TrigMuonEFStandaloneToolValidationMonitoring, self).__init__(name)
        self.defineTarget( ["Validation"] )
 
        # segment finding
        self.Histograms += [ defineHistogram('l2_cosmic_phi', type='TH1F', title="L2 CosmicMuon phi (TMEF seg); Muon EF SF Phi",
                                             xbins=100, xmin=-5, xmax=5) ]
        self.Histograms += [ defineHistogram('l2_cosmic_eta', type='TH1F', title="L2 CosmicMuon eta (TMEF seg); Muon EF SF Eta",
                                             xbins=100, xmin=-5, xmax=5)]
        self.Histograms += [ defineHistogram('ef_phi, l2_cosmic_phi', type='TH2F', title="EF phi L2 vs EF (TMEF seg); Muon EF SF Phi",
                                             xbins=48, xmin=-3.142, xmax=3.142, ybins=48, ymin=-3.142, ymax=3.142) ]
        self.Histograms += [ defineHistogram('ef_eta, l2_cosmic_eta', type='TH2F', title="EF eta L2 vs EF (TMEF seg); Muon EF SF Eta",
                                             xbins=60, xmin=-3, xmax=3, ybins=60, ymin=-3, ymax=3.) ]

        self.Histograms += [ defineHistogram('EF_segs_wasCached', type='TH1F',
                                             title="Did segments come from cache?;0=no 1=yes",
                                             xbins=2, xmin=-0.5, xmax=1.5) ]
        self.Histograms += [ defineHistogram('EF_segs_nsegcomb', type='TH1F', title="Number of segment combinations (TMEF seg); Muon EF SF segments",
                                             xbins=50, xmin=0, xmax=50)]
        self.Histograms += [ defineHistogram('EF_segs_nstations', type='TH1F', title="Number of stations (TMEF seg); Muon EF SF segments",
                                             xbins=10, xmin=0, xmax=10)]
        self.Histograms += [ defineHistogram('EF_segs_chi2pndof', type='TH1F', title="EF Segment Chi2 / ndof (TMEF seg); Muon EF SF Chi2 / ndof",
                                             xbins=100, xmin=0.0, xmax=20.0) ]
        self.Histograms += [ defineHistogram('EF_segs_nMdt', type='TH1F', title="Number mdt hits per segment (TMEF seg); Muon EF SF Mdt Hits ",
                                             xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('EF_segs_nCsc', type='TH1F', title="Number csc hits per segment (TMEF seg); Muon EF SF Csc hits",
                                             xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('EF_segs_nRpc', type='TH1F', title="Number rpc hits per segment (TMEF seg); Muon EF SF Rpc hits",
                                             xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('EF_segs_nTgc', type='TH1F', title="Number tgc hits per segment (TMEF seg); Muon EF SF Tgc hits",
                                             xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('EF_segs_mdt_radius', type='TH1F', title="Mdt tube radius (TMEF seg); Muon EF SF Drift radius [mm] of Mdt hits",
                                             xbins=50, xmin=-50, xmax=50) ]
        self.Histograms += [ defineHistogram('EF_segs_mdt_tdc', type='TH1F', title="Mdt tube tdc (TMEF seg); Muon EF SF TDC counts of Mdt hits",
                                             xbins=100, xmin=0, xmax=2000) ]
        self.Histograms += [ defineHistogram('EF_segs_mdt_adc', type='TH1F', title="Mdt tube adc (TMEF seg); Muon EF SF ADC counts of Mdt hits",
                                             xbins=100, xmin=0, xmax=2000) ]
        self.Histograms += [ defineHistogram('EF_segs_mdt_z', type='TH1F', title="Mdt tube z (TMEF seg); Muon EF SF Tube z [mm]",
                                             xbins=100, xmin=-25000, xmax=25000) ]
        
        # TMEF MS track
        self.Histograms += [ defineHistogram('EF_mstrks_wasCached', type='TH1F',
                                             title="Did MS tracks come from cache?;0=no 1=yes",
                                             xbins=2, xmin=-0.5, xmax=1.5) ]
        self.Histograms += [ defineHistogram('EF_mstrks_chi2', type='TH1F', title="chi2 (TMEF MS track); Muon EF TB Track chi2",
                                             xbins=100, xmin=0, xmax=30) ]
        self.Histograms += [ defineHistogram('EF_mstrks_d0', type='TH1F', title="d0 (TMEF MS track); Muon EF TB Track d0",
                                             xbins=100, xmin=-0.01, xmax=0.01) ]
        self.Histograms += [ defineHistogram('EF_mstrks_z0', type='TH1F', title="z0 (TMEF MS track); Muon EF TB Track z0 [mm]",
                                             xbins=100, xmin=-50, xmax=50) ]
        self.Histograms += [ defineHistogram('EF_mstrks_nMdt', type='TH1F', title="Number of Mdt (TMEF MS track); Muon EF TB Mdt hits",
                                             xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('EF_mstrks_nRpcEta', type='TH1F', title="Number of Rpc eta (TMEF MS track); Muon EF TB Rpc Eta hits",
                                             xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('EF_mstrks_nRpcPhi', type='TH1F', title="Number of Rpc phi (TMEF MS track); Muon EF TB Rpc Phi hits",
                                             xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('EF_mstrks_nTgcEta', type='TH1F', title="Number of Tgc eta (TMEF MS track); Muon EF TB Tgc Eta hits",
                                             xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('EF_mstrks_nTgcPhi', type='TH1F', title="Number of Tgc phi (TMEF MS track); Muon EF TB Tgc Phi hits",
                                             xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('EF_mstrks_nCscEta', type='TH1F', title="Number of Csc eta (TMEF MS track); Muon EF TB Csc Eta hits",
                                             xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('EF_mstrks_nCscPhi', type='TH1F', title="Number of Csc phi (TMEF MS track); Muon EF TB Csc Phi hits",
                                             xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('EF_mstrks_nHit', type='TH1F', title="Number of Hit on track (TMEF MS track); Muon EF TB Total hits",
                                             xbins=50, xmin=0, xmax=100) ]

        
        # TMEF SA track
        self.Histograms += [ defineHistogram('EF_satrks_wasCached', type='TH1F',
                                             title="Did SA tracks come from cache?;0=no 1=yes",
                                             xbins=2, xmin=-0.5, xmax=1.5) ]
        self.Histograms += [ defineHistogram('EF_satrks_chi2', type='TH1F', title="chi2 (TMEF SA track); Muon EF SA Track chi2",
                                             xbins=100, xmin=0, xmax=30) ]
        self.Histograms += [ defineHistogram('EF_satrks_d0', type='TH1F', title="d0 (TMEF SA track); Muon EF SA Track d0",
                                             xbins=100, xmin=-1000, xmax=1000) ]
        self.Histograms += [ defineHistogram('EF_satrks_z0', type='TH1F', title="z0 (TMEF SA track); Muon EF SA Track z0 [mm]",
                                             xbins=100, xmin=-1000, xmax=1000) ]
        self.Histograms += [ defineHistogram('EF_satrks_nMdt', type='TH1F', title="Number of Mdt (TMEF SA track); Muon EF SA Mdt hits",
                                             xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('EF_satrks_nRpcEta', type='TH1F', title="Number of Rpc eta (TMEF SA track); Muon EF SA Rpc Eta hits",
                                             xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('EF_satrks_nRpcPhi', type='TH1F', title="Number of Rpc phi (TMEF SA track); Muon EF SA Rpc Phi hits",
                                             xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('EF_satrks_nTgcEta', type='TH1F', title="Number of Tgc eta (TMEF SA track); Muon EF SA Tgc Eta hits",
                                             xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('EF_satrks_nTgcPhi', type='TH1F', title="Number of Tgc phi (TMEF SA track); Muon EF SA Tgc Phi hits",
                                             xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('EF_satrks_nCscEta', type='TH1F', title="Number of Csc eta (TMEF SA track); Muon EF SA Csc Eta hits",
                                             xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('EF_satrks_nCscPhi', type='TH1F', title="Number of Csc phi (TMEF SA track); Muon EF SA Csc Phi hits",
                                             xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('EF_satrks_nHit', type='TH1F', title="Number of Hit on track (TMEF SA track); Muon EF SA Total hits",
                                             xbins=50, xmin=0, xmax=100) ]


class TrigMuonEFCombinerToolMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigMuonEFCombinerToolMonitoring"):
        super(TrigMuonEFCombinerToolMonitoring, self).__init__(name)
        self.defineTarget( ["Online", "Validation"] )

        # TMEF combined track
        self.Histograms += [ defineHistogram('EF_cbtrks_n', type='TH1F', title="Number of tracks (TMEF CB track); MuonEF CB Number of Tracks",
                                             xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('EF_cbtrks_pt', type='TH1F', title="pt (TMEF CB track); MuonEF CB Track pt [GeV]",
                                             xbins=100, xmin=0, xmax=200) ]
        self.Histograms += [ defineHistogram('EF_cbtrks_phi', type='TH1F', title="phi (TMEF CB track); MuonEF CB Track Phi",
                                             xbins=100, xmin=-5, xmax=5) ]
        self.Histograms += [ defineHistogram('EF_cbtrks_eta', type='TH1F', title="eta (TMEF CB track); MuonEF CB Track Eta",
                                             xbins=100, xmin=-5.5, xmax=5.5) ]


class TrigMuonEFCombinerToolValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigMuonEFCombinerToolValidationMonitoring"):
        super(TrigMuonEFCombinerToolValidationMonitoring, self).__init__(name)
        self.defineTarget( ["Validation"] )

        # TMEF combined track
        self.Histograms += [ defineHistogram('EF_cbtrks_chi2', type='TH1F', title="chi2 (TMEF CB track); MuonEF CB Track chi2",
                                             xbins=100, xmin=0, xmax=30) ]
        self.Histograms += [ defineHistogram('EF_cbtrks_match_chi2', type='TH1F', title="chi2 (TMEF CB track); MuonEF CB Track matching chi2",
                                             xbins=100, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('EF_cbtrks_nMdt', type='TH1F', title="Number of Mdt (TMEF CB track); MuonEF CB Mdt hits",
                                             xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('EF_cbtrks_nRpcEta', type='TH1F', title="Number of Rpc eta (TMEF CB track); MuonEF CB Rpc Eta hits",
                                             xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('EF_cbtrks_nRpcPhi', type='TH1F', title="Number of Rpc phi (TMEF CB track); MuonEF CB Rpc Phi hits",
                                             xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('EF_cbtrks_nTgcEta', type='TH1F', title="Number of Tgc eta (TMEF CB track); MuonEF CB Tgc Eta hits",
                                             xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('EF_cbtrks_nTgcPhi', type='TH1F', title="Number of Tgc phi (TMEF CB track); MuonEF CB Tgc Phi hits",
                                             xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('EF_cbtrks_nCscEta', type='TH1F', title="Number of Csc eta (TMEF CB track); MuonEF CB Csc Eta hits",
                                             xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('EF_cbtrks_nCscPhi', type='TH1F', title="Number of Csc phi (TMEF CB track); MuonEF CB Csc Phi hits",
                                             xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('EF_cbtrks_nHit', type='TH1F', title="Number of Hit on track (TMEF CB track); MuonEF CB Total hits",
                                             xbins=150, xmin=0, xmax=150) ]
        self.Histograms += [ defineHistogram('EF_cbtrks_nIdPixelHit', type='TH1F', title="Number of IdPixelHit on track (TMEF CB track); MuonEF CB Pixel hits",
                                             xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('EF_cbtrks_nIdSctHit', type='TH1F', title="Number of IdSctHit on track (TMEF CB track); MuonEF CB Sct hits",
                                             xbins=20, xmin=0, xmax=20) ]
        self.Histograms += [ defineHistogram('EF_cbtrks_nIdTrtHit', type='TH1F', title="Number of IdTrtHit on track (TMEF CB track); MuonEF CB Trt hits",
                                             xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('EF_cbtrks_d0', type='TH1F', title="d0 (TMEF CB track); MuonEF CB Track d0",
                                             xbins=100, xmin=-10, xmax=10) ]
        self.Histograms += [ defineHistogram('EF_cbtrks_z0', type='TH1F', title="z0 (TMEF CB track); MuonEF CB Track z0 [mm]",
                                             xbins=100, xmin=-250, xmax=250) ]



class InDetTrkRoiMakerMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="InDetTrkRoiMakerMonitoring",**kwargs):
        super(InDetTrkRoiMakerMonitoring, self).__init__(name,**kwargs)
        self.defineTarget( ["Online", "Validation"] )

        self.Histograms += [ defineHistogram('nTrksIn', type='TH1F', title="Number of ID tracks in input",
                                             xbins=1001, xmin=-0.5, xmax=1000.5) ]

        self.Histograms += [ defineHistogram('nRoiOut', type='TH1F', title="Number of ROI's made from ID tracks",
                                             xbins=101, xmin=-0.5, xmax=100.5) ]

        self.Histograms += [ defineHistogram('etaTrkIn', type='TH1F', title="eta of input ID tracks",
                                             xbins=60, xmin=-3.0, xmax=3.0) ]

        self.Histograms += [ defineHistogram('phiTrkIn', type='TH1F', title="phi of input ID tracks",
                                             xbins=60, xmin=-math.pi, xmax=math.pi) ]
      

class InDetTrkRoiMakerValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="InDetTrkRoiMakerValidationMonitoring",**kwargs):
        super(InDetTrkRoiMakerValidationMonitoring, self).__init__(name,**kwargs)
        self.defineTarget( ["Validation"] )

        self.Histograms += [ defineHistogram('nRoiToTrkRatio', type='TH1F', title="fraction of ID tracks that become ROI's",
                                             xbins=100, xmin=0.0, xmax=1.0) ]


        self.Histograms += [ defineHistogram('ptTrkIn', type='TH1F', title="pT of input ID tracks [GeV]",
                                             xbins=100, xmin=0.0, xmax=100) ]


        self.Histograms += [ defineHistogram('ptRoiOut', type='TH1F', title="pT of ID tracks that make an ROI [GeV]",
                                           xbins=100, xmin=0.0, xmax=100) ]
      
      
        self.Histograms += [ defineHistogram('etaRoiOut', type='TH1F', title="eta of ID tracks that make an ROI",
                                             xbins=60, xmin=-3.0, xmax=3.0) ]

        self.Histograms += [ defineHistogram('phiRoiOut', type='TH1F', title="phi of ID tracks that make an ROI",
                                       xbins=60, xmin=-math.pi, xmax=math.pi) ]

        self.Histograms += [ defineHistogram('dEtaTrkSel', type='TH1F', title="eta difference between ID tracks that pass track cuts",
                                             xbins=120, xmin=-6.0, xmax=6.0) ]

        self.Histograms += [ defineHistogram('dPhiTrkSel', type='TH1F', title="phi difference between ID tracks that pass track cuts",
                                             xbins=60, xmin=-math.pi, xmax=math.pi) ]
        self.Histograms += [ defineHistogram('dRTrkSel', type='TH1F', title="deltaR between ID tracks that pass track cuts",
                                               xbins=100, xmin=0, xmax=1) ]

        self.Histograms += [ defineHistogram('logdRTrkSel', type='TH1F', title="log(deltaR) between ID tracks that pass track cuts",
                                               xbins=100, xmin=-3, xmax=1) ]


class TrigMuonEFFSRoiMakerMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigMuonEFFSRoiMakerMonitoring",**kwargs):
        super(TrigMuonEFFSRoiMakerMonitoring, self).__init__(name,**kwargs)
        self.defineTarget( ["Online", "Validation"] )

        self.Histograms += [ defineHistogram('nTrkIn', type='TH1F', title="Number of input muon tracks to EFFSRoiMaker;n(muons);Events",
                                             xbins=6, xmin=-0.5, xmax=5.5) ]
        self.Histograms += [ defineHistogram('nRoIOut', type='TH1F', title="Number of created RoIs;n(RoIs);Events",
                                             xbins=6, xmin=-0.5, xmax=5.5) ]

class TrigMuonEFIDTrackRoiMakerMonitoring(TrigGenericMonitoringToolConfig):
    def __init__(self, name="TrigMuonEFIDTrackRoiMakerMonitoring",**kwargs):
        super(TrigMuonEFIDTrackRoiMakerMonitoring, self).__init__(name,**kwargs)
        self.defineTarget( ["Online", "Validation"] )

        self.Histograms += [ defineHistogram('RoIz', type='TH1F', title="z0 of created RoIs;z0 [mm];RoIs",
                                             xbins=50, xmin=-250, xmax=250) ]
        self.Histograms += [ defineHistogram('DeltaEta', type='TH1F', title="difference in eta between muon and RoI;d(eta); Muons",
                                             xbins=50, xmin=0.0, xmax=0.3) ]
        self.Histograms += [ defineHistogram('DeltaPhi', type='TH1F', title="difference in phi between muon and RoI;d(phi); Muons",
                                             xbins=50, xmin=0.0, xmax=0.3) ]
