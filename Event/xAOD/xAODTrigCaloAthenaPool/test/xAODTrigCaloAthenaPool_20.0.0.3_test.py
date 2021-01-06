#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

import ROOT
from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":

    # Force-load some dictionaries. Needed to work around ROOT-10940.
    ROOT.xAOD.CaloCluster()

    infile = 'aod/AOD-20.0.0.3/AOD-20.0.0.3-full.pool.root'
    keys = [
        #xAOD::TrigCaloClusterAuxContainer_v1
        'HLT_xAOD__TrigEMClusterContainer_TrigT2CaloEgamma',
             ]

    TPCnvTest(infile, keys)
