#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":
    infile = 'aod/AOD-15.9.0.5/AOD-15.9.0.5-full.pool.root'
    keys = ['HLT_TrigCosmicJetRec',
            'AtlfastJetContainer',
            'HLT_TrigTauJet',
            'Cone7H1TowerAODJets',
            # Can't read these because it requires TruthParticleCnvTool
            # from AtlasReconstruction.
            #'Cone4TruthJets',
            #'AntiKt4TruthJets',
            'Cone4H1TopoJets',
            'Cone4H1TowerAODJets',
            'AntiKt6H1TopoJets',
            'AntiKt6LCTopoJets',
            'AntiKt6H1TowerAODJets',
            'AntiKt4H1TowerAODJets',
            'AntiKt4H1TopoJets',
            'HLT_TrigJetRec',
            ]

    TPCnvTest(infile, keys, useGeoModelSvc=True, doPixel=True, doSCT=True, doTRT=True, doMuon=True, doTracks=True)
