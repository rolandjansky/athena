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
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackToVertexWrapper
IDTR1TrackToVertexWrapper= DerivationFramework__TrackToVertexWrapper(name = "IDTR1TrackToVertexWrapper",
                                                                      TrackToVertexIPEstimator = IDTR1IPETool,
                                                                      DecorationPrefix = "IDTR1",
                                                                      ContainerName = "InDetTrackParticles")
ToolSvc += IDTR1TrackToVertexWrapper 
print IDTR1TrackToVertexWrapper
 
#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
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
IDTR1Stream.AddItem("xAOD::EventInfo#*")
IDTR1Stream.AddItem("xAOD::EventAuxInfo#*")
IDTR1Stream.AddItem("xAOD::TrackParticleContainer#*")
IDTR1Stream.AddItem("xAOD::TrackParticleAuxContainer#*")
IDTR1Stream.AddItem("xAOD::VertexContainer#PrimaryVertices")
IDTR1Stream.AddItem("xAOD::VertexAuxContainer#PrimaryVerticesAux.")
#IDTR1Stream.AddItem("std::vector<float>#IDTR1D0")
#IDTR1Stream.AddItem("std::vector<float>#IDTR1Z0")
#IDTR1Stream.AddItem("std::vector<float>#IDTR1D0Sigma")
#IDTR1Stream.AddItem("std::vector<float>#IDTR1Z0Sigma")
