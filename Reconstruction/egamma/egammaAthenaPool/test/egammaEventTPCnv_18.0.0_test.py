#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":
    infile = 'aod/AOD-18.0.0/AOD-18.0.0-full.pool.root'
    keys = [
        #ElectronContainer_p5
        'ElectronAODCollection',

        #PhotonContainer_p5
        'PhotonAODCollection',

        #egammaContainer_p5
        'HLT_egamma',
        'HLT_egamma_Photons',
        'HLT_egamma_Electrons',

        #egDetailContainer_p2
        # Dumped as part of the above.
    ]

    TPCnvTest(infile, keys, useGeoModelSvc=True, doPixel=True, doSCT=True, doTRT=True, doMuon=True, doTracks=True)
