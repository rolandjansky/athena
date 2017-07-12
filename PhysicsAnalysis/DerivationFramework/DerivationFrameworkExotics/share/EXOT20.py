#====================================================================
# EXOT20.py
# reductionConf flag EXOT20 in Reco_tf.py   
#====================================================================
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkCore.WeightMetadata import *

exot20Seq = CfgMgr.AthSequencer("EXOT20Sequence")

#====================================================================
# SKIMMING TOOLS 
#====================================================================

from DerivationFrameworkExotics.DerivationFrameworkExoticsConf import DerivationFramework__SkimmingToolEXOT20
EXOT20SkimmingTool = DerivationFramework__SkimmingToolEXOT20(   
                                                name = "EXOT20SkimmingTool",
                                                MSTPContainer = "MuonSpectrometerTrackParticles",
                                                Triggers = [
                                                    "HLT_mu60_0eta105_msonly",
                                                    "HLT_3mu6_msonly",
                                                    "HLT_mu20_msonly_mu6noL1_msonly_nscan05",
                                                    "HLT_mu20_msonly_mu6noL1_msonly_nscan05_noComb",
                                                    "HLT_mu20_msonly_mu10noL1_msonly_nscan05_noComb",
                                                    "HLT_mu20_msonly_mu15noL1_msonly_nscan05_noComb",
                                                    "HLT_xe80_mht_L1XE50",
                                                    "HLT_xe90_mht_L1XE50",
                                                    "HLT_xe100_mht_L1XE50",
                                                    "HLT_xe110_mht_L1XE50"],
                                                MinPtMSTP = 3.0,
                                                MinNumMSTP = 2,
                                                StartDirection = -1,
                                                ParticleType = 2)        
ToolSvc += EXOT20SkimmingTool

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

# The name of the kernel (LooseSkimKernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += exot20Seq
exot20Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT20Kernel", SkimmingTools = [EXOT20SkimmingTool])

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_EXOT20Stream.StreamName
fileName = buildFileName( derivationFlags.WriteDAOD_EXOT20Stream )
EXOT20Stream = MSMgr.NewPoolRootStream(streamName, fileName )
EXOT20Stream.AcceptAlgs(["EXOT20Kernel"])

#====================================================================
# CONTENT LIST  
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkExotics.EXOT20ContentList import *
EXOT20SlimmingHelper = SlimmingHelper("EXOT20SlimmingHelper")

EXOT20SlimmingHelper.AllVariables = EXOT20AllVariables
EXOT20SlimmingHelper.SmartCollections = EXOT20SmartCollections

EXOT20SlimmingHelper.IncludeEtMissTriggerContent = True
EXOT20SlimmingHelper.IncludeMuonTriggerContent = True
EXOT20SlimmingHelper.IncludeJetTauEtMissTriggerContent = True
EXOT20SlimmingHelper.AppendContentToStream(EXOT20Stream)
