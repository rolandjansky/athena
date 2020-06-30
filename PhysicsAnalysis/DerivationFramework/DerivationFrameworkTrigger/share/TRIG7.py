from AthenaCommon.Constants import *
#====================================================================
# TRIG7.py 
# Purpose: investigate potential for using substructure in large-R jet trigger
# Skimming: require a high pT central ungroomed a10 jet and an HLT a10 jet trigger
# Content: 
#    AntiKt10Truth[Jets,TrimmedPtFrac5SmallR20Jets]
#    AntiKt10LCTopo [Jets,TrimmedPtFrac5SmallR20Jets]
#    AntiKt [2,4] PV0TrackJets - we have to have these to get trimmed jets, don't know why
#    AntiKt4EMPFlow [Jets,TopoJets] - TJ asked for these
#    CaloCalTopoClusters matched to ungroomed a10 - for playing with jet rec and jss 
# Based loosely on JETM6, JETM15, JETM8,...
# reductionConf flag TRIG7 in Reco_tf.py   
#====================================================================

# pt and eta cuts for jet selection
jetptmin='250.'
jetetamax='2.5'


from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *

#====================================================================
# DEFAULT IS NO TRUTH - NEED TO ADD IT EXPLICITLY THEN THIN IT BELOW
#====================================================================
#if DerivationFrameworkIsMonteCarlo:

#  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
#  addStandardTruthContents()

#====================================================================
# TRIGGER SKIMMING TOOL - REQUIRE A JET TRIGGER TO HAVE BEEN PASSED
#====================================================================
from DerivationFrameworkJetEtMiss import TriggerLists
jetTriggers = TriggerLists.jetTrig()

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
TRIG7TrigSkimmingTool = DerivationFramework__TriggerSkimmingTool(name = "TRIG7TrigSkimmingTool", TriggerListOR = jetTriggers)
ToolSvc += TRIG7TrigSkimmingTool

#====================================================================
# OFFLINE SKIMMING TOOL - REQUIRE A HIGH PT CENTRAL FAT JET
#====================================================================
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool

jetSelection = '(count( AntiKt10LCTopoJets.pt > '+jetptmin+'*GeV && abs(AntiKt10LCTopoJets.eta) < '+jetetamax+' ) >=1)'

TRIG7OfflineSkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "TRIG7OfflineSkimmingTool", expression = jetSelection)
ToolSvc += TRIG7OfflineSkimmingTool


#=======================================
# CREATE PRIVATE SEQUENCE
#=======================================
trig7Seq = CfgMgr.AthSequencer("TRIG7Sequence")
DerivationFrameworkJob += trig7Seq

#====================================================================
# CREATE THE DERIVATION TRIG SKIM KERNEL ALGORITHM AND PASS TRIG SKIM TOOL  
#====================================================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
trig7Seq += CfgMgr.DerivationFramework__DerivationKernel(name="TRIG7TrigSkimKernel",SkimmingTools = [TRIG7TrigSkimmingTool])

#=======================================
# RESTORE AOD-REDUCED JET COLLECTIONS - THESE JETS ARE NOT IN THE INPUT AOD 
#=======================================
reducedJetList = ["AntiKt2PV0TrackJets","AntiKt4PV0TrackJets","AntiKt10LCTopoJets","AntiKt10TruthJets"]
replaceAODReducedJets(reducedJetList,trig7Seq,"TRIG7")
addDefaultTrimmedJets(trig7Seq,"TRIG7")


#====================================================================
# SET UP STREAM   
#====================================================================
# The base name (DAOD_TRIG7 here) must match the string in 
# DerivationFrameworkProdFlags (in DerivationFrameworkCore) 
streamName = derivationFlags.WriteDAOD_TRIG7Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_TRIG7Stream )
TRIG7Stream = MSMgr.NewPoolRootStream( streamName, fileName )
TRIG7Stream.AcceptAlgs(["TRIG7MainKernel"])

#====================================================================
# THINNING HELPER
#====================================================================

from DerivationFrameworkCore.ThinningHelper import ThinningHelper
TRIG7ThinningHelper = ThinningHelper( "TRIG7ThinningHelper" )
### https://gitlab.cern.ch/atlas/athena/blob/21.2/PhysicsAnalysis/DerivationFramework/DerivationFrameworkExamples/share/TriggerContentExample.py
TRIG7ThinningHelper.TriggerChains = '^(?!.*_[0-9]*(mu|e|xe|tau|ht|xs|te))(?!HLT_j[0-9]+_[0-9]*j[0-9]+.*)HLT_j[0-9]+_a10.*'
TRIG7ThinningHelper.AppendToStream( TRIG7Stream )

thinningTools = []


#====================================================================
# CLUSTER THINNING TOOL
#====================================================================
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__JetCaloClusterThinning

jetcuts = '(AntiKt10LCTopoJets.pt > '+jetptmin+'*GeV && abs(AntiKt10LCTopoJets.eta) < '+jetetamax+')'

## save clusters matched to ungroomed jets with jetcuts selection
TRIG7ClusterThinningTool = DerivationFramework__JetCaloClusterThinning(name                  = "TRIG7ClusterThinningTool",
                                                                       StreamName            = streamName,
                                                                       SGKey                 = "AntiKt10LCTopoJets",
                                                                       TopoClCollectionSGKey = "CaloCalTopoClusters",
                                                                       SelectionString       = jetcuts,
                                                                       AdditionalClustersKey = ["LCOriginTopoClusters"])

ToolSvc += TRIG7ClusterThinningTool
thinningTools.append(TRIG7ClusterThinningTool)

trigjetcuts = '(HLT_xAOD__JetContainer_a10tclcwsubjesFS.pt > '+jetptmin+'*GeV && abs(HLT_xAOD__JetContainer_a10tclcwsubjesFS.eta) < '+jetetamax+')'

## save clusters matched to ungroomed trigger jets passing trigjetcuts
TRIG7TrigClusterThinningTool = DerivationFramework__JetCaloClusterThinning(name                  = "TRIG7TrigClusterThinningTool",
                                                                           StreamName            = streamName,
                                                                           SGKey                 = "HLT_xAOD__JetContainer_a10tclcwsubjesFS",
                                                                           TopoClCollectionSGKey = "CaloCalTopoClusters",
                                                                           SelectionString       = trigjetcuts
                                                           )
ToolSvc += TRIG7TrigClusterThinningTool
thinningTools.append(TRIG7TrigClusterThinningTool)

#====================================================================
# TRACK THINNING TOOL
#====================================================================
"""
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning

## save tracks matched to ungroomed jets passing jetcuts
TRIG7TrackThinningTool = DerivationFramework__JetTrackParticleThinning(name      = "TRIG7TrackThinningTool",
                                                                       StreamName             = streamName,
                                                                       JetKey                 = "AntiKt10LCTopoJets",
                                                                       InDetTrackParticlesKey = "InDetTrackParticles",
                                                                       SelectionString        = jetcuts)
ToolSvc += TRIG7TrackThinningTool
thinningTools.append(TRIG7TrackThinningTool)

## save trackss matched to ungroomed trigger jets passing trigjets cuts
TRIG7TrigTrackThinningTool = DerivationFramework__JetTrackParticleThinning(name   = "TRIG7TrigTrackThinningTool",
                                                                           StreamName             = streamName,
                                                                           JetKey                 = "HLT_xAOD__JetContainer_a10tclcwsubjesFS",
                                                                           InDetTrackParticlesKey = "InDetTrackParticles",
                                                                           SelectionString        = trigjetcuts)

ToolSvc += TRIG7TrigTrackThinningTool
thinningTools.append(TRIG7TrigTrackThinningTool)
"""
#====================================================================
# TRUTH THINNING TOOL
#====================================================================
doTruthThinning = False
from AthenaCommon.GlobalFlags import globalflags
if doTruthThinning and DerivationFrameworkIsMonteCarlo:  
  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
  TRIG7TruthThinningTool = DerivationFramework__MenuTruthThinning(name               = "TRIG7TruthThinningTool",
                                                                  StreamName         = streamName,
                                                                  WriteAllStable     = True)
  ToolSvc += TRIG7TruthThinningTool
  thinningTools.append(TRIG7TruthThinningTool)



#====================================================================
# CREATE THE OFFLINE SKIM KERNEL ALGORITHM AND PASS OFFLINE SKIM TOOL AND THINNING TOOLS
#====================================================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
trig7Seq += CfgMgr.DerivationFramework__DerivationKernel( name = "TRIG7MainKernel",
                                                          AugmentationTools = [] ,
                                                          SkimmingTools     = [TRIG7OfflineSkimmingTool],
                                                          ThinningTools     = thinningTools)



#====================================================================
# CONTENT LIST  
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper

TRIG7SlimmingHelper = SlimmingHelper("TRIG7SlimmingHelper")

# can only put standard collections in here - anything else needs to be appended to dictionary
TRIG7SlimmingHelper.SmartCollections = ["PrimaryVertices",
                                        "AntiKt4EMTopoJets","AntiKt4LCTopoJets","AntiKt4EMPFlowJets",
#                                        "BTagging_AntiKt2Track","BTagging_AntiKt4EMTopo",
                                        "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"]
#                                        "TauJets"]
#                                        "InDetTrackParticles"]
                                      

TRIG7SlimmingHelper.AllVariables = ["Kt4EMTopoOriginEventShape","Kt4LCTopoOriginEventShape","Kt4EMPFlowEventShape"]
#                                    "TruthVertices",
#                                    "TruthEvents", 
#                                    "TruthParticles"]

### these jets are on the fly
TRIG7SlimmingHelper.AppendToDictionary={"AntiKt10LCTopoJets" : "xAOD::JetContainer",
                                        "AntiKt10LCTopoJetsAux." : " xAOD::JetAuxContainer",
                                        "AntiKt10TruthJets" : "xAOD::JetContainer",
                                        "AntiKt10TruthJetsAux." : " xAOD::JetAuxContainer",
                                        "HLT_xAOD__JetContainer_a10ttclcwjesFS" : "xAOD::JetContainer",
                                        "HLT_xAOD__JetContainer_a10ttclcwjesFSAux." : "xAOD::JetAuxContainer",
                                        "HLT_xAOD__JetContainer_a10r_tcemsubjesISFS" : "xAOD::JetContainer" ,
                                        "HLT_xAOD__JetContainer_a10r_tcemsubjesISFSAux." : "xAOD::JetAuxContainer",
                                        "HLT_xAOD__JetContainer_a10r_tcemsubjesFS" : "xAOD::JetContainer" ,
                                        "HLT_xAOD__JetContainer_a10r_tcemsubjesFSAux." : "xAOD::JetAuxContainer" }

### want all vars for trigger jets
from DerivationFrameworkCore.JetTriggerContent import JetTriggerContent
for trig in JetTriggerContent:
    if 'HLT' in trig and not 'Aux' in trig:
        TRIG7SlimmingHelper.AllVariables += [trig]

TRIG7SlimmingHelper.AllVariables += ["AntiKt10LCTopoJets"]
TRIG7SlimmingHelper.AllVariables += ["AntiKt10TruthJets"]
TRIG7SlimmingHelper.AllVariables += ["HLT_xAOD__JetContainer_a10ttclcwjesFS"]
TRIG7SlimmingHelper.AllVariables += ["HLT_xAOD__JetContainer_a10r_tcemsubjesISFS"]
TRIG7SlimmingHelper.AllVariables += ["HLT_xAOD__JetContainer_a10r_tcemsubjesFS"]

TRIG7SlimmingHelper.AppendToDictionary["CaloCalTopoClusters"]='xAOD::CaloClusterContainer'
TRIG7SlimmingHelper.AppendToDictionary["CaloCalTopoClustersAux"]='xAOD::CaloClusterAuxContainer'
TRIG7SlimmingHelper.ExtraVariables = ["CaloCalTopoClusters.calEta.calPhi.CENTER_MAG.calE.calM.rawEta.rawPhi.rawE.rawM"]


addJetOutputs(TRIG7SlimmingHelper,["AntiKt10LCTopoJets",
                                   "AntiKt10TruthJets",
                                   "HLT_xAOD__JetContainer_a10ttclcwjesFS",
                                   "HLT_xAOD__JetContainer_a10r_tcemsubjesISFS",
                                   "HLT_xAOD__JetContainer_a10r_tcemsubjesFS"])
#                                   "TRIG7"])


TRIG7SlimmingHelper.AppendContentToStream(TRIG7Stream)

#TRIG7Stream.RemoveItem("xAOD::TrigNavigation#*")
#TRIG7Stream.RemoveItem("xAOD::TrigNavigationAuxInfo#*")
