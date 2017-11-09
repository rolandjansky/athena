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

if globalflags.DataSource()=='geant4':
    from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
    addStandardTruthContents()

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_EXOT21Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EXOT21Stream )
EXOT21Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT21Stream.AcceptAlgs(["EXOT21Kernel"])

exot21Seq = CfgMgr.AthSequencer("EXOT21Sequence")

triggerNavChains  = ""
triggerNavChains += "HLT_3mu6* | HLT_3mu4 | HLT_mu* |"
triggerNavChains += "HLT_2mu14.* | HLT_2mu10.* | HLT_e* |"
triggerNavChains += "HLT_2e15_lhvloose_nod0_L12EM13VH |"
triggerNavChains += "HLT_g15_loose_2mu10_msonly.*| HLT_2e17_lhvloose_nod0 |"
triggerNavChains += "HLT_g35_loose_g25_loose | HLT_e17_lhloose_nod0_2e9_lhloose_nod0 |"
triggerNavChains += "HLT_j*"

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
triggers = ['HLT_mu50','HLT_mu24_ivarloose_L1MU15','HLT_mu26_ivarmedium','HLT_2mu14','HLT_2mu10','HLT_3mu6',
               'HLT_3mu6_msonly','HLT_3mu4','HLT_g15_loose_2mu10_msonly','HLT_e60_lhmedium_nod0','HLT_e24_lhtight_nod0_ivarloose',
               'HLT_e26_lhtight_nod0_ivarloose','HLT_2e15_lhvloose_nod0_L12EM13VH','HLT_2e17_lhvloose_nod0',
               'HLT_e17_lhloose_nod0_2e9_lhloose_nod0','HLT_g35_loose_g25_loose']

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
                                                                  SelectionString         =  "InDetTrackParticles.pt > 0.5*GeV && InDetTrackParticles.eta > -2.5 && InDetTrackParticles.eta < 2.5",
                                                                  InDetTrackParticlesKey  =  "InDetTrackParticles")
ToolSvc += EXOT21TPThinningTool
thinningTools.append(EXOT21TPThinningTool)


#Truth thinning
truth_expression = "(abs(TruthParticles.pdgId) == 11 || abs(TruthParticles.pdgId) == 13 || abs(TruthParticles.pdgId) == 3000005 || abs(TruthParticles.pdgId) == 3000005 || abs(TruthParticles.pdgId) == 700021 || abs(TruthParticles.pdgId) == 700022)"


from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
EXOT21MCGenThinningTool = DerivationFramework__GenericTruthThinning(name                    = "EXOT21MCGenThinningTool",
                                                                   ThinningService         = EXOT21ThinningHelper.ThinningSvc(),
                                                                   ParticleSelectionString = truth_expression,
                                                                   PreserveDescendants     = True,
                                                                   #PreserveGeneratorDescendants     = True,
                                                                   PreserveAncestors      = True)
if globalflags.DataSource()=='geant4':
	ToolSvc += EXOT21MCGenThinningTool
	thinningTools.append(EXOT21MCGenThinningTool)

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += exot21Seq
exot21Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT21Kernel_skim", SkimmingTools = [EXOT21AllEventsTool])
exot21Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT21Kernel", ThinningTools = thinningTools)

# SPECIAL LINES FOR THINNING
# Thinning service name must match the one passed to the thinning tools 
#from AthenaServices.Configurables import ThinningSvc, createThinningSvc
#augStream = MSMgr.GetStream( streamName )
#evtStream = augStream.GetEventStream()
#svcMgr += createThinningSvc( svcName="EXOT21ThinningSvc", outStreams=[evtStream] )


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
if globalflags.DataSource()=='geant4':
    EXOT21SlimmingHelper.ExtraVariables += EXOT21ExtraTruth
EXOT21SlimmingHelper.IncludeEGammaTriggerContent = True
EXOT21SlimmingHelper.IncludeMuonTriggerContent = True
EXOT21SlimmingHelper.IncludeJetTriggerContent = True
EXOT21SlimmingHelper.AppendContentToStream(EXOT21Stream)
