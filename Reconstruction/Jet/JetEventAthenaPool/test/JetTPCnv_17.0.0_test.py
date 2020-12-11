#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":
    infile = 'aod/AOD-17.0.0/AOD-17.0.0-full.pool.root'
    keys = [ 'HLT_TrigCosmicJetRec',
             'HLT_TrigTauJet',
             # Can't read these because it requires TruthParticleCnvTool
             # from AtlasReconstruction.
             #'AntiKt4TruthJets',
             #'AntiKt6TruthJets',
             'AntiKt4TopoJets',
             'AntiKt6TowerAODJets',
             'AntiKt4TowerAODJets',
             'AntiKt6TopoJets',
             'AntiKt4TopoEMJets',
             'AntiKt6TopoEMJets',
             'AntiKt4LCTopoJets',
             'AntiKt6LCTopoJets',
             'HLT_TrigJetRec',
             ]

    TPCnvTest(infile, keys, useGeoModelSvc=True, doPixel=True, doSCT=True, doTRT=True, doMuon=True, doTracks=True)
