#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":
    infile = 'aod/AOD-18.0.0/AOD-18.0.0-full.pool.root'
    keys = [
        #RingerRingsContainer_p2
        'RingerRingsContainer#HLT_TrigT2CaloEgamma',

        #TrigCaloClusterContainer_p3
        'TrigCaloClusterContainer#HLT_TrigT2Calo',

        #TrigEMClusterContainer_p4
        'TrigEMClusterContainer#HLT',
        'TrigEMClusterContainer#HLT_T2CaloSwCluster',
        'TrigEMClusterContainer#HLT_TrigT2CaloCosmic',
        'TrigEMClusterContainer#HLT_TrigcaloSwCluster',
        'TrigEMClusterContainer#HLT_TrigT2CaloEgamma',

        #TrigRNNOutputContainer_p2
        'TrigRNNOutputContainer#HLT_TrigRingerNeuralFex',
        'TrigRNNOutputContainer#HLT_TrigTRTHTCounts',

        #TrigT2JetContainer_p3
        'TrigT2JetContainer#HLT_TrigT2CosmicJet',
        'TrigT2JetContainer#HLT_TrigT2CaloJet',

        #TrigT2MbtsBitsContainer_p3
        'TrigT2MbtsBitsContainer#HLT_T2Mbts',

        #TrigTauClusterContainer_p5
        'TrigTauClusterContainer#HLT_TrigT2CaloTau',

        #TrigTauClusterDetailsContainer_p2
        'TrigTauClusterDetailsContainer#HLT_TrigT2CaloTauDetails',

        #TrigPassBitsCollection_p1
        #'TrigPassBitsCollection#HLT_passbits',

        #TrigPassFlagsCollection_p1
        #'TrigPassFlagsCollection#HLT_passflags',
        #'TrigPassFlagsCollection#HLT_isEM',

        #TrigRoiDescriptorCollection_p2
        #'TrigRoiDescriptorCollection#HLT',
        #'TrigRoiDescriptorCollection#HLT_T2TauFinal',
        #'TrigRoiDescriptorCollection#HLT_TrigT2CaloTau',
        #'TrigRoiDescriptorCollection#HLT_TrigT2CosmicJet',
        #'TrigRoiDescriptorCollection#HLT_secondaryRoI_EF',
        #'TrigRoiDescriptorCollection#HLT_forMS',
        #'TrigRoiDescriptorCollection#HLT_forID',
        #'TrigRoiDescriptorCollection#HLT_secondaryRoI_L2',
        #'TrigRoiDescriptorCollection#HLT_TrigT2CaloEgamma',
        #'TrigRoiDescriptorCollection#HLT_initialRoI',
        #'TrigRoiDescriptorCollection#HLT_TrigT2CaloJet',

        #TrigInDetTrackCollection_tlp4
        'TrigInDetTrackCollection#HLT',
        'TrigInDetTrackCollection#HLT_TRTxK',
        'TrigInDetTrackCollection#HLT_TrigIDSCAN_Jet',
        'TrigInDetTrackCollection#HLT_TrigIDSCAN_Muon',
        'TrigInDetTrackCollection#HLT_TrigIDSCAN_Tile',
        'TrigInDetTrackCollection#HLT_TrigSiTrack_Jet',
        'TrigInDetTrackCollection#HLT_TrigSiTrack_Tau',
        'TrigInDetTrackCollection#HLT_TrigSiTrack_Muon',
        'TrigInDetTrackCollection#HLT_TrigSiTrack_Tile',
        'TrigInDetTrackCollection#HLT_TrigIDSCAN_eGamma',
        'TrigInDetTrackCollection#HLT_TrigIDSCAN_Bphysics',
        'TrigInDetTrackCollection#HLT_TrigIDSCAN_Cosmics',
        'TrigInDetTrackCollection#HLT_TrigIDSCAN_FullScan',
        'TrigInDetTrackCollection#HLT_TrigIDSCAN_muonIso',
        'TrigInDetTrackCollection#HLT_TrigL2SiTrackFinder',
        'TrigInDetTrackCollection#HLT_TrigSiTrack_Cosmics',
        'TrigInDetTrackCollection#HLT_TrigSiTrack_eGamma',
        'TrigInDetTrackCollection#HLT_TrigSiTrack_muonIso',
        'TrigInDetTrackCollection#HLT_TrigSiTrack_Bphysics',
        'TrigInDetTrackCollection#HLT_TrigSiTrack_FullScan',
        'TrigInDetTrackCollection#HLT_TrigIDSCAN_eGamma_Brem',
        'TrigInDetTrackCollection#HLT_TrigSiTrack_Jet_robust',
        'TrigInDetTrackCollection#HLT_TrigSiTrack_Muon_robust',
        'TrigInDetTrackCollection#HLT_TrigSiTrack_Tau_robust',
        'TrigInDetTrackCollection#HLT_TrigSiTrack_Tile_robust',
        'TrigInDetTrackCollection#HLT_TrigL2SiTrackFinder_Tile',
        'TrigInDetTrackCollection#HLT_TrigSiTrack_eGamma_robust',
        'TrigInDetTrackCollection#HLT_TrigSiTrack_muonIso_robust',
        'TrigInDetTrackCollection#HLT_TrigL2SiTrackFinder_TauCore',
        'TrigInDetTrackCollection#HLT_TrigL2SiTrackFinder_muonIso',
        'TrigInDetTrackCollection#HLT_TrigL2SiTrackFinder_Muon',
        'TrigInDetTrackCollection#HLT_TrigIDSCAN_Tau',
        'TrigInDetTrackCollection#HLT_TRTSegmentFinder',
        'TrigInDetTrackCollection#HLT_TrigL2SiTrackFinder_TauIso',
        'TrigInDetTrackCollection#HLT_TrigL2SiTrackFinder_Bphysics',
        'TrigInDetTrackCollection#HLT_TrigL2SiTrackFinder_FullScan',
        'TrigInDetTrackCollection#HLT_TrigL2SiTrackFinder_eGamma',
        'TrigInDetTrackCollection#HLT_TrigL2SiTrackFinder_Tau',
        'TrigInDetTrackCollection#HLT_TrigL2SiTrackFinder_Jet',

        #TrigSpacePointCountsCollection_p4
        'TrigSpacePointCountsCollection#HLT_spacepoints',

        #TrigTauTracksInfoCollection_p2
        'TrigTauTracksInfoCollection#HLT',

        #TrigTrackCountsCollection_tlp1
        'TrigTrackCountsCollection#HLT_trackcounts',

        #TrigTrtHitCountsCollection_p2
        'TrigTrtHitCountsCollection#HLT_TrtHitCount',

        #TrigVertexCollection_tlp2
        'TrigVertexCollection#HLT_TrigBeamSpotVertex',
        'TrigVertexCollection#HLT_TrigBeamSpotVertex_SiTrack',
        'TrigVertexCollection#HLT_EFHistoPrmVtx',
        'TrigVertexCollection#HLT_T2HistoPrmVtx',

        #TrigVertexCountsCollection_tlp1
        'TrigVertexCountsCollection#HLT_vertexcounts',

        #TrigMissingETContainer_p3
        'TrigMissingETContainer#HLT_EFJetEtSum',
        'TrigMissingETContainer#HLT_L2JetEtSum',
        'TrigMissingETContainer#HLT_T2MissingET',
        'TrigMissingETContainer#HLT_TrigEFMissingET_topocl',
        'TrigMissingETContainer#HLT_L2MissingET_FEB',
        'TrigMissingETContainer#HLT_TrigEFMissingET_FEB',

        #CombinedMuonFeatureContainer_p4
        'CombinedMuonFeatureContainer#HLT',

        #IsoMuonFeatureContainer_p3
        'IsoMuonFeatureContainer#HLT',

        #MuonFeatureContainer_p3
        'MuonFeatureContainer#HLT',

        #MuonFeatureDetailsContainer_p2
        'MuonFeatureDetailsContainer#HLT',

        #TileMuFeatureContainer_p2
        'TileMuFeatureContainer#HLT',

        #TileTrackMuFeatureContainer_p3
        'TileTrackMuFeatureContainer#HLT',

        #TrigMuonClusterFeatureContainer_tlp1
        'TrigMuonClusterFeatureContainer#HLT_MuonCluster',

        #TrigMuonEFInfoContainer_tlp2
        'TrigMuonEFInfoContainer#HLT_MuTagIMO_EF',
        'TrigMuonEFInfoContainer#HLT_eMuonEFInfo',
        'TrigMuonEFInfoContainer#HLT_MuonEFInfo',

        #TrigMuonEFIsolationContainer_p2
        'TrigMuonEFIsolationContainer#HLT_MuonEFIsolation',

        #TrigEFBjetContainer_tlp2
        'TrigEFBjetContainer#HLT_EFBjetFex',

        #TrigEFBphysContainer_tlp2
        'TrigEFBphysContainer#HLT_EFMuPairs',
        'TrigEFBphysContainer#HLT_EFBMuMuXFex',
        'TrigEFBphysContainer#HLT_EFDsPhiPiFex',
        'TrigEFBphysContainer#HLT_EFMultiMuFex',
        'TrigEFBphysContainer#HLT_EFTrackMass',
        'TrigEFBphysContainer#HLT_EFBMuMuFex',

        #TrigElectronContainer_p3
        'TrigElectronContainer#HLT_L2IDCaloFex',
        'TrigElectronContainer#HLT_L2ElectronFex',

        #TrigL2BjetContainer_p3
        'TrigL2BjetContainer#HLT_L2BjetFex',

        #TrigL2BphysContainer_tlp2
        'TrigL2BphysContainer#HLT',
        'TrigL2BphysContainer#HLT_L2DiMuXFex',
        'TrigL2BphysContainer#HLT_L2BMuMuXFex',
        'TrigL2BphysContainer#HLT_L2JpsieeFex',
        'TrigL2BphysContainer#HLT_L2MultiMuFex',
        'TrigL2BphysContainer#HLT_L2DsPhiPiFexDs',
        'TrigL2BphysContainer#HLT_L2DsPhiPiFexPhi',
        'TrigL2BphysContainer#HLT_L2TrackMass',
        'TrigL2BphysContainer#HLT_L2BMuMuFex',
        'TrigL2BphysContainer#HLT_TrigDiMuon',

        #TrigPhotonContainer_p3
        'TrigPhotonContainer#HLT_L2PhotonFex',

        #TrigTauContainer_p3
        'TrigTauContainer#HLT',

        #ElectronMuonTopoInfoContainer_p1
        'ElectronMuonTopoInfoContainer#HLT_EF_EgMuTopoFEX',
        'ElectronMuonTopoInfoContainer#HLT_EF_PhotonMuonTopoFEX',
        'ElectronMuonTopoInfoContainer#HLT_EgMuTopoFEX',
        'ElectronMuonTopoInfoContainer#HLT_L2_PhotonMuonTopoFEX',
    ]

    TPCnvTest(infile, keys)
