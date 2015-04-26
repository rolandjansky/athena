# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#Content included in addition to the Smart Slimming Content

ExtraContent=[
    #"Electrons.topoetcone20.topoetcone30.topoetcone40",
    #"Electrons.ptcone20.ptcone30.ptcone40",
    "Electrons.author.charge.OQ.emins1.fracs1.f3core.deltaPhi2",
    "egammaClusters.rawE.e_sampl.eta_sampl.phi_sampl.calM.calE.calEta.calPhi.etaCalo.phiCalo",
    "GSFTrackParticles.z0.vz.numberOfPixelOutliers.numberOfSCTOutliers",
    "BTagging_AntiKt4LCTopo.MV1_discriminant",
    "Muons.ptcone20.ptcone30.ptcone40.etcone20.etcone30.etcone40",
    "Muons.clusterLink",
    "CombinedMuonTrackParticles.d0.z0.vz",
    "ExtrapolatedMuonTrackParticles.d0.z0.vz.numberOfPixelHits.numberOfPixelDeadSensors",
    "InDetTrackParticles.numberOfTRTHits.numberOfTRTOutliers",
    "AntiKt4LCTopoJets.NumTrkPt500.Width.TrackWidthPt1000.TrackWidthPt500.TrackCount",
    "AntiKt4LCTopoJets.JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_phi.JetEMScaleMomentum_m",
    "AntiKt4EMTopoJets.TrackWidthPt500.TrackCount",
    "AntiKt4EMTopoJets.JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_phi.JetEMScaleMomentum_m",
    "Photons.f3.f3core"]

ExtraContentTruth=[
    "Electrons.truthOrigin",
    "Electrons.truthType",
    "Electrons.truthParticleLink",
    "AntiKt4EMTopoJets.ConeTruthLabelID",
    "AntiKt4LCTopoJets.ConeTruthLabelID"]

ExtraContainers=[
    "AntiKt10LCTopoJets",
    "CamKt12LCTopoJets",
    #"BTagging_AntiKt10LCTopo",
    "BTagging_AntiKt3Track",
    "BTagging_AntiKt2Track",
    "MET_Track",
    #"METMap_Track",
    "MuonSegments"]

ExtraContainersTruth=[
    "TruthEvents",
    "TruthParticles",
    "TruthVertices",
    "AntiKt4TruthJets",
    "MuonTruthParticles",
    #"BTagging_AntiKt4Truth",
    "MET_Truth",
    "METMap_Truth"]
