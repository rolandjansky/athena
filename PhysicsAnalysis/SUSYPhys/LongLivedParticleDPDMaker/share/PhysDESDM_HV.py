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
# HV Muvtx Filter
###########################################################################################

HVMuvtxTriggerTool = DerivationFramework__TriggerSkimmingTool(   name                    = "HVMuvtxTriggerTool",
                                                             TriggerListOR          = primRPVLLDESDM.HV_MuvtxTriggerFlags.TriggerNames )
ToolSvc += HVMuvtxTriggerTool

###########################################################################################
# HV Prescaled Muvtx Trigger Filter
###########################################################################################

HVMuvtxPrescaledTriggerTool = DerivationFramework__TriggerSkimmingTool(   name                    = "HVMuvtxPrescaledTriggerTool",
                                                                      TriggerListOR          = primRPVLLDESDM.HV_prescaledMuvtxTriggerFlags.TriggerNames )
ToolSvc += HVMuvtxPrescaledTriggerTool

HVMuvtxPrescaleTool = DerivationFramework__PrescaleTool(name = "HVPrescaleMuvtxTool",
                                                   Prescale = primRPVLLDESDM.HV_prescaledMuvtxTriggerFlags.Prescale
                                                   )
ToolSvc += HVMuvtxPrescaleTool

HVPrescaledMuvtxSkimmingTool = DerivationFramework__FilterCombinationAND( name = "HVMuvtxSkimmingTool",
                                                                     FilterList = [HVMuvtxPrescaledTriggerTool,
                                                                                   HVMuvtxPrescaleTool,
                                                                                   ],
                                                                     )
ToolSvc += HVPrescaledMuvtxSkimmingTool

 
HV_MuvtxFinalFilter = DerivationFramework__FilterCombinationOR( name = "HV_MuvtxFinalFilter", FilterList=[HVMuvtxTriggerTool,HVPrescaledMuvtxSkimmingTool] )
 #                                                       OutputLevel=DEBUG

ToolSvc += HV_MuvtxFinalFilter

topSequence += kernel( "RPVLL_HV_MuvtxFilterKernel",
                       SkimmingTools = [HV_MuvtxFinalFilter]
                       )
RPVLLfilterNames.extend(["RPVLL_HV_MuvtxFilterKernel"])

###########################################################################################
# HV jet/MET Filter
###########################################################################################

HVJetMETTrigTool = DerivationFramework__TriggerSkimmingTool(   name                    = "HVJetMETTrigTool",
                                                             TriggerListOR          = primRPVLLDESDM.HV_JetMETFilterFlags.TriggerNames )
ToolSvc += HVJetMETTrigTool

from LongLivedParticleDPDMaker.LongLivedParticleDPDMakerConf import DerivationFramework__HVJetMETFilterTool

HVJetMETFilterTool = DerivationFramework__HVJetMETFilterTool(name = "HVJetMETFilterTool",
                                                             METContainerKey = "MET_Reference_AntiKt4EMTopo",
                                                             METTerm         = "FinalClus",
                                                             METCut          = primRPVLLDESDM.HV_JetMETFilterFlags.cutMetMin)

#HVJetMETFilterTool = skimtool( name = "HVJetMETFilterTool",
#                               expression = 'MET_Reference_AntiKt4EMTopo["FinalClus"].met > 50.0*GeV')

ToolSvc+=HVJetMETFilterTool

HV_JetMETFinalFilter = DerivationFramework__FilterCombinationAND( name = "HV_JetMETFinalFilter",
                                                               FilterList=[HVJetMETTrigTool,HVJetMETFilterTool],
#                                                               OutputLevel=DEBUG
                                                               )
ToolSvc += HV_JetMETFinalFilter

topSequence += kernel( "RPVLL_HV_JetMETFilterKernel",
                       SkimmingTools = [HV_JetMETFinalFilter]
                       )
RPVLLfilterNames.extend(["RPVLL_HV_JetMETFilterKernel"])

###########################################################################################
# HV CalRatio Filter
###########################################################################################

HVCalRatioTriggerTool = DerivationFramework__TriggerSkimmingTool(   name                    = "HVCalRatioTriggerTool",
                                                             TriggerListOR          = primRPVLLDESDM.HV_CalRatioTriggerFlags.TriggerNames )
ToolSvc += HVCalRatioTriggerTool

topSequence += kernel( "RPVLL_HV_CalRatioFilterKernel",
                       SkimmingTools = [HVCalRatioTriggerTool],
                       )
RPVLLfilterNames.extend(["RPVLL_HV_CalRatioFilterKernel"])



