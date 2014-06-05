#********************************************************************
# SUSY1.py 
# reductionConf flag SUSY1 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *

#====================================================================
# SKIMMING TOOL 
#====================================================================
# NOTE: need to have a way to OR isSimulation with the trigger
#mcSelection = '(isSimulation == 1)' 
muonSelection = 'EF_mu24i_tight'
egammaSelection = 'EF_e24vhi_medium1 || EF_e60_medium1'
jetTauEtMissSelection = 'EF_xe80T_tclcw_loose || EF_xe80_tclcw_loose || EF_5j55_a4tchad_L2FS || EF_6j45_a4tclcw_L2FS_5L2j15 || EF_4j45_a4tchad_L2FS'
expression = muonSelection+' || '+egammaSelection+' || '+jetTauEtMissSelection

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
SUSY1SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "SUSY1SkimmingTool",
                                                                expression = expression)

ToolSvc += SUSY1SkimmingTool



#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("SUSY1Kernel",
									SkimmingTools = [SUSY1SkimmingTool]
                                                                      )
#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_SUSY1Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_SUSY1Stream )
SUSY1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
SUSY1Stream.AcceptAlgs(["SUSY1Kernel"])

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkSUSY.SUSY1ContentList import SUSY1Content
SUSY1SlimmingHelper = SlimmingHelper("SUSY1SlimmingHelper")
SUSY1SlimmingHelper.UserContent = SUSY1Content
SUSY1SlimmingHelper.AppendContentToStream(SUSY1Stream)


