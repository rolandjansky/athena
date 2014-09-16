# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

from TrigValRegtestFile import TrigValRegtestFile
############## this is default Regtest references  #######################

TrigValRegtestLibrary = {
   'TrigSteer' :                     TrigValRegtestFile('nofile_TrigSteer.reference'),
   'Full' :                          TrigValRegtestFile('nofile_Full.reference'),
 

    #'' :    TrigValRegtestFile(''),
   ########### egamma
    'TrigIDSCAN_eGamma' :    TrigValRegtestFile('testElectronSlice_top_CSC-01-02-00_RDO_extract_TrigIDSCAN_eGamma.reference'),
    'TrigSiTrack_eGamma' :    TrigValRegtestFile('testElectronSlice_top_CSC-01-02-00_RDO_extract_TrigSiTrack_eGamma.reference'),
    'T2CaloEgamma_eGamma' :    TrigValRegtestFile('testElectronSlice_top_CSC-01-02-00_RDO_extract_T2CaloEgamma_eGamma.reference'),
    'TrigCaloTowerMaker_eGamma' :    TrigValRegtestFile('testElectronSlice_top_CSC-01-02-00_RDO_extract_TrigCaloTowerMaker_eGamma.reference'),
    'TrigCaloCellMaker_eGamma' :    TrigValRegtestFile('testElectronSlice_top_CSC-01-02-00_RDO_extract_TrigCaloCellMaker_eGamma.reference'),
    'TrigCaloClusterMaker_slw' :    TrigValRegtestFile('testElectronSlice_top_CSC-01-02-00_RDO_extract_TrigCaloClusterMaker_slw.reference'),
    'TrigEgammaRec_eGamma' :    TrigValRegtestFile('testElectronSlice_top_CSC-01-02-00_RDO_extract_TrigEgammaRec_eGamma.reference'),
    'Electron_EFID' :    TrigValRegtestFile('testElectronSlice_top_CSC-01-02-00_RDO_extract_Electron_EFID.reference'),
    'EFTrackHypo' :    TrigValRegtestFile('testElectronSlice_top_CSC-01-02-00_RDO_extract_EFTrackHypo.reference'),
    'TrigEFEgammaHypo' :    TrigValRegtestFile('testElectronSlice_top_CSC-01-02-00_RDO_extract_TrigEFEgammaHypo.reference'),

    'L2PhotonFex' :    TrigValRegtestFile('testPhotonSlice_top_CSC-01-02-00_RDO_extract_L2PhotonFex.reference'),


    ######### tau 
    'T2CaloTau_Tau' :    TrigValRegtestFile('testTauSlice_top_CSC-01-02-00_RDO_extract_T2CaloTau_Tau.reference'),
    'TauHypo' :    TrigValRegtestFile('testTauSlice_top_CSC-01-02-00_RDO_extract_TauHypo.reference'),
    'TrigTauRec' :    TrigValRegtestFile('testTauSlice_top_CSC-01-02-00_RDO_extract_TrigTauRec.reference'),
    'Tau_EFID' :    TrigValRegtestFile('testTauSlice_top_CSC-01-02-00_RDO_extract_Tau_EFID.reference'),
    'T2TauFinal' :    TrigValRegtestFile('testTauSlice_top_CSC-01-02-00_RDO_extract_T2TauFinal.reference'),
    'TrigIDSCAN_Tau' :    TrigValRegtestFile('testTauSlice_top_CSC-01-02-00_RDO_extract_TrigIDSCAN_Tau.reference'),
    'TrigCaloCellMaker_tau' :    TrigValRegtestFile('testTauSlice_top_CSC-01-02-00_RDO_extract_TrigCaloCellMaker_tau.reference'),

    ######### muons
    'muComb_Muon' :    TrigValRegtestFile('testMuonSlice_top_CSC-01-02-00_RDO_extract_muComb_Muon.reference'),
    'TrigIDSCAN_Muon' :    TrigValRegtestFile('testMuonSlice_top_CSC-01-02-00_RDO_extract_TrigIDSCAN_Muon.reference'),
    'muFast_Muon' :    TrigValRegtestFile('testMuonSlice_top_CSC-01-02-00_RDO_extract_muFast_Muon.reference'),
    'MufastHypo_Muon' :    TrigValRegtestFile('testMuonSlice_top_CSC-01-02-00_RDO_extract_MufastHypo_Muon.reference'),
    'MucombHypo_Muon' :    TrigValRegtestFile('testMuonSlice_top_CSC-01-02-00_RDO_extract_MucombHypo_Muon.reference'),
 
    ######### Jet
    'T2CaloJet_Jet' :    TrigValRegtestFile('testJetSlice_top_CSC-01-02-00_RDO_extract_T2CaloJet_Jet.reference'),

    ######### MET
    'EFMetHypo' :    TrigValRegtestFile('testMETSlice_top_CSC-01-02-00_RDO_extract_EFMetHypo.reference'),
    'EFMissingET_Met' :    TrigValRegtestFile('testMETSlice_top_CSC-01-02-00_RDO_extract_EFMissingET_Met.reference'),

    ######### Bphys
    'Bphysics_EFID' :    TrigValRegtestFile('testBphysicsSlice_top_CSC-01-02-00_RDO_extract_Bphysics_EFID.reference'),
    'TrigIDSCAN_Bphysics' :    TrigValRegtestFile('testBphysicsSlice_top_CSC-01-02-00_RDO_extract_TrigIDSCAN_Bphysics.reference'),
    'L2DsPhiPiFex_1' :    TrigValRegtestFile('testBphysicsSlice_top_CSC-01-02-00_RDO_extract_L2DsPhiPiFex_1.reference'),
    'FullScan_EFID' :    TrigValRegtestFile('testBphysicsSlice_top_CSC-01-02-00_RDO_extract_FullScan_EFID.reference'),
    'L2DsPhiPiFex_FS' :    TrigValRegtestFile('testBphysicsSlice_top_CSC-01-02-00_RDO_extract_L2DsPhiPiFex_FS.reference'),
    'L2DsPhiPiFex_FS' :    TrigValRegtestFile('testBphysicsSlice_top_CSC-01-02-00_RDO_extract_L2DsPhiPiFex_FS.reference'),
    'TrigIDSCAN_FullScan' :    TrigValRegtestFile('testBphysicsSlice_top_CSC-01-02-00_RDO_extract_TrigIDSCAN_FullScan.reference'),
    'EFDsPhiPiFex_1' :    TrigValRegtestFile('testBphysicsSlice_top_CSC-01-02-00_RDO_extract_EFDsPhiPiFex_1.reference'),

    ######### Bjet
    'Bjet_EFID' :    TrigValRegtestFile('testBjetSlice_top_CSC-01-02-00_RDO_extract_Bjet_EFID.reference'),
    'TrigSiTrack_Jet' :    TrigValRegtestFile('testBjetSlice_top_CSC-01-02-00_RDO_extract_TrigSiTrack_Jet.reference'),

    'TrigIDSCAN_Cosmics' :    TrigValRegtestFile('testCosmicsSlice_top_CSC-01-02-00_RDO_extract_TrigIDSCAN_Cosmics.reference'),
    'TrigL2CosmicMuon_Cosmic' :    TrigValRegtestFile('testCosmicsSlice_top_CSC-01-02-00_RDO_extract_TrigL2CosmicMuon_Cosmic.reference'),
    'CombinedIDHalfTracks_Cosmic' :    TrigValRegtestFile('testCosmicsSlice_top_CSC-01-02-00_RDO_extract_CombinedIDHalfTracks_Cosmic.reference'),

    ######### LVL1
    'CTPSimulation' : TrigValRegtestFile('testLVL1CTP_top_CSC-01-02-00_RDO_extract_CTPSimulation.reference'),


#   'TriggerDecisionMaker' :            TrigValRegtestFile('TriggerDecisionMaker.reference'),
#   'IdScan_Bphysics_L2' :              TrigValRegtestFile('IdScan_Bphysics.reference'),
#   'MufastHypo_900GeV_2GeV_L2':        TrigValRegtestFile('MufastHypo_Muon_2GeV_Bphysics.reference'),
#   'TrigDiMuonFast_1_L2':              TrigValRegtestFile('TrigDiMuonFast.reference'),
#   'TrigL2BMuMuHypo_B0_L2':            TrigValRegtestFile('TrigL2BMuMuHypo_B0.reference'),
#   'JpsiHypo_1_L2' :                   TrigValRegtestFile('JpsiHypo.reference'),
#   'L2dimuonHypo_Jpsi_L2' :            TrigValRegtestFile('L2dimuonHypo_Jpsi.reference'),
#   'TrigL2BMuMuXHypo_B0_L2' :          TrigValRegtestFile('TrigL2BMuMuXHypo_B0.reference'),
#   'MucombHypo_900GeV_2GeV_L2' :       TrigValRegtestFile('muComb_Muon_Bphysics.reference'),
#   'L2dimuonHypo_Jpsi_L2' :            TrigValRegtestFile('L2dimuonHypo_Jpsi.reference'),
#   'TrigL2BMuMuXHypo_B0_L2' :          TrigValRegtestFile('TrigL2BMuMuXHypo_B0.reference'),
#   'IdScan_FullScan_L2' :              TrigValRegtestFile('IdScan_FullScan.reference'),
#   'DsPhiPiHypo_1_L2' :                TrigValRegtestFile('DsPhiPiHypo.reference'),
#   'DsPhiPiHypo_Full_L2' :             TrigValRegtestFile('DsPhiPiHypo_Full.reference'),
#   'hltSteer_L2.ResultBuilder' :       TrigValRegtestFile('hltSteer_L2.ResultBuilder.reference'),
#   'hltSteer_EF.ResultBuilder' :       TrigValRegtestFile('hltSteer_EF.ResultBuilder.reference'),

            
}


############## these are full  references  #######################

TrigValRegtestLibrary[ 'Full'].addFile( "testAthenaRDO", "testAthenaRDO_top_CSC-01-02-00_RDO_extract_full.reference")
TrigValRegtestLibrary[ 'Full'].addFile( "testCalo", "testCalo_top_CSC-01-02-00_RDO_extract_full.reference")
TrigValRegtestLibrary[ 'Full'].addFile( "testID", "testID_top_CSC-01-02-00_RDO_extract_full.reference")
TrigValRegtestLibrary[ 'Full'].addFile( "testMuon", "testMuon_top_CSC-01-02-00_RDO_extract_full.reference")
TrigValRegtestLibrary[ 'Full'].addFile( "testElectronSlice", "testElectronSlice_top_CSC-01-02-00_RDO_extract_full.reference")
TrigValRegtestLibrary[ 'Full'].addFile( "testPhotonSlice", "testPhotonSlice_top_CSC-01-02-00_RDO_extract_full.reference")
TrigValRegtestLibrary[ 'Full'].addFile( "testTauSlice", "testTauSlice_top_CSC-01-02-00_RDO_extract_full.reference")
TrigValRegtestLibrary[ 'Full'].addFile( "testMuonSlice", "testMuonSlice_top_CSC-01-02-00_RDO_extract_full.reference")
TrigValRegtestLibrary[ 'Full'].addFile( "testJetSlice", "testJetSlice_top_CSC-01-02-00_RDO_extract_full.reference")
TrigValRegtestLibrary[ 'Full'].addFile( "testMETSlice", "testMETSlice_top_CSC-01-02-00_RDO_extract_full.reference")
TrigValRegtestLibrary[ 'Full'].addFile( "testBjetSlice", "testBjetSlice_top_CSC-01-02-00_RDO_extract_full.reference")
TrigValRegtestLibrary[ 'Full'].addFile( "testBphysicsSlice", "testBphysicsSlice_top_CSC-01-02-00_RDO_extract_full.reference")
TrigValRegtestLibrary[ 'Full'].addFile( "testCosmicsSlice", "testCosmicsSlice_top_CSC-01-02-00_RDO_extract_full.reference")

############## these are additional  references  #######################

TrigValRegtestLibrary[ 'T2TauFinal'].addFile( "testCalo", "testCalo_top_CSC-01-02-00_RDO_extract_T2TauFinal.reference")
TrigValRegtestLibrary[ 'TauHypo'   ].addFile( "testCalo", "testCalo_top_CSC-01-02-00_RDO_extract_TauHypo.reference")

TrigValRegtestLibrary[ 'TrigSteer'].addFile( "testMuonSlice",      "testMuonSlice_top_CSC-01-02-00_RDO_extract_TrigSteer.reference")
TrigValRegtestLibrary[ 'TrigSteer'].addFile( "testPhotonSlice",    "testPhotonSlice_top_CSC-01-02-00_RDO_extract_TrigSteer.reference")
TrigValRegtestLibrary[ 'TrigSteer'].addFile( "testMETSlice",       "testMETSlice_top_CSC-01-02-00_RDO_extract_TrigSteer.reference")
TrigValRegtestLibrary[ 'TrigSteer'].addFile( "testElectronSlice",  "testElectronSlice_top_CSC-01-02-00_RDO_extract_TrigSteer.reference")
TrigValRegtestLibrary[ 'TrigSteer'].addFile( "testTauSlice",       "testTauSlice_top_CSC-01-02-00_RDO_extract_TrigSteer.reference")
TrigValRegtestLibrary[ 'TrigSteer'].addFile( "testBphysicsSlice",  "testBphysicsSlice_top_CSC-01-02-00_RDO_extract_TrigSteer.reference")
TrigValRegtestLibrary[ 'TrigSteer'].addFile( "testJetSlice",       "testJetSlice_top_CSC-01-02-00_RDO_extract_TrigSteer.reference")
TrigValRegtestLibrary[ 'TrigSteer'].addFile( "testBjetSlice",      "testBjetSlice_top_CSC-01-02-00_RDO_extract_TrigSteer.reference")
TrigValRegtestLibrary[ 'TrigSteer'].addFile( "testCosmicsSlice",   "testCosmicsSlice_top_CSC-01-02-00_RDO_extract_TrigSteer.reference")

############## these are  sets of references  #######################

TrigValRegtestSet = {
         'Electron' : ['T2CaloEgamma_eGamma','TrigIDSCAN_eGamma', 'TrigSiTrack_eGamma','TrigCaloTowerMaker_eGamma', 
                       'TrigCaloCellMaker_eGamma',  'TrigCaloClusterMaker_slw', 'Electron_EFID','TrigEgammaRec_eGamma', 
                       'EFTrackHypo', 'TrigEFEgammaHypo' ],
         'Photon'   : ['T2CaloEgamma_eGamma', 'L2PhotonFex','TrigCaloTowerMaker_eGamma', 
                       'TrigCaloCellMaker_eGamma',  'TrigCaloClusterMaker_slw',
                       'TrigEgammaRec_eGamma' ],
         'Tau' : ['T2CaloTau_Tau','TrigIDSCAN_Tau', 'T2TauFinal', 'TrigCaloCellMaker_tau',
                  'Tau_EFID', 'TrigTauRec', 'TauHypo'],
         'Muon'  : ['muComb_Muon', 'TrigIDSCAN_Muon', 'muFast_Muon','MufastHypo_Muon','MucombHypo_Muon' ],
         
         'Jet' : ['T2CaloJet_Jet'],
         'MET' : ['EFMetHypo', 'EFMissingET_Met'],

         'IDSCAN' : ['TrigIDSCAN_Muon', 'TrigIDSCAN_eGamma', 'TrigSiTrack_eGamma', 'TrigIDSCAN_Tau'],
         'Bphysics'  : ['L2DsPhiPiFex_1', 'TrigIDSCAN_Bphysics', 'Bphysics_EFID', 'FullScan_EFID','L2DsPhiPiFex_FS','TrigIDSCAN_FullScan'],
         'Bjet' : ['Bjet_EFID','TrigSiTrack_Jet' ],

         'Cosmics' : ['TrigIDSCAN_Cosmics', 'TrigL2CosmicMuon_Cosmic', 'CombinedIDHalfTracks_Cosmic'],

         'LVL1CTP'  : ['CTPSimulation'],

}
