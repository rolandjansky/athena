#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":
    infile = 'esd/ESD-20.1.7.2.pool.root'

    keys = [
        #EventStreamInfo_p3
        'InputMetaDataStore/StreamESD',

        #PileUpEventInfo_p5
        'OverlayEvent',
    ]

    TPCnvTest(infile, keys, useIOVDbSvc=True)
