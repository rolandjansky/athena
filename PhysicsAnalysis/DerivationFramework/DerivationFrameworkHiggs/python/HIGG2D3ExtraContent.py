# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#Content included in addition to the Smart Slimming Content

HIGG2D3ExtraContent=[
    "egammaClusters.rawE.phi_sampl.calM",
    "GSFTrackParticles.expectInnermostPixelLayerHit.numberOfInnermostPixelLayerHits.numberOfInnermostPixelLayerOutliers.expectNextToInnermostPixelLayerHit.numberOfNextToInnermostPixelLayerHits.numberOfNextToInnermostPixelLayerOutliers.eProbabilityHT",
    "Muons.quality.etcone20",
    "CombinedMuonTrackParticles.definingParametersCovMatrix",
    "ExtrapolatedMuonTrackParticles.definingParametersCovMatrix.numberOfTRTHits.numberOfTRTOutliers",
    "AntiKt4EMTopoJets.JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_phi.JetEMScaleMomentum_m.Jvt.JvtJvfcorr.JvtRpt",
    "AntiKt4LCTopoJets.Jvt.JvtJvfcorr.JvtRpt"]

HIGG2D3ExtraContentTruth=[
    "AntiKt4EMTopoJets.ConeTruthLabelID.PartonTruthLabelID",
    "AntiKt4LCTopoJets.ConeTruthLabelID.PartonTruthLabelID"]

HIGG2D3ExtraContainers=[
    "MuonSegments",
    "MET_Track"]#,
    #"METMap_Track"]

HIGG2D3ExtraContainersTruth=[
    "TruthEvents",
    "TruthParticles",
    "TruthVertices",
    "AntiKt4TruthJets",
    "MuonTruthParticles"]
