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

    myGroup.defineHistogram('JPsiMuonEta,JPsiMuonPhi;Muons_Jpsi_Origin_eta_phi', title='Muons_Jpsi_Origin_eta_phi;eta;phi', type='TH2F', path='Muons/Jpsi', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('JPsiMuonEtaTight,JPsiMuonPhiTight;Muons_Jpsi_Tight_eff', title='Muons_Jpsi_Tight_eff;eta;phi', type='TH2F', path='Muons/Jpsi', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('JPsiMuonEtaMedium,JPsiMuonPhiMedium;Muons_Jpsi_Medium_eff', title='Muons_Jpsi_Medium_eff;eta;phi', type='TH2F', path='Muons/Jpsi', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('JPsiMuonD0;Muons_Jpsi_d0', title='Muons_Jpsi_d0;d0;Entries', type='TH1F', path='Muons/Jpsi', xbins=40, xmin=-1, xmax=1)
    myGroup.defineHistogram('JPsiMuonDPTIDME;Muons_Jpsi_ddpt_idme', title='Muons_Jpsi_ddpt_idme;(ptID-ptME)/ptID;Entries', type='TH1F', path='Muons/Jpsi', xbins=40, xmin=-0.5, xmax=0.5)
    myGroup.defineHistogram('JPsiMuonPt;Muons_Jpsi_pt', title='Muons_Jpsi_pt; pT[GeV];Entries', type='TH1F', path='Muons/Jpsi', xbins=50, xmin=0, xmax=50000)
    myGroup.defineHistogram('JPsiMuonPt;Muons_Jpsi_pt_broad', title='Muons_Jpsi_pt_broad;pT[GeV];Entries', type='TH1F', path='Muons/Jpsi', xbins=50, xmin=0, xmax=1000000)
    myGroup.defineHistogram('JPsiMuonLumiBlock;Overview_Jpsi_nJpsi_LB', title='Overview_Jpsi_nJpsi_LB;LumiBlock;NumberOfJPsis', type='TH1F', path='Overview/Jpsi', xbins=50, xmin=0, xmax=2500)
    myGroup.defineHistogram('JPsiMuonNBHits;TracksID_Jpsi_HitContent_NBlayerHits', title='TracksID_Jpsi_HitContent_NBlayerHits;NumberOfBLayerHits;Entries', type='TH1F', path='TracksID/Jpsi', xbins=5, xmin=0, xmax=5)
    myGroup.defineHistogram('JPsiMuonNPixHits;TracksID_Jpsi_HitContent_NPixelHits', title='TracksID_Jpsi_HitContent_NPixelHits;NumberOfPixelHits;Entries', type='TH1F', path='TracksID/Jpsi', xbins=10, xmin=0, xmax=10)
    myGroup.defineHistogram('JPsiMuonNSCTHits;TracksID_Jpsi_HitContent_NSCTHits', title='TracksID_Jpsi_HitContent_NSCTHits;NumberOfSCTHits;Entries', type='TH1F', path='TracksID/Jpsi', xbins=20, xmin=0, xmax=20)
    myGroup.defineHistogram('JPsiMuonNTRTHits;TracksID_Jpsi_HitContent_NTRTHits', title='TracksID_Jpsi_HitContent_NTRTHits;NumberOfTRTHits;Entries', type='TH1F', path='TracksID/Jpsi', xbins=50, xmin=0, xmax=50)
    myGroup.defineHistogram('JPsiMuonIDChi2NDF;TracksID_Jpsi_chi2ndof', title='TracksID_Jpsi_chi2ndof;TrackFitChi2NDF;Entries', type='TH1F', path='TracksID/Jpsi', xbins=20, xmin=0, xmax=5)
    myGroup.defineHistogram('JPsiMuonMEChi2NDF;TracksME_Jpsi_chi2ndof', title='TracksME_Jpsi_chi2ndof;TrackFitChi2NDF;Entries', type='TH1F', path='TracksME/Jpsi', xbins=20, xmin=0, xmax=5)
    myGroup.defineHistogram('JPsiMass2D,JPsiEta2D;JPsiMassAverage', title='JPsiMassAverage;mass;eta', type='TH2F', path='Muons/Jpsi', xbins=27, xmin=2600., xmax=3600, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('JPsiMass;m_Jpsi_Mass', title='m_Jpsi_Mass;M_{#mu#mu};Entries', type='TH1F', path='MuonTrkPhys/Jpsi', xbins=50, xmin=2600, xmax=3600)

    myGroup.defineHistogram('ZMuonEta,ZMuonPhi;Muons_Z_Origin_eta_phi', title='Muons_Z_Origin_eta_phi;eta;phi', type='TH2F', path='Muons/Z', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('ZMuonEtaTight,ZMuonPhiTight;Muons_Z_Tight_eff', title='Muons_Z_Tight_eff;eta;phi', type='TH2F', path='Muons/Z', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('ZMuonEtaMedium,ZMuonPhiMedium;Muons_Z_Medium_eff', title='Muons_Z_Medium_eff;eta;phi', type='TH2F', path='Muons/Z', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('ZMuonD0;Muons_Z_d0', title='Muons_Z_d0;d0;Entries', type='TH1F', path='Muons/Z', xbins=40, xmin=-1, xmax=1)
    myGroup.defineHistogram('ZMuonZ0;Muons_Z_z0', title='Muons_Z_z0;z0;Entries', type='TH1F', path='Muons/Z', xbins=50, xmin=-1000, xmax=1000)
    myGroup.defineHistogram('ZMuonDPTIDME;Muons_Z_ddpt_idme', title='Muons_Z_ddpt_idme;(ptID-ptME)/ptID;Entries', type='TH1F', path='Muons/Z', xbins=40, xmin=-0.5, xmax=0.5)
    myGroup.defineHistogram('ZMuonPt;Muons_Z_pt', title='Muons_Z_pt; pT[GeV];Entries', type='TH1F', path='Muons/Z', xbins=50, xmin=0, xmax=50000)
    myGroup.defineHistogram('ZMuonPt;Muons_Z_pt_broad', title='Muons_Z_pt_broad;pT[GeV];Entries', type='TH1F', path='Muons/Z', xbins=50, xmin=0, xmax=1000000)
    myGroup.defineHistogram('ZMuonLumiBlock;Overview_Z_nZ_LB', title='Overview_Z_nJpsi_LB;LumiBlock;NumberOfZs', type='TH1F', path='Overview/Z', xbins=50, xmin=0, xmax=2500)
    myGroup.defineHistogram('ZMuonNBHits;TracksID_Z_HitContent_NBlayerHits', title='TracksID_Z_HitContent_NBlayerHits;NumberOfBLayerHits;Entries', type='TH1F', path='TracksID/Z', xbins=5, xmin=0, xmax=5)
    myGroup.defineHistogram('ZMuonNPixHits;TracksID_Z_HitContent_NPixelHits', title='TracksID_Z_HitContent_NPixelHits;NumberOfPixelHits;Entries', type='TH1F', path='TracksID/Z', xbins=10, xmin=0, xmax=10)
    myGroup.defineHistogram('ZMuonNSCTHits;TracksID_Z_HitContent_NSCTHits', title='TracksID_Z_HitContent_NSCTHits;NumberOfSCTHits;Entries', type='TH1F', path='TracksID/Z', xbins=20, xmin=0, xmax=20)
    myGroup.defineHistogram('ZMuonNTRTHits;TracksID_Z_HitContent_NTRTHits', title='TracksID_Z_HitContent_NTRTHits;NumberOfTRTHits;Entries', type='TH1F', path='TracksID/Z', xbins=50, xmin=0, xmax=50)
    myGroup.defineHistogram('ZMuonIDChi2NDF;TracksID_Z_chi2ndof', title='TracksID_Z_chi2ndof;TrackFitChi2NDF;Entries', type='TH1F', path='TracksID/Z', xbins=20, xmin=0, xmax=5)
    myGroup.defineHistogram('ZMuonMEChi2NDF;TracksME_Z_chi2ndof', title='TracksME_Z_chi2ndof;TrackFitChi2NDF;Entries', type='TH1F', path='TracksME/Z', xbins=20, xmin=0, xmax=5)
    myGroup.defineHistogram('ZMass2D,ZEta2D;ZMassAverage', title='ZMassAverage;mass;eta', type='TH2F', path='Muons/Z', xbins=27, xmin=76000., xmax=106000., ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('ZMass;m_Z_Mass', title='m_Z_Mass;M_{#mu#mu};Entries', type='TH1F', path='MuonTrkPhys/Z', xbins=50, xmin=76000., xmax=106000.)

    myGroup.defineHistogram('CBMuonEta,CBMuonPhi;Muons_CBMuons_eta_phi', title='Muons_CBMuons_eta_phi;eta;phi', type='TH2F', path='Muons/CBMuons', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('CBMuonEtaTight,CBMuonPhiTight;Muons_CBMuons_eta_phi_tight', title='Muons_CBMuons_eta_phi_tight;eta;phi', type='TH2F', path='Muons/CBMuons', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('CBMuonEtaMedium,CBMuonPhiMedium;Muons_CBMuons_eta_phi_medium', title='Muons_CBMuons_eta_phi_medium;eta;phi', type='TH2F', path='Muons/CBMuons', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('CBMuonD0;Muons_CBMuons_d0', title='Muons_CBMuons_d0;d0;Entries', type='TH1F', path='Muons/CBMuons', xbins=40, xmin=-1, xmax=1)
    myGroup.defineHistogram('CBMuonZ0;Muons_CBMuons_z0', title='Muons_CBMuons_z0;z0;Entries', type='TH1F', path='Muons/CBMuons', xbins=50, xmin=-1000, xmax=1000)
    myGroup.defineHistogram('CBMuonDPTIDME;Muons_CB_ddpt_idme', title='Muons_CB_ddpt_idme;(ptID-ptME)/ptID;Entries', type='TH1F', path='Muons/CBMuons', xbins=40, xmin=-0.5, xmax=0.5)
    myGroup.defineHistogram('CBMuonPt;Muons_CBMuons_pt', title='Muons_CBMuons_pt;pT[GeV];Entries', type='TH1F', path='Muons/CBMuons', xbins=50, xmin=0, xmax=50000)
    myGroup.defineHistogram('CBMuonPt;Muons_CBMuons_pt_broad', title='Muons_CBMuons_pt_broad;pT[GeV];Entries', type='TH1F', path='Muons/CBMuons', xbins=50, xmin=0, xmax=1000000)
    myGroup.defineHistogram('CBMuonLumiBlock;Overview_CBMuons_nMuon_LB', title='Overview_CBMuons_nMuon_LB;LumiBlock;NumberOfCBMuons', type='TH1F', path='Overview/CBMuons', xbins=2500, xmin=0, xmax=2500)
    myGroup.defineHistogram('CBMuonNBHits;TracksID_CBMuons_HitContent_NBlayerHits', title='TracksID_CBMuons_HitContent_NBlayerHits;NumberOfBLayerHits;Entries', type='TH1F', path='TracksID/CBMuons', xbins=5, xmin=0, xmax=5)
    myGroup.defineHistogram('CBMuonNPixHits;TracksID_CBMuons_HitContent_NPixelHits', title='TracksID_CBMuons_HitContent_NPixelHits;NumberOfPixelHits;Entries', type='TH1F', path='TracksID/CBMuons', xbins=10, xmin=0, xmax=10)
    myGroup.defineHistogram('CBMuonNSCTHits;TracksID_CBMuons_HitContent_NSCTHits', title='TracksID_CBMuons_HitContent_NSCTHits;NumberOfSCTHits;Entries', type='TH1F', path='TracksID/CBMuons', xbins=20, xmin=0, xmax=20)
    myGroup.defineHistogram('CBMuonNTRTHits;TracksID_CBMuons_HitContent_NTRTHits', title='TracksID_CBMuons_HitContent_NTRTHits;NumberOfTRTHits;Entries', type='TH1F', path='TracksID/CBMuons', xbins=50, xmin=0, xmax=50)
    myGroup.defineHistogram('CBMuonIDChi2NDF;TracksID_CBMuons_chi2ndof', title='TracksID_CBMuons_chi2ndof;TrackFitChi2NDF;Entries', type='TH1F', path='TracksID/CBMuons', xbins=20, xmin=0, xmax=5)
    myGroup.defineHistogram('CBMuonMEChi2NDF;TracksME_CBMuons_chi2ndof', title='TracksME_CBMuons_chi2ndof;TrackFitChi2NDF;Entries', type='TH1F', path='TracksME/CBMuons', xbins=20, xmin=0, xmax=5)

    myGroup.defineHistogram('CBMuonAuthor;Muons_CBMuons_Author', title='Muons_CBMuons_Author;Author;Entries', type='TH1F', path='Muons/CBMuons', xbins=20, xmin=0, xmax=20)
    myGroup.defineHistogram('CBMuonEtaMedium,CBMuonPhiMedium;Muons_CBMuons_Medium_eff', title='Muons_CBMuons_Medium_eff;eta;phi', type='TH2F', path='Muons/CBMuons', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('CBMuonEta1Triggered,CBMuonPhi1Triggered;Muons_CBMuons_1Triggered_eta_phi', title='Muons_CBMuons_eta_phi_1Triggered;eta;phi', type='TH2F', path='Muons/CBMuons', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('CBMuonEta1All,CBMuonPhi1All;Muons_CBMuons_1All_eta_phi', title='Muons_CBMuons_eta_phi_1All;eta;phi', type='TH2F', path='Muons/CBMuons', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('NonCBMuonEta,NonCBMuonPhi;Muons_NonCBMuons_Origin_eta_phi', title='Muons_NonCBMuons_Origin_eta_phi;eta;phi;', type='TH2F', path='Muons/NonCBMuons', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('CBMuonSector,CBMuonCIndex;Segments_CBMuons_chamberIndex_perSector', title='Segments_CBMuons_chamberIndex_perSector;Sector;ChamberIndex', type='TH2F', path='Segments/CBMuons', xbins=30, xmin=-15, xmax=15, ybins=17, ymin=0, ymax=17)

    myGroup.defineHistogram('MSAuthor;MSAuthor', title='MSAuthor;MS-Author;Entries', type='TH1F', path='TracksMS/Container', xbins=15, xmin=0, xmax=15)
    myGroup.defineHistogram('MSQuality;MSQuality', title='MSQuality;MS-Quality;Entries', type='TH1F', path='TracksMS/Container', xbins=15, xmin=0, xmax=15)
    myGroup.defineHistogram('MSType;MSType', title='MSType;MS-Type;Entries', type='TH1F', path='TracksMS/Container', xbins=15, xmin=0, xmax=15)
    myGroup.defineHistogram('MSLumiBlockNumberOfMuonTracks;Overview_Container_nMuonTrack_LB', title='Overview_Container_nMuonTrack_LB;LumiBlock;NumberOfMuonTracks', type='TH1F', path='Overview/Container', xbins=50, xmin=0, xmax=2500)
    myGroup.defineHistogram('MSLumiBlockNumberOfSegments;Overview_Container_nSegment_LB', title='Overview_Container_nSegment_LB;LumiBlock;NumberOfMuonSegments', type='TH1F', path='Overview/Container', xbins=50, xmin=0, xmax=2500)
    myGroup.defineHistogram('MSLargeSectorR,MSLargeSectorZ;Segments_Container_rzpos_sectorLarge', title='Segments_Container_rzpos_sectorLarge;zPos[mm];r[mm]', type='TH2F', path='Segments/Container', xbins=220, xmin=-22000, xmax=22000, ybins=100, ymin=0, ymax=15000)
    myGroup.defineHistogram('MSSmallSectorR,MSSmallSectorZ;Segments_Container_rzpos_sectorSmall', title='Segments_Container_rzpos_sectorSmall;zPos[mm];r[mm]', type='TH2F', path='Segments/Container', xbins=220, xmin=-22000, xmax=22000, ybins=100, ymin=0, ymax=15000)
    myGroup.defineHistogram('MSEta,MSPhi;TracksMS_Container_eta_phi', title='TracksMS_Container_eta_phi;eta;phi', type='TH2F', path='TracksMS/Container', xbins=27, xmin=-2.7, xmax=2.7, ybins=31, ymin=-3.1415, ymax=3.1415)
    myGroup.defineHistogram('MSPt;TracksMS_Container_pt', title='TracksMS_Container_pt;pT[GeV];Entries', type='TH1F', path='TracksMS/Container', xbins=50, xmin=0, xmax=50000)
    myGroup.defineHistogram('MSPt;TracksMS_Container_pt_broad', title='TracksMS_Container_pt_broad;pT[GeV];Entries', type='TH1F', path='TracksMS/Container', xbins=50, xmin=0, xmax=1000000)
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

