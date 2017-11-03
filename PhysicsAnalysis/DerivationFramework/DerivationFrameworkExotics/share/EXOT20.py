#====================================================================
# EXOT20.py
# reductionConf flag EXOT20 in Reco_tf.py   
#====================================================================
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkCore.WeightMetadata import *

#====================================================================
# SET UP STREAM
#====================================================================
streamName = derivationFlags.WriteDAOD_EXOT20Stream.StreamName
fileName = buildFileName( derivationFlags.WriteDAOD_EXOT20Stream )
EXOT20Stream = MSMgr.NewPoolRootStream(streamName, fileName )
EXOT20Stream.AcceptAlgs(["EXOT20Kernel"])

# Init
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
EXOT20ThinningHelper = ThinningHelper( "EXOT20ThinningHelper" )
thinningTools = []
AugmentationTools = []

exot20Seq = CfgMgr.AthSequencer("EXOT20Sequence")
DerivationFrameworkJob += exot20Seq

EXOT20ThinningHelper.AppendToStream( EXOT20Stream )

#====================================================================
# THINNING TOOLS
#====================================================================

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
EXOT0MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "EXOT20MuonTPThinningTool",
                                                                         ThinningService         = EXOT20ThinningHelper.ThinningSvc(),
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                         ConeSize                =  0.4)
ToolSvc += EXOT20MuonTPThinningTool
thinningTools.append(EXOT20MuonTPThinningTool)

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
                                                    "HLT_mu20_msonly_mu15noL1_msonly_nscan05_noComb",
                                                    "HLT_xe90_pufit_L1XE50",
						    "HLT_xe100_pufit_L1XE50",
						    "HLT_xe100_pufit_L1XE55",
						    "HLT_xe110_pufit_L1XE55"]
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
exot20Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT20Kernel", SkimmingTools = [EXOT20SkimmingTool])
exot20Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT20KernelThin", ThinningTools = thinningTools)

# Add sumOfWeights metadata for LHE3 multiweights
from DerivationFrameworkCore.LHE3WeightMetadata import *

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
