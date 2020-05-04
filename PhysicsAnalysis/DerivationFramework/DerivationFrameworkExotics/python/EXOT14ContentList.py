# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

EXOT14SmartContent = [
        "TauJets",
        "MET_Reference_AntiKt4LCTopo",
        "MET_Reference_AntiKt4EMTopo",
        "PrimaryVertices",
        "Photons",
        "Electrons",
        "Muons",
        "InDetTrackParticles",
        "AntiKt4EMTopoJets",
        "AntiKt4LCTopoJets",
        "BTagging_AntiKt4LCTopo",
        "BTagging_AntiKt4EMTopo",
]

EXOT14AllVariablesContent = [
   "GSFTrackParticles",
   "egammaClusters",
   "CaloCalTopoClusters",
   "GSFConversionVertices",
   "METAssoc_AntiKt4LCTopo",
   "MET_Core_AntiKt4LCTopo",
   "METAssoc_AntiKt4EMTopo",
   "MET_Core_AntiKt4EMTopo",
   "AntiKt4TruthJets",
   "LVL1JetRoIs",
   "LVL1MuonRoIs",
   "LVL1EmTauRoIs",
   'MET_Truth',
   'TruthEvents',
   #'TruthVertex', # MenuTruthThinning does not keep vertices
   'TruthParticles',
   "HLT_xAOD__JetContainer_a4tcemsubjesFS",
   "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET",
   "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_mht",
   "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl",
   "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl_PS",
   "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl_PUC"
]

EXOT14ExtraVariablesContent = [
  'Electrons.author.pt.Medium.Tight.Loose.charge',
  'Muons.ptcone20.ptcone30.ptcone40.etcone20.etcone30.etcone40',
  'CombinedMuonTrackParticles.d0.z0.phi.theta.charge.chiSquared.numberDoF.qOverP.vz.definingParametersCovMatrix', # SUSYTools
  'ExtrapolatedMuonTrackParticles.d0.z0.phi.theta.charge.chiSquared.numberDoF.qOverP.vz.definingParametersCovMatrix', # SUSYTools
]

EXOT14Content = [
]
