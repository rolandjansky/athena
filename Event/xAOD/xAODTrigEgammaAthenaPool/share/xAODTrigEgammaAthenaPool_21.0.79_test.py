# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

infile = 'aod/AOD-21.0.79/AOD-21.0.79-full.pool.root'
keys = [
    #xAOD::ElectronTrigAuxContainer_v1
    'HLT_xAOD__ElectronContainer_egamma_Electrons',
    'HLT_xAOD__ElectronContainer_egamma_Iso_Electrons',
    'HLT_xAOD__ElectronContainer_egamma_SC_Electrons',
    'HLT_xAOD__ElectronContainer_egamma_SC_Iso_Electrons',
    'HLT_xAOD__MuonContainer_MuonEFInfo',

    #xAOD::PhotonTrigAuxContainer_v1
    'HLT_xAOD__PhotonContainer_egamma_Iso_Photons',
    'HLT_xAOD__PhotonContainer_egamma_Photons',
    'HLT_xAOD__PhotonContainer_egamma_SC_Iso_Photons',
    'HLT_xAOD__PhotonContainer_egamma_SC_Photons',

    #xAOD::TrigElectronAuxContainer_v1
    'HLT_xAOD__TrigElectronContainer_L2ElectronFex',
         ]

include ('AthenaPoolUtilities/TPCnvTest.py')
