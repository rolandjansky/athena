# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

infile = 'aod/AOD-20.1.7.2/AOD-20.1.7.2-full.pool.root'
keys = [
    #xAOD::L2CombinedMuonAuxContainer
    'HLT_xAOD__L2CombinedMuonContainer_MuonL2CBInfo',

    #xAOD::L2StandAloneMuonAuxContainer
    'HLT_xAOD__L2StandAloneMuonContainer_MuonL2SAInfo',
         ]

include ('AthenaPoolUtilities/TPCnvTest.py')
