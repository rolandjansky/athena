# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


# The additional HIGG3D1 content lists

HIGG3D1ExtraVariables = [ "GSFTrackParticles.z0.vz",
                          "CombinedMuonTrackParticles.z0.vz",
                          "ExtrapolatedMuonTrackParticles.z0.vz",
                          "BTagging_AntiKt4LCTopo.MV1_discriminant",
                          "BTagging_AntiKt4EMTopo.MV1_discriminant",
                          "Muons.clusterLink.extrapolatedMuonSpectrometerTrackParticleLink",
                          "Electrons.author",
                          "AntiKt4LCTopoJets.JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_phi.JetEMScaleMomentum_m.Jvt.JvtJvfcorr.JvtRpt",
                          "AntiKt4EMTopoJets.JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_phi.JetEMScaleMomentum_m.Jvt.JvtJvfcorr.JvtRpt" ]

HIGG3D1ExtraTruthVariables = [ "Electrons.truthOrigin.truthType.truthParticleLink",
                               "AntiKt4LCTopoJets.ConeTruthLabelID.PartonTruthLabelID",
                               "AntiKt4EMTopoJets.ConeTruthLabelID.PartonTruthLabelID" ]

HIGG3D1ExtraTruthContainers = [ "TruthEvents",
                                "TruthParticles",
                                "TruthVertices",
                                "AntiKt4TruthJets",
                                "AntiKt4TruthWZJets",
                                "MuonTruthParticles" ]
