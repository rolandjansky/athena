# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#Content included in addition to the Smart Slimming Content
ExtraContentElectrons=["Electrons.topoetcone20.topoetcone30.topoetcone40.ptcone20.ptcone30.ptcone40.maxEcell_time.maxEcell_energy.maxEcell_gain.maxEcell_onlId.maxEcell_x.maxEcell_y.maxEcell_z"] + ["egammaClusters.PHI2CALOFRAME.ETA2CALOFRAME.phi_sampl"]

ExtraContentPhotons=["Photons.core57cellsEnergyCorrection.topoetcone20.topoetcone30.topoetcone40.ptcone20.ptcone30.ptcone40.f3.f3core.maxEcell_time.maxEcell_energy.maxEcell_gain.maxEcell_onlId.maxEcell_x.maxEcell_y.maxEcell_z"] + \
    ["Photons.ptcone20_Nonprompt_All_MaxWeightTTVA_pt500.ptvarcone30_Nonprompt_All_MaxWeightTTVA_pt1000.ptvarcone30_Nonprompt_All_MaxWeightTTVA_pt500"]

ExtraContentVtx=["PrimaryVertices.covariance.trackWeights.sumPt2.EGAM10_sumPt.EGAM10_sumPt2.EGAM10_pt.EGAM10_eta.EGAM10_phi"]

ExtraContentTrk=["InDetTrackParticles.TTVA_AMVFVertices.TTVA_AMVFWeights"]

ExtraContentJets=[]
#ExtraContentJets=["AntiKt4EMPFlowJets.EMFrac.Width.TrackWidthPt500.SumPtTrkPt1000.EnergyPerSampling.GhostTrack.DFCommonJets_jetClean_LooseBad.DFCommonJets_jetClean_TightBad.Timing.FracSamplingMax.FracSamplingMaxIndex.HECFrac.LArQuality.HECQuality.NegativeE.AverageLArQF"]

ExtraContentEventShape=[
    "TopoClusterIsoCentralEventShape.Density",
    "TopoClusterIsoForwardEventShape.Density",
]

ExtraContentAll=ExtraContentElectrons+ExtraContentPhotons+ExtraContentVtx+ExtraContentTrk+ExtraContentJets+ExtraContentEventShape

# These are only added if running on MC
ExtraElectronsTruth=[".".join(["Electrons", 
    "truthOrigin", 
    "truthType", 
    "truthParticleLink", 
    "truthPdgId", 
    "lastEgMotherTruthType",
    "lastEgMotherTruthOrigin",
    "lastEgMotherTruthParticleLink",
    "lastEgMotherPdgId",
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
                      "TruthMuons",
                      "TruthElectrons",
                      "TruthPhotons",
                      "TruthNeutrinos",
                      "TruthTaus",
                      "AntiKt4TruthJets",
                      "AntiKt4TruthDressedWZJets"]

ExtraContainersTruthPhotons=["egammaTruthParticles"]


ExtraDictionary={"TruthIsoCentralEventShape":"xAOD::EventShape",
                 "TruthIsoCentralEventShapeAux":"xAOD::EventShapeAuxInfo",
                 "TruthIsoForwardEventShape":"xAOD::EventShape",
                 "TruthIsoForwardEventShapeAux":"xAOD::EventShapeAuxInfo"}


