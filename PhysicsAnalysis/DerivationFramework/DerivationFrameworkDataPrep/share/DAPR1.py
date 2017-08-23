#====================================================================
# DAPR1.py 
# reductionConf flag DAPR1 in Reco_tf.py   
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
#from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
#from DerivationFrameworkJetEtMiss.METCommon import *

#====================================================================
# SKIMMING TOOL 
#====================================================================
# NOTE: need to be able to OR isSimulated as an OR with the trigger
trigger = '(EF_j360_a4tchad || HLT_j360_a4tchad || L1_BCM_AC_CA_BGRP0 || L1_BCM_Wide_EMPTY || L1_BCM_Wide_UNPAIRED_ISO || L1_BCM_Wide_UNPAIRED_NONISO || L1_BCM_Wide_CALIB || L1_BCM_Wide_ABORTGAPNOTCALIB || L1_BCM_AC_UNPAIRED_ISO || L1_BCM_CA_UNPAIRED_ISO || L1_BCM_AC_UNPAIRED_NONISO || L1_BCM_CA_UNPAIRED_NONISO || L1_BCM_AC_ABORTGAPNOTCALIB || L1_BCM_CA_ABORTGAPNOTCALIB || L1_BCM_AC_CALIB || L1_BCM_CA_CALIB || L1_J12_UNPAIRED_ISO || L1_J12_UNPAIRED_NONISO || L1_J12_ABORTGAPNOTCALIB || L1_J12 || L1_J12_EMPTY)'
expression = trigger+' || (EventInfo.eventTypeBitmask==1)'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
DAPR1SkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "DAPR1SkimmingTool1",
                                                                    expression = expression)
ToolSvc += DAPR1SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("DAPR1Kernel",
                                                                       SkimmingTools = [DAPR1SkimmingTool])
                                                                        

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_DAPR1Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_DAPR1Stream )
DAPR1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
DAPR1Stream.AcceptAlgs(["DAPR1Kernel"])

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
DAPR1SlimmingHelper = SlimmingHelper("DAPR1SlimmingHelper")
DAPR1SlimmingHelper.SmartCollections = ["Muons", "PrimaryVertices"]
DAPR1SlimmingHelper.AllVariables = ["AntiKt4LCTopoJets",
                                   "AntiKt4EMTopoJets",
                                   "CaloCalTopoClusters",
                                   "MuonSegments" ]
DAPR1SlimmingHelper.IncludeMuonTriggerContent = True
DAPR1SlimmingHelper.IncludeEGammaTriggerContent = True
DAPR1SlimmingHelper.IncludeBPhysTriggerContent = True
DAPR1SlimmingHelper.IncludeJetTauEtMissTriggerContent = True

DAPR1SlimmingHelper.AppendContentToStream(DAPR1Stream)
