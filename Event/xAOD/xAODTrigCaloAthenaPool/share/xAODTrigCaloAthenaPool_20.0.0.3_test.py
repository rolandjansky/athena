# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

infile = 'aod/AOD-20.0.0.3/AOD-20.0.0.3-full.pool.root'
keys = [
    #xAOD::TrigCaloClusterAuxContainer_v1
    'HLT_xAOD__TrigEMClusterContainer_TrigT2CaloEgamma',
         ]

include ('AthenaPoolUtilities/TPCnvTest.py')
