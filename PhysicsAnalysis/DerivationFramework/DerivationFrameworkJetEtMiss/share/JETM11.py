#====================================================================
# JETM11.py 
# reductionConf flag JETM11 in Reco_tf.py   
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
# SET UP STREAM
#======================================================================================================================
streamName = derivationFlags.WriteDAOD_JETM11Stream.StreamName
fileName = buildFileName( derivationFlags.WriteDAOD_JETM11Stream )
JETM11Stream = MSMgr.NewPoolRootStream( streamName, fileName )
JETM11Stream.AcceptAlgs(['JETM11Kernel'])

#======================================================================================================================
# SKIMMING TOOL
#======================================================================================================================
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
singleElTriggers = TriggerLists.single_el_Trig()
singleMuTriggers = TriggerLists.single_mu_Trig()
cutExpression = "(count(Electrons.DFCommonElectronsLHLoose && Electrons.pt > (24 * GeV) && abs(Electrons.eta) < 2.47) + count(Muons.DFCommonMuonsPreselection && Muons.pt > (24*GeV) && abs(Muons.eta) < 2.47) ) >= 1"
JETM11StringSkimmingTool = DerivationFramework__xAODStringSkimmingTool(
    name       = "JETM11StringSkimmingTool",
    expression = cutExpression)
JETM11TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(
    name          = "JETM11TriggerSkimmingTool",
    TriggerListOR = singleElTriggers + singleMuTriggers)
ToolSvc += JETM11StringSkimmingTool
ToolSvc += JETM11TriggerSkimmingTool


content_manager = METTriggerDerivationContentManager.make_tight_manager(
        "JETM11", JETM11Stream)

#======================================================================================================================
# CREATE PRIVATE SEQUENCE
#======================================================================================================================
jetm11Seq = CfgMgr.AthSequencer("jetm11Seq")
DerivationFrameworkJob += jetm11Seq

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
jetm11Seq += content_manager.make_kernel(JETM11StringSkimmingTool, JETM11TriggerSkimmingTool)

content_manager.slimming_helper.AppendContentToStream(JETM11Stream)
