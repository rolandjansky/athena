#====================================================================
# SkimmingExample.py
# This example demonstrates event skimming by means of a dedicated
# C++ tool (also in this example package). Dedicated tools are 
# typically used for very complex selections - simple selections such
# as the one shown here would usually use the generic string-based 
# skimming tool.
# It requires the reductionConf flag TEST1 in Reco_tf.py   
#====================================================================

# Set up common services and job object. 
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *

#====================================================================
# SKIMMING TOOLS 
#====================================================================

# Set up your skimming tools (you can have as many as you need). 
# The tools must use the DerivationFrameworkInterfaces as in this example.
from DerivationFrameworkExamples.DerivationFrameworkExamplesConf import DerivationFramework__SkimmingToolExample
TEST1SkimmingTool = DerivationFramework__SkimmingToolExample(   name                    = "TEST1SkimmingTool",
                                                                MuonContainerKey        = "Muons",
                                                                NumberOfMuons           = 1,
                                                                MuonPtCut               = 1000.0)
ToolSvc += TEST1SkimmingTool

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

# The name of the kernel (LooseSkimKernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("TEST1Kernel",
                                                                        SkimmingTools = [TEST1SkimmingTool]
                                                                      )

#====================================================================
# SET UP STREAM   
#====================================================================

# SKIMMING
# The base name (DAOD_TEST1 here) must match the string in 
# DerivationFrameworkProdFlags (in DerivationFrameworkCore) 
streamName = derivationFlags.WriteDAOD_TEST1Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_TEST1Stream )
TEST1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
# Only events that pass the filters listed below are written out.
# Name must match that of the kernel above
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
TEST1Stream.AcceptAlgs(["TEST1Kernel"])

#====================================================================
# CONTENT LIST  
#====================================================================

# None here - output file will contain only metadata
