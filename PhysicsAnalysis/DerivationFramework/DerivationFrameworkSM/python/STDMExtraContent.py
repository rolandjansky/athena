# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#Content included in addition to the Smart Slimming Content
ExtraContentElectrons=["Electrons.topoetcone20.topoetcone30.topoetcone40.ptcone20.ptcone30.ptcone40.Loose.Medium.Tight.DFCommonElectronsLHLoose.DFCommonElectronsLHMedium.DFCommonElectronsLHTight.DFCommonElectronsML.author.OQ.LHLoose.LHMedium.LHTight.LHValue","egammaClusters.PHI2CALOFRAME.ETA2CALOFRAME.phi_sampl"]

ExtraContentPhotons=["Photons.topoetcone20.topoetcone30.topoetcone40.ptcone20.ptcone30.ptcone40.Loose.Medium.Tight.author.OQ.f3.f3core"]+["egammaTopoSeededClusters.calE.calEta.calPhi.e_sampl.eta_sampl.ETACALOFRAME.PHICALOFRAME.eta"]

ExtraContentMuons=["Muons.DFCommonGoodMuon.charge.ptcone20.ptcone30.ptcone40.topoetcone20.topoetcone30.topoetcone40"]

ExtraContentTaus=["TauJets.BDTEleScore.BDTJetScore.eta.jetLink.m.phi.pt.charge.isTauFlags.TruthCharge.TruthProng.IsTruthMatched.TruthPtVis.truthOrigin.truthType.truthParticleLink.truthJetLink"]

ExtraContentBtag=["BTagging_AntiKt4EMTopo.MV2cl100_discriminant"]

ExtraContentVtx=["PrimaryVertices.x.y.z.covariance.trackWeights.vertexType.sumPt2"]

ExtraContentTrk=["InDetTrackParticles.definingParametersCovMatrixVec.theta.z0.d0.qOverP"]

ExtraContentJets=["AntiKt4EMTopoJets.JvtJvfcorr.JvtRpt.Jvt"]

ExtraContentAll=ExtraContentElectrons+ExtraContentPhotons+ExtraContentMuons+ExtraContentTaus+ExtraContentBtag+ExtraContentVtx+ExtraContentTrk+ExtraContentJets

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

ExtraMuonsTruth=["MuonTruthParticles.truthOrigin.truthType.barcode"]

ExtraTausTruth=[]

ExtraContentAllTruth=ExtraElectronsTruth+ExtraPhotonsTruth+ExtraMuonsTruth+ExtraTausTruth

# Extra containers
ExtraContainersElectron=["ForwardElectrons","ForwardElectronClusters"]

ExtraContainersJets=[]#Place Holder

ExtraContainersOthers=["MET_Track"]

ExtraContainersTruth=["TruthEvents", "TruthParticles", "TruthVertices", "AntiKt4TruthJets","AntiKt4TruthWZJets",
                      "TruthMuons",
                      "TruthElectrons",
                      "TruthPhotons",
                      "TruthNeutrinos",
                      "TruthTaus",
                      "STDMTruthMuons",
                      "STDMTruthElectrons",
                      "STDMTruthPhotons",
                      "STDMTruthNeutrinos",
                      "STDMTruthTaus"]

ExtraDictionary={
    "STDMTruthMuons":"xAOD::TruthParticleContainer" ,
    "STDMTruthMuonsAux":"xAOD::TruthParticleAuxContainer" ,
    "STDMTruthElectrons":"xAOD::TruthParticleContainer" ,
    "STDMTruthElectronsAux":"xAOD::TruthParticleAuxContainer" ,
    "STDMTruthPhotons":"xAOD::TruthParticleContainer" ,
    "STDMTruthPhotonsAux":"xAOD::TruthParticleAuxContainer" ,
    "STDMTruthNeutrinos":"xAOD::TruthParticleContainer" ,
    "STDMTruthNeutrinosAux":"xAOD::TruthParticleAuxContainer", 
    "STDMTruthTaus":"xAOD::TruthParticleContainer" ,
    "STDMTruthTausAux":"xAOD::TruthParticleAuxContainer" }

ExtraDictionaryTruthEventShape = {"TruthIsoCentralEventShape":"xAOD::EventShape",
                                  "TruthIsoForwardEventShape":"xAOD::EventShape"}
ExtraContainersTruthPhotons=["egammaTruthParticles"]

# Do not exist (at least for now)
#ExtraContainers6Jets=["AntiKt6EMTopoJets","AntiKt6LCTopoJets", "BTagging_AntiKt6LCTopo", "BTagging_AntiKt6EMTopo"]

ExtraContainersAll=ExtraContainersElectron+ExtraContainersJets+ExtraContainersOthers


#Extra variables
ExtraVariablesEventShape=[
    "TopoClusterIsoCentralEventShape.DensitySigma.Density.DensityArea",
    "TopoClusterIsoForwardEventShape.DensitySigma.Density.DensityArea",
    "kt4EMTopoEventShape.DensitySigma.Density.DensityArea"
    ]

ExtraVariablesTruthEventShape=[
    "TruthIsoCentralEventShape.DensitySigma.Density.DensityArea",
    "TruthIsoForwardEventShape.DensitySigma.Density.DensityArea"
    ]

