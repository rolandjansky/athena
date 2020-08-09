#====================================================================
# SkimmingExampleStrings.py
# This an example job options script showing how to set up a 
# derivation of the data using the derivation framework with xAOD
# input. 
# It requires the reductionConf flag TEST2 in Reco_tf.py   
#====================================================================

# Set up common services and job object. 
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *

#====================================================================
# SKIMMING TOOLS 
#====================================================================
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
TEST2StringSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "TEST2StringSkimmingTool",
                                                                 expression = "count(abs(Electrons.eta) < 1.0) > 0 && count(Muons.pt > (20 * GeV)) >= 1")
                                                                 #expression = "EventInfo.eventNumber")

ToolSvc += TEST2StringSkimmingTool
printfunc(TEST2StringSkimmingTool)

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

# The name of the kernel (LooseSkimKernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("TEST2Kernel",
                                                                        SkimmingTools = [TEST2StringSkimmingTool]
                                                                      )


#====================================================================
# SET UP STREAM   
#====================================================================

# The base name (DAOD_TEST2 here) must match the string in 
streamName = derivationFlags.WriteDAOD_TEST2Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_TEST2Stream )
TEST2Stream = MSMgr.NewPoolRootStream( streamName, fileName )
# Only events that pass the filters listed below are written out.
# Name must match that of the kernel above
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
TEST2Stream.AcceptAlgs(["TEST2Kernel"])
