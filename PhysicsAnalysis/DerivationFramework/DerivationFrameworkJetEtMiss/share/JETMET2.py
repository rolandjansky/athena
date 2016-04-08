#====================================================================
# JETMET2.py 
# reductionConf flag JETMET2 in Reco_tf.py   
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkEGamma.EGammaCommon import *
#====================================================================
# SKIMMING TOOL 
#====================================================================
electronSelection = ' ((EF_e24vhi_medium1 ||  EF_e60_medium1 ) && ( count((ElectronCollection.pt > 25*GeV) && (DFCommonElectronsMedium == 1)) >= 1 ) && ( count((ElectronCollection.pt > 10*GeV) && (DFCommonElectronsMedium == 1)) <= 1 )) '
muonSelection = ' (( EF_mu24i_tight ||  EF_mu36_tight ) &&  ( count((Muons.pt > 25*GeV) && (Muons.muonType==0)) >= 1 ) && ( count((Muons.pt > 10*GeV) && (Muons.muonType==0)) <= 1 ) ) '
expression = electronSelection + ' || ' + muonSelection

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
JETMET2SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "JETMET2SkimmingTool1",
                                                                    expression = expression)
ToolSvc += JETMET2SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("JETMET2Kernel", SkimmingTools = [JETMET2SkimmingTool])

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_JETMET2Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_JETMET2Stream )
JETMET2Stream = MSMgr.NewPoolRootStream( streamName, fileName )
JETMET2Stream.AcceptAlgs(["JETMET2Kernel"])

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkJetEtMiss.JETMET2ContentList import JETMET2Content
JETMET2SlimmingHelper = SlimmingHelper("JETMET2SlimmingHelper")
JETMET2SlimmingHelper.UserContent = JETMET2Content
JETMET2SlimmingHelper.AppendContentToStream(JETMET2Stream)
