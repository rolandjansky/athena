# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

infile = 'aod/AOD-21.0.79/AOD-21.0.79-full.pool.root'
keys = [
    #xAOD::BTaggingAuxContainer_v1
    'BTagging_AntiKt4EMTopo',

    #xAOD::BTaggingTrigAuxContainer_v1
    'HLT_xAOD__BTaggingContainer_HLTBjetFex',

    #xAOD::BTagVertexAuxContainer_v1
    'BTagging_AntiKt4EMTopoJFVtx',
    'HLT_xAOD__BTagVertexContainer_BjetVertexFex',
         ]

include ('AthenaPoolUtilities/TPCnvTest.py')
