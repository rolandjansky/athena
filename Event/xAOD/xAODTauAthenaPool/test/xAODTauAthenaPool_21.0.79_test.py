#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":

    infile = 'aod/AOD-21.0.79/AOD-21.0.79-full.pool.root'
    keys = [
        #xAOD::DiTauJetAuxContainer_v1
        'DiTauJets',

        #xAOD::TauJetAuxContainer_v3
        'HLT_xAOD__TauJetContainer_TrigTauRecMerged',
        'HLT_xAOD__TauJetContainer_TrigTauRecPreselection',
        'TauJets',

        #xAOD::TauTrackAuxContainer_v1
        'TauTracks',
             ]

    TPCnvTest(infile, keys)
