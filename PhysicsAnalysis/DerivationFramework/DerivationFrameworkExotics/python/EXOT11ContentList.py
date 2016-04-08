# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

EXOT11SmartContent = [
"Electrons",
#"Photons",
"Muons",
#"TauJets",
"MET_Reference_AntiKt4LCTopo",
"MET_Reference_AntiKt4EMTopo",
"AntiKt4EMTopoJets",
"AntiKt4LCTopoJets",
"BTagging_AntiKt4EMTopo",
"BTagging_AntiKt4LCTopo",
"InDetTrackParticles",
"PrimaryVertices",
#"CaloCalTopoClusters"
]

EXOT11ExtraVariablesContent = [
"Electrons.author.charge.OQ.emins1.fracs1.f3core.deltaPhi2.ptcone20.topoetcone20",
"GSFTrackParticles.z0.vz.numberOfPixelOutliers.numberOfSCTOutliers",
"egammaClusters.rawE.e_sampl.eta_sampl.phi_sampl.calM.calE.calEta.calPhi.etaCalo.phiCalo",
"BTagging_AntiKt4LCTopo.MV1_discriminant",
"Muons.quality.etcone20.ptcone20.etcone30.ptcone30.etcone40.ptcone40.energyLossType.energyLoss",
"Muons.clusterLink",
"CombinedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix",
"ExtrapolatedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.numberOfTRTHits.numberOfPixelDeadSensors.numberOfTRTOutliers",
"InDetTrackParticles.numberOfTRTHits.numberOfTRTOutliers",
"AntiKt4LCTopoJets.NumTrkPt500.Width.TrackWidthPt1000.TrackWidthPt500.TrackCount.GhostTrackCount.Charge.ConeExclBHadronsFinal.ConeExclCHadronsFinal.ConeExclTausFinal",
"AntiKt4EMTopoJets.TrackWidthPt500.TrackCount.JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_phi.JetEMScaleMomentum_m.GhostTrackCount.Charge.ConeExclBHadronsFinal.ConeExclCHadronsFinal.ConeExclTausFinal",
"AntiKt4PV0TrackJets.ConeExclBHadronsFinal.ConeExclCHadronsFinal.ConeExclTausFinal",
"AntiKt3PV0TrackJets.ConeExclBHadronsFinal.ConeExclCHadronsFinal.ConeExclTausFinal",
"AntiKt2PV0TrackJets.ConeExclBHadronsFinal.ConeExclCHadronsFinal.ConeExclTausFinal",
#"Photons.f3.f3core"
]

EXOT11ExtraVariablesContentTruth = [
"Electrons.truthOrigin",
"Electrons.truthType",
"Electrons.truthParticleLink",
"MuonTruthParticles.truthOrigin",
"MuonTruthParticles.truthType",
"AntiKt4EMTopoJets.TruthLabelID.HadronConeExclTruthLabelID",
"AntiKt4LCTopoJets.HadronConeExclTruthLabelID",
"AntiKt4TruthJets.HadronConeExclTruthLabelID.ConeExclBHadronsFinal.ConeExclCHadronsFinal.ConeExclTausFinal",
"AntiKt4PV0TrackJets.HadronConeExclTruthLabelID",
"AntiKt3PV0TrackJets.HadronConeExclTruthLabelID",
"AntiKt2PV0TrackJets.HadronConeExclTruthLabelID",
]

EXOT11AllVariablesContent = [
### RECO JETS ###
#"AntiKt4LCTopoJets",
#"AntiKt4EMTopoJets",
"AntiKt10LCTopoJets",
"CamKt12LCTopoJets",
#"BTagging_AntiKt4LCTopo",
#"BTagging_AntiKt4EMTopo",
#"BTagging_AntiKt10LCTopo",

### Track JETS ###
#"AntiKt4ZTrackJets",
#"AntiKt4PV0TrackJets",
"AntiKt2PV0TrackJets",
"AntiKt3PV0TrackJets",
"BTagging_AntiKt2Track",#PV0
"BTagging_AntiKt3Track",#PV0

### MET ###
#"MET_LocHadTopo",
"MET_Track",

### MUONS ###
#"MuonSegments"
]

EXOT11AllVariablesContentTruth = [
### TRUTH ###
"TruthEvents",
"TruthVertices",
"TruthParticles",
"MuonTruthParticles",
"egammaTruthParticles",
"MET_Truth",
#"METMap_Truth",

### TRUTH JETS ###
"AntiKt4TruthJets",
"AntiKt4TruthWZJets",
"AntiKt10TruthJets",
"CamKt12TruthJets",
#"BTagging_AntiKt4TruthWZ",
#"BTagging_AntiKt10TruthWZ"
]

EXOT11Content = [
### JETS ###
#"xAOD::JetContainer#CamKt8LCTopoJets",
#"xAOD::JetContainer#CamKt10LCTopoJets",
"xAOD::JetContainer#CamKt12LCTopoBDRSFilteredMU100Y15Jets",
"xAOD::JetContainer#AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",

#"xAOD::JetAuxContainer#CamKt8LCTopoJetsAux.",
#"xAOD::JetAuxContainer#CamKt10LCTopoJetsAux.",
"xAOD::JetAuxContainer#CamKt12LCTopoBDRSFilteredMU100Y15JetsAux.",
"xAOD::JetAuxContainer#AntiKt10LCTopoTrimmedPtFrac5SmallR20JetsAux.",
]

EXOT11ContentTruth = [
### JETS ###
"xAOD::JetContainer#AntiKt10TruthTrimmedPtFrac5SmallR20Jets",
"xAOD::JetAuxContainer#AntiKt10TruthTrimmedPtFrac5SmallR20JetsAux.",
]
