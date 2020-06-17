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
cutExpression = "(count(Muons.DFCommonMuonsPreselection && Muons.pt > (20*GeV) && abs(Muons.eta) < 2.7) ) >= 2"
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

content_manager = METTriggerDerivationContentManager.make_loose_manager(
        "JETM14", JETM14Stream)

#======================================================================================================================
# CREATE PRIVATE SEQUENCE
#======================================================================================================================
jetm14Seq = CfgMgr.AthSequencer("jetm14Seq")
DerivationFrameworkJob += jetm14Seq

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
jetm14Seq += content_manager.make_kernel(JETM14StringSkimmingTool, JETM14TriggerSkimmingTool)
content_manager.slimming_helper.AppendContentToStream(JETM14Stream)
