#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":

    infile = 'aod/AOD-20.1.7.2/AOD-20.1.7.2-full.pool.root'
    keys = [
        #xAOD::TauJetAuxContainer
        'HLT_xAOD__TauJetContainer_TrigTauRecMerged',
        'HLT_xAOD__TauJetContainer_TrigTauRecPreselection',
        'TauJets',
             ]

    TPCnvTest(infile, keys)
