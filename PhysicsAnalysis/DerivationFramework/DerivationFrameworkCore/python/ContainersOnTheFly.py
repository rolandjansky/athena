# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# List of containers that are made on-the-fly by basically all DAOD types and
# can therefore be reasonably added to the NameAndTypes dictionary centrally
# rather than asking each DAOD to do it themselves
ContainersOnTheFly = [
                        ["TruthMuons","xAOD::TruthParticleContainer"],
                        ["TruthMuonsAux","xAOD::TruthParticleAuxContainer"],
                        ["TruthElectrons","xAOD::TruthParticleContainer"],
                        ["TruthElectronsAux","xAOD::TruthParticleAuxContainer"],
                        ["TruthPhotons","xAOD::TruthParticleContainer"],
                        ["TruthPhotonsAux","xAOD::TruthParticleAuxContainer"],
                        ["TruthNeutrinos","xAOD::TruthParticleContainer"],
                        ["TruthNeutrinosAux","xAOD::TruthParticleAuxContainer"],
                        ["TruthTaus","xAOD::TruthParticleContainer"],
                        ["TruthTausAux","xAOD::TruthParticleAuxContainer"],
                        ["AntiKt2PV0TrackJets","xAOD::JetContainer"],
                        ["AntiKt2PV0TrackJetsAux","xAOD::JetAuxContainer"],
                        ["AntiKt4PV0TrackJets","xAOD::JetContainer"],
                        ["AntiKt4PV0TrackJetsAux","xAOD::JetAuxContainer"],
                        ["AntiKtVR30Rmax4Rmin02TrackJets","xAOD::JetContainer"],
                        ["AntiKtVR30Rmax4Rmin02TrackJetsAux","xAOD::JetAuxContainer"],
                        ["AntiKtVR30Rmax4Rmin02TrackGhostTagJets","xAOD::JetContainer"],
                        ["AntiKtVR30Rmax4Rmin02TrackGhostTagJetsAux","xAOD::JetAuxContainer"],
			["AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt2SubJets","xAOD::JetContainer"],
			["AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt2SubJetsAux","xAOD::JetAuxContainer"],
			["AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt3SubJets","xAOD::JetContainer"],
			["AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt3SubJetsAux","xAOD::JetAuxContainer"],
                        ["AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt2GASubJets","xAOD::JetContainer"],
                        ["AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt2GASubJetsAux","xAOD::JetAuxContainer"],
                        ["AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt3GASubJets","xAOD::JetContainer"],
                        ["AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt3GASubJetsAux","xAOD::JetAuxContainer"],
			["AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2SubJets","xAOD::JetContainer"],
			["AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2SubJetsAux","xAOD::JetAuxContainer"],
                        ["AntiKt4TruthJets","xAOD::JetContainer"],
                        ["AntiKt4TruthJetsAux","xAOD::JetAuxContainer"],
                        ["AntiKt4TruthWZJets","xAOD::JetContainer"],
                        ["AntiKt4TruthWZJetsAux","xAOD::JetAuxContainer"],
                        ["AntiKt4TruthDressedWZJets","xAOD::JetContainer"],
                        ["AntiKt4TruthDressedWZJetsAux","xAOD::JetAuxContainer"],
                        ["AntiKt4EMTopoCSSKJets","xAOD::JetContainer"],
                        ["AntiKt4EMTopoCSSKJetsAux","xAOD::JetAuxContainer"],
                        ["AntiKt4EMPFlowCSSKJets","xAOD::JetContainer"],
                        ["AntiKt4EMPFlowCSSKJetsAux","xAOD::JetAuxContainer"],
                        ["AntiKt10LCTopoJets","xAOD::JetContainer"],
                        ["AntiKt10LCTopoJetsAux","xAOD::JetAuxContainer"],
                        ["AntiKt10TrackCaloClusterJets","xAOD::JetContainer"],
                        ["AntiKt10TrackCaloClusterJetsAux","xAOD::JetAuxContainer"],
                        ["AntiKt10LCTopoCSSKSoftDropBeta100Zcut10Jets","xAOD::JetContainer"],
                        ["AntiKt10LCTopoCSSKSoftDropBeta100Zcut10JetsAux","xAOD::JetAuxContainer"],
                        ["AntiKt10LCTopoCSSKJets","xAOD::JetContainer"],
                        ["AntiKt10LCTopoCSSKJetsAux","xAOD::JetAuxContainer"],

                        ["TrackCaloClustersCombinedAndNeutral","xAOD::TrackCaloClusterContainer"],
                        ["TrackCaloClustersCombinedAndNeutralAux","xAOD::TrackCaloClusterAuxContainer"],
                        ["BTagging_AntiKt4LCTopo","xAOD::BTaggingContainer"],
                        ["BTagging_AntiKt4LCTopoAux", "xAOD::BTaggingAuxContainer"],
                        ["BTagging_AntiKt2Track","xAOD::BTaggingContainer"],
                        ["BTagging_AntiKt2TrackAux","xAOD::BTaggingAuxContainer"],
                        ["BTagging_AntiKtVR30Rmax4Rmin02Track","xAOD::BTaggingContainer"],
                        ["BTagging_AntiKtVR30Rmax4Rmin02TrackAux","xAOD::BTaggingAuxContainer"],
                        ["BTagging_AntiKtVR30Rmax4Rmin02TrackGhostTag","xAOD::BTaggingContainer"],
                        ["BTagging_AntiKtVR30Rmax4Rmin02TrackGhostTagAux","xAOD::BTaggingAuxContainer"],
                        ["BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt2Sub","xAOD::BTaggingContainer"],
                        ["BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt2SubAux","xAOD::BTaggingAuxContainer"],
                        ["BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt3Sub","xAOD::BTaggingContainer"],
                        ["BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt3SubAux","xAOD::BTaggingAuxContainer"],
                        ["BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt2GASub","xAOD::BTaggingContainer"],
                        ["BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt2GASubAux","xAOD::BTaggingAuxContainer"],
                        ["BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt3GASub","xAOD::BTaggingContainer"],
                        ["BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt3GASubAux","xAOD::BTaggingAuxContainer"],
                        ["BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2Sub","xAOD::BTaggingContainer"],
                        ["BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2SubAux","xAOD::BTaggingAuxContainer"],
                        ["BTagging_AntiKt4EMPFlow","xAOD::BTaggingContainer"],
                        ["BTagging_AntiKt4EMPFlowAux","xAOD::BTaggingAuxContainer"],
                        ["BTagging_AntiKt4EMPFlow_201810","xAOD::BTaggingContainer"],
                        ["BTagging_AntiKt4EMPFlow_201810Aux","xAOD::BTaggingAuxContainer"],
                        ["BTagging_AntiKt4EMPFlow_201903","xAOD::BTaggingContainer"],
                        ["BTagging_AntiKt4EMPFlow_201903Aux","xAOD::BTaggingAuxContainer"],
]
