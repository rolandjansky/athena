#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":
    infile = 'aod/AOD-15.0.0/AOD-15.0.0-full.pool.root'

    keys = [
        #EventInfo_p2
        'McEventInfo',

        #EventStreamInfo_p1
        'InputMetaDataStore/StreamAOD'
    ]

    TPCnvTest(infile, keys, useIOVDbSvc=True)
