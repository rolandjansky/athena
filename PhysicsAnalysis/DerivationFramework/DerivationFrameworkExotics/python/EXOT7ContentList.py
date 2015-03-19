# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

EXOT7Content = [
"xAOD::JetContainer#AntiKt10LCTopoTrimmedPtFrac5SmallR30Jets",
"xAOD::JetContainer#CamKt8LCTopoJets", 
"xAOD::JetContainer#CamKt15LCTopoJets", 
"xAOD::JetAuxContainer#AntiKt10LCTopoTrimmedPtFrac5SmallR30JetsAux.",
"xAOD::JetAuxContainer#CamKt8LCTopoJetsAux.", 
"xAOD::JetAuxContainer#CamKt15LCTopoJetsAux.", 
]

EXOT7SmartCollections = ["Electrons","Muons","MET_Reference_AntiKt4LCTopo", "InDetTrackParticles", "PrimaryVertices"]
EXOT7ExtraVariables = [ "Electrons.author.Medium.Tight.Loose.charge", "Muons.charge"]
EXOT7AllVariables = ["AntiKt10TruthJets", "AntiKt4EMTopoJets", "AntiKt4LCTopoJets", "AntiKt4TruthJets", "BTagging_AntiKt4LCTopo", "TruthParticles", "TruthEvents", "TruthVertices","CombinedMuonTrackParticles","ExtrapolatedMuonTrackParticles","egammaClusters","GSFTrackParticles","CaloCalTopoClusters",'METAssoc_AntiKt4LCTopo','MET_Core_AntiKt4LCTopo']
