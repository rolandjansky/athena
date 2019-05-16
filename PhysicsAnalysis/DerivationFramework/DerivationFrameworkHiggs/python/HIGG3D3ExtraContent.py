# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


# The additional HIGG3D3 content lists

HIGG3D3ExtraVariables = [ "GSFTrackParticles.z0.vz.numberOfNextToInnermostPixelLayerHits",
                          "CombinedMuonTrackParticles.z0.vz",
                          "ExtrapolatedMuonTrackParticles.z0.vz",
                          "Muons.clusterLink.extrapolatedMuonSpectrometerTrackParticleLink",
                          "Electrons.author.ptvarcone40.ptcone20_TightTTVA_pt500.ptcone20_TightTTVA_pt1000",
                          "AntiKt4EMTopoJets.JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_phi.JetEMScaleMomentum_m.Jvt.JvtJvfcorr.JvtRpt.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1.DFCommonJets_QGTagger_NTruthCharged",
                          "AntiKt4EMPFlowJets.JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_phi.JetEMScaleMomentum_m.Jvt.JvtJvfcorr.JvtRpt.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1.DFCommonJets_QGTagger_NTruthCharged.btagging.constituentLinks.GhostBHadronsFinal.GhostBHadronsInitial.GhostBQuarksFinal.GhostCHadronsFinal.GhostCHadronsInitial.GhostCQuarksFinal.GhostHBosons.GhostPartons.GhostTQuarksFinal.GhostTausFinal.GhostWBosons.GhostZBosons.GhostTruth.OriginVertex.GhostAntiKt3TrackJet.GhostAntiKt4TrackJet.GhostTrack.HighestJVFVtx.ConeExclBHadronsFinal.ConeExclCHadronsFinal.ConeExclTausFinal.HighestJVFLooseVtx.GhostAntiKt2TrackJet.Jvt.JvtJvfcorr.JvtRpt" ]

HIGG3D3ExtraTruthVariables = [ "Electrons.truthOrigin.truthType.truthParticleLink",
                               "AntiKt4EMTopoJets.ConeTruthLabelID.PartonTruthLabelID",
                               "AntiKt4TruthDressedWZJets.pt.eta.phi.m.constituentLinks.constituentWeights.HadronConeExclTruthLabelID.ConeTruthLabelID.PartonTruthLabelID.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.btaggingLink",
                               "AntiKt4EMPFlowJets.TruthLabelID.ConeTruthLabelID.PartonTruthLabelID.HadronConeExclTruthLabelID" ]

HIGG3D3TruthDecoratorVariables = [ "Electrons.HIGG3DX_truthFlavourTag",
                                   "Muons.HIGG3DX_truthFlavourTag",
                                   "AntiKt4EMTopoJets.HIGG3DX_truthOrigin",
                                   "AntiKt4EMPFlowJets.HIGG3DX_truthOrigin"
                                   ]

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
                                "TruthBSMWithDecayParticles",
                                "TruthBSMWithDecayVertices",
                                "MuonTruthParticles" ]
