#====================================================================
# SkimmingExample.py
# This an example job options script showing how to set up a 
# derivation of the data using the derivation framework.  
# It requires the reductionConf flag TEST4 in Reco_tf.py   
#====================================================================

# Set up common services and job object. 
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

# The name of the kernel (TEST4Kernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("TEST4Kernel")

#====================================================================
# SET UP STREAM   
#====================================================================

# SKIMMING
# The base name (DAOD_TEST4 here) must match the string in 
# DerivationFrameworkProdFlags (in DerivationFrameworkCore) 
streamName = derivationFlags.WriteDAOD_TEST4Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_TEST4Stream )
TEST4Stream = MSMgr.NewPoolRootStream( streamName, fileName )
TEST4Stream.AcceptAlgs(["TEST4Kernel"])

#====================================================================
# CONTENT LIST  
#====================================================================
# This might be the kind of set-up one would have for a muon/track based analysis
TEST4Stream.AddItem("xAOD::EventInfo_v1#*")
TEST4Stream.AddItem("xAOD::EventAuxInfo_v1#*")
TEST4Stream.AddItem("xAOD::ElectronContainer_v1#*")
TEST4Stream.AddItem("xAOD::ElectronAuxContainer_v1#ElectronCollectionAux.-")
TEST4Stream.AddItem("xAOD::MuonContainer_v1#*")
TEST4Stream.AddItem("xAOD::MuonAuxContainer_v1#*")
TEST4Stream.AddItem("xAOD::PhotonContainer_v1#*")
TEST4Stream.AddItem("xAOD::PhotonAuxContainer_v1#PhotonCollectionAux.-")
TEST4Stream.AddItem("xAOD::JetContainer_v1#AntiKt4LCTopoJets")
TEST4Stream.AddItem("xAOD::JetAuxContainer_v1#AntiKt4LCTopoJetsAux.-")
TEST4Stream.AddItem("xAOD::TrackParticleContainer_v1#*")
TEST4Stream.AddItem("xAOD::TrackParticleAuxContainer_v1#*")
TEST4Stream.AddItem("xAOD::VertexContainer_v1#PrimaryVerticesAux")
TEST4Stream.AddItem("xAOD::VertexAuxContainer_v1#PrimaryVerticesAux")
