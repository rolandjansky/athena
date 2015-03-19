#********************************************************************
# EXOT1.py 
# reductionConf flag EXOT1 in Reco_tf.py   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *


#====================================================================
# SKIMMING TOOL 
#====================================================================

beamEnergy = jobproperties.Beam.energy()
expression = ''
if (beamEnergy < 4.1e+06):
    expression = '( (EventInfo.eventTypeBitmask==1) || (EF_j170_a4tchad_ht700 || EF_j15_a4tchad || EF_j25_a4tchad || EF_j35_a4tchad || EF_j45_a4tchad || EF_j55_a4tchad || EF_j80_a4tchad || EF_j110_a4tchad || EF_j145_a4tchad || EF_j180_a4tchad || EF_j220_a4tchad || EF_j280_a4tchad || EF_j360_a4tchad || EF_j460_a4tchad) )'
if (beamEnergy > 6.0e+06):
    expression = '(HLT_ht1000_L1J100 || HLT_ht400 || HLT_ht550 || HLT_ht700 || HLT_ht850 || HLT_ht1000 || HLT_j260 || HLT_j330 || HLT_j400)'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EXOT1SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "EXOT1SkimmingTool1",
                                                                    expression = expression)
ToolSvc += EXOT1SkimmingTool
print EXOT1SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("EXOT1Kernel",
									SkimmingTools = [EXOT1SkimmingTool])

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_EXOT1Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EXOT1Stream )
EXOT1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT1Stream.AcceptAlgs(["EXOT1Kernel"])


#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
# Currently using same list as EXOT1
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkExotics.EXOT1ContentList import *
EXOT1SlimmingHelper = SlimmingHelper("EXOT1SlimmingHelper")
EXOT1SlimmingHelper.StaticContent = EXOT1Content
EXOT1SlimmingHelper.SmartCollections = EXOT1SmartContent
EXOT1SlimmingHelper.AllVariables = EXOT1AllVariablesContent
EXOT1SlimmingHelper.AppendContentToStream(EXOT1Stream)

