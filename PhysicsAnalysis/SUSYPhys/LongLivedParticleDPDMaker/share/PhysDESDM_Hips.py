from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool as skimtool
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel as kernel

# ##########################################################################################
# HIP trigger filter
# ##########################################################################################

HipTriggerFilterTool = skimtool( name = "HipTriggerFilterTool",
                                 expression = 'EF_g_nocut_hiptrtL2' )

ToolSvc += HipTriggerFilterTool
topSequence += kernel( "RPVLL_HipTriggerFilterKernel",
                       SkimmingTools = [HipTriggerFilterTool] )

RPVLLfilterNames.extend(["RPVLL_HipTriggerFilterKernel"])
