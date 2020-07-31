#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":
    infile = 'aod/AOD-15.1.0/AOD-15.1.0-full.pool.root'
    keys = [
        #ElectronContainer_p1
        'AtlfastIsoElectronCollection',
        'AtlfastElectronCollection',
        'ElectronAODCollection',

        #PhotonContainer_p1
        'AtlfastIsoPhotonCollection',
        'AtlfastPhotonCollection',
        'PhotonAODCollection',

        #egammaContainer_p1
        'HLT_egamma_Photons',
        'HLT_egamma_Electrons',

        #egDetailContainer_p1
        # Dumped as part of the above.
    ]

    TPCnvTest(infile, keys, useGeoModelSvc=True, doPixel=True, doSCT=True, doTRT=True, doMuon=True, doTracks=True)
