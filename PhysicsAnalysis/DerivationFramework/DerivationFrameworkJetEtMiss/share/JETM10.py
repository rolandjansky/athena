#====================================================================
# JETM10.py 
# reductionConf flag JETM10 in Reco_tf.py   
#====================================================================
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkJetEtMiss.METTriggerDerivationContent import METTriggerDerivationContentManager

#======================================================================================================================
# SKIMMING TOOL
#======================================================================================================================
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
JETM10SkimmingTool = DerivationFramework__TriggerSkimmingTool(  name                   = "JETM10SkimmingTool",
                                                                TriggerListOR          = ["HLT_noalg_L1XE.*"] )
ToolSvc += JETM10SkimmingTool


#======================================================================================================================
# SET UP STREAM
#======================================================================================================================
streamName = derivationFlags.WriteDAOD_JETM10Stream.StreamName
fileName = buildFileName( derivationFlags.WriteDAOD_JETM10Stream )
JETM10Stream = MSMgr.NewPoolRootStream( streamName, fileName )
JETM10Stream.AcceptAlgs(['JETM10Kernel'])

content_manager = METTriggerDerivationContentManager.make_loose_manager(
        "JETM10", JETM10Stream)

#======================================================================================================================
# CREATE PRIVATE SEQUENCE
#======================================================================================================================
jetm10Seq = CfgMgr.AthSequencer("jetm10Seq")
DerivationFrameworkJob += jetm10Seq

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
jetm10Seq += content_manager.make_kernel(JETM10SkimmingTool)
content_manager.slimming_helper.AppendContentToStream(JETM10Stream)
