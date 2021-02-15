#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":
    infile = 'aod/AOD-20.0.0.3/AOD-20.0.0.3-full.pool.root'

    keys = [
        #xAOD::EventInfo
        'EventInfo'
    ]

    TPCnvTest(infile, keys)
