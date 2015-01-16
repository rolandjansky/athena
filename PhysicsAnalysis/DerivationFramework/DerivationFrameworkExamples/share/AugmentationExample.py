#==================================================================== 
# SkimmingExample.py
# This an example job options script showing how to add data to a
# derivation using the derivation framework.  
# It requires the reductionConf flag TEST7 in Reco_tf.py   
#====================================================================

# Set up common services and job object. 
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *

#====================================================================
# AUGMENTATION TOOLS 
#====================================================================
from DerivationFrameworkExamples.DerivationFrameworkExamplesConf import DerivationFramework__AugmentationToolExample
TEST7AugmentationTool = DerivationFramework__AugmentationToolExample(name = "TEST7AugmentationTool")
ToolSvc += TEST7AugmentationTool
print TEST7AugmentationTool

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
TEST7Stream.AddItem("std::vector<float>#DFAugmentationExample")
TEST7Stream.AddItem("xAOD::TrackParticleContainer#InDetTrackParticles")
TEST7Stream.AddItem("xAOD::TrackParticleAuxContainer#InDetTrackParticlesAux.") 
