# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

EXOT3SmartContent = [
    "Electrons",
    "Muons",
    "Photons",
    "InDetTrackParticles",
    "PrimaryVertices", 
    "AntiKt4TruthJets",
    "AntiKt4EMPFlowJets",
    "AntiKt4EMPFlowJets_BTagging201810",
    "AntiKt4EMPFlowJets_BTagging201903",
    "AntiKt4EMTopoJets",
    "AntiKt4EMTopoJets_BTagging201810",
    "BTagging_AntiKt4EMTopo_201810",
    "BTagging_AntiKt4EMPFlow_201810",
    "BTagging_AntiKt4EMPFlow_201903",
    "TauJets",
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
    "AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets"
]

EXOT3ExtraVariables = [
    "Electrons.AverageLArQF",
    "Electrons.author.Medium.Tight.Loose.charge", 
    "Muons.etcone30",
    "MuonSpectrometerTrackParticles.phi.theta.qOverP",
    "TruthParticles.prodVtxLink.decayVtxLink.m.px.py.pz.barcode.e.pdgId.status",
    "AntiKt4TruthJets.pt.eta.phi.m",
    "TruthVertices.incomingParticleLinks.outgoingParticleLinks",
    "AntiKt10LCTopoJets.PartonTruthLabelID.NumTrkPt500.SumPtTrkPt500.GhostAntiKt2TrackJet.TrackWidthPt500.ConeExclBHadronsFinal.ConeExclCHadronsFinal.GhostHBosons.GhostTausFinal.GhostVR30Rmax4Rmin02TrackJet.GhostWBosonsCount.GhostZBosonsCount.NumTrkPt1000.SumPtTrkPt1000.TrackWidthPt1000.GhostTrack.GhostTrackCount.EMFrac",
    "AntiKt10TrackCaloClusterJets.NumTrkPt500.SumPtTrkPt500.GhostAntiKt2TrackJet.TrackWidthPt500.ConeExclBHadronsFinal.ConeExclCHadronsFinal.GhostHBosons.GhostTausFinal.GhostVR30Rmax4Rmin02TrackJet.GhostWBosonsCount.GhostZBosonsCount.NumTrkPt1000.SumPtTrkPt1000.TrackWidthPt1000.GhostTrack.GhostTrackCount",
    "AntiKt10UFOCSSKJets.NumTrkPt500.SumPtTrkPt500.GhostAntiKt2TrackJet.TrackWidthPt500.ConeExclBHadronsFinal.ConeExclCHadronsFinal.GhostHBosons.GhostTausFinal.GhostVR30Rmax4Rmin02TrackJet.GhostWBosonsCount.GhostZBosonsCount.NumTrkPt1000.SumPtTrkPt1000.TrackWidthPt1000.GhostTrack.GhostTrackCount",
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.NumTrkPt500.SumPtTrkPt500.GhostAntiKt2TrackJet.TrackWidthPt500.ConeExclBHadronsFinal.ConeExclCHadronsFinal.GhostHBosons.GhostTausFinal.GhostVR30Rmax4Rmin02TrackJet.GhostWBosonsCount.GhostZBosonsCount.NumTrkPt1000.SumPtTrkPt1000.TrackWidthPt1000.GhostTrack.GhostTrackCount.Width",
    "AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets.NumTrkPt500.SumPtTrkPt500.GhostAntiKt2TrackJet.TrackWidthPt500.ConeExclBHadronsFinal.ConeExclCHadronsFinal.GhostHBosons.GhostTausFinal.GhostVR30Rmax4Rmin02TrackJet.GhostWBosonsCount.GhostZBosonsCount.NumTrkPt1000.SumPtTrkPt1000.TrackWidthPt1000.GhostTrack.GhostTrackCount.Width",
    "AntiKt10UFOCSSKSoftDropBeta100Zcut10Jets.NumTrkPt500.SumPtTrkPt500.GhostAntiKt2TrackJet.TrackWidthPt500.ConeExclBHadronsFinal.ConeExclCHadronsFinal.GhostHBosons.GhostTausFinal.GhostVR30Rmax4Rmin02TrackJet.GhostWBosonsCount.GhostZBosonsCount.NumTrkPt1000.SumPtTrkPt1000.TrackWidthPt1000.GhostTrack.GhostTrackCount.Width",
    "AntiKt10TruthTrimmedPtFrac5SmallR20Jets.pt.eta.phi.m.ECF1.ECF2.ECF3.Tau2_wta.Tau3_wta.Qw.nChargedParticles",
    "AntiKt10RCEMPFlowJets.NTrimSubjets",
    "AntiKt10RCTruthJets.pt.eta.phi.m.nChargedParticles",
    "AntiKt4EMPFlowJets.pt.eta.phi.m.btaggingLink.JetConstitScaleMomentum_pt.JetConstitScaleMomentum_eta.JetConstitScaleMomentum_phi.JetConstitScaleMomentum_m.btaggingLink.EnergyPerSampling.NumTrkPt500.SumPtTrkPt500.EMFrac.Width.Jvt.TrackWidthPt500.GhostMuonSegmentCount.isBJet.DetectorEta.HadronConeExclTruthLabelID.ActiveArea4vec_eta.ActiveArea4vec_m.ActiveArea4vec_phi.ActiveArea4vec_pt.AverageLArQF.FracSamplingMax.FracSamplingMaxIndex.GhostTrack.HECFrac.HECQuality.JVFCorr.LArQuality.NegativeE.NumTrkPt1000.TrackWidthPt1000.JetEtaJESScaleMomentum_eta.JetEtaJESScaleMomentum_m.JetEtaJESScaleMomentum_phi.JetEtaJESScaleMomentum_pt.JetPileupScaleMomentum_eta.JetPileupScaleMomentum_m.JetPileupScaleMomentum_phi.JetPileupScaleMomentum_pt.OriginCorrected.PileupCorrected",
"BTagging_AntiKt4EMPFlow.DL1_pb.DL1_pc.DL1_pu.DL1rmu_pb.DL1rmu_pc.DL1rmu_pu.DL1r_pb.DL1r_pc.DL1r_pu.MV2c10_discriminant.MV2r_discriminant.MV2rmu_discriminant.MV2cl100_discriminant.MV2c100_discriminant",
    "AntiKt4EMTopoJets.pt.eta.phi.m.JetConstitScaleMomentum_pt.JetConstitScaleMomentum_eta.JetConstitScaleMomentum_phi.JetConstitScaleMomentum_m.btaggingLink.EnergyPerSampling.NumTrkPt500.SumPtTrkPt500.EMFrac.Width.Jvt.TrackWidthPt500.GhostMuonSegmentCount.isBJet.DetectorEta.HadronConeExclTruthLabelID.ActiveArea4vec_eta.ActiveArea4vec_m.ActiveArea4vec_phi.ActiveArea4vec_pt.AverageLArQF.FracSamplingMax.FracSamplingMaxIndex.GhostTrack.HECFrac.HECQuality.JVFCorr.LArQuality.NegativeE.NumTrkPt1000.TrackWidthPt1000.JetEtaJESScaleMomentum_eta.JetEtaJESScaleMomentum_m.JetEtaJESScaleMomentum_phi.JetEtaJESScaleMomentum_pt.JetPileupScaleMomentum_eta.JetPileupScaleMomentum_m.JetPileupScaleMomentum_phi.JetPileupScaleMomentum_pt.OriginCorrected.PileupCorrected",
    "AntiKt2PV0TrackJets.pt.eta.phi.m.constituentLinks.HadronConeExclTruthLabelID.ConeExclBHadronsFinal.ConeExclCHadronsFinal.GhostHBosons.GhostTausFinal",
    "TauJets.pt",
    "BTagging_AntiKt4EMTopo.DL1_pb.DL1_pc.DL1_pu.DL1rmu_pb.DL1rmu_pc.DL1rmu_pu.DL1r_pb.DL1r_pc.DL1r_pu.MV2c10_discriminant.MV2r_discriminant.MV2rmu_discriminant.MV2cl100_discriminant.MV2c100_discriminant",
    "CombinedMuonTrackParticles.phi.z0.d0.theta.vz.chiSquared.definingParametersCovMatrix.numberDoF.numberOfPixelDeadSensors.numberOfPixelHits.numberOfPixelHoles.numberOfSCTDeadSensors.numberOfSCTHits.numberOfSCTHoles.numberOfTRTHits.numberOfTRTOutliers.qOverP",
    "ExtrapolatedMuonTrackParticles.phi.z0.d0.theta.vz.definingParametersCovMatrix.numberOfPixelDeadSensors.numberOfPixelHits.qOverP",
    "GSFTrackParticles.phi.z0.d0.theta.vz.definingParametersCovMatrix.eProbabilityHT.expectInnermostPixelLayerHit.expectNextToInnermostPixelLayerHit.numberOfInnermostPixelLayerHits.numberOfInnermostPixelLayerOutliers.numberOfNextToInnermostPixelLayerHits.numberOfNextToInnermostPixelLayerOutliers.numberOfPixelDeadSensors.numberOfPixelHits.numberOfSCTDeadSensors.numberOfSCTHits.parameterPX.parameterPY.parameterPZ.parameterPosition.parameterX.qOverP"
]

EXOT3AllVariablesContent = [
     ### TRUTH ###
     "TruthEvents",
     "TruthVertices",

     ### MUONS ###
     "MuonSegments",

     ### TOPOCLUSTERS ###
     "CaloCalTopoClusters",
     
     ### TRACKCALOCLUSTERS ###
     "TrackCaloClustersCombinedAndNeutral",

     "METAssoc_AntiKt4EMTopo",
     "MET_Core_AntiKt4EMTopo",
     "METAssoc_AntiKt4EMPFlow",
     "MET_Core_AntiKt4EMPFlow"
]

EXOT3StaticContent = []
