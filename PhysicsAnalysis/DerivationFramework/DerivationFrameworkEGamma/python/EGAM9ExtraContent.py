# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


#Content included in addition to the Smart Slimming Content

ExtraContentElectrons=[]

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
    "Photons.DFCommonLoosePrime5",
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

#ExtraContentHLTElectrons=[
#        "HLT_xAOD__ElectronContainer_egamma_Electrons.e.pt.Rhad.Rhad1.e277.Reta.Rphi.weta2.f1.fracs1.wtots1.weta1.DeltaE.Eratio.caloClusterLinks"
#]

from DerivationFrameworkCalo.DerivationFrameworkCaloFactories import GainDecorator, getGainDecorations
GainDecoratorTool = GainDecorator()
ExtraContentPhotons.extend( getGainDecorations(GainDecoratorTool) )
ExtraContentElectrons.extend( getGainDecorations(GainDecoratorTool) )

ExtraContentAll=ExtraContentElectrons+ExtraContentMuons+ExtraContentPhotons+ExtraContentGSFConversionVertices+ExtraContentPrimaryVertices
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
                          "egammaClusters"]

ExtraContainersPhotons=["Photons",
                        "GSFTrackParticles",
                        "egammaClusters"
                        ]


# for trigger studies
ExtraContainersTrigger=[
	# to access the HLT egamma xAOD collections
	"HLT_xAOD__ElectronContainer_egamma_Electrons",
	"HLT_xAOD__PhotonContainer_egamma_Photons",
        "HLT_xAOD__PhotonContainer_egamma_Iso_Photons",
	"HLT_xAOD__TrigElectronContainer_L2ElectronFex",
	"HLT_xAOD__TrigPhotonContainer_L2PhotonFex",
	# to access information about EF clusters and tracks
	"HLT_xAOD__CaloClusterContainer_TrigEFCaloCalibFex",
	"HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Electron_EFID",
	# for L1 studies
	"LVL1EmTauRoIs",
	"HLT_TrigPassBitsCollection_passbits",
	"HLT_TrigPassFlagsCollection_passflags",
	"HLT_TrigRoiDescriptorCollection_initialRoI"
	]

ExtraContainersTriggerDataOnly=[
	"HLT_xAOD__TrigEMClusterContainer_TrigT2CaloEgamma",
	"HLT_xAOD__CaloClusterContainer_TrigCaloClusterMaker",
	"HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Electron_FTF",
	"HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Electron_L2ID",
	"HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Electron_IDTrig"
	]

#should slim electron/fwdelectrons/cluster collections and keep only relevant subset of variables..
