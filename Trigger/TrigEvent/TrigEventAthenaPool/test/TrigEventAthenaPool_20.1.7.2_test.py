#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":
    infile = 'esd/ESD-20.1.7.2.pool.root'
    keys = [
        #TrigEMClusterContainer_p4
        'TrigEMClusterContainer#HLT_TrigEMClusterContainer_TrigT2CaloCosmic',

        #TrigT2MbtsBitsContainer_p3
        'TrigT2MbtsBitsContainer#HLT_TrigT2MbtsBitsContainer_T2Mbts',

        #TrigPassBitsCollection_p1
        #'TrigPassBitsCollection#HLT_TrigPassBitsCollection_passbits',

        #TrigPassFlagsCollection_p1
        #'TrigPassFlagsCollection#HLT_TrigPassFlagsCollection_isEM',
        #'TrigPassFlagsCollection#HLT_TrigPassFlagsCollection_passflags',

        #TrigRoiDescriptorCollection_p3
        #'TrigRoiDescriptorCollection#HLT_TrigRoiDescriptorCollection_TrigT2CaloJet',
        #'TrigRoiDescriptorCollection#HLT_TrigRoiDescriptorCollection_TrigT2CosmicJet',
        #'TrigRoiDescriptorCollection#HLT_TrigRoiDescriptorCollection_secondaryRoI_EF',
        #'TrigRoiDescriptorCollection#HLT_TrigRoiDescriptorCollection_secondaryRoI_L2',
        #'TrigRoiDescriptorCollection#HLT_TrigRoiDescriptorCollection_forMS',
        #'TrigRoiDescriptorCollection#HLT_TrigRoiDescriptorCollection_secondaryRoI_HLT',
        #'TrigRoiDescriptorCollection#HLT_TrigRoiDescriptorCollection_forID2',
        #'TrigRoiDescriptorCollection#HLT_TrigRoiDescriptorCollection_forID3',
        #'TrigRoiDescriptorCollection#HLT_TrigRoiDescriptorCollection_SuperRoi',
        #'TrigRoiDescriptorCollection#HLT_TrigRoiDescriptorCollection_SplitJet',
        #'TrigRoiDescriptorCollection#HLT_TrigRoiDescriptorCollection_forID1',
        #'TrigRoiDescriptorCollection#HLT_TrigRoiDescriptorCollection_TrigJetRec',
        #'TrigRoiDescriptorCollection#HLT_TrigRoiDescriptorCollection_initialRoI',
        #'TrigRoiDescriptorCollection#HLT_TrigRoiDescriptorCollection',

        #TrigInDetTrackCollection_tlp4
        'TrigInDetTrackCollection#HLT_TrigInDetTrackCollection',

        #TrigSpacePointCountsCollection_p4
        'TrigSpacePointCountsCollection#HLT_TrigSpacePointCountsCollection_spacepoints',

        #TrigTrackCountsCollection_tlp1
        'TrigTrackCountsCollection#HLT_TrigTrackCountsCollection_trackcounts',

        #TrigVertexCollection_tlp2
        'TrigVertexCollection#HLT_TrigVertexCollection_TrigBeamSpotVertex',
        'TrigVertexCollection#HLT_TrigVertexCollection_TrigL2SiTrackFinder_FullScan_ZF_Only',

        #TrigVertexCountsCollection_tlp1
        'TrigVertexCountsCollection#HLT_TrigVertexCountsCollection_vertexcounts',

        #TrigMissingETContainer_p3
        'TrigMissingETContainer#HLT_TrigMissingETContainer_EFJetEtSum',
        'TrigMissingETContainer#HLT_TrigMissingETContainer_L2JetEtSum',
        'TrigMissingETContainer#HLT_TrigMissingETContainer_T2MissingET',
        'TrigMissingETContainer#HLT_TrigMissingETContainer_L2MissingET_FEB',
        'TrigMissingETContainer#HLT_TrigMissingETContainer_TrigEFMissingET',
        'TrigMissingETContainer#HLT_TrigMissingETContainer_TrigEFMissingET_FEB',
        'TrigMissingETContainer#HLT_TrigMissingETContainer_TrigEFMissingET_topocl',
        'TrigMissingETContainer#HLT_TrigMissingETContainer_TrigEFMissingET_topocl_PS',

        #CombinedMuonFeatureContainer_p4
        'CombinedMuonFeatureContainer#HLT_CombinedMuonFeatureContainer',

        #IsoMuonFeatureContainer_p3
        'IsoMuonFeatureContainer#HLT_IsoMuonFeatureContainer',

        #MuonFeatureContainer_p3
        'MuonFeatureContainer#HLT_MuonFeatureContainer',

        #MuonFeatureDetailsContainer_p2
        'MuonFeatureDetailsContainer#HLT_MuonFeatureDetailsContainer',

        #TileMuFeatureContainer_p2
        'TileMuFeatureContainer#HLT_TileMuFeatureContainer',

        #TileTrackMuFeatureContainer_p3
        'TileTrackMuFeatureContainer#HLT_TileTrackMuFeatureContainer',

        #TrigMuonClusterFeatureContainer_tlp1
        'TrigMuonClusterFeatureContainer#HLT_TrigMuonClusterFeatureContainer_MuonCluster',

        #TrigMuonEFInfoContainer_tlp2
        'TrigMuonEFInfoContainer#HLT_TrigMuonEFInfoContainer_MuTagIMO_EF',
        'TrigMuonEFInfoContainer#HLT_TrigMuonEFInfoContainer_MuonEFInfo',
        'TrigMuonEFInfoContainer#HLT_TrigMuonEFInfoContainer_eMuonEFInfo',

        #TrigMuonEFIsolationContainer_p2
        'TrigMuonEFIsolationContainer#HLT_TrigMuonEFIsolationContainer_MuonEFIsolation',

        #TrigEFBjetContainer_tlp2
        'TrigEFBjetContainer#HLT_TrigEFBjetContainer_EFBjetFex',
    ]

    TPCnvTest(infile, keys)
