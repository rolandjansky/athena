#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":
    infile = 'esd/ESD-15.6.1.pool.root'
    keys = [
        #RingerRingsContainer_tlp1
        'RingerRingsContainer#HLT_TrigT2CaloEgamma',

        #TrigEMClusterContainer_tlp1
        'TrigEMClusterContainer#HLT',
        'TrigEMClusterContainer#HLT_TrigT2CaloCosmic',
        'TrigEMClusterContainer#HLT_TrigcaloSwCluster',
        'TrigEMClusterContainer#HLT_T2CaloSwCluster',
        'TrigEMClusterContainer#HLT_TrigT2CaloEgamma',

        #TrigRNNOutputContainer_tlp1
        'TrigRNNOutputContainer#HLT_TrigRingerNeuralFex',

        #TrigT2JetContainer_tlp1
        'TrigT2JetContainer#HLT_TrigT2CosmicJet',
        'TrigT2JetContainer#HLT_TrigT2CaloJet',

        #TrigT2MbtsBitsContainer_tlp1
        'TrigT2MbtsBitsContainer#HLT_T2Mbts',

        #TrigTauClusterContainer_tlp1
        'TrigTauClusterContainer#HLT_TrigT2CaloTau',

        #TrigTauClusterDetailsContainer_tlp1
        'TrigTauClusterDetailsContainer#HLT_TrigT2CaloTauDetails',

        #TrigOperationalInfoCollection_tlp1
        #'TrigOperationalInfoCollection#HLT_OPI_L2',
        #'TrigOperationalInfoCollection#HLT_OPI_EF',

        #TrigRoiDescriptorCollection_tlp1
        #'TrigRoiDescriptorCollection#HLT',
        #'TrigRoiDescriptorCollection#HLT_T2TauFinal',
        #'TrigRoiDescriptorCollection#HLT_TrigT2CaloTau',
        #'TrigRoiDescriptorCollection#HLT_TrigT2CosmicJet',
        #'TrigRoiDescriptorCollection#HLT_forMS',
        #'TrigRoiDescriptorCollection#HLT_forID',
        #'TrigRoiDescriptorCollection#HLT_secondaryRoI_EF',
        #'TrigRoiDescriptorCollection#HLT_secondaryRoI_L2',
        #'TrigRoiDescriptorCollection#HLT_TrigT2CaloJet',
        #'TrigRoiDescriptorCollection#HLT_initialRoI',
        #'TrigRoiDescriptorCollection#HLT_TrigT2CaloEgamma',

        #TrigInDetTrackCollection_tlp1
        'TrigInDetTrackCollection#HLT',
        'TrigInDetTrackCollection#HLT_TRTxK',
        'TrigInDetTrackCollection#HLT_TrigIDSCAN_Cosmics',
        'TrigInDetTrackCollection#HLT_TrigIDSCAN_eGamma_Brem',
        'TrigInDetTrackCollection#HLT_TrigSiTrack_Cosmics',
        'TrigInDetTrackCollection#HLT_TrigSiTrack_Jet_robust',
        'TrigInDetTrackCollection#HLT_TrigSiTrack_Muon_robust',
        'TrigInDetTrackCollection#HLT_TrigSiTrack_Tau',
        'TrigInDetTrackCollection#HLT_TrigSiTrack_Tile',
        'TrigInDetTrackCollection#HLT_TrigSiTrack_Tile_robust',
        'TrigInDetTrackCollection#HLT_TrigSiTrack_muonIso_robust',
        'TrigInDetTrackCollection#HLT_TrigSiTrack_muonIso',
        'TrigInDetTrackCollection#HLT_TrigIDSCAN_Muon',
        'TrigInDetTrackCollection#HLT_TrigSiTrack_Muon',
        'TrigInDetTrackCollection#HLT_TrigIDSCAN_muonIso',
        'TrigInDetTrackCollection#HLT_TrigIDSCAN_Tile',
        'TrigInDetTrackCollection#HLT_TRTSegmentFinder',
        'TrigInDetTrackCollection#HLT_TrigSiTrack_eGamma_robust',
        'TrigInDetTrackCollection#HLT_TrigSiTrack_eGamma',
        'TrigInDetTrackCollection#HLT_TrigSiTrack_Jet',
        'TrigInDetTrackCollection#HLT_TrigIDSCAN_Jet',
        'TrigInDetTrackCollection#HLT_TrigSiTrack_Tau_robust',
        'TrigInDetTrackCollection#HLT_TrigIDSCAN_FullScan',
        'TrigInDetTrackCollection#HLT_TrigIDSCAN_eGamma',
        'TrigInDetTrackCollection#HLT_TrigSiTrack_FullScan',
        'TrigInDetTrackCollection#HLT_TrigIDSCAN_Tau',
        'TrigInDetTrackCollection#HLT_TrigSiTrack_Bphysics',
        'TrigInDetTrackCollection#HLT_TrigIDSCAN_Bphysics',

        #TrigSpacePointCountsCollection_tlp1
        'TrigSpacePointCountsCollection#HLT_spacepoints',

        #TrigTauTracksInfoCollection_tlp1
        'TrigTauTracksInfoCollection#HLT',

        #TrigTrackCountsCollection_tlp1
        'TrigTrackCountsCollection#HLT_trackcounts',

        #TrigTrtHitCountsCollection_tlp1
        'TrigTrtHitCountsCollection#HLT_TrtHitCount',

        #TrigVertexCollection_tlp1
        'TrigVertexCollection#HLT_T2HistoPrmVtx',

        #TrigMissingETContainer_tlp1
        'TrigMissingETContainer#HLT_TrigEFMissingET_FEB',
        'TrigMissingETContainer#HLT_TrigEFMissingET_noiseSupp',
        'TrigMissingETContainer#HLT_T2MissingET',
        'TrigMissingETContainer#HLT_TrigEFMissingET',

        #CombinedMuonFeatureContainer_tlp1
        'CombinedMuonFeatureContainer#HLT',

        #IsoMuonFeatureContainer_tlp1
        'IsoMuonFeatureContainer#HLT',

        #MuonFeatureContainer_tlp2
        'MuonFeatureContainer#HLT',

        #MuonFeatureDetailsContainer_tlp1
        'MuonFeatureDetailsContainer#HLT',

        #TileMuFeatureContainer_tlp1
        'TileMuFeatureContainer#HLT',

        #TileTrackMuFeatureContainer_tlp1
        'TileTrackMuFeatureContainer#HLT',

        #TrigMuonEFContainer_tlp1
        'TrigMuonEFContainer#HLT_MuonEF',

        #TrigMuonEFInfoContainer_tlp1
        'TrigMuonEFInfoContainer#HLT_MuTagIMO_EF',
        'TrigMuonEFInfoContainer#HLT_eMuonEFInfo',
        'TrigMuonEFInfoContainer#HLT_MuonEFInfo',

        #TrigEFBjetContainer_tlp2
        'TrigEFBjetContainer#HLT_EFBjetFex',

        #TrigEFBphysContainer_tlp1
        'TrigEFBphysContainer#HLT_EFDsPhiPiFex',
        'TrigEFBphysContainer#HLT_EFBMuMuFex',
        'TrigEFBphysContainer#HLT_EFMuPairs',

        #TrigElectronContainer_tlp2
        'TrigElectronContainer#HLT_L2IDCaloFex',
        'TrigElectronContainer#HLT_L2ElectronFex',

        #TrigL2BjetContainer_tlp2
        'TrigL2BjetContainer#HLT_L2BjetFex',

        #TrigL2BphysContainer_tlp1
        'TrigL2BphysContainer#HLT',
        'TrigL2BphysContainer#HLT_L2BMuMuFex',
        'TrigL2BphysContainer#HLT_L2BMuMuXFex',
        'TrigL2BphysContainer#HLT_L2DiMuXFex',
        'TrigL2BphysContainer#HLT_L2JpsieeFex',
        'TrigL2BphysContainer#HLT_L2TrackMass',
        'TrigL2BphysContainer#HLT_TrigDiMuon',
        'TrigL2BphysContainer#HLT_L2DsPhiPiFexDs',
        'TrigL2BphysContainer#HLT_L2DsPhiPiFexPhi',

        #TrigPhotonContainer_tlp2
        'TrigPhotonContainer#HLT_L2PhotonFex',

        #TrigTauContainer_tlp1
        'TrigTauContainer#HLT',
    ]

    TPCnvTest(infile, keys)
