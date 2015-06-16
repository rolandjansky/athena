#====================================================================
# MCTEST2.py
# These job options are to enable groups to quickly set up tests of
# truth thinning in the derivation framework. 
# It requires the reductionConf flag MCTEST2 in Reco_tf.py   
#====================================================================

# Set up common services and job object. 
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *

#====================================================================
# THINNING TOOLS 
#====================================================================

# Set up your thinning tools (you can have as many as you need). 
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
MCTEST2Tool = DerivationFramework__GenericTruthThinning(name                    = "MCTEST2Tool",
                                                 	ThinningService         = "MCTEST2ThinningSvc",
						 	ParticleSelectionString = "abs(TruthParticle.pdgId) == 511",
							PreserveDescendants	= True)
							#PreserveAncestors	= True)
ToolSvc += MCTEST2Tool

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("MCTEST2Kernel",
                                                                        ThinningTools = [MCTEST2Tool])

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_MCTEST2Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_MCTEST2Stream )
MCTEST2Stream = MSMgr.NewPoolRootStream( streamName, fileName )
MCTEST2Stream.AcceptAlgs(["MCTEST2Kernel"])

# SPECIAL LINES FOR THINNING
# Thinning service name must match the one passed to the thinning tools 
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="MCTEST2ThinningSvc", outStreams=[evtStream] )

#====================================================================
# SLIMMING  
#====================================================================
MCTEST2Stream.AddItem("xAOD::EventInfo_v1#*")
MCTEST2Stream.AddItem("xAOD::EventAuxInfo_v1#*")
MCTEST2Stream.AddItem("xAOD::ElectronContainer_v1#ElectronCollection")
MCTEST2Stream.AddItem("xAOD::ElectronAuxContainer#ElectronCollectionAux.")
MCTEST2Stream.AddItem("xAOD::MuonContainer_v1#*")
MCTEST2Stream.AddItem("xAOD::MuonAuxContainer_v1#*")
MCTEST2Stream.AddItem("xAOD::TauJetContainer_v1#*")
MCTEST2Stream.AddItem("xAOD::TauJetAuxContainer_v1#*")
MCTEST2Stream.AddItem("xAOD::JetContainer_v1#AntiKt4LCTopoJets")
MCTEST2Stream.AddItem("xAOD::JetAuxContainer_v1#AntiKt4LCTopoJetsAux.")
MCTEST2Stream.AddItem("xAOD::JetContainer_v1#AntiKt4TruthJets")
MCTEST2Stream.AddItem("xAOD::JetAuxContainer_v1#AntiKt4TruthJetsAux.")
MCTEST2Stream.AddItem("xAOD::BTaggingContainer_v1#BTagging_AntiKt4LCTopo")
MCTEST2Stream.AddItem("xAOD::BTaggingAuxContainer_v1#BTagging_AntiKt4LCTopoAux.")
MCTEST2Stream.AddItem("xAOD::BTaggingContainer_v1#BTagging_AntiKt4Truth")
MCTEST2Stream.AddItem("xAOD::BTaggingAuxContainer_v1#BTagging_AntiKt4TruthAux.")
MCTEST2Stream.AddItem("xAOD::TrackParticleContainer_v1#*")
MCTEST2Stream.AddItem("xAOD::TrackParticleAuxContainer_v1#*")
MCTEST2Stream.AddItem("xAOD::CaloClusterContainer_v1#*")
MCTEST2Stream.AddItem("xAOD::CaloClusterAuxContainer_v1#*")
MCTEST2Stream.AddItem("xAOD::MissingETComponentMap_v1#*")
MCTEST2Stream.AddItem("xAOD::MissingETAuxComponentMap_v1#*")
MCTEST2Stream.AddItem("xAOD::MissingETContainer_v1#*")
MCTEST2Stream.AddItem("xAOD::MissingETAuxContainer_v1#*")
MCTEST2Stream.AddItem("xAOD::VertexContainer_v1#*")
MCTEST2Stream.AddItem("xAOD::VertexAuxContainer_v1#*")
MCTEST2Stream.AddItem("xAOD::TruthVertexContainer_v1#*")
MCTEST2Stream.AddItem("xAOD::TruthVertexAuxContainer_v1#*")
MCTEST2Stream.AddItem("xAOD::TruthParticleContainer_v1#*")
MCTEST2Stream.AddItem("xAOD::TruthParticleAuxContainer_v1#*")
MCTEST2Stream.AddItem("xAOD::TruthEventContainer_v1#*")
MCTEST2Stream.AddItem("xAOD::TruthEventAuxContainer_v1#*")
