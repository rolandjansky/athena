# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

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
                        ["AntiKt4TruthJets","xAOD::JetContainer"],
                        ["AntiKt4TruthJetsAux","xAOD::JetAuxContainer"],
                        ["AntiKt4TruthWZJets","xAOD::JetContainer"],
                        ["AntiKt4TruthWZJetsAux","xAOD::JetAuxContainer"],
                        ["AntiKt10LCTopoJets","xAOD::JetContainer"],
                        ["AntiKt10LCTopoJetsAux","xAOD::JetAuxContainer"],
                        ["AntiKt10TrackCaloClusterJets","xAOD::JetContainer"],
                        ["AntiKt10TrackCaloClusterJetsAux","xAOD::JetAuxContainer"],
                        ["TrackCaloClustersCombinedAndNeutral","xAOD::TrackCaloClusterContainer"],
                        ["TrackCaloClustersCombinedAndNeutralAux","xAOD::TrackCaloClusterAuxContainer"],
                        ["BTagging_AntiKt4LCTopo","xAOD::BTaggingContainer"],
                        ["BTagging_AntiKt4LCTopoAux", "xAOD::BTaggingAuxContainer"],         
                        ["BTagging_AntiKt2Track","xAOD::BTaggingContainer"],
                        ["BTagging_AntiKt2TrackAux","xAOD::BTaggingAuxContainer"]     
]
