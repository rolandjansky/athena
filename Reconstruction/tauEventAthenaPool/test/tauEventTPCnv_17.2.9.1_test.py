#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":
    infile = 'aod/AOD-17.2.9.1/AOD-17.2.9.1-full.pool.root'
    keys = [
        #TauJetContainer_p4
        'HLT_TrigTauRecCalo',
        'HLT_TrigTauRecMerged',
        'TauRecContainer',

        #TauDetailsContainer_tlp3
        # Dumped as part of the above.
    ]

    TPCnvTest(infile, keys, useGeoModelSvc=True, doPixel=True, doSCT=True, doTRT=True, doMuon=True, doTracks=True)
