# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

infile = 'aod/AOD-21.0.79/AOD-21.0.79-full.pool.root'
keys = [
    #xAOD::TrigRingerRingsAuxContainer_v2
    'HLT_xAOD__TrigRingerRingsContainer_TrigT2CaloEgamma',

    #xAOD::TrigRNNOutputAuxContainer_v2
    'HLT_xAOD__TrigRNNOutputContainer_TrigRingerNeuralFex',
    'HLT_xAOD__TrigRNNOutputContainer_TrigTRTHTCounts',
         ]

include ('AthenaPoolUtilities/TPCnvTest.py')
