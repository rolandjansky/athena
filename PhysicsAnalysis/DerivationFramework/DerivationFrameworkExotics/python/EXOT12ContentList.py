# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

EXOT12SmartContent = [
 "Electrons",
 "Muons",
 "MET_Reference_AntiKt4LCTopo",
 "InDetTrackParticles",
 "PrimaryVertices",
 "CaloCalTopoClusters"
]

EXOT12AllVariablesContent = [
### TRUTH ###
"TruthEvents",
"TruthVertices",
"TruthParticles",
"MuonTruthParticles",
"egammaTruthParticles",
"MET_Truth",

### TRUTH JETS ###
"AntiKt4TruthJets",
"AntiKt10TruthJets",
"CamKt12TruthJets",
"BTagging_AntiKt4TruthWZ",
"BTagging_AntiKt10TruthWZ",

### RECO JETS ###
"AntiKt4LCTopoJets",
"AntiKt4EMTopoJets",
"AntiKt10LCTopoJets",
"CamKt12LCTopoJets",
"BTagging_AntiKt4LCTopo",
"BTagging_AntiKt4EMTopo",
"BTagging_AntiKt10LCTopo",

### Track JETS ###
#"AntiKt4ZTrackJets",
"AntiKt4PV0TrackJets",

### MET ###
"MET_LocHadTopo",

### MUONS ###
"MuonSegments",
]

EXOT12Content = [
### JETS ###
"xAOD::JetContainer#CamKt08LCTopoJets",
"xAOD::JetContainer#CamKt10LCTopoJets",
"xAOD::JetContainer#CamKt12LCTopoSplitFilteredMu100SmallR30YCut15Jets",
"xAOD::JetContainer#AntiKt10LCTopoTrimmedPtFrac5SmallR30Jets",
"xAOD::JetContainer#AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
"xAOD::JetContainer#AntiKt10TruthTrimmedPtFrac5SmallR30Jets",
"xAOD::JetContainer#AntiKt10TruthTrimmedPtFrac5SmallR20Jets",

"xAOD::JetAuxContainer#CamKt08LCTopoJetsAux.",
"xAOD::JetAuxContainer#CamKt10LCTopoJetsAux.",
"xAOD::JetAuxContainer#CamKt12LCTopoSplitFilteredMu100SmallR30YCut15JetsAux.",
"xAOD::JetAuxContainer#AntiKt10LCTopoTrimmedPtFrac5SmallR30JetsAux.",
"xAOD::JetAuxContainer#AntiKt10LCTopoTrimmedPtFrac5SmallR20JetsAux.",
"xAOD::JetAuxContainer#AntiKt10TruthTrimmedPtFrac5SmallR30JetsAux.",
"xAOD::JetAuxContainer#AntiKt10TruthTrimmedPtFrac5SmallR20JetsAux.",
]

