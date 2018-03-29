# **************************************************************************************************************************
# TAUP4.py 
# reductionConf flag TAUP4 in Reco_tf.py   
# **************************************************************************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *

# ==========================================================================================================================
# Set up stream
# ==========================================================================================================================
streamName                                       = derivationFlags.WriteDAOD_TAUP4Stream.StreamName
fileName                                         = buildFileName( derivationFlags.WriteDAOD_TAUP4Stream )
TAUP4Stream                                      = MSMgr.NewPoolRootStream( streamName, fileName )
TAUP4Stream.AcceptAlgs(["TAUP4Kernel"])

from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream                                        = MSMgr.GetStream( streamName )
evtStream                                        = augStream.GetEventStream()
svcMgr                                          += createThinningSvc(
                                                     svcName                   = "TAUP4ThinningSvc",
                                                     outStreams                = [evtStream])


# ==========================================================================================================================
# Trigger skimming tool
# ==========================================================================================================================
trigger_list                                     = ['HLT_j10' , 'HLT_j15' , 'HLT_j20' ,
                                                    'HLT_j25' , 'HLT_j30' , 'HLT_j35' ,
                                                    'HLT_j40' , 'HLT_j45' , 'HLT_j55' ,
                                                    'HLT_j60' , 'HLT_j60_L1RD0_FILLED', 'HLT_j85',
                                                    'HLT_j100', 'HLT_j110', 'HLT_j150',
                                                    'HLT_j175', 'HLT_j200', 'HLT_j260',
                                                    'HLT_j300', 'HLT_j320', 'HLT_j360',
                                                    'HLT_j380', 'HLT_j400', 'HLT_j420' , 'HLT_j440' ,'HLT_j460']

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
TAUP4TriggerSkimmingTool                         = DerivationFramework__TriggerSkimmingTool(
                                                     name                      = "TAUP4TriggerSkimmingTool",
                                                     TriggerListOR             = trigger_list)

ToolSvc                                         += TAUP4TriggerSkimmingTool

# ==========================================================================================================================
# String skimming tool
# ==========================================================================================================================
tau_requirement                                  = "( count( (TauJets.pt > 15.0*GeV || TauJets.ptFinalCalib > 15.0*GeV ) && (abs(TauJets.eta) < 2.6) ) >= 1 )"

expression                                       = tau_requirement
# print expression

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
TAUP4StringSkimmingTool                          = DerivationFramework__xAODStringSkimmingTool(
                                                     name                      = "TAUP4StringSkimmingTool",
                                                     expression                = expression)

ToolSvc                                         += TAUP4StringSkimmingTool
#print TAUP4StringSkimmingTool
# truth thinning here:
import DerivationFrameworkTau.TAUPThinningHelper 
TAUP4TruthThinningTools = DerivationFrameworkTau.TAUPThinningHelper.setup("TAUP4",
                                                                          "TAUP4ThinningSvc",
                                                                          ToolSvc)

# ==========================================================================================================================
# Kernel algorithm
# ==========================================================================================================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob                          += CfgMgr.DerivationFramework__DerivationKernel(
                                                     "TAUP4Kernel",
                                                     SkimmingTools             = [TAUP4TriggerSkimmingTool,
                                                                                  TAUP4StringSkimmingTool],
                                                     ThinningTools             = TAUP4TruthThinningTools,
                                                     )

# ==========================================================================================================================
# Add the containers to the output stream (slimming done here)
# ==========================================================================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkTau.TAUPExtraContent import *

TAUP4SlimmingHelper                              = SlimmingHelper("TAUP4SlimmingHelper")
TAUP4SlimmingHelper.SmartCollections             = ["Electrons",
                                                    "Photons",
                                                    "Muons",
                                                    # "TauJets",
                                                    "MET_Reference_AntiKt4EMTopo",
                                                    "AntiKt4EMTopoJets",
                                                    "BTagging_AntiKt4EMTopo",
                                                    "InDetTrackParticles",
                                                    "PrimaryVertices"]

#TAUP4SlimmingHelper.StaticContent                = ["CaloCellContainer#AODCellContainer"]
#TAUP4SlimmingHelper.ExtraVariables               = ["PhotonCollection.weta2.f1.phi.weta1.emaxs1"]
TAUP4SlimmingHelper.AllVariables                 = ExtraContainersTAUP4
TAUP4SlimmingHelper.IncludeTauTriggerContent     = True
TAUP4SlimmingHelper.AppendContentToStream(TAUP4Stream)
