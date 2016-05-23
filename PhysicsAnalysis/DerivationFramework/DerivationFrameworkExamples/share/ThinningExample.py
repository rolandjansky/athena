#====================================================================
# ThinningExample.py
# This an example job options script showing how to run thinning in 
# the derivation framework using inner detector tracks as the example
# For trigger navigation thinning see TriggerContentsExample (TEST10) 
# It requires the reductionConf flag TEST8 in Reco_tf.py   
#====================================================================

# Set up common services and job object. 
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_TEST8Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_TEST8Stream )
TEST8Stream = MSMgr.NewPoolRootStream( streamName, fileName )
TEST8Stream.AcceptAlgs(["TEST8Kernel"])

#====================================================================
# THINNING  
#====================================================================

# Establish the thinning helper (which will set up the services behind the scenes)
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
TEST8ThinningHelper = ThinningHelper( "TEST8ThinningHelper" )
TEST8ThinningHelper.AppendToStream( TEST8Stream )

# Set up your thinning tools (you can have as many as you need). 
# Note how the thinning service (which must be passed to the tools) is accessed
from DerivationFrameworkExamples.DerivationFrameworkExamplesConf import DerivationFramework__ThinningToolExample
TEST8ThinningTool = DerivationFramework__ThinningToolExample( name                    = "TEST8ThinningTool",
							      ThinningService	      = TEST8ThinningHelper.ThinningSvc(),
							      TrackPtCut              = 20000.0)
ToolSvc += TEST8ThinningTool

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(	"TEST8Kernel",
									ThinningTools = [TEST8ThinningTool]
                                                                      )

#====================================================================
# CONTENT LIST  
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
TEST8SlimmingHelper = SlimmingHelper("TEST8SlimmingHelper")
TEST8SlimmingHelper.SmartCollections = ["InDetTrackParticles"]
TEST8SlimmingHelper.AppendContentToStream(TEST8Stream)
