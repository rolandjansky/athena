#********************************************************************
# EXOT21.py 
# reductionConf flag EXOT21 in Reco_tf.py   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkCore.WeightMetadata import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkEGamma.ElectronsCPDetailedContent import *
import DerivationFrameworkJetEtMiss.ExtendedJetCommon


#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_EXOT21Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EXOT21Stream )
EXOT21Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT21Stream.AcceptAlgs(["EXOT21Kernel"])

exot21Seq = CfgMgr.AthSequencer("EXOT21Sequence")

triggerNavChains  = "HLT_mu50 | HLT_mu24_ivarloose_L1MU15 | HLT_mu26_ivarmedium | HLT_mu20_iloose_L1MU15 | HLT_mu60_0eta105_msonly | HLT_mu40 |"
triggerNavChains += "HLT_2mu14 | HLT_2mu10 | HLT_3mu6 | HLT_3mu6_msonly | HLT_3mu4 | HLT_mu22_mu8noL1 | HLT_mu18_mu8noL1 | HLT_2mu14_nomucomb | HLT_mu20_2mu4noL1 | HLT_mu18_2mu4noL1 | HLT_mu6_2mu4 | HLT_mu11_nomucomb_mu6noL1_nscan03_L1MU11_2MU6_bTau | HLT_mu20_nomucomb_mu6noL1_nscan03 | HLT_2mu10_nomucomb | HLT_mu20_mu8noL1 |"
triggerNavChains += "HLT_mu20_msonly_mu6noL1_msonly_nscan05 | HLT_mu20_msonly_mu10noL1_msonly_nscan05_noComb  | HLT_mu20_msonly_mu15noL1_msonly_nscan05_noComb  | HLT_mu20_msonly_iloosems_mu6noL1_msonly_nscan05_L1MU20_J40  | HLT_mu20_msonly_iloosems_mu6noL1_msonly_nscan05_L1MU20_XE30  | HLT_mu6_dRl1_mu20_msonly_iloosems_mu6noL1_dRl1_msonly |"
triggerNavChains += "HLT_g15_loose_2mu10_msonly |"
triggerNavChains += "HLT_e60_lhmedium_nod0 | HLT_e24_lhtight_nod0_ivarloose | HLT_e26_lhtight_nod0_ivarloose | HLT_e120_lhloose | HLT_e140_lhloose_nod0 | HLT_e24_lhmedium_L1EM20VH | HLT_e26_lhtight_nod0 | HLT_e300_etcut | HLT_e60_lhmedium | HLT_e60_medium |"
triggerNavChains += "HLT_2e15_lhvloose_nod0_L12EM13VH | HLT_2e17_lhvloose_nod0 | HLT_2e12_lhvloose_L12EM10VH | HLT_2e17_lhvloose_nod0_L12EM15VHI | HLT_2e24_lhvloose_nod0 | HLT_2e24_lhvloose_nod0_L12EM20VH |"
triggerNavChains += "HLT_e17_lhloose_nod0_2e9_lhloose_nod0 | HLT_g35_loose_g25_loose | HLT_e17_lhloose_2e9_lhloose | HLT_e17_lhloose_nod0_2e10_lhloose_nod0_L1EM15VH_3EM8VH | HLT_e24_lhvloose_nod0_2e12_lhvloose_nod0_L1EM20VH_3EM10VH"


from DerivationFrameworkCore.ThinningHelper import ThinningHelper
EXOT21ThinningHelper = ThinningHelper("EXOT21ThinningHelper")
EXOT21ThinningHelper.TriggerChains = triggerNavChains
EXOT21ThinningHelper.AppendToStream(EXOT21Stream)

##=======================================
# SKIMMING   
#=======================================
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationAND

#default triggers
triggers = ['HLT_mu50','HLT_mu24_ivarloose_L1MU15','HLT_mu26_ivarmedium','HLT_mu20_iloose_L1MU15','HLT_mu60_0eta105_msonly','HLT_2mu14','HLT_2mu10','HLT_3mu6','HLT_3mu6_msonly','HLT_3mu4','HLT_mu22_mu8noL1','HLT_mu18_mu8noL1','HLT_2mu14_nomucomb','HLT_mu20_2mu4noL1','HLT_mu18_2mu4noL1','HLT_g15_loose_2mu10_msonly','HLT_e60_lhmedium_nod0','HLT_e24_lhtight_nod0_ivarloose','HLT_e26_lhtight_nod0_ivarloose','HLT_e120_lhloose','HLT_e140_lhloose_nod0','HLT_e24_lhmedium_L1EM20VH','HLT_e26_lhtight_nod0','HLT_e300_etcut','HLT_e60_lhmedium','HLT_e60_medium','HLT_2e15_lhvloose_nod0_L12EM13VH','HLT_2e17_lhvloose_nod0','HLT_2e12_lhvloose_L12EM10VH','HLT_2e17_lhvloose_nod0_L12EM15VHI','HLT_2e24_lhvloose_nod0','HLT_2e24_lhvloose_nod0_L12EM20VH','HLT_e17_lhloose_nod0_2e9_lhloose_nod0','HLT_g35_loose_g25_loose','HLT_e17_lhloose_2e9_lhloose','HLT_e17_lhloose_nod0_2e10_lhloose_nod0_L1EM15VH_3EM8VH','HLT_e24_lhvloose_nod0_2e12_lhvloose_nod0_L1EM20VH_3EM10VH','HLT_mu20_msonly_mu6noL1_msonly_nscan05','HLT_mu20_msonly_mu10noL1_msonly_nscan05_noComb','HLT_mu20_msonly_mu15noL1_msonly_nscan05_noComb','HLT_mu20_msonly_iloosems_mu6noL1_msonly_nscan05_L1MU20_J40','HLT_mu20_msonly_iloosems_mu6noL1_msonly_nscan05_L1MU20_XE30','HLT_mu6_dRl1_mu20_msonly_iloosems_mu6noL1_dRl1_msonly','HLT_mu40','HLT_mu6_2mu4','HLT_mu11_nomucomb_mu6noL1_nscan03_L1MU11_2MU6_bTau','HLT_mu20_nomucomb_mu6noL1_nscan03','HLT_2mu10_nomucomb','HLT_mu20_mu8noL1']

#QCD triggers to be used with the 10% GRL
extraTriggers = ['HLT_j25','HLT_j35','HLT_j45','HLT_j55','HLT_j60','HLT_j85','HLT_j150','HLT_j175','HLT_j340','HLT_j380','HLT_3j175','HLT_3j200']

#GoodRunsListSelectionTool that selects events in the 10% GRL
ToolSvc += CfgMgr.GoodRunsListSelectionTool("EXOT21GRLTool",GoodRunsListVec=['DerivationFrameworkExotics/data/EXOT21.grl.xml'])

#TriggerSkimmingTools
EXOT21TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool("EXOT21TriggerSkimmingTool",TriggerListOR=triggers)
ToolSvc += EXOT21TriggerSkimmingTool
EXOT21ExtraTriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool("EXOT21ExtraTriggerSkimmingTool",TriggerListOR=extraTriggers)
ToolSvc += EXOT21ExtraTriggerSkimmingTool

#combine the TriggerSkimmingTools with logical AND and OR:
#(EXOT21TriggerSkimmingTool || (EXOT21GRLTool && EXOT21ExtraTriggerSkimmingTool))
EXOT21ExtraEventsTool = DerivationFramework__FilterCombinationAND("EXOT21ExtraEventsTool",FilterList=[ToolSvc.EXOT21GRLTool,EXOT21ExtraTriggerSkimmingTool])
ToolSvc += EXOT21ExtraEventsTool
EXOT21AllEventsTool = DerivationFramework__FilterCombinationOR("EXOT21AllEventsTool",FilterList=[EXOT21TriggerSkimmingTool,EXOT21ExtraEventsTool])
ToolSvc += EXOT21AllEventsTool

#=======================================
# THINNING
#=======================================

thinningTools=[]

# Thin all inner detector tracks
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
EXOT21TPThinningTool = DerivationFramework__TrackParticleThinning(name                    =  "EXOT21TPThinningTool",
                                                                  ThinningService = EXOT21ThinningHelper.ThinningSvc(),
                                                                  SelectionString         =  "InDetTrackParticles.pt > 0.5*GeV && InDetTrackParticles.eta > -2.5 && InDetTrackParticles.eta < 2.5 && abs(DFCommonInDetTrackZ0AtPV)*sin(InDetTrackParticles.theta) <= 1.5",
                                                                  InDetTrackParticlesKey  =  "InDetTrackParticles")
ToolSvc += EXOT21TPThinningTool
thinningTools.append(EXOT21TPThinningTool)


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += exot21Seq
exot21Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT21Kernel_skim", SkimmingTools = [EXOT21AllEventsTool])
exot21Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT21Kernel", ThinningTools = thinningTools)

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
# Currently using same list as EXOT21
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkExotics.EXOT21ContentList import *
EXOT21SlimmingHelper = SlimmingHelper("EXOT21SlimmingHelper")
EXOT21SlimmingHelper.SmartCollections = EXOT21SmartContent 
EXOT21SlimmingHelper.AllVariables = EXOT21AllVariablesContent 
EXOT21SlimmingHelper.StaticContent = EXOT21UnslimmedContent 
EXOT21SlimmingHelper.ExtraVariables = EXOT21ExtraVariables
EXOT21SlimmingHelper.ExtraVariables += ElectronsCPDetailedContent
EXOT21SlimmingHelper.IncludeEGammaTriggerContent = True
EXOT21SlimmingHelper.IncludeMuonTriggerContent = True
EXOT21SlimmingHelper.IncludeJetTriggerContent = True
EXOT21SlimmingHelper.AppendContentToStream(EXOT21Stream)
