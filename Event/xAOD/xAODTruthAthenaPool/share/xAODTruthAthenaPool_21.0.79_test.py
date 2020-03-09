# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

infile = 'aod/AOD-21.0.79/AOD-21.0.79-full.pool.root'
keys = [
    #xAOD::TruthEventAuxContainer_v1
    'TruthEvents',
    
    #xAOD::TruthParticleAuxContainer_v1
    'MuonTruthParticles',

    #xAOD::TruthVertexAuxContainer_v1
    'TruthVertices',
         ]

include ('AthenaPoolUtilities/TPCnvTest.py')
