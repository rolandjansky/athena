
#############################################################################################
##### NOTE!! this file is the one called when using the HIPs filter as part of the DESDM_RPVLL.
#####  This has been superseded by PhysDPD_HIPs.py which makes a standalone DESD_HIPS format.
##############################################################################################


from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool as skimtool
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel as kernel
from LongLivedParticleDPDMaker.HipsFlags import primHIPsDESD

# ##########################################################################################
# HIP trigger filter
# ##########################################################################################

HipTriggerFilterTool = skimtool( name = "HipTriggerFilterTool",
                                 expression = primHIPsDESD.HipsTriggerFilterExpression())


ToolSvc += HipTriggerFilterTool
topSequence += kernel( "RPVLL_HipTriggerFilterKernel",
                       SkimmingTools = [HipTriggerFilterTool] )

RPVLLfilterNames.extend(["RPVLL_HipTriggerFilterKernel"])
