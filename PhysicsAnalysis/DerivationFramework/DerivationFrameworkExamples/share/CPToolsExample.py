#====================================================================
# CPToolsExample.py
# This shows how a tool from the CP groups is scheduled in a DAOD job
# and then the outcome used to skim events.
# It requires the reductionConf flag TEST5 in Reco_tf.py   
#====================================================================

# Set up common services and job object. 
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkIsMonteCarlo, DerivationFrameworkJob, buildFileName 
from DerivationFrameworkPhys import PhysCommon

#====================================================================
# CP GROUP TOOLS 
#====================================================================
from MuonSelectorTools.MuonSelectorToolsConf import CP__MuonSelectionTool
TEST5MuonTool = CP__MuonSelectionTool(name = "TEST5MuonTool")
ToolSvc += TEST5MuonTool
print(TEST5MuonTool)

#====================================================================
# AUGMENTATION TOOLS 
#====================================================================
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__AsgSelectionToolWrapper
TEST5MuonToolWrapper = DerivationFramework__AsgSelectionToolWrapper( 	name = "TEST5MuonToolWrapper",
									AsgSelectionTool = TEST5MuonTool,
									CutType = "IDHits",
									StoreGateEntryName = "TEST5GoodMuons",
									ContainerName = "Muons")									
ToolSvc += TEST5MuonToolWrapper
print(TEST5MuonToolWrapper)

#====================================================================
# SKIMMING TOOLS 
#====================================================================
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
expression = 'count((Muons.TEST5GoodMuons) && (Muons.pt > 10*GeV)) >= 1'
TEST5StringSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "TEST5StringSkimmingTool",
                                                                 expression = expression)

ToolSvc += TEST5StringSkimmingTool
print(TEST5StringSkimmingTool)

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

# The name of the kernel (LooseSkimKernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("TEST5Kernel",
                                                                        SkimmingTools = [TEST5StringSkimmingTool],
									AugmentationTools = [TEST5MuonToolWrapper]
                                                                      )


#====================================================================
# SET UP STREAM   
#====================================================================

# The base name (DAOD_TEST5 here) must match the string in 
streamName = derivationFlags.WriteDAOD_TEST5Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_TEST5Stream )
TEST5Stream = MSMgr.NewPoolRootStream( streamName, fileName )
# Only events that pass the filters listed below are written out.
# Name must match that of the kernel above
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
TEST5Stream.AcceptAlgs(["TEST5Kernel"])
