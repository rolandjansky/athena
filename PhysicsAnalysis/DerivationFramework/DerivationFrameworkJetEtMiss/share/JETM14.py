#====================================================================
# JETM14.py 
# reductionConf flag JETM14 in Reco_tf.py   
#====================================================================
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkJetEtMiss import TriggerLists
from DerivationFrameworkEGamma.EGammaCommon import*
from DerivationFrameworkMuons.MuonsCommon import*
from DerivationFrameworkJetEtMiss.METTriggerDerivationContent import METTriggerDerivationContentManager

#======================================================================================================================
# SKIMMING TOOL
#======================================================================================================================
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
singleMuTriggers = TriggerLists.single_mu_Trig()
cutExpression = "(count(Muons.DFCommonMuonsPreselection && Muons.pt > (20*GeV) && abs(Muons.eta) < 2.47) ) >= 2"
JETM14StringSkimmingTool = DerivationFramework__xAODStringSkimmingTool(
    name       = "JETM14StringSkimmingTool",
    expression = cutExpression)
JETM14TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(
    name          = "JETM14TriggerSkimmingTool",
    TriggerListOR = singleMuTriggers)
ToolSvc += JETM14StringSkimmingTool
ToolSvc += JETM14TriggerSkimmingTool


#======================================================================================================================
# SET UP STREAM
#======================================================================================================================
streamName = derivationFlags.WriteDAOD_JETM14Stream.StreamName
fileName = buildFileName( derivationFlags.WriteDAOD_JETM14Stream )
JETM14Stream = MSMgr.NewPoolRootStream( streamName, fileName )
JETM14Stream.AcceptAlgs(['JETM14Kernel'])

contentManager = METTriggerDerivationContentManager("JETM14", JETM14Stream, trackThreshold=1)

for tool in contentManager.thinningTools:
  ToolSvc += tool

#======================================================================================================================
# CREATE PRIVATE SEQUENCE
#======================================================================================================================
jetm14Seq = CfgMgr.AthSequencer("jetm14Seq")
DerivationFrameworkJob += jetm14Seq

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
jetm14Seq += CfgMgr.DerivationFramework__DerivationKernel('JETM14Kernel',
                                                          SkimmingTools = [JETM14StringSkimmingTool, JETM14TriggerSkimmingTool],
                                                          ThinningTools = contentManager.thinningTools)

contentManager.slimmingHelper.AllVariables.append("HLT_xAOD__MuonContainer_MuonEFInfo")
contentManager.slimmingHelper.AllVariables.append("CaloCalTopoClusters")
contentManager.slimmingHelper.AppendContentToStream(JETM14Stream)
