# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#Content included in addition to the Smart Slimming Content

HIGG2D4ExtraContent=[
    "Electrons.f3core",
    "egammaClusters.rawE.phi_sampl.calM",
    "GSFTrackParticles.expectInnermostPixelLayerHit.numberOfInnermostPixelLayerHits.numberOfInnermostPixelLayerOutliers.expectNextToInnermostPixelLayerHit.numberOfNextToInnermostPixelLayerHits.numberOfNextToInnermostPixelLayerOutliers.eProbabilityHT",
    "Muons.quality.etcone20.etcone30.etcone40",
    "CombinedMuonTrackParticles.definingParametersCovMatrix",
    "ExtrapolatedMuonTrackParticles.definingParametersCovMatrix.numberOfTRTHits.numberOfTRTOutliers.numberOfPixelDeadSensors",
    "AntiKt4EMTopoJets.JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_phi.JetEMScaleMomentum_m.Jvt.JvtJvfcorr.JvtRpt",
    "AntiKt4LCTopoJets.TrackWidthPt500.TrackCount.Jvt.JvtJvfcorr.JvtRpt"] # AntiKt4LCTopoJets.TrackCount is not available (2015-03-25)

HIGG2D4ExtraContentTruth=[
    "AntiKt4EMTopoJets.ConeTruthLabelID.PartonTruthLabelID",
    "AntiKt4EMTopoJets.TruthLabelID.ConeTruthLabelID.PartonTruthLabelID"] # AntiKt4EMTopoJets.TruthLabelID is not available (2015-03-25)

HIGG2D4ExtraContainers=[
    "AntiKt10LCTopoJets",
    "CamKt12LCTopoJets",
    # "BTagging_AntiKt10LCTopo", # BTagging_AntiKt10LCTopo is not available (2015-04-22)
    "MuonSegments",
    "MET_Track"]#,
    #"METMap_Track"]

HIGG2D4ExtraContainersTruth=[
    "TruthEvents",
    "TruthParticles",
    "TruthVertices",
    "AntiKt4TruthJets",
    "MuonTruthParticles"]
