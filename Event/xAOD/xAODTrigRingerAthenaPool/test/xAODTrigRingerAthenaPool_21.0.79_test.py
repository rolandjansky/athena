#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":

    infile = 'aod/AOD-21.0.79/AOD-21.0.79-full.pool.root'
    keys = [
        #xAOD::TrigRingerRingsAuxContainer_v2
        'HLT_xAOD__TrigRingerRingsContainer_TrigT2CaloEgamma',

        #xAOD::TrigRNNOutputAuxContainer_v2
        'HLT_xAOD__TrigRNNOutputContainer_TrigRingerNeuralFex',
        'HLT_xAOD__TrigRNNOutputContainer_TrigTRTHTCounts',
             ]

    TPCnvTest(infile, keys)
