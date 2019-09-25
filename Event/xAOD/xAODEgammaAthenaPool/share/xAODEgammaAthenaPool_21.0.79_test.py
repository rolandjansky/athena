# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

infile = 'aod/AOD-21.0.79/AOD-21.0.79-full.pool.root'
keys = [
    #xAOD::ElectronAuxContainer_v4
    'Electrons',
    'ForwardElectrons',

    #xAOD::PhotonAuxContainer_v4
    'Photons',
         ]

include ('AthenaPoolUtilities/TPCnvTest.py')
