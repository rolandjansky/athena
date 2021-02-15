#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":
    infile = 'aod/AOD-16.0.0/AOD-16.0.0-full.pool.root'
    keys = [
        #EventBookkeeperCollection_p1
        'InputMetaDataStore/EventBookkeepers@4',
        'InputMetaDataStore/EventSelector.Counter',

        #SkimDecisionCollection_p1
        'StreamAOD_SkimDecisionsContainer',
        'StreamESD_SkimDecisionsContainer',
             ]

    TPCnvTest(infile, keys, useIOVDbSvc=True)

