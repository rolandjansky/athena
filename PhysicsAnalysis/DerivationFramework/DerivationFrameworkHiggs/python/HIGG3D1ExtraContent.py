# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


# The additional HIGG3D1 content lists

HIGG3D1ExtraVariables = [ "GSFTrackParticles.z0.vz",
                          "CombinedMuonTrackParticles.z0.vz",
                          "ExtrapolatedMuonTrackParticles.z0.vz",
                          "Muons.clusterLink.extrapolatedMuonSpectrometerTrackParticleLink.quality.etcone20.ptconecoreTrackPtrCorrection",
                          "Electrons.author.quality.etcone20.ptconecoreTrackPtrCorrection",
                          "AntiKt4EMTopoJets.JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_phi.JetEMScaleMomentum_m.Jvt.JvtJvfcorr.JvtRpt",
                          "AntiKt4EMPFlowJets.JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_phi.JetEMScaleMomentum_m.Jvt.JvtJvfcorr.JvtRpt.btagging.constituentLinks.GhostBHadronsFinal.GhostBHadronsInitial.GhostBQuarksFinal.GhostCHadronsFinal.GhostCHadronsInitial.GhostCQuarksFinal.GhostHBosons.GhostPartons.GhostTQuarksFinal.GhostTausFinal.GhostWBosons.GhostZBosons.GhostTruth.OriginVertex.GhostAntiKt3TrackJet.GhostAntiKt4TrackJet.GhostTrack.HighestJVFVtx.ConeExclBHadronsFinal.ConeExclCHadronsFinal.ConeExclTausFinal.HighestJVFLooseVtx.GhostAntiKt2TrackJet.Jvt.JvtJvfcorr.JvtRpt.SumPtTrkPt500.SumPtTrkPt1000.TrackWidthPt1000.TrackWidthPt500"
                          "AntiKt4PV0TrackJets.pt.eta.phi.m.constituentLinks.constituentWeights.btaggingLink",
                          "CaloCalTopoClusters.rawE.rawEta.rawPhi.rawM.calE.calEta.calPhi.calM.e_sampl",
                          "MuonClusterCollection.eta_sampl.phi_sampl",
                          "BTagging_AntiKt4EMPFlow.SV1_pb.SV1_pu.IP3D_pb.IP3D_pu.MV2c00_discriminant.MV2c10_discriminant.MV2c20_discriminant.MVb_discriminant.MV1_discriminant.MSV_vertices.MV1c_discriminant.SV0_badTracksIP.SV0_vertices.SV1_badTracksIP.SV1_vertices.BTagTrackToJetAssociator.BTagTrackToJetAssociatorBB.JetFitter_JFvertices.JetFitter_tracksAtPVlinks.MSV_badTracksIP.MV2c100_discriminant.MV2m_pu.MV2m_pc.MV2m_pb",
                          "AntiKtVR30Rmax4Rmin02TrackJets.NumTrkPt1000.NumTrkPt500"
                          ]

HIGG3D1ExtraTruthVariables = [ "Electrons.truthOrigin.truthType.truthParticleLink.bkgTruthType.bkgMotherPdgId.bkgTruthOrigin.bkgTruthParticleLink.firstEgMotherTruthType.firstEgMotherTruthOrigin.firstEgMotherPdgId.firstEgMotherTruthParticleLink",
                               "AntiKt4EMTopoJets.ConeTruthLabelID.PartonTruthLabelID",
                               "InDetTrackParticles.truthOrigin.truthType",
                               "AntiKt4PV0TrackJets.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.GhostTruth.GhostTruthCount.GhostTruthPt.HadronConeExclTruthLabelID.PartonTruthLabelID.ConeTruthLabelID",
                               "AntiKt4TruthWZJets.pt.eta.phi.m.constituentLinks.constituentWeights.HadronConeExclTruthLabelID.ConeTruthLabelID.PartonTruthLabelID.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.btaggingLink",
                               "AntiKt4TruthJets.GhostBHadronsFinalCount.GhostBHadronsFinalPt.GhostCHadronsFinalCount.GhostCHadronsFinalPt.GhostTausFinalCount.GhostTausFinalPt.ActiveArea.ActiveArea4vec_m.ActiveArea4vec_pt.ActiveArea4vec_eta.ActiveArea4vec_phi",
                               "AntiKt4EMPFlowJets.TruthLabelID.ConeTruthLabelID.PartonTruthLabelID.HadronConeExclTruthLabelID"]

HIGG3D1ExtraContainers = [ ]

HIGG3D1ExtraTruthContainers = [ "TruthEvents",
                                "TruthVertices",
                                "TruthParticles",
                                "TruthElectrons",
                                "TruthMuons",
                                "TruthPhotons",
                                "TruthNeutrinos",
                                "TruthBSM",
                                "MuonTruthParticles" ]
