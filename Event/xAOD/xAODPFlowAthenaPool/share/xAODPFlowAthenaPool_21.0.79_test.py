# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

infile = 'aod/AOD-21.0.79/AOD-21.0.79-full.pool.root'
keys = [
    #xAOD::PFOAuxContainer_v1
    'JetETMissChargedParticleFlowObjects',
    'JetETMissNeutralParticleFlowObjects',
    'TauChargedParticleFlowObjects',
    'TauHadronicParticleFlowObjects',
    'TauNeutralParticleFlowObjects',
    'TauShotParticleFlowObjects',
         ]

include ('AthenaPoolUtilities/TPCnvTest.py')
