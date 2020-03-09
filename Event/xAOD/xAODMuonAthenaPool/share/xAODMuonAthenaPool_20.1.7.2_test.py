# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

infile = 'aod/AOD-20.1.7.2/AOD-20.1.7.2-full.pool.root'
keys = [
    #Muons...
    'Muons',
    'Staus',
    'HLT_xAOD__MuonContainer_MuonEFInfo',
    'HLT_xAOD__MuonContainer_MuTagIMO_EF',
    'HLT_xAOD__MuonContainer_eMuonEFInfo',
    
    #xAOD::MuonSegmentAuxContainer_v1
    'MuonSegments',
    'MuonTruthSegments',

    #xAOD::SlowMuonAuxContainer_v1
    'SlowMuons',
         ]

include ('AthenaPoolUtilities/TPCnvTest.py')
