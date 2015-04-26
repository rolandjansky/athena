# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#Content included in addition to the Smart Slimming Content

# need to eliminate obsolete isolation variables
ExtraContentElectrons=[
    "Electrons.topoetcone20.topoetcone30.topoetcone40.ptcone20.ptcone30.ptcone40.Loose.Medium.Tight.DFCommonElectronsLHLoose.DFCommonElectronsLHMedium.DFCommonElectronsLHTight.DFCommonElectronsML.author.OQ.charge.LHLoose.LHMedium.LHTight.LHValue"
    ]

ExtraElectronsTruth=[
    "Electrons.truthOrigin.truthType.truthParticleLink"
    ]

ExtraContentMuons=[
    "Muons.DFCommonGoodMuons.ptcone20.ptcone30.ptcone40.topoetcone20.topoetcone30.topoetcone40.quality.charge",
    "CombinedMuonTrackParticles.d0.z0.vz",
    "InDetTrackParticles.numberOfTRTHits.numberOfTRTOutliers"
    ]

ExtraMuonsTruth=[
    "MuonTruthParticles.truthOrigin.truthType"
    ]

ExtraContentTaus=[
    "TauJets.BDTEleScore.BDTJetScore.eta.jetLink.m.phi.pt.charge.isTauFlags.TauPi0NeutralPFOContainerAuxDyn.pt.eta.phi.m.bdtPi0Score.pantau_CellBasedInput_BDTValue_1p0n_vs_1p1n.pantau_CellBasedInput_BDTValue_1p1n_vs_1pXn.pantau_CellBasedInput_BDTValue_3p0n_vs_3pXn",
    "TauNeutralParticleFlowObjects.pt.eta.phi.m.bdtPi0Score"
    ]

ExtraContentVtx=["PrimaryVertices.x.y.z.vertexType"]

ExtraContentJets=["AntiKt4EMTopoJets.JvtJvfcorr.JvtRpt.Jvt"]

#ExtraContentTrk=["InDetTrackParticles.definingParametersCovMatrixVec.theta.z0.d0.qOverP"]

ExtraContentBtag=["BTagging_AntiKt4EMTopo.MV1_discriminant"]

ExtraTausTruth=[]

ExtraContentAll=ExtraContentElectrons+ExtraContentMuons+ExtraContentTaus+ExtraContentBtag+ExtraContentVtx+ExtraContentJets

ExtraContentAllTruth=ExtraElectronsTruth+ExtraMuonsTruth+ExtraTausTruth

ExtraContainersTruth=["TruthEvents", "TruthParticles", "TruthVertices", "AntiKt4TruthJets","AntiKt4TruthWZJets"]

ExtraContainersAll=["MET_Track"]


