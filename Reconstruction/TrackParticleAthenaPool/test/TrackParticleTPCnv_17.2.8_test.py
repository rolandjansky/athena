#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":

    infile = 'aod/AOD-17.2.8/AOD-17.2.8-full.pool.root'

    keys = [
        #Rec::TrackParticleContainer_tlp2
        'MuTagTrackParticles',
        'SegmentTagTrackParticles',
        'ResolvedForwardTrackParticles',
        'HLT_InDetTrigParticleCreation_Photon_EFID',
        'HLT_InDetTrigParticleCreation_CosmicsN_EFID',
        'HLT_InDetTrigParticleCreationTRTOnly_Tau_EFID',
        'HLT_InDetTrigParticleCreationIOTRT_CosmicsN_EFID',
        'HLT_InDetTrigParticleCreationCombined_Electron_EFID',
        'HLT_InDetTrigParticleCreationTRTOnly_FullScan_EFID',
        'HLT_InDetTrigParticleCreationTRTOnly_Muon_EFID',
        'RefittedExtrapolatedMuonSpectrometerParticles',
        'MuTagIMOTrackParticles',
        'MuonboyMuonSpectroOnlyTrackParticles',
        'HLT_InDetTrigParticleCreation_Bphysics_EFID',
        'HLT_InDetTrigParticleCreation_Muon_EFID',
        'MuonboyTrackParticles',
        'MuidExtrTrackParticles',
        'MooreTrackParticles',
        'ExtrapolatedMuonSpectrometerParticles',
        'StatCombinedMuonParticles',
        'MuonSpectrometerParticles',
        'MuidCombTrackParticles',
        'StacoTrackParticles',
        'CombinedFitMuonParticles',
        'HLT_InDetTrigParticleCreationTRTOnly_Electron_EFID',
        'HLT_InDetTrigParticleCreation_FullScan_EFID',
        'HLT_InDetTrigParticleCreation_MuonIso_EFID',
        'MuGirlRefittedTrackParticles',
        'HLT_InDetTrigParticleCreation_Tau_EFID',
        'HLT_InDetTrigParticleCreation_Bjet_EFID',
        'GSFTrackParticleCandidate',
        'HLT_InDetTrigParticleCreation_Electron_EFID',
        'TrackParticleCandidate',
    ]

    TPCnvTest(infile, keys, useGeoModelSvc=True, doPixel=True, doSCT=True, doTRT=True, doMuon=True)

