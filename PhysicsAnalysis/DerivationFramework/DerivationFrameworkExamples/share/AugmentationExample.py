#==================================================================== 
# AugmentationExample.py
# This an example job options script showing how to add data to a
# derivation using the derivation framework.
# It uses an example augmentation tool also in this package.
# The tool adds a decoration (to InDetTrackParticles) 
# and a simple vector of floats, illustrating how both can be done.  
# It requires the reductionConf flag TEST7 in Reco_tf.py   
#====================================================================

# Set up common services and job object. 
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkIsMonteCarlo, DerivationFrameworkJob, buildFileName
from DerivationFrameworkPhys import PhysCommon

#====================================================================
# AUGMENTATION TOOLS 
#====================================================================
from DerivationFrameworkExamples.DerivationFrameworkExamplesConf import DerivationFramework__AugmentationToolExample
TEST7AugmentationTool = DerivationFramework__AugmentationToolExample(name = "TEST7AugmentationTool")
ToolSvc += TEST7AugmentationTool
print(TEST7AugmentationTool)

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("TEST7Kernel",
                                                                        AugmentationTools = [TEST7AugmentationTool]
                                                                      )

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_TEST7Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_TEST7Stream )
TEST7Stream = MSMgr.NewPoolRootStream( streamName, fileName )
TEST7Stream.AcceptAlgs(["TEST7Kernel"])

#==================================================================== 
# CONTENT DEFINITION
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
TEST7SlimmingHelper = SlimmingHelper("TEST7SlimmingHelper")
TEST7SlimmingHelper.SmartCollections = ["EventInfo",
                                       "PrimaryVertices",
                                       "InDetTrackParticles",
                                       "AntiKtVR30Rmax4Rmin02PV0TrackJets",
                                      ]
# This is a decoration to an xAOD object so it is added via the slimming helper
TEST7SlimmingHelper.ExtraVariables += ["InDetTrackParticles.DFDecoratorExample"]
# Since this is a simple vector it is added directly to the output stream
TEST7Stream.AddItem("std::vector<float>#DFAugmentationExample")
# Final construction of output stream
TEST7SlimmingHelper.AppendContentToStream(TEST7Stream)






