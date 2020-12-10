#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":

    infile = 'aod/AOD-20.1.7.2/AOD-20.1.7.2-full.pool.root'
    keys = [
        #xAOD::L2CombinedMuonAuxContainer
        'HLT_xAOD__L2CombinedMuonContainer_MuonL2CBInfo',

        #xAOD::L2StandAloneMuonAuxContainer
        'HLT_xAOD__L2StandAloneMuonContainer_MuonL2SAInfo',
             ]

    TPCnvTest(infile, keys)
