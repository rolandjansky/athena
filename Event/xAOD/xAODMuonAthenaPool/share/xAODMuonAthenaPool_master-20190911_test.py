# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

infile = 'aod/AOD-master-20190911/AOD-master-20190911-full.pool.root'
keys = [
    #xAOD::MuonAuxContainer_v5
    'Muons',
    'Staus',
    'HLT_xAOD__MuonContainer_MuonEFInfo',
    'HLT_xAOD__MuonContainer_MuonEFInfo_FullScan',
    'HLT_xAOD__MuonContainer_MuonEFInfo_MSonlyTrackParticles_FullScan',

    #xAOD::MuonSegmentAuxContainer_v1
    'MuGirlMuonSegments',
    'MuonSegments',
    'MuonTruthSegments',
    'NCB_MuonSegments',

    #xAOD::SlowMuonAuxContainer_v1
    'SlowMuons',
         ]

include ('AthenaPoolUtilities/TPCnvTest.py')
