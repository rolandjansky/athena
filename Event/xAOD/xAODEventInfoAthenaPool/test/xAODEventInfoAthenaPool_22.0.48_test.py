#!/usr/bin/env python
"""
Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":
    infile = 'aod/AOD-22.0.48/AOD-22.0.48-full.pool.root'

    keys = [
        #xAOD::EventInfo
        'EventInfo'
    ]

    TPCnvTest(infile, keys)
