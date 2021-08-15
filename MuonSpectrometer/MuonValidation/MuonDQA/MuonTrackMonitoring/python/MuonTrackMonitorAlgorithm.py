"""
 Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 2020 Matthias Schott - Uni Mainz
"""


def MuonTrackConfig(inputFlags, isOld=False):
    if isOld:
        # Run-2 style configuration
        from AthenaMonitoring import AthMonitorCfgHelperOld as AthMonitorCfgHelper
        from MuonTrackMonitoring.MuonTrackMonitoringConf import MuonTrackMonitorAlgorithm
    else:
        from AthenaMonitoring import AthMonitorCfgHelper
        from AthenaConfiguration.ComponentFactory import CompFactory
        MuonTrackMonitorAlgorithm = CompFactory.MuonTrackMonitorAlgorithm

    helper = AthMonitorCfgHelper(inputFlags, "MuonTrackMonitoringConfig")

    muonTrackAlg = helper.addAlgorithm(MuonTrackMonitorAlgorithm, "MuonTrackMonitorAlg")

    myGroup = helper.addGroup(muonTrackAlg, "MuonTrackMonitorAlgorithm", "MuonPhysics/")

# Muons/Jpsi
    myGroup.defineHistogram('JPsiMuonEta,JPsiMuonPhi;Muons_Jpsi_Origin_eta_phi', title='Muons_Jpsi_Origin_eta_phi;eta;phi', type='TH2F', path='Muons/Jpsi', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('JPsiMuonEtaTight,JPsiMuonPhiTight;Muons_Jpsi_Tight_eff', title='Muons_Jpsi_Tight_eff;eta;phi', type='TH2F', path='Muons/Jpsi', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('JPsiMuonEtaMedium,JPsiMuonPhiMedium;Muons_Jpsi_Medium_eff', title='Muons_Jpsi_Medium_eff;eta;phi', type='TH2F', path='Muons/Jpsi', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('JPsiMuonD0;Muons_Jpsi_d0', title='Muons_Jpsi_d0;d0;Entries', type='TH1F', path='Muons/Jpsi', xbins=30, xmin=-1, xmax=1)
    myGroup.defineHistogram('JPsiMuonDPTIDME;Muons_Jpsi_ddpt_idme', title='Muons_Jpsi_ddpt_idme;(ptID-ptME)/ptID;Entries', type='TH1F', path='Muons/Jpsi', xbins=30, xmin=-0.5, xmax=0.5)
    myGroup.defineHistogram('JPsiMuonPt;Muons_Jpsi_pt', title='Muons_Jpsi_pt; pT[GeV];Entries', type='TH1F', path='Muons/Jpsi', xbins=30, xmin=0, xmax=50000)
    myGroup.defineHistogram('JPsiMuonPt;Muons_Jpsi_pt_broad', title='Muons_Jpsi_pt_broad;pT[GeV];Entries', type='TH1F', path='Muons/Jpsi', xbins=30, xmin=0, xmax=1000000)
    myGroup.defineHistogram('JPsiMass2D,muMinusEta;JPsimuMinusEta', title='JPsimuMinusEta;Z mass;#eta_{#mu^{-}}', type='TH2F', path='Muons/Jpsi', xbins=27, xmin=2600., xmax=3600., ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('JPsiMass2D,muPlusEta;JPsimuPlusEta', title='JPsimuPlusEta;Z mass;#eta_{#mu^{+}}', type='TH2F', path='Muons/Jpsi', xbins=27, xmin=2600., xmax=3600., ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('muMinusEta,muPlusEta;JPsimuPlusMinus', title='JPsimuPlusMinus;#eta_{#mu^{-}};#eta_{#mu^{+}}', type='TH2F', path='Muons/Jpsi', xbins=31, xmin=-3.1415, xmax=3.1415, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('JPsiMass2D,JPsiEta2D;JPsiMassAverage', title='JPsiMassAverage;mass;eta', type='TH2F', path='Muons/Jpsi', xbins=27, xmin=2600., xmax=3600, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('JPsiMass2D,Eta2D;JPsiMassEta2D', title='JPsiMassEta2D;mass;eta(#mu^{-}#mu^{+})', type='TH2F', path='Muons/Jpsi', xbins=27, xmin=2600., xmax=3600, ybins=16, ymin=0.5, ymax=16.5)

# Overview/Jpsi
    myGroup.defineHistogram('JPsiMuonLumiBlock;Overview_Jpsi_nJpsi_LB', title='Overview_Jpsi_nJpsi_LB;LumiBlock;NumberOfJPsis', type='TH1F', path='Overview/Jpsi', xbins=30, xmin=0, xmax=2500)

# TracksID/Jpsi
    myGroup.defineHistogram('JPsiMuonNBHits;TracksID_Jpsi_HitContent_NBlayerHits', title='TracksID_Jpsi_HitContent_NBlayerHits;NumberOfBLayerHits;Entries', type='TH1F', path='TracksID/Jpsi', xbins=5, xmin=-0.5, xmax=4.5)
    myGroup.defineHistogram('JPsiMuonNPixHits;TracksID_Jpsi_HitContent_NPixelHits', title='TracksID_Jpsi_HitContent_NPixelHits;NumberOfPixelHits;Entries', type='TH1F', path='TracksID/Jpsi', xbins=10, xmin=-0.5, xmax=9.5)
    myGroup.defineHistogram('JPsiMuonNSCTHits;TracksID_Jpsi_HitContent_NSCTHits', title='TracksID_Jpsi_HitContent_NSCTHits;NumberOfSCTHits;Entries', type='TH1F', path='TracksID/Jpsi', xbins=20, xmin=-0.5, xmax=19.5)
    myGroup.defineHistogram('JPsiMuonNTRTHits;TracksID_Jpsi_HitContent_NTRTHits', title='TracksID_Jpsi_HitContent_NTRTHits;NumberOfTRTHits;Entries', type='TH1F', path='TracksID/Jpsi', xbins=50, xmin=-0.5, xmax=49.5)
    myGroup.defineHistogram('JPsiMuonIDChi2NDF;TracksID_Jpsi_chi2ndof', title='TracksID_Jpsi_chi2ndof;TrackFitChi2NDF;Entries', type='TH1F', path='TracksID/Jpsi', xbins=100, xmin=0, xmax=5)

# TracksME/Jpsi
    myGroup.defineHistogram('JPsiMuonMEChi2NDF;TracksME_Jpsi_chi2ndof', title='TracksME_Jpsi_chi2ndof;TrackFitChi2NDF;Entries', type='TH1F', path='TracksME/Jpsi', xbins=100, xmin=0, xmax=5)

# MuonTrkPhys/Jpsi
    myGroup.defineHistogram('JPsiMass;m_Jpsi_Mass', title='m_Jpsi_Mass;M_{#mu#mu};Entries', type='TH1F', path='MuonTrkPhys/Jpsi', xbins=50, xmin=2600, xmax=3600)
    myGroup.defineHistogram('JPsiMass;m_Jpsi_2occupancy', title='m_Jpsi_2occupancy;#etaRegionPermutations[+#mu,-#mu];N_{#mu}', type='TH1F', path='MuonTrkPhys/Jpsi', xbins=16, xmin=-0.5, xmax=15.5)
    myGroup.defineHistogram('JPsiMass;m_Jpsi_M_BA_BA', title='m_Jpsi_M;M_{#mu#mu};Entries', type='TH1F', path='MuonTrkPhys/Jpsi', xbins=50, xmin=2600, xmax=3600)
    myGroup.defineHistogram('JPsiMass;m_Jpsi_M_BA_BC', title='m_Jpsi_M;M_{#mu#mu};Entries', type='TH1F', path='MuonTrkPhys/Jpsi', xbins=50, xmin=2600, xmax=3600)
    myGroup.defineHistogram('JPsiMass;m_Jpsi_M_BA_EA', title='m_Jpsi_M;M_{#mu#mu};Entries', type='TH1F', path='MuonTrkPhys/Jpsi', xbins=50, xmin=2600, xmax=3600)
    myGroup.defineHistogram('JPsiMass;m_Jpsi_M_BA_EC', title='m_Jpsi_M;M_{#mu#mu};Entries', type='TH1F', path='MuonTrkPhys/Jpsi', xbins=50, xmin=2600, xmax=3600)
    myGroup.defineHistogram('JPsiMass;m_Jpsi_M_BC_BA', title='m_Jpsi_M;M_{#mu#mu};Entries', type='TH1F', path='MuonTrkPhys/Jpsi', xbins=50, xmin=2600, xmax=3600)
    myGroup.defineHistogram('JPsiMass;m_Jpsi_M_BC_BC', title='m_Jpsi_M;M_{#mu#mu};Entries', type='TH1F', path='MuonTrkPhys/Jpsi', xbins=50, xmin=2600, xmax=3600)
    myGroup.defineHistogram('JPsiMass;m_Jpsi_M_BC_EA', title='m_Jpsi_M;M_{#mu#mu};Entries', type='TH1F', path='MuonTrkPhys/Jpsi', xbins=50, xmin=2600, xmax=3600)
    myGroup.defineHistogram('JPsiMass;m_Jpsi_M_BC_EC', title='m_Jpsi_M;M_{#mu#mu};Entries', type='TH1F', path='MuonTrkPhys/Jpsi', xbins=50, xmin=2600, xmax=3600)
    myGroup.defineHistogram('JPsiMass;m_Jpsi_M_EA_BA', title='m_Jpsi_M;M_{#mu#mu};Entries', type='TH1F', path='MuonTrkPhys/Jpsi', xbins=50, xmin=2600, xmax=3600)
    myGroup.defineHistogram('JPsiMass;m_Jpsi_M_EA_BC', title='m_Jpsi_M;M_{#mu#mu};Entries', type='TH1F', path='MuonTrkPhys/Jpsi', xbins=50, xmin=2600, xmax=3600)
    myGroup.defineHistogram('JPsiMass;m_Jpsi_M_EA_EA', title='m_Jpsi_M;M_{#mu#mu};Entries', type='TH1F', path='MuonTrkPhys/Jpsi', xbins=50, xmin=2600, xmax=3600)
    myGroup.defineHistogram('JPsiMass;m_Jpsi_M_EA_EC', title='m_Jpsi_M;M_{#mu#mu};Entries', type='TH1F', path='MuonTrkPhys/Jpsi', xbins=50, xmin=2600, xmax=3600)
    myGroup.defineHistogram('JPsiMass;m_Jpsi_M_EC_BA', title='m_Jpsi_M;M_{#mu#mu};Entries', type='TH1F', path='MuonTrkPhys/Jpsi', xbins=50, xmin=2600, xmax=3600)
    myGroup.defineHistogram('JPsiMass;m_Jpsi_M_EC_BC', title='m_Jpsi_M;M_{#mu#mu};Entries', type='TH1F', path='MuonTrkPhys/Jpsi', xbins=50, xmin=2600, xmax=3600)
    myGroup.defineHistogram('JPsiMass;m_Jpsi_M_EC_EA', title='m_Jpsi_M;M_{#mu#mu};Entries', type='TH1F', path='MuonTrkPhys/Jpsi', xbins=50, xmin=2600, xmax=3600)
    myGroup.defineHistogram('JPsiMass;m_Jpsi_M_EC_EC', title='m_Jpsi_M;M_{#mu#mu};Entries', type='TH1F', path='MuonTrkPhys/Jpsi', xbins=50, xmin=2600, xmax=3600)

# Muons/Z
    myGroup.defineHistogram('ZMuonEta,ZMuonPhi;Muons_Z_Origin_eta_phi', title='Muons_Z_Origin_eta_phi;eta;phi', type='TH2F', path='Muons/Z', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('ZMuonEtaTight,ZMuonPhiTight;Muons_Z_Tight_eff', title='Muons_Z_Tight_eff;eta;phi', type='TH2F', path='Muons/Z', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('ZMuonEtaMedium,ZMuonPhiMedium;Muons_Z_Medium_eff', title='Muons_Z_Medium_eff;eta;phi', type='TH2F', path='Muons/Z', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('ZMuonD0;Muons_Z_d0', title='Muons_Z_d0;d0;Entries', type='TH1F', path='Muons/Z', xbins=100, xmin=-1, xmax=1)
    myGroup.defineHistogram('ZMuonZ0;Muons_Z_z0', title='Muons_Z_z0;z0;Entries', type='TH1F', path='Muons/Z', xbins=100, xmin=-1000, xmax=1000)
    myGroup.defineHistogram('ZMuonDPTIDME;Muons_Z_ddpt_idme', title='Muons_Z_ddpt_idme;(ptID-ptME)/ptID;Entries', type='TH1F', path='Muons/Z', xbins=100, xmin=-0.5, xmax=0.5)
    myGroup.defineHistogram('ZMuonPt;Muons_Z_pt', title='Muons_Z_pt; pT[GeV];Entries', type='TH1F', path='Muons/Z', xbins=100, xmin=0, xmax=50000)
    myGroup.defineHistogram('ZMuonPt;Muons_Z_pt_broad', title='Muons_Z_pt_broad;pT[GeV];Entries', type='TH1F', path='Muons/Z', xbins=100, xmin=0, xmax=1000000)
    myGroup.defineHistogram('ZMass,muMinusEta;ZmuMinusEta', title='ZmuMinusEta;Z mass;#eta_{#mu^{-}}', type='TH2F', path='Muons/Z', xbins=27, xmin=76000., xmax=106000., ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('ZMass,muPlusEta;ZmuPlusEta', title='ZmuPlusEta;Z mass;#eta_{#mu^{+}}', type='TH2F', path='Muons/Z', xbins=27, xmin=76000., xmax=106000., ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('muMinusEta,muPlusEta;ZmuPlusMinus', title='ZmuPlusMinus;#eta_{#mu^{-}};#eta_{#mu^{+}}', type='TH2F', path='Muons/Z', xbins=31, xmin=-3.1415, xmax=3.1415, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('ZMass,ZEta;ZMassAverage', title='ZMassAverage;mass;eta', type='TH2F', path='Muons/Z', xbins=27, xmin=76000., xmax=106000., ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('ZMass,MuPlusEta;ZMassEtaPlus', title='ZMassEtaPlus;mass;eta(#mu^{-}#mu^{+})', type='TH2F', path='Muons/Z', xbins=27, xmin=76000., xmax=106000., ybins=16, ymin=0.5, ymax=16.5)
    myGroup.defineHistogram('ZMass,muMinusEta;ZMassEtaMinus', title='ZMassEtaMinus;mass;eta(#mu^{-}#mu^{+})', type='TH2F', path='Muons/Z', xbins=27, xmin=76000., xmax=106000., ybins=16, ymin=0.5, ymax=16.5)

# Overview/Z
    myGroup.defineHistogram('ZMuonLumiBlock;Overview_Z_nZ_LB', title='Overview_Z_nJpsi_LB;LumiBlock;NumberOfZs', type='TH1F', path='Overview/Z', xbins=100, xmin=0, xmax=2500)

# TracksID/Z
    myGroup.defineHistogram('ZMuonNBHits;TracksID_Z_HitContent_NBlayerHits', title='TracksID_Z_HitContent_NBlayerHits;NumberOfBLayerHits;Entries', type='TH1F', path='TracksID/Z', xbins=5, xmin=-0.5, xmax=4.5)
    myGroup.defineHistogram('ZMuonNPixHits;TracksID_Z_HitContent_NPixelHits', title='TracksID_Z_HitContent_NPixelHits;NumberOfPixelHits;Entries', type='TH1F', path='TracksID/Z', xbins=10, xmin=-0.5, xmax=9.5)
    myGroup.defineHistogram('ZMuonNSCTHits;TracksID_Z_HitContent_NSCTHits', title='TracksID_Z_HitContent_NSCTHits;NumberOfSCTHits;Entries', type='TH1F', path='TracksID/Z', xbins=20, xmin=-0.5, xmax=19.5)
    myGroup.defineHistogram('ZMuonNTRTHits;TracksID_Z_HitContent_NTRTHits', title='TracksID_Z_HitContent_NTRTHits;NumberOfTRTHits;Entries', type='TH1F', path='TracksID/Z', xbins=50, xmin=-0.5, xmax=49.5)
    myGroup.defineHistogram('ZMuonIDChi2NDF;TracksID_Z_chi2ndof', title='TracksID_Z_chi2ndof;TrackFitChi2NDF;Entries', type='TH1F', path='TracksID/Z', xbins=100, xmin=0, xmax=5)

# TracksME/Z
    myGroup.defineHistogram('ZMuonMEChi2NDF;TracksME_Z_chi2ndof', title='TracksME_Z_chi2ndof;TrackFitChi2NDF;Entries', type='TH1F', path='TracksME/Z', xbins=100, xmin=0, xmax=5)

# MuonTrkPhys/Z
    myGroup.defineHistogram('ZMass;m_Z_Mass', title='m_Z_Mass;M_{#mu#mu};Entries', type='TH1F', path='MuonTrkPhys/Z', xbins=50, xmin=76000., xmax=106000.)
    myGroup.defineHistogram('ZMass;m_Z_2occupancy', title='m_Z_2occupancy;#etaRegionPermutations[+#mu,-#mu];N_{#mu}', type='TH1F', path='MuonTrkPhys/Z', xbins=16, xmin=-0.5, xmax=15.5)
    myGroup.defineHistogram('ZMass;m_Z_M_BA_BA', title='m_Z_M;M_{#mu#mu};Entries', type='TH1F', path='MuonTrkPhys/Z', xbins=50, xmin=76000., xmax=106000.)
    myGroup.defineHistogram('ZMass;m_Z_M_BA_BC', title='m_Z_M;M_{#mu#mu};Entries', type='TH1F', path='MuonTrkPhys/Z', xbins=50, xmin=76000., xmax=106000.)
    myGroup.defineHistogram('ZMass;m_Z_M_BA_EA', title='m_Z_M;M_{#mu#mu};Entries', type='TH1F', path='MuonTrkPhys/Z', xbins=50, xmin=76000., xmax=106000.)
    myGroup.defineHistogram('ZMass;m_Z_M_BA_EC', title='m_Z_M;M_{#mu#mu};Entries', type='TH1F', path='MuonTrkPhys/Z', xbins=50, xmin=76000., xmax=106000.)
    myGroup.defineHistogram('ZMass;m_Z_M_BC_BA', title='m_Z_M;M_{#mu#mu};Entries', type='TH1F', path='MuonTrkPhys/Z', xbins=50, xmin=76000., xmax=106000.)
    myGroup.defineHistogram('ZMass;m_Z_M_BC_BC', title='m_Z_M;M_{#mu#mu};Entries', type='TH1F', path='MuonTrkPhys/Z', xbins=50, xmin=76000., xmax=106000.)
    myGroup.defineHistogram('ZMass;m_Z_M_BC_EA', title='m_Z_M;M_{#mu#mu};Entries', type='TH1F', path='MuonTrkPhys/Z', xbins=50, xmin=76000., xmax=106000.)
    myGroup.defineHistogram('ZMass;m_Z_M_BC_EC', title='m_Z_M;M_{#mu#mu};Entries', type='TH1F', path='MuonTrkPhys/Z', xbins=50, xmin=76000., xmax=106000.)
    myGroup.defineHistogram('ZMass;m_Z_M_EA_BA', title='m_Z_M;M_{#mu#mu};Entries', type='TH1F', path='MuonTrkPhys/Z', xbins=50, xmin=76000., xmax=106000.)
    myGroup.defineHistogram('ZMass;m_Z_M_EA_BC', title='m_Z_M;M_{#mu#mu};Entries', type='TH1F', path='MuonTrkPhys/Z', xbins=50, xmin=76000., xmax=106000.)
    myGroup.defineHistogram('ZMass;m_Z_M_EA_EA', title='m_Z_M;M_{#mu#mu};Entries', type='TH1F', path='MuonTrkPhys/Z', xbins=50, xmin=76000., xmax=106000.)
    myGroup.defineHistogram('ZMass;m_Z_M_EA_EC', title='m_Z_M;M_{#mu#mu};Entries', type='TH1F', path='MuonTrkPhys/Z', xbins=50, xmin=76000., xmax=106000.)
    myGroup.defineHistogram('ZMass;m_Z_M_EC_BA', title='m_Z_M;M_{#mu#mu};Entries', type='TH1F', path='MuonTrkPhys/Z', xbins=50, xmin=76000., xmax=106000.)
    myGroup.defineHistogram('ZMass;m_Z_M_EC_BC', title='m_Z_M;M_{#mu#mu};Entries', type='TH1F', path='MuonTrkPhys/Z', xbins=50, xmin=76000., xmax=106000.)
    myGroup.defineHistogram('ZMass;m_Z_M_EC_EA', title='m_Z_M;M_{#mu#mu};Entries', type='TH1F', path='MuonTrkPhys/Z', xbins=50, xmin=76000., xmax=106000.)
    myGroup.defineHistogram('ZMass;m_Z_M_EC_EC', title='m_Z_M;M_{#mu#mu};Entries', type='TH1F', path='MuonTrkPhys/Z', xbins=50, xmin=76000., xmax=106000.)

# Muons/CBMuons
    myGroup.defineHistogram('CBMuonEta,CBMuonPhi;Muons_CBMuons_eta_phi', title='Muons_CBMuons_eta_phi;eta;phi', type='TH2F', path='Muons/CBMuons', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('CBMuonEtaTight,CBMuonPhiTight;Muons_CBMuons_eta_phi_tight', title='Muons_CBMuons_eta_phi_tight;eta;phi', type='TH2F', path='Muons/CBMuons', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('CBMuonEtaMedium,CBMuonPhiMedium;Muons_CBMuons_eta_phi_medium', title='Muons_CBMuons_eta_phi_medium;eta;phi', type='TH2F', path='Muons/CBMuons', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('CBMuonD0;Muons_CBMuons_d0', title='Muons_CBMuons_d0;d0;Entries', type='TH1F', path='Muons/CBMuons', xbins=40, xmin=-1, xmax=1)
    myGroup.defineHistogram('CBMuonZ0;Muons_CBMuons_z0', title='Muons_CBMuons_z0;z0;Entries', type='TH1F', path='Muons/CBMuons', xbins=50, xmin=-1000, xmax=1000)
    myGroup.defineHistogram('CBMuonIDChi2NDF;Muons_CBMuons_tndof', title='Muons_CBMuons_tndof;TotalNumberDOF;Entries', type='TH1F', path='Muons/CBMuons', xbins=100, xmin=0, xmax=100)
    myGroup.defineHistogram('CBMuonDPTIDME;Muons_CB_ddpt_idme', title='Muons_CB_ddpt_idme;(ptID-ptME)/ptID;Entries', type='TH1F', path='Muons/CBMuons', xbins=40, xmin=-0.5, xmax=0.5)
    myGroup.defineHistogram('CBMuonPt;Muons_CBMuons_pt', title='Muons_CBMuons_pt;pT[GeV];Entries', type='TH1F', path='Muons/CBMuons', xbins=50, xmin=0, xmax=50000)
    myGroup.defineHistogram('CBMuonPt;Muons_CBMuons_pt_broad', title='Muons_CBMuons_pt_broad;pT[GeV];Entries', type='TH1F', path='Muons/CBMuons', xbins=50, xmin=0, xmax=1000000)
    myGroup.defineHistogram('CBMuonAuthor;Muons_CBMuons_Author', title='Muons_CBMuons_Author;Author;Entries', type='TH1F', path='Muons/CBMuons', xbins=20, xmin=0, xmax=20)
    myGroup.defineHistogram('CBMuonEtaMedium,CBMuonPhiMedium;Muons_CBMuons_Medium_eff', title='Muons_CBMuons_Medium_eff;eta;phi', type='TH2F', path='Muons/CBMuons', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('CBMuonEta1Triggered,CBMuonPhi1Triggered;Muons_CBMuons_1Triggered_eta_phi', title='Muons_CBMuons_eta_phi_1Triggered;eta;phi', type='TH2F', path='Muons/CBMuons', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('CBMuonEta1All,CBMuonPhi1All;Muons_CBMuons_1All_eta_phi', title='Muons_CBMuons_eta_phi_1All;eta;phi', type='TH2F', path='Muons/CBMuons', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)

# Overview/CBMuons
    myGroup.defineHistogram('CBMuonLumiBlock;Overview_CBMuons_nMuon_LB', title='Overview_CBMuons_nMuon_LB;LumiBlock;NumberOfCBMuons', type='TH1F', path='Overview/CBMuons', xbins=2500, xmin=0, xmax=2500)

# TracksID/CBMuons
    myGroup.defineHistogram('CBMuonNBHits;TracksID_CBMuons_HitContent_NBlayerHits', title='TracksID_CBMuons_HitContent_NBlayerHits;NumberOfBLayerHits;Entries', type='TH1F', path='TracksID/CBMuons', xbins=5, xmin=-0.5, xmax=4.5)
    myGroup.defineHistogram('CBMuonNPixHits;TracksID_CBMuons_HitContent_NPixelHits', title='TracksID_CBMuons_HitContent_NPixelHits;NumberOfPixelHits;Entries', type='TH1F', path='TracksID/CBMuons', xbins=10, xmin=-0.5, xmax=9.5)
    myGroup.defineHistogram('CBMuonNSCTHits;TracksID_CBMuons_HitContent_NSCTHits', title='TracksID_CBMuons_HitContent_NSCTHits;NumberOfSCTHits;Entries', type='TH1F', path='TracksID/CBMuons', xbins=20, xmin=-0.5, xmax=19.5)
    myGroup.defineHistogram('CBMuonNTRTHits;TracksID_CBMuons_HitContent_NTRTHits', title='TracksID_CBMuons_HitContent_NTRTHits;NumberOfTRTHits;Entries', type='TH1F', path='TracksID/CBMuons', xbins=50, xmin=-0.5, xmax=49.5)
    myGroup.defineHistogram('CBMuonIDChi2NDF;TracksID_CBMuons_chi2ndof', title='TracksID_CBMuons_chi2ndof;TrackFitChi2NDF;Entries', type='TH1F', path='TracksID/CBMuons', xbins=100, xmin=0, xmax=5)

# TracksME/CBMuons
    myGroup.defineHistogram('CBMuonMEChi2NDF;TracksME_CBMuons_chi2ndof', title='TracksME_CBMuons_chi2ndof;TrackFitChi2NDF;Entries', type='TH1F', path='TracksME/CBMuons', xbins=100, xmin=0, xmax=5)

# Muons/NonCBMuons
    myGroup.defineHistogram('NonCBMuonEta,NonCBMuonPhi;Muons_NonCBMuons_Origin_eta_phi', title='Muons_NonCBMuons_Origin_eta_phi;eta;phi;', type='TH2F', path='Muons/NonCBMuons', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('NonCBMuonEta,NonCBMuonPhi;Muons_NonCBMuons_eta_phi', title='Muons_NonCBMuons_eta_phi;eta;phi', type='TH2F', path='Muons/NonCBMuons', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('NonCBMuonEtaTight,NonCBMuonPhiTight;Muons_NonCBMuons_eta_phi_tight', title='Muons_NonCBMuons_eta_phi_tight;eta;phi', type='TH2F', path='Muons/NonCBMuons', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('NonCBMuonEtaMedium,NonCBMuonPhiMedium;Muons_NonCBMuons_eta_phi_medium', title='Muons_NonCBMuons_eta_phi_medium;eta;phi', type='TH2F', path='Muons/NonCBMuons', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('NonCBMuonD0;Muons_NonCBMuons_d0', title='Muons_NonCBMuons_d0;d0;Entries', type='TH1F', path='Muons/NonCBMuons', xbins=40, xmin=-1, xmax=1)
    myGroup.defineHistogram('NonCBMuonZ0;Muons_NonCBMuons_z0', title='Muons_NonCBMuons_z0;z0;Entries', type='TH1F', path='Muons/NonCBMuons', xbins=50, xmin=-1000, xmax=1000)
    myGroup.defineHistogram('CBMuonIDChi2NDF;Muons_CBMuons_tndof', title='Muons_CBMuons_tndof;TotalNumberDOF;Entries', type='TH1F', path='NoTrig/Muons/CBMuons', xbins=100, xmin=0, xmax=100)
    myGroup.defineHistogram('NonCBMuonDPTIDME;Muons_CB_ddpt_idme', title='Muons_CB_ddpt_idme;(ptID-ptME)/ptID;Entries', type='TH1F', path='Muons/NonCBMuons', xbins=40, xmin=-0.5, xmax=0.5)
    myGroup.defineHistogram('NonCBMuonPt;Muons_NonCBMuons_pt', title='Muons_NonCBMuons_pt;pT[GeV];Entries', type='TH1F', path='Muons/NonCBMuons', xbins=50, xmin=0, xmax=50000)
    myGroup.defineHistogram('NonCBMuonPt;Muons_NonCBMuons_pt_broad', title='Muons_NonCBMuons_pt_broad;pT[GeV];Entries', type='TH1F', path='Muons/NonCBMuons', xbins=50, xmin=0, xmax=1000000)
    myGroup.defineHistogram('NonCBMuonAuthor;Muons_NonCBMuons_Author', title='Muons_NonCBMuons_Author;Author;Entries', type='TH1F', path='Muons/NonCBMuons', xbins=20, xmin=0, xmax=20)
    myGroup.defineHistogram('NonCBMuonEtaMedium,NonCBMuonPhiMedium;Muons_NonCBMuons_Medium_eff', title='Muons_NonCBMuons_Medium_eff;eta;phi', type='TH2F', path='Muons/NonCBMuons', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('NonCBMuonEta1Triggered,NonCBMuonPhi1Triggered;Muons_NonCBMuons_1Triggered_eta_phi', title='Muons_NonCBMuons_eta_phi_1Triggered;eta;phi', type='TH2F', path='Muons/NonCBMuons', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('NonCBMuonEta1All,NonCBMuonPhi1All;Muons_NonCBMuons_1All_eta_phi', title='Muons_NonCBMuons_eta_phi_1All;eta;phi', type='TH2F', path='Muons/NonCBMuons', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)

# Overview/NonCBMuons
    myGroup.defineHistogram('NonCBMuonLumiBlock;Overview_NonCBMuons_nMuon_LB', title='Overview_NonCBMuons_nMuon_LB;LumiBlock;NumberOfNonCBMuons', type='TH1F', path='Overview/NonCBMuons', xbins=2500, xmin=0, xmax=2500)

# TracksID/NonCBMuons
    myGroup.defineHistogram('NonCBMuonNBHits;TracksID_NonCBMuons_HitContent_NBlayerHits', title='TracksID_NonCBMuons_HitContent_NBlayerHits;NumberOfBLayerHits;Entries', type='TH1F', path='TracksID/NonCBMuons', xbins=5, xmin=-0.5, xmax=4.5)
    myGroup.defineHistogram('NonCBMuonNPixHits;TracksID_NonCBMuons_HitContent_NPixelHits', title='TracksID_NonCBMuons_HitContent_NPixelHits;NumberOfPixelHits;Entries', type='TH1F', path='TracksID/NonCBMuons', xbins=10, xmin=-0.5, xmax=9.5)
    myGroup.defineHistogram('NonCBMuonNSCTHits;TracksID_NonCBMuons_HitContent_NSCTHits', title='TracksID_NonCBMuons_HitContent_NSCTHits;NumberOfSCTHits;Entries', type='TH1F', path='TracksID/NonCBMuons', xbins=20, xmin=-0.5, xmax=19.5)
    myGroup.defineHistogram('NonCBMuonNTRTHits;TracksID_NonCBMuons_HitContent_NTRTHits', title='TracksID_NonCBMuons_HitContent_NTRTHits;NumberOfTRTHits;Entries', type='TH1F', path='TracksID/NonCBMuons', xbins=50, xmin=-0.5, xmax=49.5)
    myGroup.defineHistogram('NonCBMuonIDChi2NDF;TracksID_NonCBMuons_chi2ndof', title='TracksID_NonCBMuons_chi2ndof;TrackFitChi2NDF;Entries', type='TH1F', path='TracksID/NonCBMuons', xbins=100, xmin=0, xmax=5)

# TracksME/NonCBMuons
    myGroup.defineHistogram('NonCBMuonMEChi2NDF;TracksME_NonCBMuons_chi2ndof', title='TracksME_NonCBMuons_chi2ndof;TrackFitChi2NDF;Entries', type='TH1F', path='TracksME/NonCBMuons', xbins=100, xmin=0, xmax=5)

# Segments/CBMuons
    myGroup.defineHistogram('CBMuonSector,CBMuonCIndex;Segments_CBMuons_chamberIndex_perSector', title='Segments_CBMuons_chamberIndex_perSector;Sector;ChamberIndex', type='TH2F', path='Segments/CBMuons', xbins=30, xmin=-15, xmax=15, ybins=17, ymin=0, ymax=17)
    myGroup.defineHistogram('CBMuonSectorEta,CBMuonSectorPhi;Segments_CBMuons_etaphidir', title='Segments_CBMuons_etaphidir;#eta_{dir};#varphi_{dir}', type='TH2F', path='Segments/CBMuons', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('CBMuonLargeSectorZ,CBMuonLargeSectorR;Segments_CBMuons_rzpos_sectorLarge', title='Segments_CBMuons_rzpos_sectorLarge;zPos[mm];r[mm]', type='TH2F', path='Segments/CBMuons', xbins=220, xmin=-22000, xmax=22000, ybins=100, ymin=0, ymax=15000)
    myGroup.defineHistogram('CBMuonSmallSectorZ,CBMuonSmallSectorR;Segments_CBMuons_rzpos_sectorSmall', title='Segments_CBMuons_rzpos_sectorSmall;zPos[mm];r[mm]', type='TH2F', path='Segments/CBMuons', xbins=220, xmin=-22000, xmax=22000, ybins=100, ymin=0, ymax=15000)
    myGroup.defineHistogram('CBMuonSegmentFitChi2NDF;Segments_CBMuons_segmentfitChi2oNdof', title='Segments_CBMuons_segmentfitChi2oNdof;SegmentFit#Chi^{2}/N_{dof};Entries', type='TH1F', path='Segments/CBMuons', xbins=120, xmin=0, xmax=12)
    myGroup.defineHistogram('CBMuonSegmentT0;Segments_CBMuons_t0', title='Segments_CBMuons_t0;t_{0};Entries', type='TH1F', path='Segments/CBMuons', xbins=200, xmin=-25, xmax=25)
    myGroup.defineHistogram('CBMuonSegmentT0Err;Segments_CBMuons_t0err', title='Segments_CBMuons_t0err;t_{0}Error;Entries', type='TH1F', path='Segments/CBMuons', xbins=100, xmin=0, xmax=10)
    myGroup.defineHistogram('CBMuonSegmentXYPosEndcap;Segments_CBMuons_xypos_endcap', title='Segments_CBMuons_xypos_endcap;x_{pos};y_{pos}', type='TH1F', path='Segments/CBMuons', xbins=24, xmin=-12000, xmax=12000, ybins=24, ymin=-12000, ymax=12000)
    myGroup.defineHistogram('CBMuonSegmentXYPosBarrel;Segments_CBMuons_xypos_barrel', title='Segments_CBMuons_xypos_barrel;x_{pos};y_{pos}', type='TH1F', path='Segments/CBMuons', xbins=24, xmin=-12000, xmax=12000, ybins=24, ymin=-12000, ymax=12000)

# Segments/NonCBMuons
    myGroup.defineHistogram('CBMuonSector,CBMuonCIndex;Segments_NonCBMuons_chamberIndex_perSector', title='Segments_NonCBMuons_chamberIndex_perSector;Sector;ChamberIndex', type='TH2F', path='Segments/NonCBMuons', xbins=30, xmin=-15, xmax=15, ybins=17, ymin=0, ymax=17)
    myGroup.defineHistogram('CBMuonSectorEta,CBMuonSectorPhi;Segments_NonCBMuons_etaphidir', title='Segments_NonCBMuons_etaphidir;#eta_{dir};#varphi_{dir}', type='TH2F', path='Segments/NonCBMuons', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('CBMuonLargeSectorZ,CBMuonLargeSectorR;Segments_NonCBMuons_rzpos_sectorLarge', title='Segments_NonCBMuons_rzpos_sectorLarge;zPos[mm];r[mm]', type='TH2F', path='Segments/NonCBMuons', xbins=220, xmin=-22000, xmax=22000, ybins=100, ymin=0, ymax=15000)
    myGroup.defineHistogram('CBMuonSmallSectorZ,CBMuonSmallSectorR;Segments_NonCBMuons_rzpos_sectorSmall', title='Segments_NonCBMuons_rzpos_sectorSmall;zPos[mm];r[mm]', type='TH2F', path='Segments/NonCBMuons', xbins=220, xmin=-22000, xmax=22000, ybins=100, ymin=0, ymax=15000)
    myGroup.defineHistogram('CBMuonSegmentFitChi2NDF;Segments_NonCBMuons_segmentfitChi2oNdof', title='Segments_NonCBMuons_segmentfitChi2oNdof;SegmentFit#Chi^{2}/N_{dof};Entries', type='TH1F', path='Segments/NonCBMuons', xbins=120, xmin=0, xmax=12)
    myGroup.defineHistogram('CBMuonSegmentT0;Segments_NonCBMuons_t0', title='Segments_NonCBMuons_t0;t_{0};Entries', type='TH1F', path='Segments/NonCBMuons', xbins=200, xmin=-25, xmax=25)
    myGroup.defineHistogram('CBMuonSegmentT0Err;Segments_NonCBMuons_t0err', title='Segments_NonCBMuons_t0err;t_{0}Error;Entries', type='TH1F', path='Segments/NonCBMuons', xbins=100, xmin=0, xmax=10)
    myGroup.defineHistogram('CBMuonSegmentXYPosEndcap;Segments_NonCBMuons_xypos_endcap', title='Segments_NonCBMuons_xypos_endcap;x_{pos};y_{pos}', type='TH1F', path='Segments/NonCBMuons', xbins=24, xmin=-12000, xmax=12000, ybins=24, ymin=-12000, ymax=12000)
    myGroup.defineHistogram('CBMuonSegmentXYPosBarrel;Segments_NonCBMuons_xypos_barrel', title='Segments_NonCBMuons_xypos_barrel;x_{pos};y_{pos}', type='TH1F', path='Segments/NonCBMuons', xbins=24, xmin=-12000, xmax=12000, ybins=24, ymin=-12000, ymax=12000)

# NoTrig/Segments/CBMuons
    myGroup.defineHistogram('NoTrigCBMuonSector,CBMuonCIndex;Segments_NoTrig_CBMuons_chamberIndex_perSector', title='Segments_NoTrig_CBMuons_chamberIndex_perSector;Sector;ChamberIndex', type='TH2F', path='NoTrig/Segments/CBMuons', xbins=30, xmin=-15, xmax=15, ybins=17, ymin=0, ymax=17)
    myGroup.defineHistogram('NoTrigCBMuonSectorEta,NoTrigCBMuonSectorPhi;Segments_NoTrig_CBMuons_etaphidir', title='Segments_NoTrig_CBMuons_etaphidir;#eta_{dir};#varphi_{dir}', type='TH2F', path='NoTrig/Segments/CBMuons', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('NoTrigCBMuonLargeSectorZ,NoTrigCBMuonLargeSectorR;Segments_NoTrig_CBMuons_rzpos_sectorLarge', title='Segments_NoTrig_CBMuons_rzpos_sectorLarge;zPos[mm];r[mm]', type='TH2F', path='NoTrig/Segments/CBMuons', xbins=220, xmin=-22000, xmax=22000, ybins=100, ymin=0, ymax=15000)
    myGroup.defineHistogram('NoTrigCBMuonSmallSectorZ,NoTrigCBMuonSmallSectorR;Segments_NoTrig_CBMuons_rzpos_sectorSmall', title='Segments_NoTrig_CBMuons_rzpos_sectorSmall;zPos[mm];r[mm]', type='TH2F', path='NoTrig/Segments/CBMuons', xbins=220, xmin=-22000, xmax=22000, ybins=100, ymin=0, ymax=15000)
    myGroup.defineHistogram('NoTrigCBMuonSegmentFitChi2NDF;Segments_NoTrig_CBMuons_segmentfitChi2oNdof', title='Segments_NoTrig_CBMuons_segmentfitChi2oNdof;SegmentFit#Chi^{2}/N_{dof};Entries', type='TH1F', path='NoTrig/Segments/CBMuons', xbins=120, xmin=0, xmax=12)
    myGroup.defineHistogram('NoTrigCBMuonSegmentT0;Segments_NoTrig_CBMuons_t0', title='Segments_NoTrig_CBMuons_t0;t_{0};Entries', type='TH1F', path='NoTrig/Segments/CBMuons', xbins=200, xmin=-25, xmax=25)
    myGroup.defineHistogram('NoTrigCBMuonSegmentT0Err;Segments_NoTrig_CBMuons_t0err', title='Segments_NoTrig_CBMuons_t0err;t_{0}Error;Entries', type='TH1F', path='NoTrig/Segments/CBMuons', xbins=100, xmin=0, xmax=10)
    myGroup.defineHistogram('NoTrigCBMuonSegmentXYPosEndcap;Segments_NoTrig_CBMuons_xypos_endcap', title='Segments_NoTrig_CBMuons_xypos_endcap;x_{pos};y_{pos}', type='TH1F', path='NoTrig/Segments/CBMuons', xbins=24, xmin=-12000, xmax=12000, ybins=24, ymin=-12000, ymax=12000)
    myGroup.defineHistogram('NoTrigCBMuonSegmentXYPosBarrel;Segments_NoTrig_CBMuons_xypos_barrel', title='Segments_NoTrig_CBMuons_xypos_barrel;x_{pos};y_{pos}', type='TH1F', path='NoTrig/Segments/CBMuons', xbins=24, xmin=-12000, xmax=12000, ybins=24, ymin=-12000, ymax=12000)

# NoTrig/NoTrigSegments/NonCBMuons
    myGroup.defineHistogram('NoTrigNonCBMuonSector,CBMuonCIndex;Segments_NoTrig_NonCBMuons_chamberIndex_perSector', title='Segments_NoTrig_NonCBMuons_chamberIndex_perSector;Sector;ChamberIndex', type='TH2F', path='NoTrigSegments/NonCBMuons', xbins=30, xmin=-15, xmax=15, ybins=17, ymin=0, ymax=17)
    myGroup.defineHistogram('NoTrigNonCBMuonSectorEta,NoTrigNonCBMuonSectorPhi;Segments_NoTrig_NonCBMuons_etaphidir', title='Segments_NoTrig_NonCBMuons_etaphidir;#eta_{dir};#varphi_{dir}', type='TH2F', path='NoTrigSegments/NonCBMuons', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('NoTrigNonCBMuonLargeSectorZ,NoTrigNonCBMuonLargeSectorR;Segments_NoTrig_NonCBMuons_rzpos_sectorLarge', title='Segments_NoTrig_NonCBMuons_rzpos_sectorLarge;zPos[mm];r[mm]', type='TH2F', path='NoTrigSegments/NonCBMuons', xbins=220, xmin=-22000, xmax=22000, ybins=100, ymin=0, ymax=15000)
    myGroup.defineHistogram('NoTrigNonCBMuonSmallSectorZ,NoTrigNonCBMuonSmallSectorR;Segments_NoTrig_NonCBMuons_rzpos_sectorSmall', title='Segments_NoTrig_NonCBMuons_rzpos_sectorSmall;zPos[mm];r[mm]', type='TH2F', path='NoTrigSegments/NonCBMuons', xbins=220, xmin=-22000, xmax=22000, ybins=100, ymin=0, ymax=15000)
    myGroup.defineHistogram('NoTrigNonCBMuonSegmentFitChi2NDF;Segments_NoTrig_NonCBMuons_segmentfitChi2oNdof', title='Segments_NoTrig_NonCBMuons_segmentfitChi2oNdof;SegmentFit#Chi^{2}/N_{dof};Entries', type='TH1F', path='NoTrigSegments/NonCBMuons', xbins=120, xmin=0, xmax=12)
    myGroup.defineHistogram('NoTrigNonCBMuonSegmentT0;Segments_NoTrig_NonCBMuons_t0', title='Segments_NoTrig_NonCBMuons_t0;t_{0};Entries', type='TH1F', path='NoTrigSegments/NonCBMuons', xbins=200, xmin=-25, xmax=25)
    myGroup.defineHistogram('NoTrigNonCBMuonSegmentT0Err;Segments_NoTrig_NonCBMuons_t0err', title='Segments_NoTrig_NonCBMuons_t0err;t_{0}Error;Entries', type='TH1F', path='NoTrigSegments/NonCBMuons', xbins=100, xmin=0, xmax=10)
    myGroup.defineHistogram('NoTrigNonCBMuonSegmentXYPosEndcap;Segments_NoTrig_NonCBMuons_xypos_endcap', title='Segments_NoTrig_NonCBMuons_xypos_endcap;x_{pos};y_{pos}', type='TH1F', path='NoTrigSegments/NonCBMuons', xbins=24, xmin=-12000, xmax=12000, ybins=24, ymin=-12000, ymax=12000)
    myGroup.defineHistogram('NoTrigNonCBMuonSegmentXYPosBarrel;Segments_NoTrig_NonCBMuons_xypos_barrel', title='Segments_NoTrig_NonCBMuons_xypos_barrel;x_{pos};y_{pos}', type='TH1F', path='NoTrigSegments/NonCBMuons', xbins=24, xmin=-12000, xmax=12000, ybins=24, ymin=-12000, ymax=12000)

# TracksMS/Container
    myGroup.defineHistogram('MSAuthor;MSAuthor', title='MSAuthor;MS-Author;Entries', type='TH1F', path='TracksMS/Container', xbins=15, xmin=0, xmax=15)
    myGroup.defineHistogram('MSQuality;MSQuality', title='MSQuality;MS-Quality;Entries', type='TH1F', path='TracksMS/Container', xbins=15, xmin=0, xmax=15)
    myGroup.defineHistogram('MSType;MSType', title='MSType;MS-Type;Entries', type='TH1F', path='TracksMS/Container', xbins=15, xmin=0, xmax=15)
    myGroup.defineHistogram('MSEta,MSPhi;TracksMS_Container_eta_phi', title='TracksMS_Container_eta_phi;eta;phi', type='TH2F', path='TracksMS/Container', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('MSPt;TracksMS_Container_pt', title='TracksMS_Container_pt;pT[GeV];Entries', type='TH1F', path='TracksMS/Container', xbins=50, xmin=0, xmax=50000)
    myGroup.defineHistogram('MSPt;TracksMS_Container_pt_broad', title='TracksMS_Container_pt_broad;pT[GeV];Entries', type='TH1F', path='TracksMS/Container', xbins=50, xmin=0, xmax=1000000)

# Overview/Container
    myGroup.defineHistogram('MSLumiBlockNumberOfMuonTracks;Overview_Container_nMuonTrack_LB', title='Overview_Container_nMuonTrack_LB;LumiBlock;NumberOfMuonTracks', type='TH1F', path='Overview/Container', xbins=50, xmin=0, xmax=2500)
    myGroup.defineHistogram('MSLumiBlockNumberOfSegments;Overview_Container_nSegment_LB', title='Overview_Container_nSegment_LB;LumiBlock;NumberOfMuonSegments', type='TH1F', path='Overview/Container', xbins=50, xmin=0, xmax=2500)


######################################################################################################################

# Segments/Container
    myGroup.defineHistogram('MSLargeSectorZ,MSLargeSectorR;Segments_Container_rzpos_sectorLarge', title='Segments_Container_rzpos_sectorLarge;zPos[mm];r[mm]', type='TH2F', path='Segments/Container', xbins=220, xmin=-22000, xmax=22000, ybins=100, ymin=0, ymax=15000)
    myGroup.defineHistogram('MSSmallSectorZ,MSSmallSectorR;Segments_Container_rzpos_sectorSmall', title='Segments_Container_rzpos_sectorSmall;zPos[mm];r[mm]', type='TH2F', path='Segments/Container', xbins=220, xmin=-22000, xmax=22000, ybins=100, ymin=0, ymax=15000)

# MuonPhysics/NoTrig/MSVertices
    myGroup.defineHistogram('nMDT;NoTrig_MSVertices_m_MSVx_nMDT', title='NoTrig_MSVertices_m_MSVx_nMDT;;N_{MDT}', type='TH1F', path='NoTrig/MSVertices', xbins=200, xmin=0, xmax=3000)
    myGroup.defineHistogram('nRPC;NoTrig_MSVertices_m_MSVx_nRPC', title='NoTrig_MSVertices_m_MSVx_nRPC;;N_{RPC}', type='TH1F', path='NoTrig/MSVertices', xbins=100, xmin=0, xmax=1000)
    myGroup.defineHistogram('nTGC;NoTrig_MSVertices_m_MSVx_nTGC', title='NoTrig_MSVertices_m_MSVx_nTGC;;N_{TGC}', type='TH1F', path='NoTrig/MSVertices', xbins=100, xmin=0, xmax=1000)
    myGroup.defineHistogram('nTracklets;NoTrig_MSVertices_m_MSVx_nTracklets', title='NoTrig_MSVertices_m_MSVx_nTracklets;N_{trackletS};', type='TH1F', path='NoTrig/MSVertices', xbins=20, xmin=0, xmax=20)
    myGroup.defineHistogram('MSEta,MSPhi;NoTrig_MSVertices_m_VertexEtaPhi', title='NoTrig_MSVertices_m_VertexEtaPhi;#eta;#varphi', type='TH2F', path='NoTrig/MSVertices', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)

# MuonPhysics/NoTrig/Muons/CBMuons/
    myGroup.defineHistogram('CBMuonAuthorNoTrig;NoTrig_Muons_CBMuons_author', title='NoTrig_Muons_CBMuons_author;Author;Entries', type='TH1F', path='NoTrig/Muons/CBMuons', xbins=20, xmin=0, xmax=20)
    myGroup.defineHistogram('CBMuonIDChi2NDFNoTrig;NoTrig_Muons_CBMuons_chi2ndof', title='NoTrig_Muons_CBMuons_chi2ndof;TrackFitChi2/ndof', type='TH1F', path='NoTrig/Muons/CBMuons', xbins=220, xmin=0, xmax=5)
    myGroup.defineHistogram('CBMuonD0NoTrig;NoTrig_Muons_CBMuons_d0', title='NoTrig_Muons_CBMuons_d0;d0;Entries', type='TH1F', path='NoTrig/Muons/CBMuons', xbins=40, xmin=-1, xmax=1)
    myGroup.defineHistogram('CBMuonDPTIDMENoTrig;NoTrig_Muons_CBMuons_ddpt_idme', title='NoTrig_Muons_CBMuons_ddpt_idme;(ptID-ptME)/ptID;Entries', type='TH1F', path='NoTrig/Muons/CBMuons', xbins=50, xmin=-5, xmax=5)
    myGroup.defineHistogram('CBMuonDPTCBMENoTrig;NoTrig_Muons_CBMuons_ddpt_cbme', title='NoTrig_Muons_CBMuons_ddpt_cbme;(ptCB-ptME)/ptCB;Entries', type='TH1F', path='NoTrig/Muons/CBMuons', xbins=200, xmin=-5, xmax=5)
    myGroup.defineHistogram('CBMuonEtaNoTrig;NoTrig_Muons_CBMuons_eta', title='NoTrig_Muons_CBMuons_eta;#eta;Entries', type='TH1F', path='NoTrig/Muons/CBMuons', xbins=50, xmin=-2.7, xmax=2.7)
    myGroup.defineHistogram('CBMuonType;NoTrig_Muons_CBMuons_muonType', title='NoTrig_Muons_CBMuons_muonType;MuonType;Entries', type='TH1F', path='NoTrig/Muons/CBMuons', xbins=6, xmin=-0.5, xmax=5.5)
    myGroup.defineHistogram('CBMuonPhiNoTrig;NoTrig_Muons_CBMuons_phi', title='NoTrig_Muons_CBMuons_phi;#varphi;Entries', type='TH1F', path='NoTrig/Muons/CBMuons', xbins=100, xmin=-3.14, xmax=3.14)
    myGroup.defineHistogram('CBMuonPtNoTrig;NoTrig_Muons_CBMuons_pt', title='NoTrig_Muons_CBMuons_pt;p_{T};Entries', type='TH1F', path='NoTrig/Muons/CBMuons', xbins=100, xmin=0, xmax=200)
    myGroup.defineHistogram('CBMuonPtNoTrig;NoTrig_Muons_CBMuons_pt_broad', title='NoTrig_Muons_CBMuons_pt_broad;p_{T};Entries', type='TH1F', path='NoTrig/Muons/CBMuons', xbins=100, xmin=0, xmax=1000)
    myGroup.defineHistogram('CBMuonIDChi2NDFNoTrig;NoTrig_Muons_CBMuons_tndof', title='NoTrig_Muons_CBMuons_tndof;TrackFitChi2NDF;Entries', type='TH1F', path='NoTrig/Muons/CBMuons', xbins=100, xmin=0, xmax=100)
    myGroup.defineHistogram('CBMuonZ0NoTrig;NoTrig_Muons_CBMuons_z0', title='NoTrig_Muons_CBMuons_z0;SignedImpactParameterZ0(mm);Entries', type='TH1F', path='NoTrig/Muons/CBMuons', xbins=100, xmin=-200, xmax=200)
    myGroup.defineHistogram('CBMuonEtaNoTrig,CBMuonPhiNoTrig;NoTrig_Muons_CBMuons_eta_phi', title='NoTrig_Muons_CBMuons_eta_phi;eta;phi', type='TH2F', path='NoTrig/Muons/CBMuons', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('CBMuonEtaTightNoTrig,CBMuonPhiTightNoTrig;NoTrig_Muons_CBMuons_eta_phi_tight', title='NoTrig_Muons_CBMuons_eta_phi_tight;eta;phi', type='TH2F', path='NoTrig/Muons/CBMuons', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('CBMuonEtaMediumNoTrig,CBMuonPhiMediumNoTrig;NoTrig_Muons_CBMuons_eta_phi_medium', title='NoTrig_Muons_CBMuons_eta_phi_medium;eta;phi', type='TH2F', path='NoTrig/Muons/CBMuons', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('CBMuonEtaMediumNoTrig,CBMuonPhiMediumNoTrig;NoTrig_Muons_CBMuons_Medium_eff', title='NoTrig_Muons_CBMuons_Medium_eff;eta;phi', type='TH2F', path='NoTrig/Muons/CBMuons', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('CBMuonEta1NoTrig,CBMuonPhi1NoTrig;NoTrig_Muons_CBMuons_1Triggered_eta_phi', title='NoTrig_Muons_CBMuons_eta_phi_1Triggered;eta;phi', type='TH2F', path='NoTrig/Muons/CBMuons', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('CBMuonEta1AllNoTrig,CBMuonPhi1AllNoTrig;NoTrig_Muons_CBMuons_1All_eta_phi', title='NoTrig_Muons_CBMuons_eta_phi_1All;eta;phi', type='TH2F', path='NoTrig/Muons/CBMuons', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)

# MuonPhysics/NoTrig/Muons/NonCBMuons/
    myGroup.defineHistogram('NonCBMuonAuthor;NoTrig_Muons_NonCBMuons_author', title='NoTrig_Muons_NonCBMuons_author;Author;Entries', type='TH1F', path='NoTrig/Muons/NonCBMuons', xbins=20, xmin=0, xmax=20)
    myGroup.defineHistogram('NonCBMuonIDChi2NDF;NoTrig_Muons_NonCBMuons_chi2ndof', title='NoTrig_Muons_NonCBMuons_chi2ndof;TrackFitChi2/ndof', type='TH1F', path='NoTrig/Muons/NonCBMuons', xbins=220, xmin=0, xmax=5)
    myGroup.defineHistogram('NonCBMuonD0;NoTrig_Muons_NonCBMuons_d0', title='NoTrig_Muons_NonCBMuons_d0;d0;Entries', type='TH1F', path='NoTrig/Muons/NonCBMuons', xbins=40, xmin=-1, xmax=1)
    myGroup.defineHistogram('NonCBMuonDPTIDME;NoTrig_Muons_NonCBMuons_ddpt_idme', title='NoTrig_Muons_NonCBMuons_ddpt_idme;(ptID-ptME)/ptID;Entries', type='TH1F', path='NoTrig/Muons/NonCBMuons', xbins=50, xmin=-5, xmax=5)
    myGroup.defineHistogram('NonCBMuonDPTCBME;NoTrig_Muons_NonCBMuons_ddpt_cbme', title='NoTrig_Muons_NonCBMuons_ddpt_cbme;(ptCB-ptME)/ptCB;Entries', type='TH1F', path='NoTrig/Muons/NonCBMuons', xbins=200, xmin=-5, xmax=5)
    myGroup.defineHistogram('NonCBMuonEta;NoTrig_Muons_NonCBMuons_eta', title='NoTrig_Muons_NonCBMuons_eta;#eta;Entries', type='TH1F', path='NoTrig/Muons/NonCBMuons', xbins=50, xmin=-2.7, xmax=2.7)
    myGroup.defineHistogram('NonCBMuonType;NoTrig_Muons_NonCBMuons_muonType', title='NoTrig_Muons_NonCBMuons_muonType;MuonType;Entries', type='TH1F', path='NoTrig/Muons/NonCBMuons', xbins=6, xmin=-0.5, xmax=5.5)
    myGroup.defineHistogram('NonCBMuonPhi;NoTrig_Muons_NonCBMuons_phi', title='NoTrig_Muons_NonCBMuons_phi;#varphi;Entries', type='TH1F', path='NoTrig/Muons/NonCBMuons', xbins=100, xmin=-3.14, xmax=3.14)
    myGroup.defineHistogram('NonCBMuonPt;NoTrig_Muons_NonCBMuons_pt', title='NoTrig_Muons_NonCBMuons_pt;p_{T};Entries', type='TH1F', path='NoTrig/Muons/NonCBMuons', xbins=100, xmin=0, xmax=200)
    myGroup.defineHistogram('NonCBMuonPt;NoTrig_Muons_NonCBMuons_pt_broad', title='NoTrig_Muons_NonCBMuons_pt_broad;p_{T};Entries', type='TH1F', path='NoTrig/Muons/NonCBMuons', xbins=100, xmin=0, xmax=1000)
    myGroup.defineHistogram('NonCBMuonIDChi2NDF;NoTrig_Muons_NonCBMuons_tndof', title='NoTrig_Muons_NonCBMuons_tndof;TrackFitChi2NDF;Entries', type='TH1F', path='NoTrig/Muons/NonCBMuons', xbins=100, xmin=0, xmax=100)
    myGroup.defineHistogram('NonCBMuonZ0;NoTrig_Muons_NonCBMuons_z0', title='NoTrig_Muons_NonCBMuons_z0;SignedImpactParameterZ0(mm);Entries', type='TH1F', path='NoTrig/Muons/NonCBMuons', xbins=100, xmin=-200, xmax=200)
    myGroup.defineHistogram('NonCBMuonEta,CBMuonPhi;NoTrig_Muons_NonCBMuons_eta_phi', title='NoTrig_Muons_NonCBMuons_eta_phi;eta;phi', type='TH2F', path='NoTrig/Muons/NonCBMuons', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('NonCBMuonEtaTight,CBMuonPhiTight;NoTrig_Muons_NonCBMuons_eta_phi_tight', title='NoTrig_Muons_NonCBMuons_eta_phi_tight;eta;phi', type='TH2F', path='NoTrig/Muons/NonCBMuons', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('NonCBMuonEtaMedium,CBMuonPhiMedium;NoTrig_Muons_NonCBMuons_eta_phi_medium', title='NoTrig_Muons_NonCBMuons_eta_phi_medium;eta;phi', type='TH2F', path='NoTrig/Muons/NonCBMuons', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('NonCBMuonEtaMedium,CBMuonPhiMedium;NoTrig_Muons_NonCBMuons_Medium_eff', title='NoTrig_Muons_NonCBMuons_Medium_eff;eta;phi', type='TH2F', path='NoTrig/Muons/NonCBMuons', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('NonCBMuonEta1Triggered,CBMuonPhi1Triggered;NoTrig_Muons_NonCBMuons_1Triggered_eta_phi', title='NoTrig_Muons_NonCBMuons_eta_phi_1Triggered;eta;phi', type='TH2F', path='NoTrig/Muons/NonCBMuons', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('NonCBMuonEta1All,CBMuonPhi1All;NoTrig_Muons_NonCBMuons_1All_eta_phi', title='NoTrig_Muons_NonCBMuons_eta_phi_1All;eta;phi', type='TH2F', path='NoTrig/Muons/NonCBMuons', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)


    myGroup.defineHistogram('MuonType;MuonType', title='MuonType', type='TH1F', path='Muons', xbins=10, xmin=0, xmax=10)
    #myGroup.defineHistogram('', title='', type='TH1F', path='NoTrig/Muons/CBMuons', xbins=, xmin=, xmax=)

    return helper.result()

if __name__=="__main__":
    # Setup the RunIII behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1
    
    # Setup logs
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO
    log.setLevel(INFO)
    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    TestFiles = ['/eos/atlas/atlascerngroupdisk/det-rpc/data/DESDM_MCP/data18_13TeV.00358615.physics_Main.merge.DESDM_MCP.f961_m2024/data18_13TeV.00358615.physics_Main.merge.DESDM_MCP.f961_m2024._0084.1']
    ConfigFlags.Input.Files = TestFiles
    ConfigFlags.Output.HISTFileName = 'TestOutput.root'
    ConfigFlags.lock()
    # Initialize configuration object, add accumulator, merge and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))
    acc = MuonTrackConfig(ConfigFlags)
    cfg.merge(acc)
    cfg.printConfig(withDetails=False)
    cfg.run(20)

