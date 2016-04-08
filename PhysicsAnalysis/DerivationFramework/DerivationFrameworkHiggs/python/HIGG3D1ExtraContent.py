# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


# The additional HIGG3D1 content lists

HIGG3D1ExtraVariables = [ "GSFTrackParticles.z0.vz",
                          "CombinedMuonTrackParticles.z0.vz",
                          "ExtrapolatedMuonTrackParticles.z0.vz",
                          "Muons.clusterLink.extrapolatedMuonSpectrometerTrackParticleLink",
                          "Electrons.author",
                          "AntiKt4LCTopoJets.JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_phi.JetEMScaleMomentum_m.Jvt.JvtJvfcorr.JvtRpt",
                          "AntiKt4EMTopoJets.JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_phi.JetEMScaleMomentum_m.Jvt.JvtJvfcorr.JvtRpt",
                          "AntiKt4PV0TrackJets.pt.eta.phi.m.constituentLinks.constituentWeights.btaggingLink",
                          "AntiKt2PV0TrackJets.pt.eta.phi.m.constituentLinks.constituentWeights.btaggingLink"]

HIGG3D1ExtraTruthVariables = [ "Electrons.truthOrigin.truthType.truthParticleLink.bkgTruthType.bkgMotherPdgId.bkgTruthOrigin",
                               "AntiKt4LCTopoJets.ConeTruthLabelID.PartonTruthLabelID",
                               "AntiKt4EMTopoJets.ConeTruthLabelID.PartonTruthLabelID",
                               "InDetTrackParticles.truthOrigin.truthType",
                               "AntiKt4PV0TrackJets.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.GhostTruth.GhostTruthCount.GhostTruthPt.HadronConeExclTruthLabelID.PartonTruthLabelID.ConeTruthLabelID",
                               "AntiKt2PV0TrackJets.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.GhostTruth.GhostTruthCount.GhostTruthPt.HadronConeExclTruthLabelID.PartonTruthLabelID.ConeTruthLabelID"]

HIGG3D1ExtraContainers = [ ]

HIGG3D1ExtraTruthContainers = [ "TruthEvents",
                                "TruthParticles",
                                "TruthVertices",
                                "AntiKt4TruthJets",
                                "MuonTruthParticles" ]
