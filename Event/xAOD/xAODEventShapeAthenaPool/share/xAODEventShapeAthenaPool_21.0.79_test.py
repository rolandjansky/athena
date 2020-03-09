# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

infile = 'aod/AOD-21.0.79/AOD-21.0.79-full.pool.root'
keys = [
    #xAOD::EventShapeAuxInfo_v1
    'Kt4EMPFlowEventShape',
    'Kt4EMTopoOriginEventShape',
    'Kt4LCTopoOriginEventShape',
    'NeutralParticleFlowIsoCentralEventShape',
    'NeutralParticleFlowIsoForwardEventShape',
    'ParticleFlowIsoCentralEventShape',
    'ParticleFlowIsoForwardEventShape',
    'TopoClusterIsoCentralEventShape',
    'TopoClusterIsoForwardEventShape',
    'TopoClusterIsoVeryForwardEventShape',
         ]

include ('AthenaPoolUtilities/TPCnvTest.py')
