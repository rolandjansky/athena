# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

EXOT4Content = [
'xAOD::JetContainer#AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets',
'xAOD::JetAuxContainer#AntiKt10LCTopoTrimmedPtFrac5SmallR20JetsAux.',
"xAOD::JetContainer#CamKt15LCTopoJets", 
"xAOD::JetAuxContainer#CamKt15LCTopoJetsAux.",
]
EXOT4SmartCollections = ["Electrons","Muons", "InDetTrackParticles", "PrimaryVertices", 'MET_Reference_AntiKt4EMTopo', "AntiKt4EMTopoJets", "AntiKt4LCTopoJets", "BTagging_AntiKt4LCTopo", "BTagging_AntiKt4EMTopo", "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets", "BTagging_AntiKt2Track", "BTagging_AntiKt3Track"]
EXOT4ExtraVariables = [ "Electrons.author.Medium.Tight.Loose.charge", "Muons.charge"]
EXOT4AllVariables = ["AntiKt10TruthJets",
                     #"AntiKt4EMTopoJets",
                     #"AntiKt4LCTopoJets",
                     "AntiKt4TruthJets",
                     "AntiKt4TruthWZJets",
                     #"BTagging_AntiKt4LCTopo",
                     #"BTagging_AntiKt4EMTopo",
                     "TruthParticles",
                     "TruthEvents",
                     "TruthVertices",
                     "CombinedMuonTrackParticles","ExtrapolatedMuonTrackParticles","GSFTrackParticles",
                     "CaloCalTopoClusters",
                     "AntiKt2PV0TrackJets", "AntiKt3PV0TrackJets", "AntiKt4PV0TrackJets",
                     #"BTagging_AntiKt2Track", "BTagging_AntiKt3Track",
                     "egammaTopoSeededClusters"]
