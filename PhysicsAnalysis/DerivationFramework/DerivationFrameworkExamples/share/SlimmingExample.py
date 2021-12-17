#====================================================================
# SlimmingExample.py
# This an example job options script showing how to set up slimming 
# in the derivation framework. 
# It requires the reductionConf flag TEST4 in Reco_tf.py   
#====================================================================

# Set up common services and job object. 
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkIsMonteCarlo, DerivationFrameworkJob, buildFileName 

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
TEST4SlimmingHelper.SmartCollections = ["EventInfo",
                                        "Electrons",
                                        "Photons",
                                        "Muons",
                                        "PrimaryVertices",
                                        "InDetTrackParticles",
                                        "AntiKt4EMTopoJets",
                                        "AntiKt4EMPFlowJets",
                                        "BTagging_AntiKt4EMPFlow",
                                        "BTagging_AntiKtVR30Rmax4Rmin02Track", 
                                        "MET_Baseline_AntiKt4EMTopo",
                                        "MET_Baseline_AntiKt4EMPFlow",
                                        "TauJets",
                                        "DiTauJets",
                                        "DiTauJetsLowPt",
                                        "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                        "AntiKtVR30Rmax4Rmin02PV0TrackJets",
                                      ]
#TEST4SlimmingHelper.ExtraVariables = ["PhotonCollection.weta2.f1.phi.weta1.emaxs1"]
#TEST4SlimmingHelper.AllVariables = ["Muons"]
#TEST4SlimmingHelper.StaticContent = ["CaloCellContainer#AODCellContainer"]
TEST4SlimmingHelper.AppendContentToStream(TEST4Stream)
