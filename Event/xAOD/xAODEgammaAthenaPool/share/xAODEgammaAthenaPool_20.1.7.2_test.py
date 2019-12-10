# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

infile = 'aod/AOD-20.1.7.2/AOD-20.1.7.2-full.pool.root'
keys = [
    #xAOD::ElectronAuxContainer
    'Electrons',
    'ForwardElectrons',
    'HLT_xAOD__ElectronContainer_egamma_Electrons',

    #xAOD::PhotonAuxContainer
    'Photons',
    'HLT_xAOD__PhotonContainer_egamma_Photons',
         ]

include ('AthenaPoolUtilities/TPCnvTest.py')
