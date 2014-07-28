# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

###################################################
## RoI seeded chains monitoring
###################################################


ERROR_labels = ' noEventInfo : noTriggerInfo : noRoI : moreThanOneRoI : noMdtCont : noRpcCont : noTgcCont : noTrackColl : muDetailsFails : bPhysCollFails : vtxCollFails : convMdtFails : convRpcFails : convTgcFails : addTrackFails : calcMassFails'
ERROR_number = 16

DEBUG_labels = ' trkCollEmpty : moreTrkColl : vtxFailed '
DEBUG_number = 3

ACCEPTANCE_labels = ' input : gotRoI : gotTracks : gotTrkPair : gotOSTrkPair : passInvMass : passMuonId : passRoIAssoc : passVtx : : Each_gotTrkPair : Each_gotOSTrkPair : Each_passInvMass : Each_passMuonId : Each_passRoIAssoc : Each_passVtx'
ACCEPTANCE_number = 16

SETTINGS_labels = 'doLutTgcRdo : UseTiltedBField : doFillESD : doTagAndProbe : doOppChargeCheck : doVertexing : doRoiCheck : doTiming : retrieveRoI '
SETTINGS_number = 9

class TrigDiMuonValidationMonitoring_RoI(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigDiMuonValidationMonitoring_RoI"):
        super(TrigDiMuonValidationMonitoring_RoI, self).__init__(name)
        self.defineTarget("Validation")

####Monitoring Histograms

        self.Histograms = [ defineHistogram('NMuonBarrelMdtHits', type='TH1F', title="TrigDiMuon_RoI - Negative Muon Hit multiplicity in the MDT Barrel; MDT hits",
                                            xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('PMuonBarrelMdtHits', type='TH1F', title="TrigDiMuon_RoI - Positive Muon Hit multiplicity in the MDT Barrel; MDT hits",
                                            xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('PMuonCapMdtHits', type='TH1F', title="TrigDiMuon_RoI - Positive Muon Hit multiplicity in the MDT end-cap; MDT hits",
                                            xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('NMuonCapMdtHits', type='TH1F', title="TrigDiMuon_RoI - Negative Muon Hit multiplicity in the MDT end-cap; MDT hits",
                                            xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('PMuonTgcHits', type='TH1F', title="TrigDiMuon_RoI - Positive Muon Hit multiplicity in the TGC; TGC hits",
                                            xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('PMuonRpcHits', type='TH1F', title="TrigDiMuon_RoI - Positive Muon Hit multiplicity in the RPC; RPC hits",
                                            xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('NMuonTgcHits', type='TH1F', title="TrigDiMuon_RoI - Negative Muon Hit multiplicity in the TGC; TGC hits",
                                            xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('NMuonRpcHits', type='TH1F', title="TrigDiMuon_RoI - Negative Muon Hit multiplicity in the RPC; RPC hits",
                                            xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('JPsiMass', type='TH1F', title="TrigDiMuon_RoI - Mass of J/psi; Mass (MeV)",
                                            xbins=100, xmin=0, xmax=6000) ]
        self.Histograms += [ defineHistogram('JPsiMassWideRange', type='TH1F', title="TrigDiMuon_RoI - Mass of J/psi; Mass (MeV)",
                                            xbins=100, xmin=0, xmax=100000) ]
        self.Histograms += [ defineHistogram('JPsiEta', type='TH1F', title="TrigDiMuon_RoI - Eta of J/psi; Eta",
                                            xbins=30, xmin=-2.5, xmax=2.5) ]
        self.Histograms += [ defineHistogram('JPsiPhi', type='TH1F', title="TrigDiMuon_RoI - Phi of J/psi; Phi",
                                            xbins=30, xmin=-3.5, xmax=3.5) ]
        self.Histograms += [ defineHistogram('JPsiPt', type='TH1F', title="TrigDiMuon_RoI - PT of J/psi; PT (MeV)",
                                            xbins=100, xmin=0, xmax=50000) ]
        self.Histograms += [ defineHistogram('JPsiPtWideRange', type='TH1F', title="TrigDiMuon_RoI - PT of J/psi; PT (MeV)",
                                            xbins=100, xmin=0, xmax=150000) ]
        self.Histograms += [ defineHistogram('JPsiChi2', type='TH1F', title="TrigDiMuon_RoI - Chi2 of J/psi Vertex fit; Chi2",
                                            xbins=100, xmin=0, xmax=20) ]
        self.Histograms += [ defineHistogram('JPsiPerRoi', type='TH1F', title="TrigDiMuon_RoI - Number of J/psi per ROI; J/psi per ROI",
                                            xbins=20, xmin=0, xmax=20) ]
        self.Histograms += [ defineHistogram('JPsiLifeTime', type='TH1F', title="TrigDiMuon_RoI - J/psi Deacy distance; 1/(MeV)",
                                            xbins=100, xmin=0, xmax=100) ]

        self.Histograms += [ defineHistogram('PMuonEta', type='TH1F', title="TrigDiMuon_RoI - Eta of Positive Muon; Eta",
                                            xbins=30, xmin=-2.5, xmax=2.5) ]
        self.Histograms += [ defineHistogram('PMuonPhi', type='TH1F', title="TrigDiMuon_RoI - Phi of Positive Muon; Phi",
                                            xbins=30, xmin=-3.5, xmax=3.5) ]
        self.Histograms += [ defineHistogram('PMuonPt', type='TH1F', title="TrigDiMuon_RoI - PT of Positive Muon; PT (MeV)",
                                            xbins=100, xmin=0, xmax=100000) ]
        self.Histograms += [ defineHistogram('NMuonEta', type='TH1F', title="TrigDiMuon_RoI - Eta of Negative Muon; Eta",
                                            xbins=30, xmin=-2.5, xmax=2.5) ]
        self.Histograms += [ defineHistogram('NMuonPhi', type='TH1F', title="TrigDiMuon_RoI - Phi of Negative Muon; Phi",
                                            xbins=30, xmin=-3.5, xmax=3.5) ]
        self.Histograms += [ defineHistogram('NMuonPt', type='TH1F', title="TrigDiMuon_RoI - PT of Negative Muon; PT (MeV)",
                                            xbins=100, xmin=0, xmax=100000) ]

        self.Histograms += [ defineHistogram ( 'PMuonEta, PMuonPhi', type = 'TH2F',
                                               title = 'TrigDiMuon_RoI - positive muon direction; #eta ; #phi ; # tracks',
                                               xbins = 60, xmin = -3.0, xmax = 3.0,
                                               ybins = 64, ymin = -3.2, ymax = 3.2 ) ]

        self.Histograms += [ defineHistogram ( 'NMuonEta, NMuonPhi', type = 'TH2F',
                                               title = 'TrigDiMuon_RoI - negative muon direction; #eta ; #phi ; # tracks',
                                               xbins = 60, xmin = -3.0, xmax = 3.0,
                                               ybins = 64, ymin = -3.2, ymax = 3.2 ) ]


        self.Histograms += [ defineHistogram('roiNoHits_pt', type='TH1F', title="TrigDiMuon_RoI - RoI track with no hits; PT (MeV)",
                                            xbins=100, xmin=0, xmax=100000) ]
        self.Histograms += [ defineHistogram('roiNoHits_charge', type='TH1F', title="TrigDiMuon_RoI - RoI track with no hits; charge",
                                            xbins=5, xmin=-2, xmax=3) ]
        self.Histograms += [ defineHistogram('roiNoHits_eta', type='TH1F', title="TrigDiMuon_RoI - RoI track with no hits; Eta",
                                            xbins=30, xmin=-2.5, xmax=2.5) ]
        self.Histograms += [ defineHistogram('roiNoHits_phi', type='TH1F', title="TrigDiMuon_RoI - RoI track with no hits; Phi",
                                            xbins=30, xmin=-3.5, xmax=3.5) ]



        self.Histograms += [ defineHistogram('RoiEndcap_dEta', type='TH1F', title="TrigDiMuon_RoI - RoI - extrapolated track (Endcap); #Delta#eta",
                                            xbins=100, xmin=0, xmax=0.75) ]
        self.Histograms += [ defineHistogram('RoiEndcap_dPhi', type='TH1F', title="TrigDiMuon_RoI - RoI - extrapolated track (Endcap); #Delta#phi",
                                            xbins=100, xmin=0, xmax=0.75) ]
        self.Histograms += [ defineHistogram('RoiEndcap_dR', type='TH1F', title="TrigDiMuon_RoI - RoI - extrapolated track (Endcap); #DeltaR",
                                            xbins=100, xmin=0, xmax=0.75) ]
        self.Histograms += [ defineHistogram ( 'RoiEndcap_dEta, RoiEndcap_dPhi', type = 'TH2F',
                                               title = 'TrigDiMuon_RoI - RoI - extrapolated track (Endcap); #Delta#eta ; #Delta#phi ; # tracks',
                                               xbins = 100, xmin = 0, xmax = 0.75,
                                               ybins = 100, ymin = 0, ymax = 0.75 ) ]

        self.Histograms += [ defineHistogram('RoiBarrel_dEta', type='TH1F', title="TrigDiMuon_RoI - RoI - extrapolated track (Barrel); #Delta#eta",
                                            xbins=100, xmin=0, xmax=0.75) ]
        self.Histograms += [ defineHistogram('RoiBarrel_dPhi', type='TH1F', title="TrigDiMuon_RoI - RoI - extrapolated track (Barrel); #Delta#phi",
                                            xbins=100, xmin=0, xmax=0.75) ]
        self.Histograms += [ defineHistogram('RoiBarrel_dR', type='TH1F', title="TrigDiMuon_RoI - RoI - extrapolated track (Barrel); #DeltaR",
                                            xbins=100, xmin=0, xmax=0.75) ]
        self.Histograms += [ defineHistogram ( 'RoiBarrel_dEta, RoiBarrel_dPhi', type = 'TH2F',
                                               title = 'TrigDiMuon_RoI - RoI - extrapolated track (Barrel); #Delta#eta ; #Delta#phi ; # tracks',
                                               xbins = 100, xmin = 0, xmax = 0.75,
                                               ybins = 100, ymin = 0, ymax = 0.75 ) ]

        self.Histograms += [ defineHistogram('RoiEndcapNoEx_dEta', type='TH1F', title="TrigDiMuon_RoI - RoI - track (Endcap); #Delta#eta",
                                            xbins=100, xmin=0, xmax=0.75) ]
        self.Histograms += [ defineHistogram('RoiEndcapNoEx_dPhi', type='TH1F', title="TrigDiMuon_RoI - RoI - track (Endcap); #Delta#phi",
                                            xbins=100, xmin=0, xmax=0.75) ]
        self.Histograms += [ defineHistogram('RoiEndcapNoEx_dR', type='TH1F', title="TrigDiMuon_RoI - RoI - track (Endcap); #DeltaR",
                                            xbins=100, xmin=0, xmax=0.75) ]
        self.Histograms += [ defineHistogram ( 'RoiEndcapNoEx_dEta, RoiEndcapNoEx_dPhi', type = 'TH2F',
                                               title = 'TrigDiMuon_RoI - RoI - track (Endcap); #Delta#eta ; #Delta#phi ; # tracks',
                                               xbins = 100, xmin = 0, xmax = 0.75,
                                               ybins = 100, ymin = 0, ymax = 0.75 ) ]

        self.Histograms += [ defineHistogram('RoiBarrelNoEx_dEta', type='TH1F', title="TrigDiMuon_RoI - RoI - track (Barrel); #Delta#eta",
                                            xbins=100, xmin=0, xmax=0.75) ]
        self.Histograms += [ defineHistogram('RoiBarrelNoEx_dPhi', type='TH1F', title="TrigDiMuon_RoI - RoI - track (Barrel); #Delta#phi",
                                            xbins=100, xmin=0, xmax=0.75) ]
        self.Histograms += [ defineHistogram('RoiBarrelNoEx_dR', type='TH1F', title="TrigDiMuon_RoI - RoI - track (Barrel); #DeltaR",
                                            xbins=100, xmin=0, xmax=0.75) ]
        self.Histograms += [ defineHistogram ( 'RoiBarrelNoEx_dEta, RoiBarrelNoEx_dPhi', type = 'TH2F',
                                               title = 'TrigDiMuon_RoI - RoI - track (Barrel); #Delta#eta ; #Delta#phi ; # tracks',
                                               xbins = 100, xmin = 0, xmax = 0.75,
                                               ybins = 100, ymin = 0, ymax = 0.75 ) ]



        self.Histograms += [ defineHistogram('TagAndProbe_trTr', type='TH1F', title="TrigDiMuon_RoI - T&P: all tracks ; Mass (MeV)",
                                            xbins=100, xmin=0, xmax=6000) ]
        self.Histograms += [ defineHistogram('TagAndProbe_trTr_OS', type='TH1F', title="TrigDiMuon_RoI - T&P: opposite-sign tracks ; Mass (MeV)",
                                            xbins=100, xmin=0, xmax=6000) ]
        self.Histograms += [ defineHistogram('TagAndProbe_trRoI', type='TH1F', title="TrigDiMuon_RoI - T&P: RoI - track; Mass (MeV)",
                                            xbins=100, xmin=0, xmax=6000) ]
        self.Histograms += [ defineHistogram('TagAndProbe_muRoI', type='TH1F', title="TrigDiMuon_RoI - T&P: RoI - track with hits ; Mass (MeV)",
                                            xbins=100, xmin=0, xmax=6000) ]
        self.Histograms += [ defineHistogram('TagAndProbe_muMuRoI', type='TH1F', title="TrigDiMuon_RoI - T&P: RoI w/ hits - track w/ hits ; Mass (MeV)",
                                            xbins=100, xmin=0, xmax=6000) ]

        self.Histograms += [ defineHistogram('Counter_trTr', type='TH1F', title="TrigDiMuon_RoI -  # of di-track candidates; number of candidates per RoI",
                                            xbins=20, xmin=0, xmax=20) ]
        self.Histograms += [ defineHistogram('Counter_muMu', type='TH1F', title="TrigDiMuon_RoI -  # of di-muon candidates; number of candidates per RoI",
                                            xbins=20, xmin=0, xmax=20) ]
        self.Histograms += [ defineHistogram('Counter_muMuRoI', type='TH1F', title="TrigDiMuon_RoI -  # of di-muon candidates after RoI confirmation; number of candidates per RoI",
                                            xbins=20, xmin=0, xmax=20) ]
        self.Histograms += [ defineHistogram('Counter_muMuVtx', type='TH1F', title="TrigDiMuon_RoI -  # of di-muon candidates after vertexing; number of candidates per RoI",
                                            xbins=20, xmin=0, xmax=20) ]
        self.Histograms += [ defineHistogram('Counter_muMuRoIVtx', type='TH1F', title="TrigDiMuon_RoI -  # of di-muon candidates after RoI confirmation and vertexing; number of candidates per RoI",
                                            xbins=20, xmin=0, xmax=20) ]

        self.Histograms += [ defineHistogram('Counter_allTrk', type='TH1F', title="TrigDiMuon_RoI -  # of all tracks; # tracks per RoI",
                                            xbins=100, xmin=0, xmax=100) ]
        self.Histograms += [ defineHistogram('Counter_accTrk', type='TH1F', title="TrigDiMuon_RoI -  # of pre-selected tracks; # tracks per RoI",
                                            xbins=100, xmin=0, xmax=100) ]



        self.Histograms += [ defineHistogram('Hit_Endcap_mdt_inner_dEta' , type='TH1F',
                                             title="TrigDiMuon_RoI - Hits - extrapolated track (MDT Inner Endcap); #Delta#eta",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]
        self.Histograms += [ defineHistogram('Hit_Endcap_mdt_middle_dEta', type='TH1F',
                                             title="TrigDiMuon_RoI - Hits - extrapolated track (MDT Middle Endcap); #Delta#eta",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]
        self.Histograms += [ defineHistogram('Hit_Barrel_mdt_inner_dEta' , type='TH1F',
                                             title="TrigDiMuon_RoI - Hits - extrapolated track (MDT Inner Barrel); #Delta#eta",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]
        self.Histograms += [ defineHistogram('Hit_Barrel_mdt_middle_dEta', type='TH1F',
                                             title="TrigDiMuon_RoI - Hits - extrapolated track (MDT Middle Barrel); #Delta#eta",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]

        self.Histograms += [ defineHistogram('Hit_rpc_middle_dEta'       ,type='TH1F',
                                             title="TrigDiMuon_RoI - Hits - extrapolated track (RPC Middle); #Delta#eta",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]
        self.Histograms += [ defineHistogram('Hit_rpc_middle_dPhi'       ,type='TH1F',
                                             title="TrigDiMuon_RoI - Hits - extrapolated track (RPC Middle); #Delta#phi",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]

        self.Histograms += [ defineHistogram('Hit_tgc_wire_inner_dEta'   ,type='TH1F',
                                             title="TrigDiMuon_RoI - Hits - extrapolated track (TGC Wire Inner); #Delta#eta",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]
        self.Histograms += [ defineHistogram('Hit_tgc_wire_middle_dEta'  , type='TH1F',
                                             title="TrigDiMuon_RoI - Hits - extrapolated track (TGC Wire Middle); #Delta#eta",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]
        self.Histograms += [ defineHistogram('Hit_tgc_wire_inner_dPhi'   ,type='TH1F',
                                             title="TrigDiMuon_RoI - Hits - extrapolated track (TGC Wire Inner); #Delta#phi",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]
        self.Histograms += [ defineHistogram('Hit_tgc_wire_middle_dPhi'  , type='TH1F',
                                             title="TrigDiMuon_RoI - Hits - extrapolated track (TGC Wire Middle); #Delta#phi",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]

        self.Histograms += [ defineHistogram('Hit_tgc_strip_inner_dPhi'  , type='TH1F',
                                             title="TrigDiMuon_RoI - Hits - extrapolated track (TGC Strip Inner); #Delta#phi",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]
        self.Histograms += [ defineHistogram('Hit_tgc_strip_middle_dPhi' , type='TH1F',
                                             title="TrigDiMuon_RoI - Hits - extrapolated track (TGC Strip Middle); #Delta#phi",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]


        self.Histograms += [ defineHistogram ( 'Roi_eta, Roi_phi', type = 'TH2F',
                                               title = 'TrigDiMuon_RoI - ROI position ; #eta ; #phi ; # Events',
                                               xbins = 60, xmin = -3.0, xmax = 3.0,
                                               ybins = 64, ymin = -3.2, ymax = 3.2 ) ]


        self.Histograms += [ defineHistogram('vtx_mass'    , type='TH1F', title="TrigDiMuon_RoI - Mass from vertex fit; M (MeV)", xbins=100, xmin=0, xmax=6000) ]
        self.Histograms += [ defineHistogram('vtx_massWideRange', type='TH1F', title="TrigDiMuon_RoI - Mass from vertex fit; M (MeV)", xbins=100, xmin=0, xmax=100000) ]
        self.Histograms += [ defineHistogram('vtx_x'       , type='TH1F', title="TrigDiMuon_RoI - Vertex ; x (mm)", xbins=100, xmin=-10, xmax=10) ]
        self.Histograms += [ defineHistogram('vtx_y'       , type='TH1F', title="TrigDiMuon_RoI - Vertex ; y (mm)", xbins=100, xmin=-10, xmax=10) ]
        self.Histograms += [ defineHistogram('vtx_z'       , type='TH1F', title="TrigDiMuon_RoI - Vertex ; z (mm)", xbins=100, xmin=-100, xmax=100) ]



        self.Histograms += [ defineHistogram ( 'Errors', type = 'TH1F',
                                                title = 'TrigDiMuon_RoI - Algorithm errors ; ; # candidates',
                                                xbins = ERROR_number , xmin = 0.5, xmax = ERROR_number + 0.5,
                                                labels = ERROR_labels ) ]
        self.Histograms += [ defineHistogram ( 'Debug', type = 'TH1F',
                                                title = 'TrigDiMuon_RoI - Debug ; ; # candidates',
                                                xbins = DEBUG_number , xmin = 0.5, xmax = DEBUG_number + 0.5,
                                                labels = DEBUG_labels ) ]
        self.Histograms += [ defineHistogram ( 'Acceptance', type = 'TH1F',
                                                title = 'TrigDiMuon_RoI - Reached steps - acceptance ; ; # input RoIs',
                                                xbins = ACCEPTANCE_number , xmin = 0.5, xmax = ACCEPTANCE_number + 0.5,
                                                labels = ACCEPTANCE_labels ) ]
        self.Histograms += [ defineHistogram ( 'Settings', type = 'TH1F',
                                                title = 'TrigDiMuon_RoI - algorithm settings flags',
                                                xbins = SETTINGS_number , xmin = 0.5, xmax = SETTINGS_number + 0.5,
                                                labels = SETTINGS_labels ) ]

class TrigDiMuonOnlineMonitoring_RoI(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigDiMuonOnlineMonitoring_RoI"):
        super(TrigDiMuonOnlineMonitoring_RoI, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms = [ defineHistogram('NMuonBarrelMdtHits', type='TH1F', title="TrigDiMuon_RoI - Negative Muon Hit multiplicity in the MDT Barrel; MDT hits",
                                            xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('PMuonBarrelMdtHits', type='TH1F', title="TrigDiMuon_RoI - Positive Muon Hit multiplicity in the MDT Barrel; MDT hits",
                                            xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('PMuonCapMdtHits', type='TH1F', title="TrigDiMuon_RoI - Positive Muon Hit multiplicity in the MDT end-cap; MDT hits",
                                            xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('NMuonCapMdtHits', type='TH1F', title="TrigDiMuon_RoI - Negative Muon Hit multiplicity in the MDT end-cap; MDT hits",
                                            xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('PMuonTgcHits', type='TH1F', title="TrigDiMuon_RoI - Positive Muon Hit multiplicity in the TGC; TGC hits",
                                            xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('PMuonRpcHits', type='TH1F', title="TrigDiMuon_RoI - Positive Muon Hit multiplicity in the RPC; RPC hits",
                                            xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('NMuonTgcHits', type='TH1F', title="TrigDiMuon_RoI - Negative Muon Hit multiplicity in the TGC; TGC hits",
                                            xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('NMuonRpcHits', type='TH1F', title="TrigDiMuon_RoI - Negative Muon Hit multiplicity in the RPC; RPC hits",
                                            xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('JPsiMass', type='TH1F', title="TrigDiMuon_RoI - Mass of J/psi; Mass (MeV)",
                                            xbins=100, xmin=0, xmax=6000) ]
        self.Histograms += [ defineHistogram('JPsiMassWideRange', type='TH1F', title="TrigDiMuon_RoI - Mass of J/psi; Mass (MeV)",
                                            xbins=100, xmin=0, xmax=100000) ]
        self.Histograms += [ defineHistogram('JPsiEta', type='TH1F', title="TrigDiMuon_RoI - Eta of J/psi; Eta",
                                            xbins=30, xmin=-2.5, xmax=2.5) ]
        self.Histograms += [ defineHistogram('JPsiPhi', type='TH1F', title="TrigDiMuon_RoI - Phi of J/psi; Phi",
                                            xbins=30, xmin=-3.5, xmax=3.5) ]
        self.Histograms += [ defineHistogram('JPsiPt', type='TH1F', title="TrigDiMuon_RoI - PT of J/psi; PT (MeV)",
                                            xbins=100, xmin=0, xmax=50000) ]
        self.Histograms += [ defineHistogram('JPsiPtWideRange', type='TH1F', title="TrigDiMuon_RoI - PT of J/psi; PT (MeV)",
                                            xbins=100, xmin=0, xmax=150000) ]
        self.Histograms += [ defineHistogram('JPsiChi2', type='TH1F', title="TrigDiMuon_RoI - Chi2 of J/psi Vertex fit; Chi2",
                                            xbins=100, xmin=0, xmax=20) ]
        self.Histograms += [ defineHistogram('JPsiPerRoi', type='TH1F', title="TrigDiMuon_RoI - Number of J/psi per ROI; J/psi per ROI",
                                            xbins=20, xmin=0, xmax=20) ]
        self.Histograms += [ defineHistogram('JPsiLifeTime', type='TH1F', title="TrigDiMuon_RoI - J/psi Deacy distance; 1/(MeV)",
                                            xbins=100, xmin=0, xmax=100) ]
        self.Histograms += [ defineHistogram('PMuonEta', type='TH1F', title="TrigDiMuon_RoI - Eta of Positive Muon; Eta",
                                            xbins=30, xmin=-2.5, xmax=2.5) ]
        self.Histograms += [ defineHistogram('PMuonPhi', type='TH1F', title="TrigDiMuon_RoI - Phi of Positive Muon; Phi",
                                            xbins=30, xmin=-3.5, xmax=3.5) ]
        self.Histograms += [ defineHistogram('PMuonPt', type='TH1F', title="TrigDiMuon_RoI - PT of Positive Muon; PT (MeV)",
                                            xbins=100, xmin=0, xmax=100000) ]
        self.Histograms += [ defineHistogram('NMuonEta', type='TH1F', title="TrigDiMuon_RoI - Eta of Negative Muon; Eta",
                                            xbins=30, xmin=-2.5, xmax=2.5) ]
        self.Histograms += [ defineHistogram('NMuonPhi', type='TH1F', title="TrigDiMuon_RoI - Phi of Negative Muon; Phi",
                                            xbins=30, xmin=-3.5, xmax=3.5) ]
        self.Histograms += [ defineHistogram('NMuonPt', type='TH1F', title="TrigDiMuon_RoI - PT of Negative Muon; PT (MeV)",
                                            xbins=100, xmin=0, xmax=100000) ]

        self.Histograms += [ defineHistogram ( 'PMuonEta, PMuonPhi', type = 'TH2F',
                                               title = 'TrigDiMuon_RoI - positive muon direction; #eta ; #phi ; # tracks',
                                               xbins = 60, xmin = -3.0, xmax = 3.0,
                                               ybins = 64, ymin = -3.2, ymax = 3.2 ) ]

        self.Histograms += [ defineHistogram ( 'NMuonEta, NMuonPhi', type = 'TH2F',
                                               title = 'TrigDiMuon_RoI - negative muon direction; #eta ; #phi ; # tracks',
                                               xbins = 60, xmin = -3.0, xmax = 3.0,
                                               ybins = 64, ymin = -3.2, ymax = 3.2 ) ]

        self.Histograms += [ defineHistogram('roiNoHits_pt', type='TH1F', title="TrigDiMuon_RoI - RoI track with no hits; PT (MeV)",
                                            xbins=100, xmin=0, xmax=100000) ]
        self.Histograms += [ defineHistogram('roiNoHits_charge', type='TH1F', title="TrigDiMuon_RoI - RoI track with no hits; charge",
                                            xbins=5, xmin=-2, xmax=3) ]
        self.Histograms += [ defineHistogram('roiNoHits_eta', type='TH1F', title="TrigDiMuon_RoI - RoI track with no hits; Eta",
                                            xbins=30, xmin=-2.5, xmax=2.5) ]
        self.Histograms += [ defineHistogram('roiNoHits_phi', type='TH1F', title="TrigDiMuon_RoI - RoI track with no hits; Phi",
                                            xbins=30, xmin=-3.5, xmax=3.5) ]



        self.Histograms += [ defineHistogram('RoiEndcap_dEta', type='TH1F', title="TrigDiMuon_RoI - RoI - extrapolated track (Endcap); #Delta#eta",
                                            xbins=100, xmin=0, xmax=0.75) ]
        self.Histograms += [ defineHistogram('RoiEndcap_dPhi', type='TH1F', title="TrigDiMuon_RoI - RoI - extrapolated track (Endcap); #Delta#phi",
                                            xbins=100, xmin=0, xmax=0.75) ]
        self.Histograms += [ defineHistogram('RoiEndcap_dR', type='TH1F', title="TrigDiMuon_RoI - RoI - extrapolated track (Endcap); #DeltaR",
                                            xbins=100, xmin=0, xmax=0.75) ]
        self.Histograms += [ defineHistogram ( 'RoiEndcap_dEta, RoiEndcap_dPhi', type = 'TH2F',
                                               title = 'TrigDiMuon_RoI - RoI - extrapolated track (Endcap); #Delta#eta ; #Delta#phi ; # tracks',
                                               xbins = 100, xmin = 0, xmax = 0.75,
                                               ybins = 100, ymin = 0, ymax = 0.75 ) ]

        self.Histograms += [ defineHistogram('RoiBarrel_dEta', type='TH1F', title="TrigDiMuon_RoI - RoI - extrapolated track (Barrel); #Delta#eta",
                                            xbins=100, xmin=0, xmax=0.75) ]
        self.Histograms += [ defineHistogram('RoiBarrel_dPhi', type='TH1F', title="TrigDiMuon_RoI - RoI - extrapolated track (Barrel); #Delta#phi",
                                            xbins=100, xmin=0, xmax=0.75) ]
        self.Histograms += [ defineHistogram('RoiBarrel_dR', type='TH1F', title="TrigDiMuon_RoI - RoI - extrapolated track (Barrel); #DeltaR",
                                            xbins=100, xmin=0, xmax=0.75) ]
        self.Histograms += [ defineHistogram ( 'RoiBarrel_dEta, RoiBarrel_dPhi', type = 'TH2F',
                                               title = 'TrigDiMuon_RoI - RoI - extrapolated track (Barrel); #Delta#eta ; #Delta#phi ; # tracks',
                                               xbins = 100, xmin = 0, xmax = 0.75,
                                               ybins = 100, ymin = 0, ymax = 0.75 ) ]

        self.Histograms += [ defineHistogram('RoiEndcapNoEx_dEta', type='TH1F', title="TrigDiMuon_RoI - RoI - track (Endcap); #Delta#eta",
                                            xbins=100, xmin=0, xmax=0.75) ]
        self.Histograms += [ defineHistogram('RoiEndcapNoEx_dPhi', type='TH1F', title="TrigDiMuon_RoI - RoI - track (Endcap); #Delta#phi",
                                            xbins=100, xmin=0, xmax=0.75) ]
        self.Histograms += [ defineHistogram('RoiEndcapNoEx_dR', type='TH1F', title="TrigDiMuon_RoI - RoI - track (Endcap); #DeltaR",
                                            xbins=100, xmin=0, xmax=0.75) ]
        self.Histograms += [ defineHistogram ( 'RoiEndcapNoEx_dEta, RoiEndcapNoEx_dPhi', type = 'TH2F',
                                               title = 'TrigDiMuon_RoI - RoI - track (Endcap); #Delta#eta ; #Delta#phi ; # tracks',
                                               xbins = 100, xmin = 0, xmax = 0.75,
                                               ybins = 100, ymin = 0, ymax = 0.75 ) ]

        self.Histograms += [ defineHistogram('RoiBarrelNoEx_dEta', type='TH1F', title="TrigDiMuon_RoI - RoI - track (Barrel); #Delta#eta",
                                            xbins=100, xmin=0, xmax=0.75) ]
        self.Histograms += [ defineHistogram('RoiBarrelNoEx_dPhi', type='TH1F', title="TrigDiMuon_RoI - RoI - track (Barrel); #Delta#phi",
                                            xbins=100, xmin=0, xmax=0.75) ]
        self.Histograms += [ defineHistogram('RoiBarrelNoEx_dR', type='TH1F', title="TrigDiMuon_RoI - RoI - track (Barrel); #DeltaR",
                                            xbins=100, xmin=0, xmax=0.75) ]
        self.Histograms += [ defineHistogram ( 'RoiBarrelNoEx_dEta, RoiBarrelNoEx_dPhi', type = 'TH2F',
                                               title = 'TrigDiMuon_RoI - RoI - track (Barrel); #Delta#eta ; #Delta#phi ; # tracks',
                                               xbins = 100, xmin = 0, xmax = 0.75,
                                               ybins = 100, ymin = 0, ymax = 0.75 ) ]



        self.Histograms += [ defineHistogram('TagAndProbe_trTr', type='TH1F', title="TrigDiMuon_RoI - T&P: all tracks ; Mass (MeV)",
                                            xbins=100, xmin=0, xmax=6000) ]
        self.Histograms += [ defineHistogram('TagAndProbe_trTr_OS', type='TH1F', title="TrigDiMuon_RoI - T&P: opposite-sign tracks ; Mass (MeV)",
                                            xbins=100, xmin=0, xmax=6000) ]
        self.Histograms += [ defineHistogram('TagAndProbe_trRoI', type='TH1F', title="TrigDiMuon_RoI - T&P: RoI - track; Mass (MeV)",
                                            xbins=100, xmin=0, xmax=6000) ]
        self.Histograms += [ defineHistogram('TagAndProbe_muRoI', type='TH1F', title="TrigDiMuon_RoI - T&P: RoI - track with hits ; Mass (MeV)",
                                            xbins=100, xmin=0, xmax=6000) ]
        self.Histograms += [ defineHistogram('TagAndProbe_muMuRoI', type='TH1F', title="TrigDiMuon_RoI - T&P: RoI w/ hits - track w/ hits ; Mass (MeV)",
                                            xbins=100, xmin=0, xmax=6000) ]

        self.Histograms += [ defineHistogram('Counter_trTr', type='TH1F', title="TrigDiMuon_RoI -  # of di-track candidates; number of candidates per RoI",
                                            xbins=20, xmin=0, xmax=20) ]
        self.Histograms += [ defineHistogram('Counter_muMu', type='TH1F', title="TrigDiMuon_RoI -  # of di-muon candidates; number of candidates per RoI",
                                            xbins=20, xmin=0, xmax=20) ]
        self.Histograms += [ defineHistogram('Counter_muMuRoI', type='TH1F', title="TrigDiMuon_RoI -  # of di-muon candidates after RoI confirmation; number of candidates per RoI",
                                            xbins=20, xmin=0, xmax=20) ]
        self.Histograms += [ defineHistogram('Counter_muMuVtx', type='TH1F', title="TrigDiMuon_RoI -  # of di-muon candidates after vertexing; number of candidates per RoI",
                                            xbins=20, xmin=0, xmax=20) ]
        self.Histograms += [ defineHistogram('Counter_muMuRoIVtx', type='TH1F', title="TrigDiMuon_RoI -  # of di-muon candidates after RoI confirmation and vertexing; number of candidates per RoI",
                                            xbins=20, xmin=0, xmax=20) ]

        self.Histograms += [ defineHistogram('Counter_allTrk', type='TH1F', title="TrigDiMuon_RoI -  # of all tracks; # tracks per RoI",
                                            xbins=100, xmin=0, xmax=100) ]
        self.Histograms += [ defineHistogram('Counter_accTrk', type='TH1F', title="TrigDiMuon_RoI -  # of pre-selected tracks; # tracks per RoI",
                                            xbins=100, xmin=0, xmax=100) ]



        self.Histograms += [ defineHistogram('Hit_Endcap_mdt_inner_dEta' , type='TH1F',
                                             title="TrigDiMuon_RoI - Hits - extrapolated track (MDT Inner Endcap); #Delta#eta",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]
        self.Histograms += [ defineHistogram('Hit_Endcap_mdt_middle_dEta', type='TH1F',
                                             title="TrigDiMuon_RoI - Hits - extrapolated track (MDT Middle Endcap); #Delta#eta",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]
        self.Histograms += [ defineHistogram('Hit_Barrel_mdt_inner_dEta' , type='TH1F',
                                             title="TrigDiMuon_RoI - Hits - extrapolated track (MDT Inner Barrel); #Delta#eta",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]
        self.Histograms += [ defineHistogram('Hit_Barrel_mdt_middle_dEta', type='TH1F',
                                             title="TrigDiMuon_RoI - Hits - extrapolated track (MDT Middle Barrel); #Delta#eta",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]

        self.Histograms += [ defineHistogram('Hit_rpc_middle_dEta'       ,type='TH1F',
                                             title="TrigDiMuon_RoI - Hits - extrapolated track (RPC Middle); #Delta#eta",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]
        self.Histograms += [ defineHistogram('Hit_rpc_middle_dPhi'       ,type='TH1F',
                                             title="TrigDiMuon_RoI - Hits - extrapolated track (RPC Middle); #Delta#phi",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]

        self.Histograms += [ defineHistogram('Hit_tgc_wire_inner_dEta'   ,type='TH1F',
                                             title="TrigDiMuon_RoI - Hits - extrapolated track (TGC Wire Inner); #Delta#eta",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]
        self.Histograms += [ defineHistogram('Hit_tgc_wire_middle_dEta'  , type='TH1F',
                                             title="TrigDiMuon_RoI - Hits - extrapolated track (TGC Wire Middle); #Delta#eta",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]
        self.Histograms += [ defineHistogram('Hit_tgc_wire_inner_dPhi'   ,type='TH1F',
                                             title="TrigDiMuon_RoI - Hits - extrapolated track (TGC Wire Inner); #Delta#phi",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]
        self.Histograms += [ defineHistogram('Hit_tgc_wire_middle_dPhi'  , type='TH1F',
                                             title="TrigDiMuon_RoI - Hits - extrapolated track (TGC Wire Middle); #Delta#phi",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]

        self.Histograms += [ defineHistogram('Hit_tgc_strip_inner_dPhi'  , type='TH1F',
                                             title="TrigDiMuon_RoI - Hits - extrapolated track (TGC Strip Inner); #Delta#phi",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]
        self.Histograms += [ defineHistogram('Hit_tgc_strip_middle_dPhi' , type='TH1F',
                                             title="TrigDiMuon_RoI - Hits - extrapolated track (TGC Strip Middle); #Delta#phi",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]


        self.Histograms += [ defineHistogram('vtx_mass'    , type='TH1F', title="TrigDiMuon_RoI - Mass from vertex fit; M (MeV)", xbins=100, xmin=0, xmax=6000) ]
        self.Histograms += [ defineHistogram('vtx_massWideRange', type='TH1F', title="TrigDiMuon_RoI - Mass from vertex fit; M (MeV)", xbins=100, xmin=0, xmax=100000) ]
        self.Histograms += [ defineHistogram('vtx_x'       , type='TH1F', title="TrigDiMuon_RoI - Vertex ; x (mm)", xbins=100, xmin=-10, xmax=10) ]
        self.Histograms += [ defineHistogram('vtx_y'       , type='TH1F', title="TrigDiMuon_RoI - Vertex ; y (mm)", xbins=100, xmin=-10, xmax=10) ]
        self.Histograms += [ defineHistogram('vtx_z'       , type='TH1F', title="TrigDiMuon_RoI - Vertex ; z (mm)", xbins=100, xmin=-100, xmax=100) ]



        self.Histograms += [ defineHistogram ( 'Roi_eta, Roi_phi', type = 'TH2F',
                                               title = 'TrigDiMuon_RoI - ROI position ; #eta ; #phi ; # Events',
                                               xbins = 60, xmin = -3.0, xmax = 3.0,
                                               ybins = 64, ymin = -3.2, ymax = 3.2 ) ]


        self.Histograms += [ defineHistogram ( 'Errors', type = 'TH1F',
                                                title = 'TrigDiMuon_RoI - Algorithm errors ; ; # candidates',
                                                xbins = ERROR_number , xmin = 0.5, xmax = ERROR_number + 0.5,
                                                labels = ERROR_labels ) ]
        self.Histograms += [ defineHistogram ( 'Debug', type = 'TH1F',
                                                title = 'TrigDiMuon_RoI - Debug ; ; # candidates',
                                                xbins = DEBUG_number , xmin = 0.5, xmax = DEBUG_number + 0.5,
                                                labels = DEBUG_labels ) ]
        self.Histograms += [ defineHistogram ( 'Acceptance', type = 'TH1F',
                                                title = 'TrigDiMuon_RoI - Reached steps - acceptance ; ; # input RoIs',
                                                xbins = ACCEPTANCE_number , xmin = 0.5, xmax = ACCEPTANCE_number + 0.5,
                                                labels = ACCEPTANCE_labels ) ]
        self.Histograms += [ defineHistogram ( 'Settings', type = 'TH1F',
                                                title = 'TrigDiMuon_RoI - algorithm settings flags',
                                                xbins = SETTINGS_number , xmin = 0.5, xmax = SETTINGS_number + 0.5,
                                                labels = SETTINGS_labels ) ]


###################################################
## Full scan chains monitoring
###################################################

class TrigDiMuonValidationMonitoring_FS(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigDiMuonValidationMonitoring_FS"):
        super(TrigDiMuonValidationMonitoring_FS, self).__init__(name)
        self.defineTarget("Validation")

####Monitoring Histograms


        self.Histograms = [ defineHistogram('NMuonBarrelMdtHits', type='TH1F', title="TrigDiMuon_FS - Negative Muon Hit multiplicity in the MDT Barrel; MDT hits",
                                            xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('PMuonBarrelMdtHits', type='TH1F', title="TrigDiMuon_FS - Positive Muon Hit multiplicity in the MDT Barrel; MDT hits",
                                            xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('PMuonCapMdtHits', type='TH1F', title="TrigDiMuon_FS - Positive Muon Hit multiplicity in the MDT end-cap; MDT hits",
                                            xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('NMuonCapMdtHits', type='TH1F', title="TrigDiMuon_FS - Negative Muon Hit multiplicity in the MDT end-cap; MDT hits",
                                            xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('PMuonTgcHits', type='TH1F', title="TrigDiMuon_FS - Positive Muon Hit multiplicity in the TGC; TGC hits",
                                            xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('PMuonRpcHits', type='TH1F', title="TrigDiMuon_FS - Positive Muon Hit multiplicity in the RPC; RPC hits",
                                            xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('NMuonTgcHits', type='TH1F', title="TrigDiMuon_FS - Negative Muon Hit multiplicity in the TGC; TGC hits",
                                            xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('NMuonRpcHits', type='TH1F', title="TrigDiMuon_FS - Negative Muon Hit multiplicity in the RPC; RPC hits",
                                            xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('JPsiMass', type='TH1F', title="TrigDiMuon_FS - Mass of J/psi; Mass (MeV)",
                                            xbins=100, xmin=0, xmax=6000) ]
        self.Histograms += [ defineHistogram('JPsiMassWideRange', type='TH1F', title="TrigDiMuon_FS - Mass of J/psi; Mass (MeV)",
                                            xbins=100, xmin=0, xmax=100000) ]
        self.Histograms += [ defineHistogram('JPsiEta', type='TH1F', title="TrigDiMuon_FS - Eta of J/psi; Eta",
                                            xbins=30, xmin=-2.5, xmax=2.5) ]
        self.Histograms += [ defineHistogram('JPsiPhi', type='TH1F', title="TrigDiMuon_FS - Phi of J/psi; Phi",
                                            xbins=30, xmin=-3.5, xmax=3.5) ]
        self.Histograms += [ defineHistogram('JPsiPt', type='TH1F', title="TrigDiMuon_FS - PT of J/psi; PT (MeV)",
                                            xbins=100, xmin=0, xmax=50000) ]
        self.Histograms += [ defineHistogram('JPsiPtWideRange', type='TH1F', title="TrigDiMuon_FS - PT of J/psi; PT (MeV)",
                                            xbins=100, xmin=0, xmax=150000) ]
        self.Histograms += [ defineHistogram('JPsiChi2', type='TH1F', title="TrigDiMuon_FS - Chi2 of J/psi Vertex fit; Chi2",
                                            xbins=100, xmin=0, xmax=20) ]
        self.Histograms += [ defineHistogram('JPsiPerRoi', type='TH1F', title="TrigDiMuon_FS - Number of J/psi per ROI; J/psi per ROI",
                                            xbins=20, xmin=0, xmax=20) ]
        self.Histograms += [ defineHistogram('JPsiLifeTime', type='TH1F', title="TrigDiMuon_FS - J/psi Deacy distance; 1/(MeV)",
                                            xbins=100, xmin=0, xmax=100) ]
        self.Histograms += [ defineHistogram('PMuonEta', type='TH1F', title="TrigDiMuon_FS - Eta of Positive Muon; Eta",
                                            xbins=30, xmin=-2.5, xmax=2.5) ]
        self.Histograms += [ defineHistogram('PMuonPhi', type='TH1F', title="TrigDiMuon_FS - Phi of Positive Muon; Phi",
                                            xbins=30, xmin=-3.5, xmax=3.5) ]
        self.Histograms += [ defineHistogram('PMuonPt', type='TH1F', title="TrigDiMuon_FS - PT of Positive Muon; PT (MeV)",
                                            xbins=100, xmin=0, xmax=100000) ]
        self.Histograms += [ defineHistogram('NMuonEta', type='TH1F', title="TrigDiMuon_FS - Eta of Negative Muon; Eta",
                                            xbins=30, xmin=-2.5, xmax=2.5) ]
        self.Histograms += [ defineHistogram('NMuonPhi', type='TH1F', title="TrigDiMuon_FS - Phi of Negative Muon; Phi",
                                            xbins=30, xmin=-3.5, xmax=3.5) ]
        self.Histograms += [ defineHistogram('NMuonPt', type='TH1F', title="TrigDiMuon_FS - PT of Negative Muon; PT (MeV)",
                                            xbins=100, xmin=0, xmax=100000) ]

        self.Histograms += [ defineHistogram ( 'PMuonEta, PMuonPhi', type = 'TH2F',
                                               title = 'TrigDiMuon_FS - positive muon direction; #eta ; #phi ; # tracks',
                                               xbins = 60, xmin = -3.0, xmax = 3.0,
                                               ybins = 64, ymin = -3.2, ymax = 3.2 ) ]

        self.Histograms += [ defineHistogram ( 'NMuonEta, NMuonPhi', type = 'TH2F',
                                               title = 'TrigDiMuon_FS - negative muon direction; #eta ; #phi ; # tracks',
                                               xbins = 60, xmin = -3.0, xmax = 3.0,
                                               ybins = 64, ymin = -3.2, ymax = 3.2 ) ]

        self.Histograms += [ defineHistogram('roiNoHits_pt', type='TH1F', title="TrigDiMuon_FS - RoI track with no hits; PT (MeV)",
                                            xbins=100, xmin=0, xmax=100000) ]
        self.Histograms += [ defineHistogram('roiNoHits_charge', type='TH1F', title="TrigDiMuon_FS - RoI track with no hits; charge",
                                            xbins=5, xmin=-2, xmax=3) ]
        self.Histograms += [ defineHistogram('roiNoHits_eta', type='TH1F', title="TrigDiMuon_FS - RoI track with no hits; Eta",
                                            xbins=30, xmin=-2.5, xmax=2.5) ]
        self.Histograms += [ defineHistogram('roiNoHits_phi', type='TH1F', title="TrigDiMuon_FS - RoI track with no hits; Phi",
                                            xbins=30, xmin=-3.5, xmax=3.5) ]




        self.Histograms += [ defineHistogram('TagAndProbe_trTr', type='TH1F', title="TrigDiMuon_FS - T&P: all tracks ; Mass (MeV)",
                                            xbins=100, xmin=0, xmax=6000) ]
        self.Histograms += [ defineHistogram('TagAndProbe_trTr_OS', type='TH1F', title="TrigDiMuon_FS - T&P: opposite-sign tracks ; Mass (MeV)",
                                            xbins=100, xmin=0, xmax=6000) ]
        self.Histograms += [ defineHistogram('TagAndProbe_trRoI', type='TH1F', title="TrigDiMuon_FS - T&P: RoI - track; Mass (MeV)",
                                            xbins=100, xmin=0, xmax=6000) ]
        self.Histograms += [ defineHistogram('TagAndProbe_muRoI', type='TH1F', title="TrigDiMuon_FS - T&P: RoI - track with hits ; Mass (MeV)",
                                            xbins=100, xmin=0, xmax=6000) ]
        self.Histograms += [ defineHistogram('TagAndProbe_muMuRoI', type='TH1F', title="TrigDiMuon_FS - T&P: RoI w/ hits - track w/ hits ; Mass (MeV)",
                                            xbins=100, xmin=0, xmax=6000) ]

        self.Histograms += [ defineHistogram('Counter_trTr', type='TH1F', title="TrigDiMuon_FS -  # of di-track candidates; number of candidates per RoI",
                                            xbins=20, xmin=0, xmax=20) ]
        self.Histograms += [ defineHistogram('Counter_muMu', type='TH1F', title="TrigDiMuon_FS -  # of di-muon candidates; number of candidates per RoI",
                                            xbins=20, xmin=0, xmax=20) ]
        self.Histograms += [ defineHistogram('Counter_muMuRoI', type='TH1F', title="TrigDiMuon_FS -  # of di-muon candidates after RoI confirmation; number of candidates per RoI",
                                            xbins=20, xmin=0, xmax=20) ]
        self.Histograms += [ defineHistogram('Counter_muMuVtx', type='TH1F', title="TrigDiMuon_FS -  # of di-muon candidates after vertexing; number of candidates per RoI",
                                            xbins=20, xmin=0, xmax=20) ]
        self.Histograms += [ defineHistogram('Counter_muMuRoIVtx', type='TH1F', title="TrigDiMuon_FS -  # of di-muon candidates after RoI confirmation and vertexing; number of candidates per RoI",
                                            xbins=20, xmin=0, xmax=20) ]

        self.Histograms += [ defineHistogram('Counter_allTrk', type='TH1F', title="TrigDiMuon_FS -  # of all tracks; # tracks per RoI",
                                            xbins=100, xmin=0, xmax=100) ]
        self.Histograms += [ defineHistogram('Counter_accTrk', type='TH1F', title="TrigDiMuon_FS -  # of pre-selected tracks; # tracks per RoI",
                                            xbins=100, xmin=0, xmax=100) ]


        self.Histograms += [ defineHistogram('Hit_Endcap_mdt_inner_dEta' , type='TH1F',
                                             title="TrigDiMuon_FS - Hits - extrapolated track (MDT Inner Endcap); #Delta#eta",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]
        self.Histograms += [ defineHistogram('Hit_Endcap_mdt_middle_dEta', type='TH1F',
                                             title="TrigDiMuon_FS - Hits - extrapolated track (MDT Middle Endcap); #Delta#eta",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]
        self.Histograms += [ defineHistogram('Hit_Barrel_mdt_inner_dEta' , type='TH1F',
                                             title="TrigDiMuon_FS - Hits - extrapolated track (MDT Inner Barrel); #Delta#eta",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]
        self.Histograms += [ defineHistogram('Hit_Barrel_mdt_middle_dEta', type='TH1F',
                                             title="TrigDiMuon_FS - Hits - extrapolated track (MDT Middle Barrel); #Delta#eta",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]

        self.Histograms += [ defineHistogram('Hit_rpc_middle_dEta'       ,type='TH1F',
                                             title="TrigDiMuon_FS - Hits - extrapolated track (RPC Middle); #Delta#eta",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]
        self.Histograms += [ defineHistogram('Hit_rpc_middle_dPhi'       ,type='TH1F',
                                             title="TrigDiMuon_FS - Hits - extrapolated track (RPC Middle); #Delta#phi",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]

        self.Histograms += [ defineHistogram('Hit_tgc_wire_inner_dEta'   ,type='TH1F',
                                             title="TrigDiMuon_FS - Hits - extrapolated track (TGC Wire Inner); #Delta#eta",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]
        self.Histograms += [ defineHistogram('Hit_tgc_wire_middle_dEta'  , type='TH1F',
                                             title="TrigDiMuon_FS - Hits - extrapolated track (TGC Wire Middle); #Delta#eta",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]
        self.Histograms += [ defineHistogram('Hit_tgc_wire_inner_dPhi'   ,type='TH1F',
                                             title="TrigDiMuon_FS - Hits - extrapolated track (TGC Wire Inner); #Delta#phi",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]
        self.Histograms += [ defineHistogram('Hit_tgc_wire_middle_dPhi'  , type='TH1F',
                                             title="TrigDiMuon_FS - Hits - extrapolated track (TGC Wire Middle); #Delta#phi",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]

        self.Histograms += [ defineHistogram('Hit_tgc_strip_inner_dPhi'  , type='TH1F',
                                             title="TrigDiMuon_FS - Hits - extrapolated track (TGC Strip Inner); #Delta#phi",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]
        self.Histograms += [ defineHistogram('Hit_tgc_strip_middle_dPhi' , type='TH1F',
                                             title="TrigDiMuon_FS - Hits - extrapolated track (TGC Strip Middle); #Delta#phi",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]



        self.Histograms += [ defineHistogram('vtx_mass'    , type='TH1F', title="TrigDiMuon_FS - Mass from vertex fit; M (MeV)", xbins=100, xmin=0, xmax=6000) ]
        self.Histograms += [ defineHistogram('vtx_massWideRange', type='TH1F', title="TrigDiMuon_FS - Mass from vertex fit; M (MeV)", xbins=100, xmin=0, xmax=100000) ]
        self.Histograms += [ defineHistogram('vtx_x'       , type='TH1F', title="TrigDiMuon_FS - Vertex ; x (mm)", xbins=100, xmin=-10, xmax=10) ]
        self.Histograms += [ defineHistogram('vtx_y'       , type='TH1F', title="TrigDiMuon_FS - Vertex ; y (mm)", xbins=100, xmin=-10, xmax=10) ]
        self.Histograms += [ defineHistogram('vtx_z'       , type='TH1F', title="TrigDiMuon_FS - Vertex ; z (mm)", xbins=100, xmin=-100, xmax=100) ]





        self.Histograms += [ defineHistogram ( 'Errors', type = 'TH1F',
                                                title = 'TrigDiMuon_FS - Algorithm errors ; ; # candidates',
                                                xbins = ERROR_number , xmin = 0.5, xmax = ERROR_number + 0.5,
                                                labels = ERROR_labels ) ]
        self.Histograms += [ defineHistogram ( 'Debug', type = 'TH1F',
                                                title = 'TrigDiMuon_FS - Debug ; ; # candidates',
                                                xbins = DEBUG_number , xmin = 0.5, xmax = DEBUG_number + 0.5,
                                                labels = DEBUG_labels ) ]
        self.Histograms += [ defineHistogram ( 'Acceptance', type = 'TH1F',
                                                title = 'TrigDiMuon_FS - Reached steps - acceptance ; ; # events',
                                                xbins = ACCEPTANCE_number , xmin = 0.5, xmax = ACCEPTANCE_number + 0.5,
                                                labels = ACCEPTANCE_labels ) ]
        self.Histograms += [ defineHistogram ( 'Settings', type = 'TH1F',
                                                title = 'TrigDiMuon_FS - algorithm settings flags',
                                                xbins = SETTINGS_number , xmin = 0.5, xmax = SETTINGS_number + 0.5,
                                                labels = SETTINGS_labels ) ]


class TrigDiMuonOnlineMonitoring_FS(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigDiMuonOnlineMonitoring_FS"):
        super(TrigDiMuonOnlineMonitoring_FS, self).__init__(name)
        self.defineTarget("Online")


        self.Histograms = [ defineHistogram('NMuonBarrelMdtHits', type='TH1F', title="TrigDiMuon_FS - Negative Muon Hit multiplicity in the MDT Barrel; MDT hits",
                                            xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('PMuonBarrelMdtHits', type='TH1F', title="TrigDiMuon_FS - Positive Muon Hit multiplicity in the MDT Barrel; MDT hits",
                                            xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('PMuonCapMdtHits', type='TH1F', title="TrigDiMuon_FS - Positive Muon Hit multiplicity in the MDT end-cap; MDT hits",
                                            xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('NMuonCapMdtHits', type='TH1F', title="TrigDiMuon_FS - Negative Muon Hit multiplicity in the MDT end-cap; MDT hits",
                                            xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('PMuonTgcHits', type='TH1F', title="TrigDiMuon_FS - Positive Muon Hit multiplicity in the TGC; TGC hits",
                                            xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('PMuonRpcHits', type='TH1F', title="TrigDiMuon_FS - Positive Muon Hit multiplicity in the RPC; RPC hits",
                                            xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('NMuonTgcHits', type='TH1F', title="TrigDiMuon_FS - Negative Muon Hit multiplicity in the TGC; TGC hits",
                                            xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('NMuonRpcHits', type='TH1F', title="TrigDiMuon_FS - Negative Muon Hit multiplicity in the RPC; RPC hits",
                                            xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('JPsiMass', type='TH1F', title="TrigDiMuon_FS - Mass of J/psi; Mass (MeV)",
                                            xbins=100, xmin=0, xmax=6000) ]
        self.Histograms += [ defineHistogram('JPsiMassWideRange', type='TH1F', title="TrigDiMuon_FS - Mass of J/psi; Mass (MeV)",
                                            xbins=100, xmin=0, xmax=100000) ]
        self.Histograms += [ defineHistogram('JPsiEta', type='TH1F', title="TrigDiMuon_FS - Eta of J/psi; Eta",
                                            xbins=30, xmin=-2.5, xmax=2.5) ]
        self.Histograms += [ defineHistogram('JPsiPhi', type='TH1F', title="TrigDiMuon_FS - Phi of J/psi; Phi",
                                            xbins=30, xmin=-3.5, xmax=3.5) ]
        self.Histograms += [ defineHistogram('JPsiPt', type='TH1F', title="TrigDiMuon_FS - PT of J/psi; PT (MeV)",
                                            xbins=100, xmin=0, xmax=50000) ]
        self.Histograms += [ defineHistogram('JPsiPtWideRange', type='TH1F', title="TrigDiMuon_FS - PT of J/psi; PT (MeV)",
                                            xbins=100, xmin=0, xmax=150000) ]
        self.Histograms += [ defineHistogram('JPsiChi2', type='TH1F', title="TrigDiMuon_FS - Chi2 of J/psi Vertex fit; Chi2",
                                            xbins=100, xmin=0, xmax=20) ]
        self.Histograms += [ defineHistogram('JPsiPerRoi', type='TH1F', title="TrigDiMuon_FS - Number of J/psi per ROI; J/psi per ROI",
                                            xbins=20, xmin=0, xmax=20) ]
        self.Histograms += [ defineHistogram('JPsiLifeTime', type='TH1F', title="TrigDiMuon_FS - J/psi Deacy distance; 1/(MeV)",
                                            xbins=100, xmin=0, xmax=100) ]
        self.Histograms += [ defineHistogram('PMuonEta', type='TH1F', title="TrigDiMuon_FS - Eta of Positive Muon; Eta",
                                            xbins=30, xmin=-2.5, xmax=2.5) ]
        self.Histograms += [ defineHistogram('PMuonPhi', type='TH1F', title="TrigDiMuon_FS - Phi of Positive Muon; Phi",
                                            xbins=30, xmin=-3.5, xmax=3.5) ]
        self.Histograms += [ defineHistogram('PMuonPt', type='TH1F', title="TrigDiMuon_FS - PT of Positive Muon; PT (MeV)",
                                            xbins=100, xmin=0, xmax=100000) ]
        self.Histograms += [ defineHistogram('NMuonEta', type='TH1F', title="TrigDiMuon_FS - Eta of Negative Muon; Eta",
                                            xbins=30, xmin=-2.5, xmax=2.5) ]
        self.Histograms += [ defineHistogram('NMuonPhi', type='TH1F', title="TrigDiMuon_FS - Phi of Negative Muon; Phi",
                                            xbins=30, xmin=-3.5, xmax=3.5) ]
        self.Histograms += [ defineHistogram('NMuonPt', type='TH1F', title="TrigDiMuon_FS - PT of Negative Muon; PT (MeV)",
                                            xbins=100, xmin=0, xmax=100000) ]

        self.Histograms += [ defineHistogram ( 'PMuonEta, PMuonPhi', type = 'TH2F',
                                               title = 'TrigDiMuon_FS - positive muon direction; #eta ; #phi ; # tracks',
                                               xbins = 60, xmin = -3.0, xmax = 3.0,
                                               ybins = 64, ymin = -3.2, ymax = 3.2 ) ]

        self.Histograms += [ defineHistogram ( 'NMuonEta, NMuonPhi', type = 'TH2F',
                                               title = 'TrigDiMuon_FS - negative muon direction; #eta ; #phi ; # tracks',
                                               xbins = 60, xmin = -3.0, xmax = 3.0,
                                               ybins = 64, ymin = -3.2, ymax = 3.2 ) ]

        self.Histograms += [ defineHistogram('roiNoHits_pt', type='TH1F', title="TrigDiMuon_FS - RoI track with no hits; PT (MeV)",
                                            xbins=100, xmin=0, xmax=100000) ]
        self.Histograms += [ defineHistogram('roiNoHits_charge', type='TH1F', title="TrigDiMuon_FS - RoI track with no hits; charge",
                                            xbins=5, xmin=-2, xmax=3) ]
        self.Histograms += [ defineHistogram('roiNoHits_eta', type='TH1F', title="TrigDiMuon_FS - RoI track with no hits; Eta",
                                            xbins=30, xmin=-2.5, xmax=2.5) ]
        self.Histograms += [ defineHistogram('roiNoHits_phi', type='TH1F', title="TrigDiMuon_FS - RoI track with no hits; Phi",
                                            xbins=30, xmin=-3.5, xmax=3.5) ]



        self.Histograms += [ defineHistogram('TagAndProbe_trTr', type='TH1F', title="TrigDiMuon_FS - T&P: all tracks ; Mass (MeV)",
                                            xbins=100, xmin=0, xmax=6000) ]
        self.Histograms += [ defineHistogram('TagAndProbe_trTr_OS', type='TH1F', title="TrigDiMuon_FS - T&P: opposite-sign tracks ; Mass (MeV)",
                                            xbins=100, xmin=0, xmax=6000) ]
        self.Histograms += [ defineHistogram('TagAndProbe_trRoI', type='TH1F', title="TrigDiMuon_FS - T&P: RoI - track; Mass (MeV)",
                                            xbins=100, xmin=0, xmax=6000) ]
        self.Histograms += [ defineHistogram('TagAndProbe_muRoI', type='TH1F', title="TrigDiMuon_FS - T&P: RoI - track with hits ; Mass (MeV)",
                                            xbins=100, xmin=0, xmax=6000) ]
        self.Histograms += [ defineHistogram('TagAndProbe_muMuRoI', type='TH1F', title="TrigDiMuon_FS - T&P: RoI w/ hits - track w/ hits ; Mass (MeV)",
                                            xbins=100, xmin=0, xmax=6000) ]

        self.Histograms += [ defineHistogram('Counter_trTr', type='TH1F', title="TrigDiMuon_FS -  # of di-track candidates; number of candidates per RoI",
                                            xbins=20, xmin=0, xmax=20) ]
        self.Histograms += [ defineHistogram('Counter_muMu', type='TH1F', title="TrigDiMuon_FS -  # of di-muon candidates; number of candidates per RoI",
                                            xbins=20, xmin=0, xmax=20) ]
        self.Histograms += [ defineHistogram('Counter_muMuRoI', type='TH1F', title="TrigDiMuon_FS -  # of di-muon candidates after RoI confirmation; number of candidates per RoI",
                                            xbins=20, xmin=0, xmax=20) ]
        self.Histograms += [ defineHistogram('Counter_muMuVtx', type='TH1F', title="TrigDiMuon_FS -  # of di-muon candidates after vertexing; number of candidates per RoI",
                                            xbins=20, xmin=0, xmax=20) ]
        self.Histograms += [ defineHistogram('Counter_muMuRoIVtx', type='TH1F', title="TrigDiMuon_FS -  # of di-muon candidates after RoI confirmation and vertexing; number of candidates per RoI",
                                            xbins=20, xmin=0, xmax=20) ]

        self.Histograms += [ defineHistogram('Counter_allTrk', type='TH1F', title="TrigDiMuon_FS -  # of all tracks; # tracks per RoI",
                                            xbins=100, xmin=0, xmax=100) ]
        self.Histograms += [ defineHistogram('Counter_accTrk', type='TH1F', title="TrigDiMuon_FS -  # of pre-selected tracks; # tracks per RoI",
                                            xbins=100, xmin=0, xmax=100) ]



        self.Histograms += [ defineHistogram('Hit_Endcap_mdt_inner_dEta' , type='TH1F',
                                             title="TrigDiMuon_FS - Hits - extrapolated track (MDT Inner Endcap); #Delta#eta",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]
        self.Histograms += [ defineHistogram('Hit_Endcap_mdt_middle_dEta', type='TH1F',
                                             title="TrigDiMuon_FS - Hits - extrapolated track (MDT Middle Endcap); #Delta#eta",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]
        self.Histograms += [ defineHistogram('Hit_Barrel_mdt_inner_dEta' , type='TH1F',
                                             title="TrigDiMuon_FS - Hits - extrapolated track (MDT Inner Barrel); #Delta#eta",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]
        self.Histograms += [ defineHistogram('Hit_Barrel_mdt_middle_dEta', type='TH1F',
                                             title="TrigDiMuon_FS - Hits - extrapolated track (MDT Middle Barrel); #Delta#eta",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]

        self.Histograms += [ defineHistogram('Hit_rpc_middle_dEta'       ,type='TH1F',
                                             title="TrigDiMuon_FS - Hits - extrapolated track (RPC Middle); #Delta#eta",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]
        self.Histograms += [ defineHistogram('Hit_rpc_middle_dPhi'       ,type='TH1F',
                                             title="TrigDiMuon_FS - Hits - extrapolated track (RPC Middle); #Delta#phi",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]

        self.Histograms += [ defineHistogram('Hit_tgc_wire_inner_dEta'   ,type='TH1F',
                                             title="TrigDiMuon_FS - Hits - extrapolated track (TGC Wire Inner); #Delta#eta",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]
        self.Histograms += [ defineHistogram('Hit_tgc_wire_middle_dEta'  , type='TH1F',
                                             title="TrigDiMuon_FS - Hits - extrapolated track (TGC Wire Middle); #Delta#eta",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]
        self.Histograms += [ defineHistogram('Hit_tgc_wire_inner_dPhi'   ,type='TH1F',
                                             title="TrigDiMuon_FS - Hits - extrapolated track (TGC Wire Inner); #Delta#phi",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]
        self.Histograms += [ defineHistogram('Hit_tgc_wire_middle_dPhi'  , type='TH1F',
                                             title="TrigDiMuon_FS - Hits - extrapolated track (TGC Wire Middle); #Delta#phi",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]

        self.Histograms += [ defineHistogram('Hit_tgc_strip_inner_dPhi'  , type='TH1F',
                                             title="TrigDiMuon_FS - Hits - extrapolated track (TGC Strip Inner); #Delta#phi",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]
        self.Histograms += [ defineHistogram('Hit_tgc_strip_middle_dPhi' , type='TH1F',
                                             title="TrigDiMuon_FS - Hits - extrapolated track (TGC Strip Middle); #Delta#phi",
                                             xbins=100, xmin=-0.75, xmax=0.75) ]


        self.Histograms += [ defineHistogram('vtx_mass'    , type='TH1F', title="TrigDiMuon_FS - Mass from vertex fit; M (MeV)", xbins=100, xmin=0, xmax=6000) ]
        self.Histograms += [ defineHistogram('vtx_massWideRange', type='TH1F', title="TrigDiMuon_FS - Mass from vertex fit; M (MeV)", xbins=100, xmin=0, xmax=100000) ]
        self.Histograms += [ defineHistogram('vtx_x'       , type='TH1F', title="TrigDiMuon_FS - Vertex ; x (mm)", xbins=100, xmin=-10, xmax=10) ]
        self.Histograms += [ defineHistogram('vtx_y'       , type='TH1F', title="TrigDiMuon_FS - Vertex ; y (mm)", xbins=100, xmin=-10, xmax=10) ]
        self.Histograms += [ defineHistogram('vtx_z'       , type='TH1F', title="TrigDiMuon_FS - Vertex ; z (mm)", xbins=100, xmin=-100, xmax=100) ]



        self.Histograms += [ defineHistogram ( 'Errors', type = 'TH1F',
                                                title = 'TrigDiMuon_FS - Algorithm errors ; ; # candidates',
                                                xbins = ERROR_number , xmin = 0.5, xmax = ERROR_number + 0.5,
                                                labels = ERROR_labels ) ]
        self.Histograms += [ defineHistogram ( 'Debug', type = 'TH1F',
                                                title = 'TrigDiMuon_FS - Debug ; ; # candidates',
                                                xbins = DEBUG_number , xmin = 0.5, xmax = DEBUG_number + 0.5,
                                                labels = DEBUG_labels ) ]
        self.Histograms += [ defineHistogram ( 'Acceptance', type = 'TH1F',
                                                title = 'TrigDiMuon_FS - Reached steps - acceptance ; ; # events',
                                                xbins = ACCEPTANCE_number , xmin = 0.5, xmax = ACCEPTANCE_number + 0.5,
                                                labels = ACCEPTANCE_labels ) ]
        self.Histograms += [ defineHistogram ( 'Settings', type = 'TH1F',
                                                title = 'TrigDiMuon_FS - algorithm settings flags',
                                                xbins = SETTINGS_number , xmin = 0.5, xmax = SETTINGS_number + 0.5,
                                                labels = SETTINGS_labels ) ]
