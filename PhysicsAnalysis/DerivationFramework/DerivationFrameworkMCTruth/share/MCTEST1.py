#====================================================================
# MCTEST1.py
# These job options are to enable groups to quickly set up tests of
# truth thinning in the derivation framework. 
# It requires the reductionConf flag MCTEST1 in Reco_tf.py   
#====================================================================

# Set up common services and job object. 
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *

#====================================================================
# THINNING TOOLS 
#====================================================================

# Set up your thinning tools (you can have as many as you need). 
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
MCTEST1Tool = DerivationFramework__MenuTruthThinning(name                    	= "MCTEST1Tool",
                                                     ThinningService         	= "MCTEST1ThinningSvc",
						     WritePartons	 	= False,
						     WriteHadrons		= False,
						     WriteBHadrons		= True,
						     WriteGeant		 	= False,
						     GeantPhotonPtThresh	= -1.0,
						     WriteTauHad		= True,
						     PartonPtThresh		= -1.0,
						     WriteBSM		 	= False,
						     WriteBosons		= False,
						     WriteBSMProducts	 	= False,
						     WriteTopAndDecays	 	= True,
						     WriteEverything	 	= False,
						     WriteAllLeptons	 	= False,
						     WriteLeptonsNotFromHadrons	 	= True,
						     WriteStatus3		= False,
						     WriteFirstN		= -1,
                 PreserveDescendants = True)
ToolSvc += MCTEST1Tool

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("MCTEST1Kernel",
                                                                        ThinningTools = [MCTEST1Tool])

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_MCTEST1Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_MCTEST1Stream )
MCTEST1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
MCTEST1Stream.AcceptAlgs(["MCTEST1Kernel"])

# SPECIAL LINES FOR THINNING
# Thinning service name must match the one passed to the thinning tools 
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="MCTEST1ThinningSvc", outStreams=[evtStream] )

#====================================================================
# SLIMMING  
#====================================================================
MCTEST1Stream.AddItem("xAOD::EventInfo_v1#*")
MCTEST1Stream.AddItem("xAOD::EventAuxInfo_v1#*")
MCTEST1Stream.AddItem("xAOD::ElectronContainer_v1#ElectronCollection")
MCTEST1Stream.AddItem("xAOD::ElectronAuxContainer#ElectronCollectionAux.")
MCTEST1Stream.AddItem("xAOD::MuonContainer_v1#*")
MCTEST1Stream.AddItem("xAOD::MuonAuxContainer_v1#*")
MCTEST1Stream.AddItem("xAOD::TauJetContainer_v1#*")
MCTEST1Stream.AddItem("xAOD::TauJetAuxContainer_v1#*")
MCTEST1Stream.AddItem("xAOD::JetContainer_v1#AntiKt4LCTopoJets")
MCTEST1Stream.AddItem("xAOD::JetAuxContainer_v1#AntiKt4LCTopoJetsAux.")
MCTEST1Stream.AddItem("xAOD::JetContainer_v1#AntiKt4TruthJets")
MCTEST1Stream.AddItem("xAOD::JetAuxContainer_v1#AntiKt4TruthJetsAux.")
MCTEST1Stream.AddItem("xAOD::BTaggingContainer_v1#BTagging_AntiKt4LCTopo")
MCTEST1Stream.AddItem("xAOD::BTaggingAuxContainer_v1#BTagging_AntiKt4LCTopoAux.")
MCTEST1Stream.AddItem("xAOD::BTaggingContainer_v1#BTagging_AntiKt4Truth")
MCTEST1Stream.AddItem("xAOD::BTaggingAuxContainer_v1#BTagging_AntiKt4TruthAux.")
MCTEST1Stream.AddItem("xAOD::TrackParticleContainer_v1#*")
MCTEST1Stream.AddItem("xAOD::TrackParticleAuxContainer_v1#*")
MCTEST1Stream.AddItem("xAOD::CaloClusterContainer_v1#*")
MCTEST1Stream.AddItem("xAOD::CaloClusterAuxContainer_v1#*")
MCTEST1Stream.AddItem("xAOD::MissingETComponentMap_v1#*")
MCTEST1Stream.AddItem("xAOD::MissingETAuxComponentMap_v1#*")
MCTEST1Stream.AddItem("xAOD::MissingETContainer_v1#*")
MCTEST1Stream.AddItem("xAOD::MissingETAuxContainer_v1#*")
MCTEST1Stream.AddItem("xAOD::VertexContainer_v1#*")
MCTEST1Stream.AddItem("xAOD::VertexAuxContainer_v1#*")
MCTEST1Stream.AddItem("xAOD::TruthVertexContainer_v1#*")
MCTEST1Stream.AddItem("xAOD::TruthVertexAuxContainer_v1#*")
MCTEST1Stream.AddItem("xAOD::TruthParticleContainer_v1#*")
MCTEST1Stream.AddItem("xAOD::TruthParticleAuxContainer_v1#*")
MCTEST1Stream.AddItem("xAOD::TruthEventContainer_v1#*")
MCTEST1Stream.AddItem("xAOD::TruthEventAuxContainer_v1#*")
