#====================================================================
# JETMET5.py 
# reductionConf flag JETMET5 in Reco_tf.py   
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import *

#====================================================================
# SKIMMING TOOL 
#====================================================================
expression = '( L1_ZB )'
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
JETMET5SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "JETMET5SkimmingTool1",
                                                                    expression = expression)
ToolSvc += JETMET5SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("JETMET5Kernel",SkimmingTools = [JETMET5SkimmingTool])

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_JETMET5Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_JETMET5Stream )
JETMET5Stream = MSMgr.NewPoolRootStream( streamName, fileName )
JETMET5Stream.AcceptAlgs(["JETMET5Kernel"])

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkJetEtMiss.JETMET5ContentList import JETMET5Content
JETMET5SlimmingHelper = SlimmingHelper("JETMET5SlimmingHelper")
JETMET5SlimmingHelper.UserContent = JETMET5Content
JETMET5SlimmingHelper.AppendContentToStream(JETMET5Stream)
