#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":

    infile = 'aod/AOD-21.0.79/AOD-21.0.79-full.pool.root'
    keys = [
        #xAOD::MuonAuxContainer_v4
        'Muons',
        'Staus',
        'HLT_xAOD__MuonContainer_MuonEFInfo',

        #xAOD::MuonSegmentAuxContainer_v1
        'MuonSegments',
        'MuonTruthSegments',
        'NCB_MuonSegments',

        #xAOD::SlowMuonAuxContainer_v1
        'SlowMuons',
             ]

    TPCnvTest(infile, keys)
