# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

infile = 'aod/AOD-21.0.79/AOD-21.0.79-full.pool.root'
keys = [
    #xAOD::CaloRingsAuxContainer_v1
    'ElectronCaloRings',

    #xAOD::RingSetAuxContainer_v1
    'ElectronRingSets',
         ]

include ('AthenaPoolUtilities/TPCnvTest.py')
