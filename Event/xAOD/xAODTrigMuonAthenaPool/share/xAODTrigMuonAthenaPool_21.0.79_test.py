# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

infile = 'aod/AOD-21.0.79/AOD-21.0.79-full.pool.root'
keys = [
    #xAOD::L2CombinedMuonAuxContainer_v1
    'HLT_xAOD__L2CombinedMuonContainer_MuonL2CBInfo',

    #xAOD::L2StandAloneMuonAuxContainer_v2
    'HLT_xAOD__L2StandAloneMuonContainer_MuonL2SAInfo',
         ]

include ('AthenaPoolUtilities/TPCnvTest.py')
