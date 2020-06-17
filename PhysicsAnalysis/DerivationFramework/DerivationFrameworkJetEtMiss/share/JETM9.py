#====================================================================
# JETM9.py 
# reductionConf flag JETM9 in Reco_tf.py   
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
#from DerivationFrameworkJetEtMiss.METCommon import *

#====================================================================
# SKIMMING TOOL 
#====================================================================
from DerivationFrameworkJetEtMiss import TriggerLists
triggers = TriggerLists.jetTrig()

# NOTE: need to be able to OR isSimulated as an OR with the trigger
#orstr =' || '
#trigger = '('+orstr.join(triggers)+')'
expression = 'EventInfo.eventTypeBitmask==1'


from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
JETM9TrigSkimmingTool = DerivationFramework__TriggerSkimmingTool(   name                    = "JETM9TrigSkimmingTool1",
                                                                TriggerListOR          = triggers )
ToolSvc += JETM9TrigSkimmingTool

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
JETM9OfflineSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "JETM9OfflineSkimmingTool1",
                                                                    expression = expression)
ToolSvc += JETM9OfflineSkimmingTool

# OR of the above two selections
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
JETM9ORTool = DerivationFramework__FilterCombinationOR(name="JETM9ORTool", FilterList=[JETM9TrigSkimmingTool,JETM9OfflineSkimmingTool] )
ToolSvc+=JETM9ORTool

#=======================================
# CREATE PRIVATE SEQUENCE
#=======================================

jetm9Seq = CfgMgr.AthSequencer("JETM9Sequence")
DerivationFrameworkJob += jetm9Seq
#jetm9Seq = DerivationFrameworkJob


# Truth particle thinning
thinningTools = []
from AthenaCommon.GlobalFlags import globalflags
if DerivationFrameworkIsMonteCarlo:
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
    JETM9TruthThinning = DerivationFramework__MenuTruthThinning(name                  = "JETM9TruthThinning",
                                                                StreamName                 = streamName,
                                                                WriteStatus3               = True,
                                                                PreserveAncestors          = True,
                                                                WriteFirstN                = 10)
    # from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
    # JETM9TruthParticleThinning = DerivationFramework__GenericTruthThinning(name                    = "JETM9TruthThinning",
    #                                                                        StreamName                 = streamName,
    #                                                                        ParticlesKey            = "TruthParticles",  
    #                                                                        ParticleSelectionString = "")
    ToolSvc += JETM9TruthThinning
    thinningTools.append(JETM9TruthThinning)

#====================================================================
# Special jets
#====================================================================

#=======================================
# RESTORE AOD-REDUCED JET COLLECTIONS
#=======================================
reducedJetList = ["AntiKt2PV0TrackJets",
                  "AntiKt4PV0TrackJets",
                  "AntiKt4TruthJets",
                  "AntiKt4TruthWZJets"]
replaceAODReducedJets(reducedJetList,jetm9Seq,"JETM9")

OutputJets["JETM9"] = ["AntiKt4EMTopoJets","AntiKt4LCTopoJets",
                       "AntiKt4TruthJets","AntiKt4TruthWZJets","AntiKt2PV0TrackJets"]

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
jetm9Seq += CfgMgr.DerivationFramework__DerivationKernel(       name = "JETM9Kernel",
                                                                SkimmingTools = [JETM9ORTool],
                                                                ThinningTools = thinningTools)

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
# Jets for R-scan 
#====================================================================
for radius in [0.2, 0.3, 0.5, 0.6, 0.7, 0.8]:
    if jetFlags.useTruth:
        addRscanJets("AntiKt",radius,"Truth",jetm9Seq,"JETM9")
        addRscanJets("AntiKt",radius,"TruthWZ",jetm9Seq,"JETM9")
    addRscanJets("AntiKt",radius,"LCTopo",jetm9Seq,"JETM9")



#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
JETM9SlimmingHelper = SlimmingHelper("JETM9SlimmingHelper")
JETM9SlimmingHelper.SmartCollections = ["AntiKt4EMTopoJets","AntiKt2LCTopoJets","AntiKt3LCTopoJets","AntiKt4LCTopoJets","AntiKt5LCTopoJets","AntiKt6LCTopoJets","AntiKt7LCTopoJets","AntiKt8LCTopoJets","PrimaryVertices"]
JETM9SlimmingHelper.AllVariables = ["TruthEvents","MuonSegments","Kt4EMTopoOriginEventShape","Kt4LCTopoOriginEventShape","Kt4EMPFlowEventShape"]
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
