#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":

    infile = 'aod/AOD-21.0.79/AOD-21.0.79-full.pool.root'
    keys = [
        #xAOD::CaloRingsAuxContainer_v1
        'ElectronCaloRings',

        #xAOD::RingSetAuxContainer_v1
        'ElectronRingSets',
             ]

    TPCnvTest(infile, keys)
