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

# contentManager.thinningHelper.AppendToStream( JETM10Stream )
contentManager = METTriggerDerivationContentManager("JETM10", JETM10Stream, trackThreshold=1)
for tool in contentManager.thinningTools:
  ToolSvc += tool

#======================================================================================================================
# CREATE PRIVATE SEQUENCE
#======================================================================================================================
jetm10Seq = CfgMgr.AthSequencer("jetm10Seq")
DerivationFrameworkJob += jetm10Seq

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
jetm10Seq += CfgMgr.DerivationFramework__DerivationKernel('JETM10Kernel',
                                                          SkimmingTools = [JETM10SkimmingTool],
                                                          ThinningTools = contentManager.thinningTools)
contentManager.slimmingHelper.AllVariables.append("HLT_xAOD__MuonContainer_MuonEFInfo")
contentManager.slimmingHelper.AllVariables.append("CaloCalTopoClusters")
contentManager.slimmingHelper.AppendContentToStream(JETM10Stream)
