# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

infile = 'aod/AOD-21.0.79/AOD-21.0.79-full.pool.root'
keys = [
    #xAOD::ParticleAuxContainer_v1
    'finalTauPi0s',
         ]

include ('AthenaPoolUtilities/TPCnvTest.py')
