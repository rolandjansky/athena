#====================================================================
# JETMET4.py 
# reductionConf flag JETMET4 in Reco_tf.py   
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import *

#====================================================================
# SKIMMING TOOL 
#====================================================================
# NOTE: need to add isSimulation as OR with trigger
expression = '( EF_g20_loose || EF_g40_loose || EF_g60_loose || EF_g80_loose || EF_g100_loose || EF_g120_loose )'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
JETMET4SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "JETMET4SkimmingTool1",
                                                                    expression = expression)
ToolSvc += JETMET4SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("JETMET4Kernel",SkimmingTools = [JETMET4SkimmingTool])

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_JETMET4Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_JETMET4Stream )
JETMET4Stream = MSMgr.NewPoolRootStream( streamName, fileName )
JETMET4Stream.AcceptAlgs(["JETMET4Kernel"])

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkJetEtMiss.JETMET4ContentList import JETMET4Content
JETMET4SlimmingHelper = SlimmingHelper("JETMET4SlimmingHelper")
JETMET4SlimmingHelper.UserContent = JETMET4Content
JETMET4SlimmingHelper.AppendContentToStream(JETMET4Stream)
