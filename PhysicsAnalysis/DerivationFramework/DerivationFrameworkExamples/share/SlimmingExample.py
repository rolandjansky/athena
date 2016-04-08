#====================================================================
# SlimmingExample.py
# This an example job options script showing how to set up slimming 
# in the derivation framework. For trigger content see the 
# TriggerContentExample.py (TEST10) 
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
# This demonstrates the use of smart slimming
# For trigger information see TriggerContentExample.py 
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
TEST4SlimmingHelper = SlimmingHelper("TEST4SlimmingHelper")
TEST4SlimmingHelper.SmartCollections = ["Electrons",
					"Photons",
					"Muons",
					"TauJets",
					"MET_Reference_AntiKt4EMTopo",
                                        "AntiKt4EMTopoJets",
                                        "BTagging_AntiKt4EMTopo",
                                        "BTagging_AntiKt2Track",
                                        "BTagging_AntiKt3Track",
                                        "BTagging_AntiKt4Track", 
                                        "InDetTrackParticles",
					"PrimaryVertices" ]
#TEST4SlimmingHelper.ExtraVariables = ["PhotonCollection.weta2.f1.phi.weta1.emaxs1"]
#TEST4SlimmingHelper.AllVariables = ["Muons"]
#TEST4SlimmingHelper.StaticContent = ["CaloCellContainer#AODCellContainer"]
TEST4SlimmingHelper.AppendContentToStream(TEST4Stream)
