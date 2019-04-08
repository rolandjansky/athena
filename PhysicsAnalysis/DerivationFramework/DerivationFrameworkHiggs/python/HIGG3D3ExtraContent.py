# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


# The additional HIGG3D3 content lists

HIGG3D3ExtraVariables = [ "GSFTrackParticles.z0.vz.numberOfNextToInnermostPixelLayerHits",
                          "CombinedMuonTrackParticles.z0.vz",
                          "ExtrapolatedMuonTrackParticles.z0.vz",
                          "Muons.clusterLink.extrapolatedMuonSpectrometerTrackParticleLink",
                          "Electrons.author",
                          "AntiKt4EMTopoJets.JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_phi.JetEMScaleMomentum_m.Jvt.JvtJvfcorr.JvtRpt",
                          "AntiKt4EMPFlowJets.JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_phi.JetEMScaleMomentum_m.Jvt.JvtJvfcorr.JvtRpt.btagging.constituentLinks.GhostBHadronsFinal.GhostBHadronsInitial.GhostBQuarksFinal.GhostCHadronsFinal.GhostCHadronsInitial.GhostCQuarksFinal.GhostHBosons.GhostPartons.GhostTQuarksFinal.GhostTausFinal.GhostWBosons.GhostZBosons.GhostTruth.OriginVertex.GhostAntiKt3TrackJet.GhostAntiKt4TrackJet.GhostTrack.HighestJVFVtx.ConeExclBHadronsFinal.ConeExclCHadronsFinal.ConeExclTausFinal.HighestJVFLooseVtx.GhostAntiKt2TrackJet.Jvt.JvtJvfcorr.JvtRpt.SumPtTrkPt500.SumPtTrkPt1000.TrackWidthPt1000.TrackWidthPt500" ]

HIGG3D3ExtraTruthVariables = [ "Electrons.truthOrigin.truthType.truthParticleLink",
                               "AntiKt4EMTopoJets.ConeTruthLabelID.PartonTruthLabelID",
                               "AntiKt4TruthDressedWZJets.pt.eta.phi.m.constituentLinks.constituentWeights.HadronConeExclTruthLabelID.ConeTruthLabelID.PartonTruthLabelID.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.btaggingLink",
                               "AntiKt4EMPFlowJets.TruthLabelID.ConeTruthLabelID.PartonTruthLabelID.HadronConeExclTruthLabelID" ]

HIGG3D3ExtraContainers = [ ]

HIGG3D3ExtraTruthContainers = [ "TruthEvents",
                                "TruthVertices",
                                "TruthParticles",
                                "TruthElectrons",
                                "TruthMuons",
                                "TruthPhotons",
                                "TruthTaus",
                                "TruthNeutrinos",
                                "TruthBSM",
                                "TruthTop",
                                "TruthBoson",
                                "TruthWbosonWithDecayParticles",
                                "TruthWbosonWithDecayVertices",
                                "MuonTruthParticles" ]
