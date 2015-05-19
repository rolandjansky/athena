# ##########################################################################################
# Selection algorithms setup
# ##########################################################################################
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool as skimtool
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel as kernel

def HVTriggerSelectionString(flags):
    cutString=""
    if flags.TriggerNames.__len__() >=1:
        cutString+=flags.TriggerNames[0]
        if flags.TriggerNames.__len__() >1:
            for trigger in flags.TriggerNames[1:]:
                cutString+=" || "+trigger
                pass
            pass
        pass
    return cutString
    pass

###########################################################################################
# HV Trigger Filter
###########################################################################################

HVTriggerFilter = skimtool( name = "HVTriggerFilter",
                                  expression = HVTriggerSelectionString(primRPVLLDESDM.HV_triggerFilterFlags)
                                  )
ToolSvc += HVTriggerFilter

topSequence += kernel( "RPVLL_HV_TriggerFilterKernel",
                       SkimmingTools = [HVTriggerFilter],
                       )
RPVLLfilterNames.extend(["RPVLL_HV_TriggerFilterKernel"])
