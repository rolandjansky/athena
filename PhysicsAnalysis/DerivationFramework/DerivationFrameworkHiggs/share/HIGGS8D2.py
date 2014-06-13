#********************************************************************
# HIGGS8D2.py 
# reductionConf flag HIGGS8D2 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *

#====================================================================
# SKIMMING TOOL 
#====================================================================

expression = 'count(Muons.pt >= 10*GeV) + count (ElectronCollection.pt >= 10*GeV) >= 1'
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
HIGGS8D2SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "HIGGS8D2SkimmingTool1",
                                                                    expression = expression)
ToolSvc += HIGGS8D2SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("HIGGS8D2Kernel",
									SkimmingTools = [HIGGS8D2SkimmingTool]
                                                                      )

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_HIGGS8D2Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HIGGS8D2Stream )
HIGGS8D2Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HIGGS8D2Stream.AcceptAlgs(["HIGGS8D2Kernel"])

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkHiggs.HIGGS8D2ContentList import HIGGS8D2Content
HIGGS8D2SlimmingHelper = SlimmingHelper("HIGGS8D2SlimmingHelper")
HIGGS8D2SlimmingHelper.UserContent = HIGGS8D2Content
HIGGS8D2SlimmingHelper.AppendContentToStream(HIGGS8D2Stream)
