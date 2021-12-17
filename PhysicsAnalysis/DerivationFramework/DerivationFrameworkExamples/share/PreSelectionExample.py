#====================================================================
# PreSelectionExample.py
# This example shows how to use a sequencer such that a pre-selection
# algorithm can make a quick decision as to whether to keep an event
# before passing it to the main kernel. The use case would be for 
# derivations requiring the use of CPU-heavy tools: in these cases
# one would not wish to run the tools for every event; one would 
# therefore put the CPU-heavy tools in the second (main) algorithm,
# shielding them from having to process every event.
# It requires the reductionConf flag TEST6 in Reco_tf.py   
#====================================================================

# Set up common services and job object. 
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkIsMonteCarlo, DerivationFrameworkJob, buildFileName
from DerivationFrameworkPhys import PhysCommon


#====================================================================
# SKIMMING TOOLS 
#====================================================================
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
TEST6StringSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "TEST6StringSkimmingTool",
                                                                 expression = "count(abs(Electrons.eta) < 1.0) > 0 && count(Muons.pt > (20 * GeV)) >= 1")
                                                                 #expression = "EventInfo.eventNumber")

ToolSvc += TEST6StringSkimmingTool
print(TEST6StringSkimmingTool)

#====================================================================
# CREATE THE SEQUENCER  
#====================================================================
TEST6Sequence = CfgMgr.AthSequencer("TEST6Sequence")

#====================================================================
# CREATE THE PRESELECTION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
# Note that it is added to the sequence, NOT the job
TEST6Sequence += CfgMgr.DerivationFramework__DerivationKernel("TEST6PreselectionKernel",
                                                               SkimmingTools = [TEST6StringSkimmingTool]
                                                             )

#====================================================================
# SET UP THE CP TOOLS  
#====================================================================

from DerivationFrameworkExamples.DerivationFrameworkExamplesConf import DerivationFramework__AugmentationToolExample
TEST6AugmentationTool = DerivationFramework__AugmentationToolExample(name= "TEST6AugmentationTool")
ToolSvc += TEST6AugmentationTool
print(TEST6AugmentationTool)

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

TEST6Sequence += CfgMgr.DerivationFramework__DerivationKernel("TEST6Kernel",
                                                              AugmentationTools = [TEST6AugmentationTool]
                                                             )

# Now we add the sequencer to the job
DerivationFrameworkJob += TEST6Sequence

#====================================================================
# SET UP STREAM   
#====================================================================

# The base name (DAOD_TEST6 here) must match the string in 
streamName = derivationFlags.WriteDAOD_TEST6Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_TEST6Stream )
TEST6Stream = MSMgr.NewPoolRootStream( streamName, fileName )
# Only events that pass the filters listed below are written out.
# Name must match that of the kernel above
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
TEST6Stream.AcceptAlgs(["TEST6Kernel"])
