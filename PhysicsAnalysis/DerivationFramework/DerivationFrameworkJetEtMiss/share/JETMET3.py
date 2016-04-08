#====================================================================
# JETMET3.py 
# reductionConf flag JETMET3 in Reco_tf.py   
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkEGamma.EGammaCommon import *
#====================================================================
# SKIMMING TOOL 
#====================================================================
# NOTE: muon and electron charges requested...
electronSelection = ' ((EF_2e12Tvh_loose1 == 1) && ( count((ElectronCollection.pt > 20*GeV) && (DFCommonElectronsMedium == 1)) >= 1 )  )'
muonSelection = ' (( EF_mu18_tight_mu8_EFFS == 1 ) &&  ( count((Muons.pt > 20*GeV) && (Muons.muonType == 0) ))  ) '
expression = electronSelection + ' || ' + muonSelection

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
JETMET3SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "JETMET3SkimmingTool1",
                                                                    expression = expression)
ToolSvc += JETMET3SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("JETMET3Kernel",SkimmingTools = [JETMET3SkimmingTool])

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_JETMET3Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_JETMET3Stream )
JETMET3Stream = MSMgr.NewPoolRootStream( streamName, fileName )
JETMET3Stream.AcceptAlgs(["JETMET3Kernel"])

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkJetEtMiss.JETMET3ContentList import JETMET3Content
JETMET3SlimmingHelper = SlimmingHelper("JETMET3SlimmingHelper")
JETMET3SlimmingHelper.UserContent = JETMET3Content
JETMET3SlimmingHelper.AppendContentToStream(JETMET3Stream)
