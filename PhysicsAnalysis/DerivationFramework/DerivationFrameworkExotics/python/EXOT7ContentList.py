# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

EXOT7Content = [
"xAOD::JetContainer#AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
"xAOD::JetContainer#CamKt8LCTopoJets", 
"xAOD::JetContainer#CamKt15LCTopoJets", 
"xAOD::JetAuxContainer#AntiKt10LCTopoTrimmedPtFrac5SmallR20JetsAux.",
"xAOD::JetAuxContainer#CamKt8LCTopoJetsAux.", 
"xAOD::JetAuxContainer#CamKt15LCTopoJetsAux.", 
]

EXOT7SmartCollections = ["Electrons","Muons","MET_Reference_AntiKt4EMTopo", "InDetTrackParticles", "PrimaryVertices", 'MET_Reference_AntiKt4EMTopo']
EXOT7ExtraVariables = [ "Electrons.author.Medium.Tight.Loose.charge", "Muons.charge"]
EXOT7AllVariables = ["AntiKt10TruthJets", "AntiKt4EMTopoJets", "AntiKt4LCTopoJets", "AntiKt4TruthJets", "BTagging_AntiKt4LCTopo", "BTagging_AntiKt4EMTopo", "TruthParticles", "TruthEvents", "TruthVertices","CombinedMuonTrackParticles","ExtrapolatedMuonTrackParticles","egammaClusters","GSFTrackParticles","CaloCalTopoClusters",'METAssoc_AntiKt4EMTopo','MET_Core_AntiKt4EMTopo',"AntiKt3PV0TrackJets","BTagging_AntiKt3Track", "AntiKt2PV0TrackJets", "BTagging_AntiKt2Track"]
