#********************************************************************
# EXOT2.py 
# reductionConf flag EXOT2 in Reco_tf.py   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *

from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *

#=======================================
# SKIMMING TOOL   
#=======================================
beamEnergy = jobproperties.Beam.energy()
expression = ''
if (beamEnergy < 4.1e+06):
    expression = '(EventInfo.eventTypeBitmask==1) || (EF_j145_a4tchad_ht700_L2FS || EF_j280_a4tchad || EF_j220_a4tchad || EF_j180_a4tchad || EF_j145_a4tchad || EF_j110_a4tchad || EF_j80_a4tchad || EF_j55_a4tchad || EF_j35_a4tchad || EF_j25_a4tchad || EF_j15_a4tchad || EF_j220_a10tcem_delayed)'
if (beamEnergy > 6.0e+06):
    expression = '(EventInfo.eventTypeBitmask==1) || (HLT_j60 || HLT_j110 || HLT_j150 || HLT_j200 || HLT_j200_PS || HLT_j260 || HLT_j330 || HLT_j400)'
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EXOT2SkimmingTool = DerivationFramework__xAODStringSkimmingTool(    name = "EXOT2SkimmingTool",
                                                                        expression = expression)
ToolSvc += EXOT2SkimmingTool
print EXOT2SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(name = "EXOT2Kernel",
								       SkimmingTools = [EXOT2SkimmingTool])

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_EXOT2Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EXOT2Stream )
EXOT2Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT2Stream.AcceptAlgs(["EXOT2Kernel"])

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkExotics.EXOT2ContentList import *
EXOT2SlimmingHelper = SlimmingHelper("EXOT2SlimmingHelper")
EXOT2SlimmingHelper.SmartCollections = EXOT2SmartContent
EXOT2SlimmingHelper.AllVariables = EXOT2AllVariablesContent
EXOT2SlimmingHelper.UserContent = EXOT2Content
EXOT2SlimmingHelper.AppendContentToStream(EXOT2Stream)

