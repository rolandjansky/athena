# ##########################################################################################
# Selection algorithms setup
# ##########################################################################################
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool as skimtool
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel as kernel
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool


###########################################################################################
# HV Trigger Filter
###########################################################################################

HVSkimmingTool = DerivationFramework__TriggerSkimmingTool(   name                    = "HVSkimmingTool",
                                                             TriggerListOR          = primRPVLLDESDM.HV_triggerFilterFlags.TriggerNames )
ToolSvc += HVSkimmingTool
topSequence += kernel( "RPVLL_HV_TriggerFilterKernel",
                       SkimmingTools = [HVSkimmingTool],
                       )
RPVLLfilterNames.extend(["RPVLL_HV_TriggerFilterKernel"])
