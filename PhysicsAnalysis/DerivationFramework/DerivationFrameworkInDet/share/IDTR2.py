#====================================================================
# IDTR2.py
# Test set-up
# It requires the reductionConf flag IDTR2 in Reco_tf.py  
#====================================================================
 
# Set up common services and job object.
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from InDetRecExample import TrackingCommon

#====================================================================
# CP GROUP TOOLS
#====================================================================
IDTR2IPETool = TrackingCommon.getTrackToVertexIPEstimator(name = "IDTR2IPETool")
ToolSvc += IDTR2IPETool

IDTR2T2VTool = TrackingCommon.getInDetTrackToVertexTool(name = "IDTR2T2VTool")
ToolSvc += IDTR2T2VTool
 
#====================================================================
# AUGMENTATION TOOLS
#====================================================================
# Add unbiased track parameters to track particles
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackToVertexWrapper
IDTR2TrackToVertexWrapper= DerivationFramework__TrackToVertexWrapper(name = "IDTR2TrackToVertexWrapper",
                                                                      TrackToVertexIPEstimator = IDTR2IPETool,
                                                                      DecorationPrefix = "IDTR2",
                                                                      ContainerName = "InDetTrackParticles")
ToolSvc += IDTR2TrackToVertexWrapper 
 
#====================================================================
# Set up sequence for this format and add to the top sequence 
#====================================================================
SeqIDTR2 = CfgMgr.AthSequencer("SeqIDTR2")
DerivationFrameworkJob += SeqIDTR2

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("IDTR2Kernel",
                                                                        AugmentationTools = [IDTR2TrackToVertexWrapper]
                                                                      )

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_IDTR2Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_IDTR2Stream )
IDTR2Stream = MSMgr.NewPoolRootStream( streamName, fileName )
IDTR2Stream.AcceptAlgs(["IDTR2Kernel"])


#====================================================================
# Run the LRT merger
#====================================================================
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleMerger
LRTAndStandardTrackParticleMerger = DerivationFramework__TrackParticleMerger(name                        = "LRTAndStandardTrackParticleMerger",
                                                                             TrackParticleLocation       = ["InDetTrackParticles","InDetLargeD0TrackParticles"],
                                                                             OutputTrackParticleLocation = "InDetWithLRTTrackParticles",
                                                                             CreateViewColllection       = True)

ToolSvc += LRTAndStandardTrackParticleMerger
SeqIDTR2 += CfgMgr.DerivationFramework__CommonAugmentation("InDetWithLRTLRTMerge",
                                                                         AugmentationTools = [LRTAndStandardTrackParticleMerger])

#====================================================================
# Run VSI
#====================================================================
from VrtSecInclusive.VrtSecInclusive import VrtSecInclusive
from VrtSecInclusive.VrtSecInclusive_Configuration import setupVSI
from TrkExTools.AtlasExtrapolator import AtlasExtrapolator

ToolSvc += AtlasExtrapolator(name     = "AtlasExtrapolator")

# set options related to the vertex fitter
from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
InclusiveVxFitterTool = Trk__TrkVKalVrtFitter(name                = "InclusiveVxFitter",
                                              Extrapolator        = ToolSvc.AtlasExtrapolator,
                                              IterationNumber     = 30,
                                              )
ToolSvc +=  InclusiveVxFitterTool;
InclusiveVxFitterTool.OutputLevel = INFO

VrtSecInclusive_InDet = setupVSI( "InDet" )

VrtSecInclusive_InDet.VertexFitterTool             = InclusiveVxFitterTool
VrtSecInclusive_InDet.Extrapolator                 = ToolSvc.AtlasExtrapolator
VrtSecInclusive_InDet.TrackToVertexIPEstimatorTool = ToolSvc.IDTR2IPETool
VrtSecInclusive_InDet.TrackToVertexTool            = ToolSvc.IDTR2T2VTool
VrtSecInclusive_InDet.FillIntermediateVertices     = False
VrtSecInclusive_InDet.TrackLocation                = "InDetWithLRTTrackParticles"

SeqIDTR2 += VrtSecInclusive_InDet

#====================================================================
# CONTENTS   
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
IDTR2SlimmingHelper = SlimmingHelper("IDTR2SlimmingHelper")

IDTR2SlimmingHelper.AllVariables = ["EventInfo",
                                    "PrimaryVertices",
                                    "InDetTrackParticles",
                                    "InDetLargeD0TrackParticles",
                                   ]


StaticContent = []
StaticContent += ["xAOD::VertexContainer#VrtSecInclusive_SecondaryVertices"]
StaticContent += ["xAOD::VertexAuxContainer#VrtSecInclusive_SecondaryVerticesAux."]

IDTR2SlimmingHelper.StaticContent = StaticContent

# Final construction of output stream
IDTR2SlimmingHelper.AppendContentToStream(IDTR2Stream)

if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()

ServiceMgr.THistSvc.Output += [ "AANT  DATAFILE='VrtSecInclusive.root' OPT='RECREATE'" ]

