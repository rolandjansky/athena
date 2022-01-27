#====================================================================
# ThinningExample.py
# This an example job options script showing how to run thinning in 
# the derivation framework using inner detector tracks as the example
# It requires the reductionConf flag TEST3 in Reco_tf.py   
#====================================================================

# Set up common services and job object. 
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkIsMonteCarlo, DerivationFrameworkJob, buildFileName

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_TEST3Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_TEST3Stream )
TEST3Stream = MSMgr.NewPoolRootStream( streamName, fileName )
TEST3Stream.AcceptAlgs(["TEST3Kernel"])

#====================================================================
# THINNING  
#====================================================================

# Set up your thinning tools (you can have as many as you need). 
# Note how the thinning service (which must be passed to the tools) is accessed
from DerivationFrameworkExamples.DerivationFrameworkExamplesConf import DerivationFramework__ThinningToolExample
TEST3ThinningTool = DerivationFramework__ThinningToolExample( name                    = "TEST3ThinningTool",
                                                              StreamName              = streamName,
							      TrackPtCut              = 20000.0)
ToolSvc += TEST3ThinningTool

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(	"TEST3Kernel",
									ThinningTools = [TEST3ThinningTool]
                                                                      )

#====================================================================
# CONTENT LIST  
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
TEST3SlimmingHelper = SlimmingHelper("TEST3SlimmingHelper")
TEST3SlimmingHelper.SmartCollections = ["EventInfo","InDetTrackParticles"]
TEST3SlimmingHelper.AppendContentToStream(TEST3Stream)
