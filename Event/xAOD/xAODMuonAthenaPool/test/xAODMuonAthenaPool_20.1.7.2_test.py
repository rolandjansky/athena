#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

import ROOT
from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":

    # Force-load some dictionaries. Needed to work around ROOT-10940.
    ROOT.xAOD.CaloCluster()

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

    TPCnvTest(infile, keys)
