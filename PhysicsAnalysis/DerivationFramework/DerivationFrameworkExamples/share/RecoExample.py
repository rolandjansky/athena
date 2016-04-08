#====================================================================
# RecExample.py
# This example schedules MET re-reconstruction and includes the new
# containers at the end. In fact all of the reco stuff is in the 
# JetMET header file.
# The pre-selection can also be included as required.
# It requires the reductionConf flag TEST9 in Reco_tf.py   
#====================================================================

# Set up common services and job object. 
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetEtMissCommon import *
#====================================================================
# SKIMMING TOOLS FOR PRESELECTION
#====================================================================
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
TEST9StringSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "TEST9StringSkimmingTool",
                                                                 expression = "count(abs(Electrons.eta) < 1.0) > 0 && count(Muons.pt > (20 * GeV)) >= 1")
                                                                 #expression = "EventInfo.eventNumber")

ToolSvc += TEST9StringSkimmingTool
print TEST9StringSkimmingTool

#====================================================================
# CREATE THE SEQUENCER  
#====================================================================
TEST9Sequence = CfgMgr.AthSequencer("TEST9Sequence")

#====================================================================
# CREATE THE PRESELECTION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
# Note that it is added to the sequence, NOT the job
# Uncomment these lines to activate pre-selection
#TEST9Sequence += CfgMgr.DerivationFramework__DerivationKernel("TEST9PreselectionKernel",
#                                                               SkimmingTools = [TEST9StringSkimmingTool]
#                                                             )

#====================================================================
# ATTACH THE RECONSTRUCTION TO THE SEQUENCER  
#====================================================================

#TEST9Sequence += DerivationFrameworkMetAlg

#====================================================================
# CREATE THE MAIN DERIVATION KERNEL ALGORITHM  
#====================================================================

TEST9Sequence += CfgMgr.DerivationFramework__DerivationKernel("TEST9Kernel")

# Now we add the sequencer to the job
DerivationFrameworkJob += TEST9Sequence

#====================================================================
# SET UP STREAM   
#====================================================================

# The base name (DAOD_TEST9 here) must match the string in 
streamName = derivationFlags.WriteDAOD_TEST9Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_TEST9Stream )
TEST9Stream = MSMgr.NewPoolRootStream( streamName, fileName )
# Only events that pass the filters listed below are written out.
# Name must match that of the kernel above
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
TEST9Stream.AcceptAlgs(["TEST9Kernel"])
TEST9Stream.AddItem("xAOD::EventInfo_v1#*")
TEST9Stream.AddItem("xAOD::EventAuxInfo_v1#*")
# The new containers

suffixlist = METConfigs_SUSY.keys()+METConfigs_Top.keys()+METConfigs_JetMet.keys()
addMETOutputs(TEST9Stream,suffixlist)
