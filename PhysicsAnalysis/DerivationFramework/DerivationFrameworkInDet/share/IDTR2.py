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
# Run the LRT merger
#====================================================================
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleMerger
LRTAndStandardTrackParticleMerger = DerivationFramework__TrackParticleMerger(name                        = "LRTAndStandardTrackParticleMerger",
                                                                             TrackParticleLocation       = ["InDetTrackParticles","InDetLargeD0TrackParticles"],
                                                                             OutputTrackParticleLocation = "InDetWithLRTTrackParticles",
                                                                             CreateViewColllection       = True)

ToolSvc += LRTAndStandardTrackParticleMerger
 
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

doSimpleV0Finder = False
if doSimpleV0Finder:
  include("DerivationFrameworkBPhys/configureSimpleV0Finder.py")
else:
  include("DerivationFrameworkBPhys/configureV0Finder.py")


from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
ToolSvc += AtlasExtrapolator(name     = "AtlasExtrapolator")



IDTR2_V0FinderTools = BPHYV0FinderTools("IDTR2")

from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetConversionTrackSelectorTool
IDTR2InDetV0VxTrackSelectorLoose = InDet__InDetConversionTrackSelectorTool(name                = "IDTR2InDetV0VxTrackSelectorLoose",
                                                                      maxSiD0             = 99999.,
                                                                      maxTrtD0            = 99999.,
                                                                      maxSiZ0             = 99999.,
                                                                      maxTrtZ0            = 99999.,
                                                                      minPt               = 500.0,
                                                                      significanceD0_Si   = 0.,
                                                                      significanceD0_Trt  = 0.,
                                                                      significanceZ0_Trt  = 0.,
                                                                      Extrapolator        = IDTR2_V0FinderTools.InDetExtrapolator,
                                                                      IsConversion        = False)
ToolSvc += IDTR2InDetV0VxTrackSelectorLoose
print(IDTR2InDetV0VxTrackSelectorLoose)

IDTR2_V0FinderTools.V0FinderTool.TrackParticleCollection = "InDetWithLRTTrackParticles"
IDTR2_V0FinderTools.V0FinderTool.TrackSelectorTool = IDTR2InDetV0VxTrackSelectorLoose

print(IDTR2_V0FinderTools)
print(IDTR2_V0FinderTools.V0FinderTool)
print(IDTR2_V0FinderTools.V0FinderTool.TrackParticleCollection)
print(IDTR2_V0FinderTools.V0FinderTool.TrackSelectorTool)

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_V0Finder
IDTR2_Reco_V0Finder   = DerivationFramework__Reco_V0Finder(
    name                   = "IDTR2_Reco_V0Finder",
    V0FinderTool           = IDTR2_V0FinderTools.V0FinderTool,
    V0Tools                = TrackingCommon.getV0Tools(),
    #OutputLevel            = DEBUG,
    V0ContainerName        = "IDTR2RecoV0Candidates",
    KshortContainerName    = "IDTR2RecoKshortCandidates",
    LambdaContainerName    = "IDTR2RecoLambdaCandidates",
    LambdabarContainerName = "IDTR2RecoLambdabarCandidates",
    CheckVertexContainers  = ["PrimaryVertices"]
)

ToolSvc += IDTR2_Reco_V0Finder
print(IDTR2_Reco_V0Finder)

 
#====================================================================
# Set up sequence for this format and add to the top sequence 
#====================================================================
SeqIDTR2 = CfgMgr.AthSequencer("SeqIDTR2")
DerivationFrameworkJob += SeqIDTR2
SeqIDTR2 += CfgMgr.DerivationFramework__CommonAugmentation("InDetWithLRTLRTMerge",
                                                                         AugmentationTools = [LRTAndStandardTrackParticleMerger])

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("IDTR2Kernel",
                                                                        AugmentationTools = [IDTR2TrackToVertexWrapper, IDTR2_Reco_V0Finder]
                                                                      )

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_IDTR2Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_IDTR2Stream )
IDTR2Stream = MSMgr.NewPoolRootStream( streamName, fileName )
IDTR2Stream.AcceptAlgs(["IDTR2Kernel"])



#====================================================================
# Run VSI
#====================================================================
from VrtSecInclusive.VrtSecInclusive import VrtSecInclusive
from VrtSecInclusive.VrtSecInclusive_Configuration import setupVSI

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

VrtSecInclusive_InDet.doAugmentDVimpactParametersToMuons     = False
VrtSecInclusive_InDet.doAugmentDVimpactParametersToElectrons = False

VrtSecInclusive_InDet.twoTrkVtxFormingD0Cut = 0.0
VrtSecInclusive_InDet.TrkPtCut = 500

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
StaticContent += ["xAOD::VertexContainer#%s"        %                 'IDTR2RecoV0Candidates']
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % 'IDTR2RecoV0Candidates']
StaticContent += ["xAOD::VertexContainer#%s"        %                 'IDTR2RecoKshortCandidates']
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % 'IDTR2RecoKshortCandidates']
StaticContent += ["xAOD::VertexContainer#%s"        %                 'IDTR2RecoLambdaCandidates']
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % 'IDTR2RecoLambdaCandidates']
StaticContent += ["xAOD::VertexContainer#%s"        %                 'IDTR2RecoLambdabarCandidates']
StaticContent += ["xAOD::VertexAuxContainer#%sAux.-vxTrackAtVertex" % 'IDTR2RecoLambdabarCandidates']


IDTR2SlimmingHelper.StaticContent = StaticContent

# Final construction of output stream
IDTR2SlimmingHelper.AppendContentToStream(IDTR2Stream)

if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()

ServiceMgr.THistSvc.Output += [ "AANT  DATAFILE='VrtSecInclusive.root' OPT='RECREATE'" ]

