###########################################################################################
# Selection algorithms setup
# ##########################################################################################
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool as skimtool
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel as kernel
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__PrescaleTool

 ## needs package PhysicsAnalysis/DerivationFramework/DerivationFrameworkTools-00-00-20 or later..
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationAND
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR



###########################################################################################
# HV Trigger Filter
###########################################################################################

HVSkimmingTool = DerivationFramework__TriggerSkimmingTool(   name                    = "HVSkimmingTool",
                                                             TriggerListOR          = primRPVLLDESDM.HV_triggerFilterFlags.TriggerNames )
ToolSvc += HVSkimmingTool

#topSequence += kernel( "RPVLL_HV_TriggerFilterKernel",
#                       SkimmingTools = [HVSkimmingTool],
#                       )
#RPVLLfilterNames.extend(["RPVLL_HV_TriggerFilterKernel"])

###########################################################################################
# HV Prescaled Trigger Filter
###########################################################################################

HVPrescaledTriggerTool = DerivationFramework__TriggerSkimmingTool(   name                    = "HVPrescaledTriggerTool",
                                                                      TriggerListOR          = primRPVLLDESDM.HV_prescaledTriggerFilterFlags.TriggerNames )
ToolSvc += HVPrescaledTriggerTool

HVPrescaleTool = DerivationFramework__PrescaleTool(name = "HVPrescaleTool",
                                                   Prescale = primRPVLLDESDM.HV_prescaledTriggerFilterFlags.Prescale
                                                   )
ToolSvc += HVPrescaleTool

HVPrescaledSkimmingTool = DerivationFramework__FilterCombinationAND( name = "HVPrescaledSkimmingTool",
                                                                     FilterList = [HVPrescaledTriggerTool,
                                                                                   HVPrescaleTool,
                                                                                   ],
                                                                     )
ToolSvc += HVPrescaledSkimmingTool

###########################################################################################
# HV jet/MET Filter
###########################################################################################

HVJetMETTrigTool = DerivationFramework__TriggerSkimmingTool(   name                    = "HVJetMETTrigTool",
                                                             TriggerListOR          = primRPVLLDESDM.HV_jetMETFilterFlags.TriggerNames )
ToolSvc += HVJetMETTrigTool

from LongLivedParticleDPDMaker.LongLivedParticleDPDMakerConf import DerivationFramework__HVJetMETFilterTool

HVJetMETFilterTool = DerivationFramework__HVJetMETFilterTool(name = "HVJetMETFilterTool",
                                                             METContainerKey = "MET_Reference_AntiKt4EMTopo",
                                                             METTerm         = "FinalClus",
                                                             METCut          = primRPVLLDESDM.HV_jetMETFilterFlags.cutMetMin)

#HVJetMETFilterTool = skimtool( name = "HVJetMETFilterTool",
#                               expression = 'MET_Reference_AntiKt4EMTopo["FinalClus"].met > 50.0*GeV')

ToolSvc+=HVJetMETFilterTool

HV_JetMETFinalFilter = DerivationFramework__FilterCombinationAND( name = "HV_JetMETFinalFilter",
                                                               FilterList=[HVJetMETTrigTool,HVJetMETFilterTool],
#                                                               OutputLevel=DEBUG
                                                               )
ToolSvc += HV_JetMETFinalFilter

###########################################################################################
# HV Filter combination
###########################################################################################

#HV_FinalFilter =  DerivationFramework__FilterCombinationOR( name = "HV_FinalFilter",
#                                                          FilterList=[HVSkimmingTool,HV_JetMETFinalFilter],
#                                                          OutputLevel=DEBUG
#                                                          )
 
HV_FinalFilter = DerivationFramework__FilterCombinationOR( name = "HV_FinalFilter", FilterList=[HVSkimmingTool,HVPrescaledSkimmingTool,HV_JetMETFinalFilter] )
 #                                                       OutputLevel=DEBUG
                                 
ToolSvc+=HV_FinalFilter
                          
topSequence += kernel( "RPVLL_HVTriggerFilterKernel",
                       SkimmingTools = [HV_FinalFilter],
                       )
RPVLLfilterNames.extend(["RPVLL_HVTriggerFilterKernel"])


