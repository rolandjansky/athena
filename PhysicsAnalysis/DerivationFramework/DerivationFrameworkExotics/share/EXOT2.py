#********************************************************************
# EXOT2.py 
# reductionConf flag EXOT2 in Reco_tf.py   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkCore.WeightMetadata import *

exot2Seq = CfgMgr.AthSequencer("EXOT2Sequence")

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_EXOT2Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EXOT2Stream )
EXOT2Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT2Stream.AcceptAlgs(["EXOT2Kernel"])

#=====================
# TRIGGER NAV THINNING
#=====================
#Establish the thinning helper
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
EXOT2ThinningHelper = ThinningHelper( "EXOT2ThinningHelper" )
#trigger navigation content
EXOT2ThinningHelper.TriggerChains = 'HLT_j.*|HLT_noalg_L1J.*'
EXOT2ThinningHelper.AppendToStream( EXOT2Stream )


#=======================================
# SKIMMING TOOL   
#=======================================
beamEnergy = jobproperties.Beam.energy()
expression = ''
if (beamEnergy < 4.1e+06):
    expression = '(EventInfo.eventTypeBitmask==1) || (EF_j145_a4tchad_ht700_L2FS || EF_j280_a4tchad || EF_j220_a4tchad || EF_j180_a4tchad || EF_j145_a4tchad || EF_j110_a4tchad || EF_j80_a4tchad || EF_j55_a4tchad || EF_j35_a4tchad || EF_j25_a4tchad || EF_j15_a4tchad || EF_j220_a10tcem_delayed)'
if (beamEnergy > 6.0e+06):
    expression = '(EventInfo.eventTypeBitmask==1) || (HLT_j15 || HLT_j25 || HLT_j35 || HLT_j55 || HLT_j60 || HLT_j85 || HLT_j100 || HLT_j110 || HLT_j150 || HLT_j175 || HLT_j200 || HLT_j260 || HLT_j300 || HLT_j320 || HLT_j360 || HLT_j380 || HLT_j400 || HLT_j420 || HLT_j440 || HLT_j460 || HLT_noalg_L1J400)'
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EXOT2SkimmingTool = DerivationFramework__xAODStringSkimmingTool(    name = "EXOT2SkimmingTool",
                                                                        expression = expression)
ToolSvc += EXOT2SkimmingTool
print EXOT2SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += exot2Seq
exot2Seq += CfgMgr.DerivationFramework__DerivationKernel(name = "EXOT2Kernel",
								       SkimmingTools = [EXOT2SkimmingTool])

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
EXOT2SlimmingHelper = SlimmingHelper("EXOT2SlimmingHelper")
EXOT2SlimmingHelper.SmartCollections = ["PrimaryVertices"]
EXOT2SlimmingHelper.AllVariables = ["TruthEvents",
                                    "AntiKt4TruthJets",
                                    "AntiKt4EMTopoJets",
                                    "AntiKt4LCTopoJets",
                                    "BTagging_AntiKt4EMTopo",
                                    "BTagging_AntiKt4LCTopo",
                                    "MuonSegments",
                                    "LVL1JetRoIs",
                                    "HLT_xAOD__JetContainer_a4tcemsubFS",
                                    "HLT_xAOD__JetContainer_a4tcemsubjesFS"]
EXOT2SlimmingHelper.IncludeJetTriggerContent = True
addMETOutputs(EXOT2SlimmingHelper)
EXOT2SlimmingHelper.AppendContentToStream(EXOT2Stream)
