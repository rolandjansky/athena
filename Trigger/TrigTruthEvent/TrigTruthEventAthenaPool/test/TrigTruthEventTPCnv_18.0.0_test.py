#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":
    infile = 'aod/AOD-18.0.0/AOD-18.0.0-full.pool.root'
    keys = [
        #TrigInDetTrackTruthMap_tlp2
        'TrigInDetTrackTruthMap',
    ]

    TPCnvTest(infile, keys)

