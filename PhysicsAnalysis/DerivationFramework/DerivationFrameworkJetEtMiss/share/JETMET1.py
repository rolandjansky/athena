#====================================================================
# JETMET1.py 
# reductionConf flag JETMET1 in Reco_tf.py   
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import *

#====================================================================
# SKIMMING TOOL 
#====================================================================
# NOTE: need to be able to OR isSimulated as an OR with the trigger
expression = '(EF_j15_a4tchad || EF_j25_a4tchad || EF_j35_a4tchad || EF_j45_a4tchad || EF_j55_a4tchad || EF_j80_a4tchad || EF_j110_a4tchad || EF_j145_a4tchad || EF_j180_a4tchad || EF_j220_a4tchad || EF_j280_a4tchad || EF_j360_a4tchad || EF_j460_a4tchad || EF_fj15_a4tchad || EF_fj25_a4tchad || EF_fj35_a4tchad || EF_fj80_a4tchad || EF_fj110_a4tchad || EF_fj145_a4tchad || EF_fj180_a4tchad || EF_fj220_a4tchad || EF_j45_a4tchad_L2FS_L1J15 || EF_fj45_a4tchad_L2FS || EF_j55_a4tchad_L2FS )'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
JETMET1SkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "JETMET1SkimmingTool1",
                                                                    expression = expression)
ToolSvc += JETMET1SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("JETMET1Kernel" , SkimmingTools = [JETMET1SkimmingTool])

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_JETMET1Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_JETMET1Stream )
JETMET1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
JETMET1Stream.AcceptAlgs(["JETMET1Kernel"])

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkJetEtMiss.JETMET1ContentList import JETMET1Content
JETMET1SlimmingHelper = SlimmingHelper("JETMET1SlimmingHelper")
JETMET1SlimmingHelper.UserContent = JETMET1Content
JETMET1SlimmingHelper.AppendContentToStream(JETMET1Stream)
