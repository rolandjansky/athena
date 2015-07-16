from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool as skimtool
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel as kernel

# ##########################################################################################
# HIP trigger filter
# ##########################################################################################

HipTriggerFilterTool = skimtool( name = "HipTriggerFilterTool",
                                 expression = 'HLT_g0_hiptrt_L1EM18VH || HLT_g0_hiptrt_L1EM20VH || HLT_g0_hiptrt_L1EM20VHI || HLT_g0_hiptrt_L1EM22VHI' )

ToolSvc += HipTriggerFilterTool
topSequence += kernel( "RPVLL_HipTriggerFilterKernel",
                       SkimmingTools = [HipTriggerFilterTool] )

RPVLLfilterNames.extend(["RPVLL_HipTriggerFilterKernel"])
