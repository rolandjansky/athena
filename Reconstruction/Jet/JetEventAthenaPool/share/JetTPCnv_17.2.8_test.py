infile = 'aod/AOD-17.2.8/AOD-17.2.8-full.pool.root'
keys = [ 'HLT_TrigCosmicJetRec',
         'AntiKt6TowerAODJets',
         'HLT_TrigTauJet',
         'AntiKt6GhostTowerAODJets',
         # Can't read these because it requires TruthParticleCnvTool
         # from AtlasReconstruction.
         #'AntiKt4TruthJets',
         'AntiKt4TopoEMJets',
         'AntiKt6TopoEMJets',
         'AntiKt4LCTopoJets',
         'AntiKt6LCTopoJets',
         'HLT_TrigJetRec',
         ]

include ('AthenaPoolUtilities/TPCnvTest.py')

