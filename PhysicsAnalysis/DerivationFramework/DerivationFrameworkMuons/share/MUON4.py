#====================================================================
# MUON4.py keeps muon info and ID track info
# This requires the reductionConf flag MUON4 in Reco_tf.py   
#====================================================================
# Set up common services and job object. 
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkMuons.MuonsCommon import *
#====================================================================
# AUGMENTATION TOOLS 
#====================================================================
MUON4_aug_tools = []

### track to vertex tool
from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__TrackToVertexIPEstimator
MUON4IPETool = Trk__TrackToVertexIPEstimator(name = "MUON4IPETool")
ToolSvc += MUON4IPETool
print MUON4IPETool

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackToVertexWrapper
## ID tracks
MUON4TrackToVertexWrapper= DerivationFramework__TrackToVertexWrapper(name = "MUON4TrackToVertexWrapper",
                                                                     TrackToVertexIPEstimator = MUON4IPETool,
                                                                     DecorationPrefix = "MUON4",
                                                                     ContainerName = "InDetTrackParticles")
print MUON4TrackToVertexWrapper
ToolSvc += MUON4TrackToVertexWrapper
MUON4_aug_tools.append(MUON4TrackToVertexWrapper)

# ## combined tracks
# MUON4TrackToVertexWrapper2= DerivationFramework__TrackToVertexWrapper(name = "MUON4TrackToVertexWrapper2",
#                                                                      TrackToVertexIPEstimator = MUON4IPETool,
#                                                                      DecorationPrefix = "MUON4",
#                                                                      ContainerName = "CombinedMuonTrackParticles")
# print MUON4TrackToVertexWrapper2
# ToolSvc += MUON4TrackToVertexWrapper2
# MUON4_aug_tools.append(MUON4TrackToVertexWrapper2)
# 
# ## Extraplated tracks
# MUON4TrackToVertexWrapper3= DerivationFramework__TrackToVertexWrapper(name = "MUON4TrackToVertexWrapper3",
#                                                                      TrackToVertexIPEstimator = MUON4IPETool,
#                                                                      DecorationPrefix = "MUON4",
#                                                                      ContainerName = "ExtrapolatedMuonTrackParticles")
# print MUON4TrackToVertexWrapper3
# ToolSvc += MUON4TrackToVertexWrapper3
# MUON4_aug_tools.append(MUON4TrackToVertexWrapper3)
#====================================================================
# STRING BASED SKIMMING TOOL 
#====================================================================
MUON4_skimming_tools = []

#====================================================================
# THINNING TOOL 
#====================================================================
# Tracks associated with Muons
MUON4_thinning_tools = []

thinning_expression1 = "InDetTrackParticles.pt > 2*GeV"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
MUON4ThinningTool1 = DerivationFramework__TrackParticleThinning(name                    = "MUON4ThinningTool1",
                                                                ThinningService         = "MUON4ThinningSvc",
                                                                SelectionString         = thinning_expression1,
                                                                ApplyAnd                = False,
                                                                InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += MUON4ThinningTool1
MUON4_thinning_tools.append(MUON4ThinningTool1)
#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("MUON4Kernel",
									AugmentationTools = MUON4_aug_tools,
                                                                        ThinningTools = MUON4_thinning_tools,
                                                                        SkimmingTools = MUON4_skimming_tools)
#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_MUON4Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_MUON4Stream )
MUON4Stream = MSMgr.NewPoolRootStream( streamName, fileName )
MUON4Stream.AcceptAlgs(["MUON4Kernel"])

from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="MUON4ThinningSvc", outStreams=[evtStream] ) 
#====================================================================
# CONTENT LIST  
#====================================================================
from DerivationFrameworkMuons.StreamConfig import MuonsDxAODStreamConfigurer as conf
conf.Config(MUON4Stream, 'MUON4')
