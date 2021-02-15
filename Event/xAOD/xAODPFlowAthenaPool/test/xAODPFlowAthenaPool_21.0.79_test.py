#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

import ROOT
from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":

    # Force-load some dictionaries. Needed to work around ROOT-10940.
    ROOT.xAOD.CaloCluster()

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

    TPCnvTest(infile, keys)
