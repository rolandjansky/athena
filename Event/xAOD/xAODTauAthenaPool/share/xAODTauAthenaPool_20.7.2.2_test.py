# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

infile = 'aod/AOD-20.7.2.2/AOD-20.7.2.2-full.pool.root'
keys = [
    #xAOD::DiTauJetAuxContainer_v1
    'DiTauJets',

    #xAOD::TauJetAuxContainer_v2
    'HLT_xAOD__TauJetContainer_TrigTauRecMerged',
    'HLT_xAOD__TauJetContainer_TrigTauRecPreselection',
    'TauJets',
         ]

include ('AthenaPoolUtilities/TPCnvTest.py')
