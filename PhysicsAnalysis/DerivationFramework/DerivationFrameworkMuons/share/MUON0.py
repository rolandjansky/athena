#====================================================================
# MUON0.py for events passed muon triggers
# This requires the reductionConf flag MUON0 in Reco_tf.py   
#====================================================================

# Set up common services and job object. 
# This should appear in ALL derivation job options
# include("DerivationFrameworkCore/DerivationFrameworkMaster.py")
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkMuons.MuonsCommon import *
# from DerivationFrameworkJetEtMiss.METCommon import *
#====================================================================
# AUGMENTATION TOOLS 
#====================================================================

#====================================================================
# STRING BASED SKIMMING TOOL 
#====================================================================
MUON0_skimming_tools = []

### trigger seleciton
triggerList = ['HLT_.*mu.*', 'L1_.*MU.*', 'HLT_noalg_L1.*MU.*']
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
MUON0SkimmingTool0a = DerivationFramework__TriggerSkimmingTool(name = "MUON0SkimmingTool0a",
                                                              TriggerListOR = triggerList,
                                                              TriggerListAND = [])
ToolSvc += MUON0SkimmingTool0a

### muon selection
expression = 'count(Muons.pt>30*GeV)>0'
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
MUON0SkimmingTool0b = DerivationFramework__xAODStringSkimmingTool(name = "MUON0SkimmingTool0b",
                                                                 expression = expression)
ToolSvc += MUON0SkimmingTool0b

### OR combination
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
MUON0SkimmingTool0 = DerivationFramework__FilterCombinationOR(name="MUON0SkimmingTool0",
                                                              FilterList=[MUON0SkimmingTool0a, MUON0SkimmingTool0b])
ToolSvc += MUON0SkimmingTool0

### adding the combined tool
MUON0_skimming_tools.append(MUON0SkimmingTool0)
#====================================================================
# THINNING TOOL 
#====================================================================
# Tracks associated with Muons
MUON0_thinning_tools = []

## keep only tracks near muon
thinning_expression2 = "Muons.pt > 4*GeV"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
MUON0ThinningTool2 = DerivationFramework__MuonTrackParticleThinning(name                    = "MUON0ThinningTool2",
                                                                    ThinningService         = "MUON0ThinningSvc",
                                                                    MuonKey                 = "Muons",
                                                                    SelectionString         = thinning_expression2,
                                                                    ConeSize                = 0.4,
                                                                    ApplyAnd                = True,
                                                                    InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += MUON0ThinningTool2
MUON0_thinning_tools.append(MUON0ThinningTool2)

## keep tracks pt>2GeV
# thinning_expression3 = "InDetTrackParticles.pt > 2*GeV"
# from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
# MUON0ThinningTool3 = DerivationFramework__TrackParticleThinning(name                    = "MUON0ThinningTool3",
#                                                                 ThinningService         = "MUON0ThinningSvc",
#                                                                 SelectionString         = thinning_expression3,
#                                                                 ApplyAnd                = False,
#                                                                 InDetTrackParticlesKey  = "InDetTrackParticles")
# ToolSvc += MUON0ThinningTool3
# MUON0_thinning_tools.append(MUON0ThinningTool3)
#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("MUON0Kernel",
# 									AugmentationTools = [],
                                                                        ThinningTools = MUON0_thinning_tools,
                                                                        SkimmingTools = MUON0_skimming_tools
                                                                      )
#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_MUON0Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_MUON0Stream )
MUON0Stream = MSMgr.NewPoolRootStream( streamName, fileName )
# Only events that pass the filters listed below are written out.
MUON0Stream.AcceptAlgs(["MUON0Kernel"])

from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="MUON0ThinningSvc", outStreams=[evtStream] ) 
#====================================================================
# CONTENT LIST  
#====================================================================
from DerivationFrameworkMuons.StreamConfig import MuonsDxAODStreamConfigurer as conf
conf.Config(MUON0Stream, 'MUON0')
