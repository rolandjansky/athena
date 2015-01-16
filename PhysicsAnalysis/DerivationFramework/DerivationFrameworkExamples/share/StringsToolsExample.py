#====================================================================
# SkimmingExampleStrings.py
# This an example job options script showing how to set up a 
# derivation of the data using the derivation framework with xAOD
# input. 
# It requires the reductionConf flag TEST3 in Reco_tf.py   
#====================================================================

# Set up common services and job object. 
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkMuons.MuonsCommon import *

#====================================================================
# SKIMMING TOOLS 
#====================================================================
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
expression = 'count((Muons.DFCommonGoodMuon) && (Muons.pt > 15*GeV)) >= 2'
TEST3StringSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "TEST3StringSkimmingTool",
                                                                 expression = expression)

ToolSvc += TEST3StringSkimmingTool
print TEST3StringSkimmingTool

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

# The name of the kernel (LooseSkimKernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("TEST3Kernel", SkimmingTools = [TEST3StringSkimmingTool])


#====================================================================
# SET UP STREAM   
#====================================================================

# The base name (DAOD_TEST3 here) must match the string in 
streamName = derivationFlags.WriteDAOD_TEST3Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_TEST3Stream )
TEST3Stream = MSMgr.NewPoolRootStream( streamName, fileName )
# Only events that pass the filters listed below are written out.
# Name must match that of the kernel above
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
TEST3Stream.AcceptAlgs(["TEST3Kernel"])
