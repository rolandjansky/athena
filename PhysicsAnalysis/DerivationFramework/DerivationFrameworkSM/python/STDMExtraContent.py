# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#Content included in addition to the Smart Slimming Content
ExtraContentElectrons=["Electrons.topoetcone20.topoetcone30.topoetcone40.ptcone20.ptcone30.ptcone40.Loose.Medium.Tight.DFCommonElectronsLHLoose.DFCommonElectronsLHMedium.DFCommonElectronsLHTight.DFCommonElectronsML.author.OQ.LHLoose.LHMedium.LHTight.LHValue"]

ExtraContentPhotons=["Photons.topoetcone20.topoetcone30.topoetcone40.ptcone20.ptcone30.ptcone40.Loose.Medium.Tight.author.OQ.f3.f3core"]+["egammaTopoSeededClusters.calE.calEta.calPhi.e_sampl.eta_sampl.ETACALOFRAME.PHICALOFRAME.eta"]

ExtraContentMuons=["Muons.DFCommonGoodMuon.charge.ptcone20.ptcone30.ptcone40.topoetcone20.topoetcone30.topoetcone40"]

ExtraContentTaus=["TauJets.BDTEleScore.BDTJetScore.eta.jetLink.m.phi.pt.charge.isTauFlags"]

ExtraContentBtag=["BTagging_AntiKt4LCTopo.MV1_discriminant"]

ExtraContentVtx=["PrimaryVertices.x.y.z.covariance.trackWeights.vertexType.sumPt2"]

ExtraContentTrk=["InDetTrackParticles.definingParametersCovMatrixVec.theta.z0.d0.qOverP"]

ExtraContentJets=["AntiKt4EMTopoJets.JvtJvfcorr.JvtRpt.Jvt"]

ExtraContentAll=ExtraContentElectrons+ExtraContentPhotons+ExtraContentMuons+ExtraContentTaus+ExtraContentBtag+ExtraContentVtx+ExtraContentTrk+ExtraContentJets

# These are only added if running on MC
ExtraElectronsTruth=["Electrons.truthOrigin.truthType.truthParticleLink"]

ExtraPhotonsTruth=["Photons.truthOrigin.truthParticleLink.truthType"]

ExtraMuonsTruth=["MuonTruthParticles.truthOrigin.truthType"]

ExtraTausTruth=[]

ExtraContentAllTruth=ExtraElectronsTruth+ExtraPhotonsTruth+ExtraMuonsTruth+ExtraTausTruth

# Extra containers
ExtraContainersElectron=["ForwardElectrons"]

ExtraContainersJets=[]#Place Holder

ExtraContainersTruth=["TruthEvents", "TruthParticles", "TruthVertices", "AntiKt4TruthJets","AntiKt4TruthWZJets"]
ExtraContainersTruthPhotons=["egammaTruthParticles"]

# Do not exist (at least for now)
#ExtraContainers6Jets=["AntiKt6EMTopoJets","AntiKt6LCTopoJets", "BTagging_AntiKt6LCTopo", "BTagging_AntiKt6EMTopo"]

ExtraContainersAll=ExtraContainersElectron+ExtraContainersJets
