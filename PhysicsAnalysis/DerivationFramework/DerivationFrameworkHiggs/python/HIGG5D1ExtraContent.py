# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#Content included in addition to the Smart Slimming Content

ExtraContent=[
    "egammaClusters.rawE.phi_sampl.calM",
    "Muons.clusterLink.EnergyLoss.energyLossType",
    "AntiKt4EMTopoJets.TrackWidthPt500.GhostTrackCount.Jvt.JvtJvfcorr.JvtRpt",
    "AntiKt4EMTopoJets.JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_phi.JetEMScaleMomentum_m.DetectorEta",
    "AntiKt4EMTopoJets.DFCommonJets_Calib_pt.DFCommonJets_Calib_eta.DFCommonJets_Calib_phi.DFCommonJets_Calib_m",
    "Photons.f3core",
    "TauJets.IsTruthMatched.truthJetLink.truthParticleLink.ptDetectorAxis.etaDetectorAxis.phiDetectorAxis.mDetectorAxis",
    "BTagging_AntiKt4EMTopo.MV2cl100_discriminant",
    "BTagging_AntiKtVR30Rmax4Rmin02Track.MV2c10_discriminant",
    "AntiKtVR30Rmax4Rmin02TrackJets.-JetConstitScaleMomentum_pt.-JetConstitScaleMomentum_eta.-JetConstitScaleMomentum_phi.-JetConstitScaleMomentum_m.-constituentLinks.-constituentWeight.-ConstituentScale"    
    ]
#    "BTagging_AntiKt2Track.DL1_pc.DL1_pb.DL1_pu.DL1mu_pu.DL1mu_pc.DL1mu_pb.DL1rnn_pu.DL1rnn_pb.DL1rnn_pc.MV2cl100_discriminant.MV2c100_discriminant.MV2c10mu_discriminant.MV2c10_discriminant.MV2c10rnn_discriminant",

ExtraContentTruth=[
    "AntiKt4EMTopoJets.ConeTruthLabelID"
    ]

ExtraContainers=[
    "MuonSegments",
    "CaloCalTopoClusters",
    "TauChargedParticleFlowObjects"
    ]

ExtraContainersTruth=[
    "TruthEvents",
    "TruthParticles",
    "TruthVertices",
    "MuonTruthParticles"]
