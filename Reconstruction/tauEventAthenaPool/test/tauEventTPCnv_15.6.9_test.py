#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":
    infile = 'aod/AOD-15.6.9/AOD-15.6.9-full.pool.root'
    keys = [
        #TauJetContainer_p3
        'HLT_TrigTauRecCalo',
        'AtlfastTauJetContainer',
        'HLT_TrigTauRecMerged',
        'TauRecContainer',

        #TauDetailsContainer_tlp2
        # Dumped as part of the above.
    ]

    TPCnvTest(infile, keys, useGeoModelSvc=True, doPixel=True, doSCT=True, doTRT=True, doMuon=True, doTracks=True)
