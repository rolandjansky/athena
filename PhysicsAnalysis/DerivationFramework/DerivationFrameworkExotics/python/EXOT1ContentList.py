# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

EXOT1SmartContent = [
	#"Muons",
	#"PrimaryVertices",
	#"AntiKt4EMTopoJets",
	#"AntiKt4LCTopoJets",
	#"BTagging_AntiKt4EMTopo"
	#"xAOD::EventShape_v1#*",
	#"xAOD::EventShapeAuxInfo_v1#*"
]

EXOT1AllVariablesContent = [
	#"PhotonCollection",
	#"ElectronCollection",
	#"InDetTrackParticles",
	#"GSFTrackParticles",
	#"egClusterCollection",
	"CaloCalTopoClusters",
        #"GSFConversionVertices",
        "PrimaryVertices",
	"AntiKt4TruthJets",
	"AntiKt4EMTopoJets",
	"AntiKt4LCTopoJets",
	"BTagging_AntiKt4EMTopo",
	"TruthEvents",
	"MuonSegments",
        "MET_Reference_AntiKt4LCTopo"
	#"CaloCalTopoClusters"
]

EXOT1Content = [
]

#	"xAOD::EventInfo_v1#*",
#	"xAOD::EventAuxInfo_v1#*",
#	"xAOD::ElectronContainer_v1#*",
#	"xAOD::ElectronAuxContainer#*",
#	"xAOD::MuonContainer_v1#*",
#	"xAOD::MuonAuxContainer_v1#*", 
#	"xAOD::PhotonContainer_v1#*",
#	"xAOD::PhotonAuxContainer#*",

	# tracks
	#"xAOD::TrackParticleContainer_v1#*",
	#"xAOD::TrackParticleAuxContainer_v1#*.",
	# InDect
#	"xAOD::TrackParticleContainer_v1#InDetTrackParticles",
#	"xAOD::TrackParticleAuxContainer_v1#InDetTrackParticlesAux.",
	# GSF
#	"xAOD::TrackParticleContainer_v1#GSFTrackParticles",
#	"xAOD::TrackParticleAuxContainer_v1#GSFTrackParticlesAux.",
	# fwd
	#"xAOD::TrackParticleContainer_v1#InDetTrackParticlesForward",
	#"xAOD::TrackParticleAuxContainer_v1#InDetTrackParticlesForwardAux.",

	# Jets
	#"xAOD::JetContainer_v1#AntiKt10LCTopoJets", 
	#"xAOD::JetContainer_v1#AntiKt10TruthJets", 
	#"xAOD::JetContainer_v1#AntiKt6EMTopoJets", 
	#"xAOD::JetContainer_v1#AntiKt6LCTopoJets", 
#	"xAOD::JetContainer_v1#AntiKt4EMTopoJets", 
#	"xAOD::JetContainer_v1#AntiKt4LCTopoJets", 
	#"xAOD::JetContainer_v1#AntiKt4TruthJets", 
	#"xAOD::JetAuxContainer_v1#AntiKt10LCTopoJetsAux.", 
	#"xAOD::JetAuxContainer_v1#AntiKt10TruthJetsAux.", 
	#"xAOD::JetAuxContainer_v1#AntiKt6EMTopoJetsAux.", 
	#"xAOD::JetAuxContainer_v1#AntiKt6LCTopoJetsAux.", 
#	"xAOD::JetAuxContainer_v1#AntiKt4EMTopoJetsAux.", 
#	"xAOD::JetAuxContainer_v1#AntiKt4LCTopoJetsAux.", 
	#"xAOD::JetAuxContainer_v1#AntiKt4TruthJetsAux.",

	# calo clusters, topo/non
	#"xAOD::CaloClusterContainer_v1#*",
	#"xAOD::CaloClusterAuxContainer_v1#*.",
	# calo clusters, central
#	"xAOD::CaloClusterContainer_v1#egClusterCollection",
#	"xAOD::CaloClusterAuxContainer_v1#egClusterCollectionAux.",
	# calo clusters, fwd
	#"xAOD::CaloClusterContainer_v1#LArClusterEMFrwd",
	#"xAOD::CaloClusterAuxContainer_v1#LArClusterEMFrwdAux.",
	# topo clusters
#	"xAOD::CaloClusterContainer_v1#CaloCalTopoCluster",
#	"xAOD::CaloClusterAuxContainer_v1#CaloCalTopoClusterAux.",

#	"xAOD::EventShape_v1#*",
#	"xAOD::EventShapeAuxInfo_v1#*",
#	"xAOD::VertexContainer_v1#*",
#	"xAOD::VertexAuxContainer_v1#*"

#xAOD::TruthVertexContainer_v1#*",
#xAOD::TruthVertexAuxContainer_v1#*",
#xAOD::TruthParticleContainer_v1#*",
#xAOD::TruthParticleAuxContainer_v1#*",
#xAOD::TruthEventContainer_v1#*",
#xAOD::TruthEventAuxContainer_v1#*" ]

#"xAOD::MuonContainer_v1#*",
#"xAOD::MuonAuxContainer_v1#*", 
# Jets
#"xAOD::JetContainer_v1#AntiKt10LCTopoJets", 
#"xAOD::JetContainer_v1#AntiKt10TruthJets", 
#"xAOD::JetContainer_v1#AntiKt6EMTopoJets", 
#"xAOD::JetContainer_v1#AntiKt6LCTopoJets", 
#"xAOD::JetContainer_v1#AntiKt4EMTopoJets", 
#"xAOD::JetContainer_v1#AntiKt4LCTopoJets", 
#"xAOD::JetContainer_v1#AntiKt4TruthJets", 
#"xAOD::JetAuxContainer_v1#AntiKt10LCTopoJetsAux.", 
#"xAOD::JetAuxContainer_v1#AntiKt10TruthJetsAux.", 
#"xAOD::JetAuxContainer_v1#AntiKt6EMTopoJetsAux.", 
#"xAOD::JetAuxContainer_v1#AntiKt6LCTopoJetsAux.", 
#"xAOD::JetAuxContainer_v1#AntiKt4EMTopoJetsAux.", 
#"xAOD::JetAuxContainer_v1#AntiKt4LCTopoJetsAux." #, 
#"xAOD::JetAuxContainer_v1#AntiKt4TruthJetsAux.",
#xAOD::TruthVertexContainer_v1#*",
#xAOD::TruthVertexAuxContainer_v1#*",
#xAOD::TruthParticleContainer_v1#*",
#xAOD::TruthParticleAuxContainer_v1#*",
#xAOD::TruthEventContainer_v1#*",
#xAOD::TruthEventAuxContainer_v1#*"
#"xAOD::EventInfo_v1#*",
#"xAOD::EventAuxInfo_v1#*",
# tracks
#"xAOD::TrackParticleContainer_v1#*",
#"xAOD::TrackParticleAuxContainer_v1#*.",
# InDect
#"xAOD::TrackParticleContainer_v1#InDetTrackParticles",
#"xAOD::TrackParticleAuxContainer_v1#InDetTrackParticlesAux.",
# GSF
#"xAOD::TrackParticleContainer_v1#GSFTrackParticles",
#"xAOD::TrackParticleAuxContainer_v1#GSFTrackParticlesAux.",
# fwd
#"xAOD::TrackParticleContainer_v1#InDetTrackParticlesForward",
#"xAOD::TrackParticleAuxContainer_v1#InDetTrackParticlesForwardAux.",
# calo clusters, topo/non
#"xAOD::CaloClusterContainer_v1#*",
#"xAOD::CaloClusterAuxContainer_v1#*.",
# calo clusters, central
#"xAOD::CaloClusterContainer_v1#egClusterCollection",
#"xAOD::CaloClusterAuxContainer_v1#egClusterCollectionAux.",
# calo clusters, fwd
#"xAOD::CaloClusterContainer_v1#LArClusterEMFrwd",
#"xAOD::CaloClusterAuxContainer_v1#LArClusterEMFrwdAux.",
# topo clusters
#"xAOD::CaloClusterContainer_v1#CaloCalTopoCluster",
#"xAOD::CaloClusterAuxContainer_v1#CaloCalTopoClusterAux.",
#"xAOD::VertexContainer_v1#*",
#"xAOD::VertexAuxContainer_v1#*"

