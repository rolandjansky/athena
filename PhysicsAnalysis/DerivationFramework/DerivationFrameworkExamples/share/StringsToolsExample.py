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

#====================================================================
# AUGMENTATION TOOLS 
#====================================================================
from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__EGammaPassSelectionWrapper
TEST3ElectronPassSelection = DerivationFramework__EGammaPassSelectionWrapper( name = "TEST3ElectronPassSelection",
                                                                         SelectionVariables = ["Loose","Medium","Tight"],
                                                                         CollectionName = "ElectronCollection",
									 SGPrefix = "TEST3Electrons"
                                                                      )
ToolSvc += TEST3ElectronPassSelection
print TEST3ElectronPassSelection

#====================================================================
# SKIMMING TOOLS 
#====================================================================
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
expression = 'count((TEST3ElectronsLoose) && (ElectronCollection.pt > 10*GeV)) >= 1'
TEST3StringSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "TEST3StringSkimmingTool",
                                                                 expression = expression)
                                                                 #expression = "EventInfo.eventNumber")

ToolSvc += TEST3StringSkimmingTool
print TEST3StringSkimmingTool

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

# The name of the kernel (LooseSkimKernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("TEST3Kernel",
                                                                        SkimmingTools = [TEST3StringSkimmingTool],
									AugmentationTools = [TEST3ElectronPassSelection]
                                                                      )


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
