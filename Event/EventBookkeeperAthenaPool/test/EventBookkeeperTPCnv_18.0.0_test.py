#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":
    infile = 'aod/AOD-18.0.0/AOD-18.0.0-full.pool.root'
    keys = [
        #EventBookkeeperCollection_p2
        'InputMetaDataStore/IncompleteEventBookkeepers',
        'InputMetaDataStore/EventBookkeepers@4',

        #SkimDecisionCollection_p1
        'StreamESD_SkimDecisionsContainer',
             ]

    TPCnvTest(infile, keys, useIOVDbSvc=True)

