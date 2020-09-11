#====================================================================
# HION1.py
# author: Aaron Angerami <angerami@cern.ch>
# Application: Global event correlations
# Triggers: MB
# Selection: Offline MB event selection
# Content: All tracks and HIEventShape 
#====================================================================
from DerivationFrameworkCore.DerivationFrameworkMaster import *

# Skimming
#Offline MB event selection skimming tool needs to be implemented

# Thinning
# Tracks from primary vertex
# Just a placeholder for now to test thinning
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
HION1TPThinningTool = DerivationFramework__TrackParticleThinning( name                    = "HION1TPThinningTool",
                                                                  ThinningService         = "HION1ThinningSvc",
                                                                  SelectionString         = "InDetTrackParticles.pt > (1* GeV)",
                                                                  InDetTrackParticlesKey  = "InDetTrackParticles")

ToolSvc += HION1TPThinningTool

# Set up derivation kernel
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("HION1Kernel",
                                                                       AugmentationTools = [],
                                                                       SkimmingTools = [],
                                                                       ThinningTools = [HION1TPThinningTool]
                                                                       )

# Configure stream
# The base name (DAOD_HION1 here) must match the string in
streamName = derivationFlags.WriteDAOD_HION1Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HION1Stream )
HION1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HION1Stream.AcceptAlgs(["HION1Kernel"])

# Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="HION1ThinningSvc", outStreams=[evtStream] )

# Slimming
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
HION1SlimmingHelper = SlimmingHelper("HION1SlimmingHelper")
allVariables = []
allVariables.append("InDetTrackParticles")
allVariables.append("PrimaryVertices")
if DerivationFrameworkHasTruth:
    allVariables.append("TruthParticles")
    allVariables.append("TruthVertices")
    allVariables.append("TruthEvents") 
HION1SlimmingHelper.AllVariables = allVariables
HION1SlimmingHelper.AppendContentToStream(HION1Stream) 


# from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
# HION1TrackSelectionTool = InDet__InDetTrackSelectionTool("HION1TracksSelectionTool",
#                                                          minPt                = 400.0,
#                                                          maxAbsEta            = 2.5,
#                                                          minNSiHits           = 7,
#                                                          maxNPixelSharedHits  = 1,
#                                                          maxOneSharedModule   = True,
#                                                          maxNSiHoles          = 2,
#                                                           maxNPixelHoles       = 1)
# ToolSvc += HION1TrackSelectionTool
# from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__AsgSelectionToolWrapper
# HION1TrackSelectionToolWrapper = DerivationFramework__AsgSelectionToolWrapper(name = "HION1TrackSelectionToolWrapper",
#                                                                               AsgSelectionTool = HION1TrackSelectionTool,
#                                                                               CutType = "IDHits",
#                                                                               StoreGateEntryName = "HION1GoodTracks",
#                                                                               ContainerName = "InDetTrackParticles")                                                                        
# ToolSvc += HION1TrackSelectionToolWrapper
# print HION1TrackSelectionToolWrapper

