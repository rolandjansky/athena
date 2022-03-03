# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#Content included in addition to the Smart Slimming Content
ExtraContentElectrons=["Electrons.topoetcone20.topoetcone30.topoetcone40.ptcone20.ptcone30.ptcone40.maxEcell_time.maxEcell_energy.maxEcell_gain.maxEcell_onlId.maxEcell_x.maxEcell_y.maxEcell_z"] + ["egammaClusters.PHI2CALOFRAME.ETA2CALOFRAME.phi_sampl"]

ExtraContentPhotons=["Photons.topoetcone20.topoetcone30.topoetcone40.ptcone20.ptcone30.ptcone40.f3.f3core.maxEcell_time.maxEcell_energy.maxEcell_gain.maxEcell_onlId.maxEcell_x.maxEcell_y.maxEcell_z.neflowisol20.neflowisol30.neflowisol40.neflowisolCorrBitset.neflowisolcoreConeEnergyCorrection.core57cellsEnergyCorrection"]

ExtraContentVtx=["PrimaryVertices.x.y.z.covariance.trackWeights.vertexType.sumPt2.EGAM10_sumPt.EGAM10_sumPt2.EGAM10_pt.EGAM10_eta.EGAM10_phi"]

ExtraContentTrk=["InDetTrackParticles.definingParametersCovMatrixVec.theta.z0.d0.qOverP"]

ExtraContentJets=[]
#ExtraContentJets=["AntiKt4EMPFlowJets.EMFrac.Width.TrackWidthPt500.SumPtTrkPt1000.EnergyPerSampling.GhostTrack.DFCommonJets_jetClean_LooseBad.DFCommonJets_jetClean_TightBad.Timing.FracSamplingMax.FracSamplingMaxIndex.HECFrac.LArQuality.HECQuality.NegativeE.AverageLArQF"]

ExtraContentEventShape=[
    "TopoClusterIsoCentralEventShape.Density",
    "TopoClusterIsoForwardEventShape.Density",
    "NeutralParticleFlowIsoCentralEventShape.Density",
    "NeutralParticleFlowIsoForwardEventShape.Density"
]

ExtraContentAll=ExtraContentElectrons+ExtraContentPhotons+ExtraContentVtx+ExtraContentTrk+ExtraContentJets+ExtraContentEventShape

# These are only added if running on MC
ExtraElectronsTruth=[".".join(["Electrons", 
    "truthOrigin", 
    "truthType", 
    "truthParticleLink", 
    "truthPdgId", 
    "bkgTruthType", 
    "bkgTruthOrigin", 
    "bkgTruthParticleLink", 
    "bkgMotherPdgId", 
    "firstEgMotherTruthType", 
    "firstEgMotherTruthOrigin", 
    "firstEgMotherTruthParticleLink", 
    "firstEgMotherPdgId"
    ]) ]

ExtraPhotonsTruth=["Photons.truthOrigin.truthParticleLink.truthType"]

ExtraVariablesTruthEventShape=[
    "TruthIsoCentralEventShape.DensitySigma.Density.DensityArea",
    "TruthIsoForwardEventShape.DensitySigma.Density.DensityArea"
    ]

ExtraContentAllTruth=ExtraElectronsTruth+ExtraPhotonsTruth+ExtraVariablesTruthEventShape


# Extra containers
ExtraContainers=["CaloCalTopoClusters"]

ExtraContainersTruth=["TruthEvents",
                      "TruthParticles",
                      "TruthVertices",
                      "AntiKt4TruthJets",
                      "AntiKt4TruthWZJets",
                      "TruthMuons",
                      "TruthElectrons",
                      "TruthPhotons",
                      "TruthNeutrinos",
                      "TruthTaus"]

ExtraContainersTruthPhotons=["egammaTruthParticles"]


ExtraDictionary={"TruthIsoCentralEventShape":"xAOD::EventShape",
                 "TruthIsoCentralEventShapeAux":"xAOD::EventShapeAuxInfo",
                 "TruthIsoForwardEventShape":"xAOD::EventShape",
                 "TruthIsoForwardEventShapeAux":"xAOD::EventShapeAuxInfo"}


