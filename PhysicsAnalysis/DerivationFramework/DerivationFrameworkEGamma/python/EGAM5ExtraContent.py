# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#Content included in addition to the Smart Slimming Content

ExtraContentMuons=[
    "Muons.DFCommonMuonsPreselection",
    "Muons.ptcone20",
    "Muons.ptcone30",
    "Muons.ptcone40",
    "Muons.etcone20",
    "Muons.etcone30",
    "Muons.etcone40"
]

ExtraMuonsTruth=[
    "MuonTruthParticles.e",
    "MuonTruthParticles.px",
    "MuonTruthParticles.py",
    "MuonTruthParticles.pz",
    "MuonTruthParticles.status",
    "MuonTruthParticles.pdgId",
    "MuonTruthParticles.truthOrigin",
    "MuonTruthParticles.truthType"
]

ExtraContentPhotons=[
]

ExtraContentElectrons=[
]

ExtraContentPrimaryVertices=["PrimaryVertices.x.y.sumPt2"]

ExtraPhotonsTruth=[
    "Photons.truthOrigin",
    "Photons.truthType",
    "Photons.truthParticleLink"
]

ExtraContentGSFConversionVertices=[
    "GSFConversionVertices.x",
    "GSFConversionVertices.y",
    "GSFConversionVertices.z",
    "GSFConversionVertices.px",
    "GSFConversionVertices.py",
    "GSFConversionVertices.pz",
    "GSFConversionVertices.pt1",
    "GSFConversionVertices.pt2",
    "GSFConversionVertices.etaAtCalo",
    "GSFConversionVertices.phiAtCalo",
    "GSFConversionVertices.trackParticleLinks"
]

# for use of low mu hadronic recoil package within TagAndProbe framework
ExtraContentPFO = [
  "JetETMissChargedParticleFlowObjects.pt.eta.phi.m.DFCommonPFlow_PVMatched.DFCommonPFlow_CaloCorrectedPt",
  "JetETMissNeutralParticleFlowObjects.pt.eta.phi.m.centerMag.ptEM.mEM"]



from DerivationFrameworkCalo.DerivationFrameworkCaloFactories import GainDecorator, getGainDecorations
GainDecoratorTool = GainDecorator()
ExtraContentPhotons.extend( getGainDecorations(GainDecoratorTool) )
ExtraContentElectrons.extend( getGainDecorations(GainDecoratorTool) )

ExtraContentAll=ExtraContentElectrons+ExtraContentMuons+ExtraContentPhotons+ExtraContentGSFConversionVertices+ExtraContentPrimaryVertices+ExtraContentPFO
ExtraContentAllTruth=ExtraMuonsTruth+ExtraPhotonsTruth

ExtraContainersTruth=["TruthEvents", 
                      "TruthParticles",
                      "TruthVertices",
                      "AntiKt4TruthJets",
                      "egammaTruthParticles"
                      #,"BTagging_AntiKt4TruthWZ"
                      #,"AntiKt4TruthWZJets"
                      ]

ExtraContainersElectrons=["Electrons",
                          "GSFTrackParticles",
                          "egammaClusters"
                          ]

# for trigger studies
ExtraContainersTrigger=[
    "HLT_xAOD__ElectronContainer_egamma_Electrons",
    "HLT_xAOD__ElectronContainer_egamma_ElectronsAux.",
    "HLT_xAOD__PhotonContainer_egamma_Photons",
    "HLT_xAOD__PhotonContainer_egamma_PhotonsAux.",
    "HLT_xAOD__TrigElectronContainer_L2ElectronFex",
    "HLT_xAOD__TrigElectronContainer_L2ElectronFexAux.",
    "HLT_xAOD__TrigPhotonContainer_L2PhotonFex",
    "HLT_xAOD__TrigPhotonContainer_L2PhotonFexAux.",
    "HLT_xAOD__CaloClusterContainer_TrigEFCaloCalibFex",
    "HLT_xAOD__CaloClusterContainer_TrigEFCaloCalibFexAux.",
    "HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Electron_EFID",
    "HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Electron_EFIDAux.",
    "LVL1EmTauRoIs",
    "LVL1EmTauRoIsAux.",
    "HLT_TrigPassBitsCollection_passbits",
    "HLT_TrigPassBitsCollection_passbitsAux.",
    "HLT_TrigPassFlagsCollection_passflags",
    "HLT_TrigPassFlagsCollection_passflagsAux.",
    "HLT_TrigRoiDescriptorCollection_initialRoI",
    "HLT_TrigRoiDescriptorCollection_initialRoIAux."
    ]

ExtraContainersTriggerDataOnly=[
    "HLT_xAOD__TrigEMClusterContainer_TrigT2CaloEgamma",
    "HLT_xAOD__TrigEMClusterContainer_TrigT2CaloEgammaAux."
    ]

#should probably slim electron/fwdelectrons/cluster collections and keep only relevant subset of variables..
# no ForwardElectrons, InDetTrackParticlesForward, ForwardElectronClusters, CaloCalTopoCluster
