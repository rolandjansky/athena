# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

infile = 'aod/AOD-20.7.9.9/AOD-20.7.9.9-full.pool.root'
keys = [
    #xAOD::MuonAuxContainer_v4
    'Muons',
    'Staus',
    'HLT_xAOD__MuonContainer_MuonEFInfo',
    'HLT_xAOD__MuonContainer_MuTagIMO_EF',
    'HLT_xAOD__MuonContainer_MuonEFInfo_FullScan',
    'HLT_xAOD__MuonContainer_MuonEFInfo_MSonlyTrackParticles_FullScan',
    'HLT_xAOD__MuonContainer_eMuonEFInfo',
    
    #xAOD::MuonSegmentAuxContainer_v1
    'MuonSegments',
    'MuonTruthSegments',
    'NCB_MuonSegments',

    #xAOD::SlowMuonAuxContainer_v1
    'SlowMuons',
         ]

include ('AthenaPoolUtilities/TPCnvTest.py')
