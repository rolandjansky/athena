# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

EXOT4Content = [
'xAOD::JetContainer#AntiKt10LCTopoTrimmedPtFrac5SmallR30Jets',
'xAOD::JetAuxContainer#AntiKt10LCTopoTrimmedPtFrac5SmallR30JetsAux.',
'xAOD::JetContainer#AntiKtVR60Rmax4Rmin2LCTopoJets',
'xAOD::JetAuxContainer#AntiKtVR60Rmax4Rmin2LCTopoJetsAux.',
'xAOD::JetContainer#AntiKtVR60Rmax4Rmin2TrackJets',
'xAOD::JetAuxContainer#AntiKtVR60Rmax4Rmin2TrackJetsAux.',
'xAOD::JetContainer#AntiKtVR600Rmax10Rmin2LCTopoJets',
'xAOD::JetAuxContainer#AntiKtVR600Rmax10Rmin2LCTopoJetsAux.',
'xAOD::JetContainer#AntiKtVR600Rmax10Rmin2TrackJets',
'xAOD::JetAuxContainer#AntiKtVR600Rmax10Rmin2TrackJetsAux.',
'xAOD::JetContainer#AntiKtVR10Rmax4Rmin1TrackJets',
'xAOD::JetAuxContainer#AntiKtVR10Rmax4Rmin1TrackJetsAux.',
"xAOD::JetContainer#CamKt15LCTopoJets", 
"xAOD::JetAuxContainer#CamKt15LCTopoJetsAux.",
]
EXOT4SmartCollections = ["Electrons","Muons", "InDetTrackParticles", "PrimaryVertices"]
EXOT4ExtraVariables = [ "Electrons.author.Medium.Tight.Loose.charge", "Muons.charge"]
EXOT4AllVariables = ["AntiKt10TruthJets", "AntiKt4EMTopoJets", "AntiKt4LCTopoJets", "AntiKt4TruthJets", "BTagging_AntiKt4LCTopo", "TruthParticles", "TruthEvents", "TruthVertices","CombinedMuonTrackParticles","ExtrapolatedMuonTrackParticles","egammaClusters","GSFTrackParticles","CaloCalTopoClusters", "MET_Reference_AntiKt4LCTopo", "AntiKt3PV0TrackJets", "AntiKt4PV0TrackJets",'METAssoc_AntiKt4LCTopo','MET_Core_AntiKt4LCTopo']
