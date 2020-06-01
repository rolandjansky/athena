# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

#Content included in addition to the Smart Slimming Content

ExtraContentElectrons=[
    "Electrons.Loose",
    "Electrons.Medium",
    "Electrons.Tight",
    ]

ExtraElectronsTruth=[
    "Electrons.truthOrigin",
    "Electrons.truthType",
    "Electrons.truthParticleLink"
    ]

ExtraContentMuons=[
#    "Muons.DFCommonGoodMuon",
#    "Muons.DFCommonMuonsLoose",
#    "Muons.DFCommonMuonsMedium",
#    "Muons.DFCommonMuonsTight",
    "Muons.DFCommonMuonsPreselection",
    "Muons.ptcone20",
    "Muons.ptcone30",
    "Muons.ptcone40",
    "Muons.etcone20",
    "Muons.etcone30",
    "Muons.etcone40"
    ]

ExtraMuonsTruth=[
#    "MuonTruthParticles.e",
#    "MuonTruthParticles.px",
#    "MuonTruthParticles.py",
#    "MuonTruthParticles.pz",
#    "MuonTruthParticles.status",
#    "MuonTruthParticles.pdgId",
#    "MuonTruthParticles.truthOrigin",
#    "MuonTruthParticles.truthType"
    ]

ExtraContentPhotons=[
]

ExtraContentPrimaryVertices=["PrimaryVertices.x.y.sumPt2"]

ExtraPhotonsTruth=[
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

ExtraContentHLTPhotons=[
        "HLT_xAOD__PhotonContainer_egamma_Photons.e.pt.m.author.Rhad.Rhad1.e277.Reta.Rphi.weta2.f1.fracs1.wtots1.weta1.DeltaE.Eratio.caloClusterLinks",
	"HLT_xAOD__CaloClusterContainer_TrigEFCaloCalibFex.calE.calEta.calPhi.calM.e_sampl.eta_sampl.etaCalo.phiCalo.ETACALOFRAME.PHICALOFRAME"
]

from DerivationFrameworkCalo.DerivationFrameworkCaloFactories import GainDecorator, getGainDecorations
GainDecoratorTool = GainDecorator()
ExtraContentPhotons.extend( getGainDecorations(GainDecoratorTool) )
ExtraContentElectrons.extend( getGainDecorations(GainDecoratorTool) )

ExtraContentAll=ExtraContentElectrons+ExtraContentMuons+ExtraContentPhotons+ExtraContentGSFConversionVertices+ExtraContentHLTPhotons+ExtraContentPrimaryVertices
ExtraContentAllTruth=ExtraElectronsTruth+ExtraMuonsTruth+ExtraPhotonsTruth


ExtraContainersTruth=["TruthEvents", 
                      "TruthParticles",
                      "TruthVertices",
                      "AntiKt4TruthJets",
		      "egammaTruthParticles",
		      "MuonTruthParticles"
                      #,"BTagging_AntiKt4TruthWZ"
                      #,"AntiKt4TruthWZJets"
                      ]

ExtraContainersPhotons=["Photons",
                        "GSFTrackParticles",
                        "egammaClusters",
                        "NewSwPhotons", # only if DoCellReweighting is ON                        
                        "MaxVarSwPhotons", # if variations are ON
                        "MinVarSwPhotons"  # if variations are ON
                        ]

# for trigger studies and for trigger matching
ExtraContainersTrigger=[
	"HLT_xAOD__MuonContainer_MuonEFInfo",
	"HLT_xAOD__MuonContainer_MuonEFInfoAux.",
        "HLT_xAOD__MuonContainer_MuonEFInfo_FullScan",
        "HLT_xAOD__MuonContainer_MuonEFInfo_FullScanAux.",
	"HLT_xAOD__PhotonContainer_egamma_Photons",
	"HLT_xAOD__PhotonContainer_egamma_PhotonsAux.",
        "HLT_xAOD__PhotonContainer_egamma_Iso_Photons",
        "HLT_xAOD__PhotonContainer_egamma_Iso_PhotonsAux.",
	"HLT_xAOD__TrigPhotonContainer_L2PhotonFex",
	"HLT_xAOD__TrigPhotonContainer_L2PhotonFexAux.",
	"HLT_xAOD__CaloClusterContainer_TrigEFCaloCalibFex",
	"HLT_xAOD__CaloClusterContainer_TrigEFCaloCalibFexAux.",
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
	"HLT_xAOD__TrigEMClusterContainer_TrigT2CaloEgammaAux.",
	"HLT_xAOD__CaloClusterContainer_TrigCaloClusterMaker",
	"HLT_xAOD__CaloClusterContainer_TrigCaloClusterMakerAux."
	]
