# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


# The additional HIGG3D1 content lists

HIGG3D1ExtraVariables = [ "GSFTrackParticles.z0.vz",
                          "CombinedMuonTrackParticles.z0.vz",
                          "ExtrapolatedMuonTrackParticles.z0.vz",
                          "Muons.clusterLink.extrapolatedMuonSpectrometerTrackParticleLink.quality.etcone20.ptconecoreTrackPtrCorrection",
                          "Electrons.author.quality.etcone20.ptconecoreTrackPtrCorrection.ptvarcone40.ptcone20_TightTTVA_pt500.ptcone20_TightTTVA_pt1000",
                          "AntiKt4EMTopoJets.JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_phi.JetEMScaleMomentum_m.Jvt.JvtJvfcorr.JvtRpt.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1.DFCommonJets_QGTagger_NTruthCharged.SumPtTrkPt500.SumPtTrkPt1000.TrackWidthPt500.TrackWidthPt1000.NumTrkPt500.NumTrkPt1000.GhostTruthAssociationLink",
                          "AntiKt4EMPFlowJets.JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_phi.JetEMScaleMomentum_m.Jvt.JvtJvfcorr.JvtRpt.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1.DFCommonJets_QGTagger_NTruthCharged.btagging.constituentLinks.GhostBHadronsFinal.GhostBHadronsInitial.GhostBQuarksFinal.GhostCHadronsFinal.GhostCHadronsInitial.GhostCQuarksFinal.GhostHBosons.GhostPartons.GhostTQuarksFinal.GhostTausFinal.GhostWBosons.GhostZBosons.GhostTruth.OriginVertex.GhostAntiKt3TrackJet.GhostAntiKt4TrackJet.GhostTrack.HighestJVFVtx.ConeExclBHadronsFinal.ConeExclCHadronsFinal.ConeExclTausFinal.HighestJVFLooseVtx.GhostAntiKt2TrackJet.Jvt.JvtJvfcorr.JvtRpt.SumPtTrkPt500.SumPtTrkPt1000.TrackWidthPt500.TrackWidthPt1000.NumTrkPt500.NumTrkPt1000.GhostTruthAssociationLink",
                          "CaloCalTopoClusters.calE.calEta.calPhi.calM",
                          "JetETMissChargedParticleFlowObjects.pt.eta.phi.m",
                          "JetETMissNeutralParticleFlowObjects.pt.eta.phi.m",
                          "MuonClusterCollection.eta_sampl.phi_sampl",
                          "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810.NumTrkPt1000.NumTrkPt500"
                          ]

HIGG3D1ExtraTruthVariables = [ "Electrons.truthOrigin.truthType.truthParticleLink.bkgTruthType.bkgMotherPdgId.bkgTruthOrigin.bkgTruthParticleLink.firstEgMotherTruthType.firstEgMotherTruthOrigin.firstEgMotherPdgId.firstEgMotherTruthParticleLink",
                               "AntiKt4EMTopoJets.ConeTruthLabelID.PartonTruthLabelID",
                               "InDetTrackParticles.truthOrigin.truthType",
                               "AntiKt4TruthDressedWZJets.pt.eta.phi.m.constituentLinks.constituentWeights.HadronConeExclTruthLabelID.ConeTruthLabelID.PartonTruthLabelID.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.btaggingLink.GhostBHadronsFinalCount.GhostBHadronsFinalPt.GhostCHadronsFinalCount.GhostCHadronsFinalPt.GhostTausFinalCount.GhostTausFinalPt.ActiveArea.ActiveArea4vec_m.ActiveArea4vec_pt.ActiveArea4vec_eta.ActiveArea4vec_phi",
                               "AntiKt4EMPFlowJets.TruthLabelID.ConeTruthLabelID.PartonTruthLabelID.HadronConeExclTruthLabelID"]

HIGG3D1TruthDecoratorVariables = [ "Electrons.HIGG3DX_truthFlavourTag",
                                   "Muons.HIGG3DX_truthFlavourTag",
                                   "AntiKt4EMTopoJets.HIGG3DX_truthOrigin",
                                   "AntiKt4EMPFlowJets.HIGG3DX_truthOrigin"
                                   ]

HIGG3D1ExtraContainers = [ ]

HIGG3D1ExtraTruthContainers = [ "TruthEvents",
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
                                "AntiKt10TruthTrimmedPtFrac5SmallR20Jets",
                                "MuonTruthParticles" ]
