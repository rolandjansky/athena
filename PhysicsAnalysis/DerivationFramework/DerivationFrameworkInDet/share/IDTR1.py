#====================================================================
# IDTR1.py
# Test set-up
# It requires the reductionConf flag IDTR1 in Reco_tf.py  
#====================================================================
 
# Set up common services and job object.
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *
 
#====================================================================
# CP GROUP TOOLS
#====================================================================
from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__TrackToVertexIPEstimator
IDTR1IPETool = Trk__TrackToVertexIPEstimator(name = "IDTR1IPETool")
ToolSvc += IDTR1IPETool
print IDTR1IPETool
 
#====================================================================
# AUGMENTATION TOOLS
#====================================================================
# Add unbiased track parameters to track particles
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackToVertexWrapper
IDTR1TrackToVertexWrapper= DerivationFramework__TrackToVertexWrapper(name = "IDTR1TrackToVertexWrapper",
                                                                      TrackToVertexIPEstimator = IDTR1IPETool,
                                                                      DecorationPrefix = "IDTR1",
                                                                      ContainerName = "InDetTrackParticles")
ToolSvc += IDTR1TrackToVertexWrapper 
print IDTR1TrackToVertexWrapper

# Add decoration with truth parameters if running on simulation
if DerivationFrameworkHasTruth:
    from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParametersForTruthParticles
    TruthDecor = DerivationFramework__TrackParametersForTruthParticles( name = "TruthTPDecor",
                                                                        DecorationPrefix = "IDTR1")
    ToolSvc += TruthDecor
    print TruthDecor
 
#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
if DerivationFrameworkHasTruth:
    DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("IDTR1Kernel",
                                                                            AugmentationTools = [IDTR1TrackToVertexWrapper,TruthDecor]
                                                                          )
else:
    DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("IDTR1Kernel",
                                                                            AugmentationTools = [IDTR1TrackToVertexWrapper]
                                                                          )
#====================================================================
# SET UP STREAM  
#====================================================================
 
# The base name (DAOD_TEST5 here) must match the string in
streamName = derivationFlags.WriteDAOD_IDTR1Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_IDTR1Stream )
IDTR1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
IDTR1Stream.AcceptAlgs(["IDTR1Kernel"])

# Add event info, tracks, vertices and the decoration
# Use slimming helper
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
IDTR1SlimmingHelper = SlimmingHelper("IDTR1SlimmingHelper")
allVariables = []
allVariables.append("InDetTrackParticles")
allVariables.append("GSFTrackParticles")
allVariables.append("PrimaryVertices")
allVariables.append("AntiKt4EMTopoJets")
allVariables.append("BTagging_AntiKt4EMTopo")
if DerivationFrameworkHasTruth:
    allVariables.append("TruthParticles")
    allVariables.append("TruthVertices")
    allVariables.append("TruthEvents") 
IDTR1SlimmingHelper.AllVariables = allVariables
IDTR1SlimmingHelper.AppendContentToStream(IDTR1Stream) 
