#====================================================================
# JETM9.py 
# reductionConf flag JETM9 in Reco_tf.py   
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkIsMonteCarlo, DerivationFrameworkJob, buildFileName
from DerivationFrameworkJetEtMiss.JetCommon import addDAODJets, OutputJets, addJetOutputs

#====================================================================
# SET UP STREAM
#====================================================================
streamName = derivationFlags.WriteDAOD_JETM9Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_JETM9Stream )
JETM9Stream = MSMgr.NewPoolRootStream( streamName, fileName )
JETM9Stream.AcceptAlgs(["JETM9Kernel"])
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()

#====================================================================                                                                                                              
# SKIMMING TOOL 
#====================================================================

JETM9SkimmingTools = []

if DerivationFrameworkIsMonteCarlo:
    from DerivationFrameworkJetEtMiss import TriggerLists
    triggers = TriggerLists.jetTrig()

    from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
    JETM9TrigSkimmingTool = DerivationFramework__TriggerSkimmingTool(   name                    = "JETM9TrigSkimmingTool1",
                                                                        TriggerListOR          = triggers )
    ToolSvc += JETM9TrigSkimmingTool

    JETM9SkimmingTools += [JETM9TrigSkimmingTool]

#=======================================
# CREATE PRIVATE SEQUENCE
#=======================================

jetm9Seq = CfgMgr.AthSequencer("JETM9Sequence")
DerivationFrameworkJob += jetm9Seq

#=======================================
# Truth particle thinning
#=======================================
thinningTools = []
from AthenaCommon.GlobalFlags import globalflags
if DerivationFrameworkIsMonteCarlo:
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
    JETM9TruthThinning = DerivationFramework__MenuTruthThinning(name                  = "JETM9TruthThinning",
                                                                StreamName                 = streamName,
                                                                WriteStatus3               = True,
                                                                PreserveAncestors          = True,
                                                                WriteFirstN                = 10)
    ToolSvc += JETM9TruthThinning
    thinningTools.append(JETM9TruthThinning)


#==================================================================== 
# Jets for R-scan
#==================================================================== 
from JetRecConfig.JetDefinition import JetDefinition
from JetRecConfig.StandardSmallRJets import standardghosts,flavourghosts,standardmods,clustermods,truthmods
from JetRecConfig.StandardJetConstits import stdConstitDic as cst

calibmods = ("ConstitFourMom", "CaloEnergies", "Sort", )

RscanJets = []

for radius in [0.2, 0.3, 0.5, 0.6, 0.7, 0.8]:
    if DerivationFrameworkIsMonteCarlo:
        truthJets = JetDefinition("AntiKt",radius,cst.Truth,
                                  ghostdefs = flavourghosts,
                                  modifiers = ("Sort", "Width")+truthmods,
                                  lock = True)

        truthJetsWZ = JetDefinition("AntiKt",radius,cst.TruthWZ,
                                    ghostdefs = flavourghosts,
                                    modifiers = ("Sort", "Width")+truthmods,
                                    lock = True)

        RscanJets += [truthJets]
        RscanJets += [truthJetsWZ]

    jets = JetDefinition("AntiKt",radius,cst.LCTopoOrigin,
                         ghostdefs = standardghosts+flavourghosts,
                         modifiers = calibmods+("OriginSetPV",)+standardmods+clustermods,
                         lock = True)
    RscanJets += [jets]
                         
OutputJets["JETM9"] = ["AntiKt4EMTopoJets","AntiKt4LCTopoJets","AntiKt4TruthJets","AntiKt4TruthWZJets"]

#====================================================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
jetm9Seq += CfgMgr.DerivationFramework__DerivationKernel(       name = "JETM9Kernel",
                                                                SkimmingTools = JETM9SkimmingTools,
                                                                ThinningTools = thinningTools)


#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
JETM9SlimmingHelper = SlimmingHelper("JETM9SlimmingHelper")
JETM9SlimmingHelper.SmartCollections = ["AntiKt2LCTopoJets","AntiKt3LCTopoJets","AntiKt4LCTopoJets","AntiKt5LCTopoJets","AntiKt6LCTopoJets","AntiKt7LCTopoJets","AntiKt8LCTopoJets","PrimaryVertices"]
JETM9SlimmingHelper.AllVariables = ["TruthEvents","MuonSegments","Kt4LCTopoOriginEventShape","Kt4EMPFlowEventShape"]
JETM9SlimmingHelper.ExtraVariables = ["TruthVertices.z"]

# Trigger content
JETM9SlimmingHelper.IncludeJetTriggerContent = True

# Add the jet containers to the stream
SmartListJets = [
    "AntiKt2LCTopoJets",
    "AntiKt3LCTopoJets",
    "AntiKt5LCTopoJets",
    "AntiKt6LCTopoJets",
    "AntiKt7LCTopoJets",
    "AntiKt8LCTopoJets"
    ]
addJetOutputs(JETM9SlimmingHelper,["JETM9"],SmartListJets)
JETM9SlimmingHelper.AppendContentToStream(JETM9Stream)
JETM9Stream.RemoveItem("xAOD::TrigNavigation#*")
JETM9Stream.RemoveItem("xAOD::TrigNavigationAuxInfo#*")
