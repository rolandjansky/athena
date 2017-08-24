# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

EXOT0Content = []

EXOT0SmartCollections = [
    "Photons",
    "TauJets",
    "Electrons",
    "Muons",
    "MET_Reference_AntiKt4EMTopo",
    "BTagging_AntiKt4EMTopo",
    "AntiKt4EMTopoJets",
    "PrimaryVertices",
    "InDetTrackParticles"
]

EXOT0AllVariables = [
    "TruthParticles",
    "TruthEvents",
    "TruthVertices",
    "TruthElectrons",
    "TruthMuons",
    "TruthTaus",
    "TruthPhotons",
    "TruthNeutrinos",
    "TruthTop",
    "TruthBSM",
    "TruthBoson",
    "MET_Truth",
    "MET_Track",
    "MuonSpectrometerTrackParticles",
    "CombinedMuonTrackParticles",
    "ExtrapolatedMuonTrackParticles",
    "MuonTruthParticles"
]

EXOT0ExtraVariables = [
    "Muons.pt.eta.phi.truthType.truthOrigin.author.muonType.quality.inDetTrackParticleLink.muonSpectrometerTrackParticleLink.combinedTrackParticleLink.InnerDetectorPt.MuonSpectrometerPt.DFCommonGoodMuon.momentumBalanceSignificance.clusterLink.ptcone20.ptcone30.ptcone40.ptvarcone20.ptvarcone30.ptvarcone40.topoetcone20.topoetcone30.topoetcone40.etcone20.etcone30.etcone40.innerSmallHits.truthParticleLink.charge.CaloLRLikelihood.CaloMuonIDTag.extrapolatedMuonSpectrometerTrackParticleLink.numberOfPrecisionLayers.numberOfPrecisionHoleLayers.innerLargeHits.muonSegmentLinks.numberOfGoodPrecisionLayers.innerSmallHoles.innerLargeHoles.middleSmallHoles.middleLargeHoles.outerSmallHoles.outerLargeHoles.extendedSmallHoles.extendedLargeHoles.innerClosePrecisionHits.middleClosePrecisionHits.outerClosePrecisionHits.extendedClosePrecisionHits.innerOutBoundsPrecisionHits.middleOutBoundsPrecisionHits.outerOutBoundsPrecisionHits.extendedOutBoundsPrecisionHits.combinedTrackOutBoundsPrecisionHits.isEndcapGoodLayers.isSmallGoodSectors.middleLargeHits.middleSmallHits.outerLargeHits.outerSmallHits.extendedSmallHits.extendedLargeHits.cscEtaHits.cscUnspoiledEtaHits.spectrometerFieldIntegral.scatteringCurvatureSignificance.scatteringNeighbourSignificance.msOnlyExtrapolatedMuonSpectrometerTrackParticleLink.allAuthors.numberOfPhiLayers.numberOfPhiHoleLayers.numberOfTriggerEtaLayers.numberOfTriggerEtaHoleLayers.nUnspoiledCscHits.FSR_CandidateEnergy.EnergyLoss.EnergyLossSigma.ParamEnergyLoss.ParamEnergyLossSigmaPlus.ParamEnergyLossSigmaMinus.MeasEnergyLoss.MeasEnergyLossSigma.msInnerMatchChi2.msInnerMatchDOF.energyLossType.primarySector.secondarySector.phiLayer1Hit.phiLayer2Hits.phiLayer3Hits.phiLayer4Hits.etaLayer1Hits.etaLayer2Hits.etaLayer3Hits.etaLayer4Hits.phiLayer1Holes.phiLayer2Holes.phiLayer3Holes.phiLayer4Holes.etaLayer1Holes.etaLayer2Holes.etaLayer3Holes.etaLayer4Holes.phiLayer1RPCHits.phiLayer2RPCHits.phiLayer3RPCHits.phiLayer4RPCHits.etaLayer1RPCHits.etaLayer2RPCHits.etaLayer3RPCHits.etaLayer3RPCHits.phiLayer1RPCHoles.phiLayer2RPCHoles.phiLayer3RPCHoles.phiLayer4RPCHoles.etaLayer1RPCHoles.etaLayer2RPCHoles.etaLayer3RPCHoles.etaLayer4RPCHoles.phiLayer1TGCHits.phiLayer2TGCHits.phiLayer3TGCHits.phiLayer4TGCHits.etaLayer1TGCHits.etaLayer2TGCHits.etaLayer3TGCHits.etaLayer4TGCHits.phiLayer1TGCHoles.phiLayer2TGCHoles.phiLayer3TGCHoles.phiLayer4TGCHoles.etaLayer1TGCHoles.etaLayer2TGCHoles.etaLayer3TGCHoles.etaLayer4TGCHoles.spectrometerFieldIntegral.segmentDeltaEta.segmentDeltaPhi.segmentChi2OverDoF.t0.beta.annBarrel.annEndCap.innAngle.midAngle.meanDeltaADCCountsMDT.msOuterMatchDOF",
    "Photons.author.Loose.Tight",
    "AntiKt4EMTopoJets.NumTrkPt1000.TrackWidthPt1000.NumTrkPt500.DFCommonJets_Calib_pt.DFCommonJets_Calib_eta.DFCommonJets_Calib_phi", # TODO: .DFCommonJets_Jvt",
    "GSFTrackParticles.z0.d0.vz.definingParametersCovMatrix",
    "CombinedMuonTrackParticles.d0.z0.vz.truthOrigin.truthType.phi.theta.qOverP.numberOfPixelHits.numberOfPixelHoles.numberOfPixelDeadSensors.numberOfSCTHits.numberOfSCTHoles.numberOfSCTDeadSensors.numberOfTRTHits.numberOfTRTOutliers.numberOfPrecisionLayers.numberOfPrecisionHoleLayers.d0.z0.vz.definingParametersCovMatrix.vertexLink.truthParticleLink.chiSquared.numberDoF.numberOfPhiLayers.numberOfPhiHoleLayers.numberOfInnermostPixelLayerHits.alignEffectChId.alignEffectDeltaTrans.alignEffectSigmaDeltaTrans.alignEffectDeltaAngle.alignEffectSigmaDeltaAngle.msInnerMatchChi2",
    "ExtrapolatedMuonTrackParticles.d0.z0.vz.truthOrigin.truthType.phi.theta.qOverP.numberOfPixelHits.numberOfPixelDeadSensors.numberOfPrecisionLayers.d0.z0.vz.definingParametersCovMatrix.vertexLink.truthParticleLink.chiSquared.numberDoF.numberOfPhiLayers.numberOfPhiHoleLayers.numberOfPixelHoles.numberOfPrecisionHoleLayers.numberOfSCTDeadSensors.numberOfSCTHits.numberOfSCTHoles.numberOfTRTHits.numberOfTRTOutliers",
    "MuonSpectrometerTrackParticles.d0.z0.vz.truthOrigin.truthType.phi.theta.qOverP.numberOfPixelHits.numberOfPixelDeadSensors.numberOfPrecisionLayers.d0.z0.vz.definingParametersCovMatrix.vertexLink.truthParticleLink.chiSquared.numberDoF.numberOfPhiLayers.numberOfPhiHoleLayers.numberOfPixelHoles.numberOfPrecisionHoleLayers.numberOfSCTDeadSensors.numberOfSCTHits.numberOfSCTHoles.numberOfTRTHits.numberOfTRTOutliers",
    "TauJets.IsTruthMatched.truthOrigin.truthType.truthParticleLink.truthJetLink",
    "MuonTruthParticles.barcode.decayVtxLink.e.m.pdgId.prodVtxLink.px.py.pz.recoMuonLink.status.truthOrigin.truthType",
    "AntiKt4TruthJets.eta.m.phi.pt.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.TruthLabelID.ConeTruthLabelID.PartonTruthLabelID",
    "InDetTrackParticles.TrkIsoPt1000_ptcone20.TrkIsoPt1000_ptcone30.TrkIsoPt1000_ptcone40.TrkIsoPt500_ptcone20.TrkIsoPt500_ptcone30.TrkIsoPt500_ptcone40",
]
