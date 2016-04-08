
#********************************************************************
# EXOT1.py 
# reductionConf flag EXOT1 in Reco_tf.py   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *

from DerivationFrameworkCore.WeightMetadata import *

exot1Seq  = CfgMgr.AthSequencer("EXOT1Sequence")

#====================================================================
# SKIMMING TOOL 
#====================================================================

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
skimmingTools = []

beamEnergy = jobproperties.Beam.energy()
expression = ''
if (beamEnergy < 4.1e+06):
    trigger = '( (EventInfo.eventTypeBitmask==1) || (EF_j170_a4tchad_ht700 || EF_j15_a4tchad || EF_j25_a4tchad || EF_j35_a4tchad || EF_j45_a4tchad || EF_j55_a4tchad || EF_j80_a4tchad || EF_j110_a4tchad || EF_j145_a4tchad || EF_j180_a4tchad || EF_j220_a4tchad || EF_j280_a4tchad || EF_j360_a4tchad || EF_j460_a4tchad) )'
if (beamEnergy > 6.0e+06):
    #trigger = '((EventInfo.eventTypeBitmask==1) || HLT_ht1000_L1J100 || HLT_ht400 || HLT_ht550 || HLT_ht700 || HLT_ht850 || HLT_ht1000 || HLT_j260 || HLT_j330 || HLT_j400)'
    #trigger = '((EventInfo.eventTypeBitmask==1) || HLT_ht1000_L1J100 || HLT_ht400 || HLT_ht550 || HLT_ht700 || HLT_ht850 || HLT_ht1000 || HLT_j260 || HLT_j330 || HLT_j400 || HLT_j15 || HLT_j25 || HLT_j35 || HLT_j55 || HLT_j60 || HLT_j85 || HLT_j100 || HLT_j110 || HLT_j150 || HLT_j175 || HLT_j200 || HLT_j300 || HLT_j320 || HLT_j360 || HLT_j380 || HLT_j400 || HLT_j420 || HLT_j440 || HLT_j460 || HLT_noalg_L1J50)'
    trigger = '(EventInfo.eventTypeBitmask==1)'
    EXOT1TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(name = "EXOT1TriggerSkimmingTool",
                                                                       TriggerListAND = [],
                                                                       TriggerListOR  = [
                                                                                         "HLT_ht1000_L1J100",
                                                                                         "HLT_ht850_L1J75",
                                                                                         "HLT_ht700_L1J75",
                                                                                         "HLT_ht850_L1J100",
                                                                                         "HLT_ht700_L1J100",
                                                                                         "HLT_ht400",
                                                                                         "HLT_ht400_L1HT150-J20.ETA30",
                                                                                         "HLT_ht550",
                                                                                         "HLT_ht550_L1HT150-J20.ETA30",
                                                                                         "HLT_ht700",
                                                                                         "HLT_ht700_L1HT190-J15.ETA20",
                                                                                         "HLT_ht850",
                                                                                         "HLT_ht850_L1HT190-J15.ETA20",
                                                                                         "HLT_ht1000",
                                                                                         "HLT_ht1000_L1HT190-J15.ETA20",
                                                                                         "HLT_j200",
                                                                                         "HLT_j175",
                                                                                         "HLT_j150",
                                                                                         "HLT_7j45",
                                                                                         "HLT_6j45",
                                                                                         "HLT_5j45",
                                                                                         "HLT_4j45"
                                                                                        ])
    ToolSvc += EXOT1TriggerSkimmingTool
    skimmingTools.append(EXOT1TriggerSkimmingTool)

expression = trigger

thinningTools=[]

from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__JetCaloClusterThinning
EXOT1Ak4CCThinningTool_LC = DerivationFramework__JetCaloClusterThinning(name                   = "EXOT1Ak4CCThinningTool_LC",
                                                                        ThinningService        = "EXOT1ThinningSvc",
                                                                        SGKey                  = "AntiKt4LCTopoJets",
                                                                        TopoClCollectionSGKey  = "CaloCalTopoClusters",
                                                                        SelectionString        = "AntiKt4LCTopoJets.pt > 40*GeV",
                                                                        ConeSize               = 0)
ToolSvc += EXOT1Ak4CCThinningTool_LC
thinningTools.append(EXOT1Ak4CCThinningTool_LC)

from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__JetCaloClusterThinning
EXOT1Ak4CCThinningTool_EM = DerivationFramework__JetCaloClusterThinning(name                   = "EXOT1Ak4CCThinningTool_EM",
                                                                     ThinningService        = "EXOT1ThinningSvc",
                                                                     SGKey                  = "AntiKt4EMTopoJets",
                                                                     TopoClCollectionSGKey  = "CaloCalTopoClusters",
                                                                     SelectionString        = "AntiKt4EMTopoJets.pt > 40*GeV",
                                                                     ConeSize               = 0)
ToolSvc += EXOT1Ak4CCThinningTool_EM
thinningTools.append(EXOT1Ak4CCThinningTool_EM)


from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EXOT1SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "EXOT1SkimmingTool1",
                                                                 expression = expression)
ToolSvc += EXOT1SkimmingTool
skimmingTools.append(EXOT1SkimmingTool)

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
EXOT1ORSkimmingTool = DerivationFramework__FilterCombinationOR(name = "EXOT1ORSkimmingTool", FilterList = skimmingTools )
ToolSvc += EXOT1ORSkimmingTool
print EXOT1SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += exot1Seq
exot1Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT1Kernel_skim", SkimmingTools = [EXOT1ORSkimmingTool])
exot1Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT1Kernel", ThinningTools = thinningTools)


#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_EXOT1Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EXOT1Stream )
EXOT1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT1Stream.AcceptAlgs(["EXOT1Kernel"])

# Thinning
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="EXOT1ThinningSvc", outStreams=[evtStream] )


#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
# Currently using same list as EXOT1
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkExotics.EXOT1ContentList import *
EXOT1SlimmingHelper = SlimmingHelper("EXOT1SlimmingHelper")
EXOT1SlimmingHelper.StaticContent = EXOT1Content
EXOT1SlimmingHelper.SmartCollections = EXOT1SmartContent
EXOT1SlimmingHelper.AllVariables = EXOT1AllVariablesContent
EXOT1SlimmingHelper.AppendContentToStream(EXOT1Stream)

