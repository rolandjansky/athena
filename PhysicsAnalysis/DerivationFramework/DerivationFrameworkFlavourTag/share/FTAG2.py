#====================================================================
# FTAG2.py
# It requires the reductionConf flag FTAG2 in Reco_tf.py
#====================================================================

# Set up common services and job object.
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
from DerivationFrameworkExotics.JetDefinitions import *
from JetRec.JetRecStandard import jtm
from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__TrackToVertexIPEstimator
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
from DerivationFrameworkCore.LHE3WeightMetadata import *
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
from DerivationFrameworkFlavourTag.HbbCommon import *
from DerivationFrameworkFlavourTag.FlavourTagCommon import *
from DerivationFrameworkFlavourTag.SoftBtagCommon import applySoftBtagging
from DerivationFrameworkFlavourTag.FlavourTagAllVariables import FTAllVars_bjetTriggerVtx
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackToVertexWrapper
from DerivationFrameworkJetEtMiss.AntiKt4EMTopoJetsCPContent import AntiKt4EMTopoJetsCPContent

#====================================================================
# Create Private Sequence
#====================================================================

FTAG2Seq = CfgMgr.AthSequencer("FTAG2Sequence");

#====================================================================
# SKIMMING TOOLS
# (SKIMMING = REMOVING WHOLE EVENTS THAT FAIL CRITERIA)
# Create skimming tool, and create + add kernel to sequence
#====================================================================
FTAG2StringSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "FTAG2StringSkimmingTool",
                          expression = 'count( (Muons.pt > 18*GeV) && (0 == Muons.muonType || 1 == Muons.muonType || 4 == Muons.muonType) ) + count(( Electrons.pt > 18*GeV) && ((Electrons.Loose) || (Electrons.DFCommonElectronsLHLoose))) >= 2 ')

ToolSvc += FTAG2StringSkimmingTool
printfunc (FTAG2StringSkimmingTool)

FTAG2Seq += CfgMgr.DerivationFramework__DerivationKernel("FTAG2SkimKernel",
                                                         SkimmingTools = [FTAG2StringSkimmingTool],
                                                         )

#====================================================================
# TRUTH SETUP
#====================================================================
if globalflags.DataSource()!='data':
    from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents, addHFAndDownstreamParticles
    addStandardTruthContents()
    addHFAndDownstreamParticles()

#====================================================================
# AUGMENTATION TOOLS
# ( AUGMENTATION = adding information to the output DxAOD that is not found in the input file )
# Create IPE tool, then create augmenter and add to sequence
#====================================================================

#make IPE tool for BTagTrackAugmenter
FTAG2IPETool = Trk__TrackToVertexIPEstimator(name = "FTAG2IPETool")
ToolSvc += FTAG2IPETool
printfunc (FTAG2IPETool)

#augment jets with track info
FTAG2Seq += CfgMgr.BTagVertexAugmenter()

#Add unbiased track parameters to track particles
#FTAG2TrackToVertexWrapper= DerivationFramework__TrackToVertexWrapper(name = "FTAG2TrackToVertexWrapper",
#        TrackToVertexIPEstimator = FTAG2IPETool,
#        DecorationPrefix = "FTAG2",
#        ContainerName = "InDetTrackParticles")
#ToolSvc += FTAG2TrackToVertexWrapper
#printfunc (FTAG2TrackToVertexWrapper)

#====================================================================
# Basic Jet Collections
#====================================================================

#put custom jet names here
OutputJets["FTAG2"] = ["AntiKt4EMTopoJets",
                       "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810",
                       "AntiKt4EMPFlowJets"]


reducedJetList = ["AntiKt2PV0TrackJets",
                  "AntiKt4PV0TrackJets",
                  "AntiKt10LCTopoJets",
                  "AntiKt4TruthJets"]

extendedFlag = 1 # --- = 0 for Standard Taggers & =1 for ExpertTaggers
replaceAODReducedJets(reducedJetList,FTAG2Seq,"FTAG2", extendedFlag)

addDefaultTrimmedJets(FTAG2Seq,"FTAG2",dotruth=True)

#===================================================================
# Add Large-R RC jets w/ ExKt 2 & 3 subjets
#===================================================================

addExKtDoubleTaggerRCJets(FTAG2Seq, ToolSvc)

#===================================================================
# Variable Radius (VR) Jets
#===================================================================

# Create variable-R trackjets and dress AntiKt10LCTopo with ghost VR-trkjet
addVRJets(FTAG2Seq, doFlipTagger=True)
addVRJets(FTAG2Seq, doFlipTagger=True, training='201903')
addVRJets(FTAG2Seq, do_ghost=True, doFlipTagger=True)

# Also add Hbb Tagger
addHbbTagger(FTAG2Seq, ToolSvc)

# alias for VR
BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR30Rmax4Rmin02Track->AntiKtVR30Rmax4Rmin02Track,AntiKt4EMTopo"]

#==============================================================================
# Soft Tagging
#==============================================================================
from InDetVKalVxInJetTool.InDetVKalVxInJetFinder import InDetVKalVxInJetFinder

# used by Loose and Medium
SoftBJetSVFinderTool      = InDetVKalVxInJetFinder("SoftBJetSVFinder")
ToolSvc += SoftBJetSVFinderTool
SoftBJetSVFinderTool.ConeForTag = 0.75

softTagAlgLoose = CfgMgr.SoftBVrt__SoftBVrtClusterTool(  "SoftBVrtClusterToolLoose",
                           OutputLevel=INFO, #DEBUG
                           )

softTagAlgLoose.SVFinderName = 'SoftBJetSVFinder'
softTagAlgLoose.TrackJetCollectionName = 'AntiKt4PV0TrackJets'
softTagAlgLoose.TrackSelectionTool.CutLevel = "LoosePrimary"
softTagAlgLoose.OperatingPoint = 'Loose'

FTAG2Seq += softTagAlgLoose

softTagAlgMedium = CfgMgr.SoftBVrt__SoftBVrtClusterTool(  "SoftBVrtClusterToolMedium",
                           OutputLevel=INFO, #DEBUG
                           )

softTagAlgMedium.SVFinderName = 'SoftBJetSVFinder'
softTagAlgMedium.TrackJetCollectionName = 'AntiKt4PV0TrackJets'
softTagAlgMedium.TrackSelectionTool.CutLevel = "LoosePrimary"
softTagAlgMedium.OperatingPoint = 'Medium'

FTAG2Seq += softTagAlgMedium

# used by Tight
SoftBJetSVFinderToolTight      = InDetVKalVxInJetFinder("SoftBJetSVFinderTight")
ToolSvc += SoftBJetSVFinderToolTight
SoftBJetSVFinderTool.ConeForTag = 0.6

softTagAlgTight = CfgMgr.SoftBVrt__SoftBVrtClusterTool(  "SoftBVrtClusterToolTight",
                           OutputLevel=INFO, #DEBUG
                           )

softTagAlgTight.SVFinderName = 'SoftBJetSVFinderTight'
softTagAlgTight.TrackJetCollectionName = 'AntiKt4PV0TrackJets'
softTagAlgTight.TrackSelectionTool.CutLevel = "LoosePrimary"
softTagAlgTight.OperatingPoint = 'Tight'

FTAG2Seq += softTagAlgTight

applySoftBtagging("softBtag", FTAG2Seq ) # SV tagger in VrtSecInclusive

#===================================================================
# Tag custom or pre-built jet collections
#===================================================================

FlavorTagInit(scheduleFlipped = True, JetCollections  = ['AntiKt4EMPFlowJets','AntiKt4EMTopoJets'], Sequencer = FTAG2Seq)


#====================================================================
# Add sequence (with all kernels needed) to DerivationFrameworkJob
#====================================================================

DerivationFrameworkJob += FTAG2Seq

#====================================================================
# SET UP STREAM
#====================================================================

# The base name (DAOD_FTAG2 here) must match the string in
streamName = derivationFlags.WriteDAOD_FTAG2Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_FTAG2Stream )
FTAG2Stream = MSMgr.NewPoolRootStream( streamName, fileName )
# Only events that pass the filters listed below are written out.
# Name must match that of the kernel above
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
FTAG2Stream.AcceptAlgs(["FTAG2SkimKernel"])

FTAG2SlimmingHelper = SlimmingHelper("FTAG2SlimmingHelper")

# nb: BTagging_AntiKt4EMTopo smart collection includes both AntiKt4EMTopoJets and BTagging_AntiKt4EMTopo
# container variables. Thus BTagging_AntiKt4EMTopo is needed in SmartCollections as well as AllVariables
FTAG2SlimmingHelper.SmartCollections = ["Electrons","Muons",
                                        "AntiKt4EMTopoJets",
                                        "MET_Reference_AntiKt4EMTopo",
                                        "AntiKt4EMPFlowJets",
                                        "MET_Reference_AntiKt4EMPFlow",
                                        "AntiKt8EMPFlowJets",
                                        "AntiKt8EMPFlowExKt2SubJets",
                                        "AntiKt8EMPFlowExKt3SubJets",
                                        "BTagging_AntiKt8EMPFlowExKt2Sub",
                                        "BTagging_AntiKt8EMPFlowExKt3Sub",
                                        "BTagging_AntiKtVR30Rmax4Rmin02Track_201810GhostTag_expert",
                                        ]

FTAG2SlimmingHelper.AllVariables = ["AntiKt4EMTopoJets",
                                    "BTagging_AntiKtVR30Rmax4Rmin02Track_201810",
                                    "BTagging_AntiKtVR30Rmax4Rmin02Track_2018JFVtx",
                                    "BTagging_AntiKt4EMPFlow_201810",
                                    "BTagging_AntiKt4EMPFlow_201903",
                                    "BTagging_AntiKt4EMPFlowJFVtx",
                                    "BTagging_AntiKt4EMPFlow_201810JFVtx",
                                    "BTagging_AntiKt4EMPFlow_201903JFVtx",
                                    "BTagging_AntiKt2Track",
                                    "BTagging_AntiKt2TrackJFVtx",
                                    "PrimaryVertices",
                                    "TruthEvents",
                                    "TruthParticles",
                                    "TruthVertices",
                                    "TruthHFWithDecayParticles",
                                    "TruthHFWithDecayVertices",
                                    "MET_Truth",
                                    "MET_TruthRegions"]

for FT2_bjetTriggerVtx in FTAllVars_bjetTriggerVtx:
    FTAG2SlimmingHelper.AllVariables.append(FT2_bjetTriggerVtx)

FTAG2SlimmingHelper.ExtraVariables += [AntiKt4EMTopoJetsCPContent[1].replace("AntiKt4EMTopoJetsAux","AntiKt10LCTopoJets"),
                                       #"InDetTrackParticles.truthMatchProbability.x.y.z.vx.vy.vz",
                                       #"InDetTrackParticles.numberOfContribPixelLayers.numberOfTRTHits.numberOfInnermostPixelLayerSharedHits.numberOfNextToInnermostPixelLayerSharedHits",
                                       #"InDetTrackParticles.numberOfPixelSplitHits.numberOfInnermostPixelLayerSplitHits.numberOfNextToInnermostPixelLayerSplitHits",
                                       #"InDetTrackParticles.hitPattern.radiusOfFirstHit",
                                       #"InDetTrackParticles.FTAG2_unbiased_d0.FTAG2_unbiased_z0.FTAG2_unbiased_d0Sigma.FTAG2_unbiased_z0Sigma",
                                       #"CombinedMuonTrackParticles.vx.vy.vz",
                                       #"ExtrapolatedMuonTrackParticles.vx.vy.vz",
                                       #"MSOnlyExtrapolatedMuonTrackParticles.vx.vy.vz",
                                       #"MuonSpectrometerTrackParticles.vx.vy.vz",
                                       #"InDetForwardTrackParticles.phi.qOverP.theta",
                                       "BTagging_AntiKt4EMPFlow_201810SecVtx.-vxTrackAtVertex",
                                       "BTagging_AntiKt4EMPFlow_201903SecVtx.-vxTrackAtVertex",
                                       "BTagging_AntiKt2TrackSecVtx.-vxTrackAtVertex",
                                       "BTagging_AntiKtVR30Rmax4Rmin02Track_201810SecVtx.-vxTrackAtVertex",
                                       "BTagging_AntiKt4EMPFlow.MV1_discriminant.MV1c_discriminant.SV1_pb.SV1_pu.IP3D_pb.IP3D_pu.MV2c10_discriminant",
                                       "AntiKt10LCTopoJets.GhostVR30Rmax4Rmin02TrackJet.GhostVR30Rmax4Rmin02TrackJetPt.GhostVR30Rmax4Rmin02TrackJetCount",
                                       "AntiKt4EMPFlowJets.EMFrac.HECFrac.LArQuality.HECQuality.FracSamplingMax.NegativeE.AverageLArQF.FracSamplingMaxIndex.HadronConeExclTruthLabelID.GhostTrack",
                                       "AntiKt4EMPFlowJets.Jvt.JvtRpt.JvtJvfcorr",
                                       "AntiKt4EMPFlowJets.NumTrkPt1000.NumTrkPt500.SumPtTrkPt500.SumPtTrkPt1000",
                                       "InDetTrackParticles.btagIp_d0.btagIp_z0SinTheta.btagIp_d0Uncertainty.btagIp_z0SinThetaUncertainty.btagIp_trackDisplacement.btagIp_trackMomentum",
                                       "InDetTrackParticles.is_selected.is_associated.is_svtrk_final.pt_wrtSV.eta_wrtSV.phi_wrtSV.d0_wrtSV.z0_wrtSV.errP_wrtSV.errd0_wrtSV.errz0_wrtSV.chi2_toSV",
                                       ]

FTAG2SlimmingHelper.SmartCollections += ["AntiKt4EMTopoJets_BTagging201810",
                                     "AntiKt4EMPFlowJets_BTagging201810",
                                     "AntiKt4EMPFlowJets_BTagging201903",
                 "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810",
                 "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903"]
FTAG2SlimmingHelper.AllVariables += ["BTagging_AntiKt4EMPFlow_201810",
                                 "BTagging_AntiKt4EMPFlow_201903",
                                 "BTagging_AntiKt4EMTopo_201810",
             "BTagging_AntiKtVR30Rmax4Rmin02Track_201810",
             "BTagging_AntiKtVR30Rmax4Rmin02Track_201903",
                                 "AntiKt4EMTopoJets_BTagging201810",
                                 "AntiKt4EMPFlowJets_BTagging201810",
                                 "AntiKt4EMPFlowJets_BTagging201903"]

#----------------------------------------------------------------------
# Add needed dictionary stuff
FTAG2SlimmingHelper.AppendToDictionary = {
  "TruthHFWithDecayParticles"                      :   "xAOD::TruthParticleContainer",
  "TruthHFWithDecayParticlesAux"                   :   "xAOD::TruthParticleAuxContainer",
  "TruthHFWithDecayVertices"                       :   "xAOD::TruthVertexContainer",
  "TruthHFWithDecayVerticesAux"                    :   "xAOD::TruthVertexAuxContainer",
  "BTagging_AntiKt4EMPFlow"                        :   "xAOD::BTaggingContainer",
  "BTagging_AntiKt4EMPFlowAux"                     :   "xAOD::BTaggingAuxContainer",
  "BTagging_AntiKt4EMPFlowJFVtx"                   :   "xAOD::BTagVertexContainer",
  "BTagging_AntiKt4EMPFlowJFVtxAux"                :   "xAOD::BTagVertexAuxContainer",
  "BTagging_AntiKt4EMPFlow_201810JFVtx"                   :   "xAOD::BTagVertexContainer",
  "BTagging_AntiKt4EMPFlow_201810JFVtxAux"                :   "xAOD::BTagVertexAuxContainer",
  "BTagging_AntiKt4EMPFlow_201903JFVtx"                   :   "xAOD::BTagVertexContainer",
  "BTagging_AntiKt4EMPFlow_201903JFVtxAux"                :   "xAOD::BTagVertexAuxContainer",
  "BTagging_AntiKt4EMPFlow_201810SecVtx"      :   "xAOD::VertexContainer"   ,
  "BTagging_AntiKt4EMPFlow_201810SecVtxAux"   :   "xAOD::VertexAuxContainer",
  "BTagging_AntiKt4EMPFlow_201903SecVtx"      :   "xAOD::VertexContainer"   ,
  "BTagging_AntiKt4EMPFlow_201903SecVtxAux"   :   "xAOD::VertexAuxContainer",
  "AntiKtVR30Rmax4Rmin02Track_201903"                     :   "xAOD::JetContainer"        ,
  "AntiKtVR30Rmax4Rmin02Track_201903Aux"                  :   "xAOD::JetAuxContainer"     ,
  "BTagging_AntiKtVR30Rmax4Rmin02Track_201903"            :   "xAOD::BTaggingContainer"   ,
  "BTagging_AntiKtVR30Rmax4Rmin02Track_201903Aux"         :   "xAOD::BTaggingAuxContainer",
  "BTagging_AntiKtVR30Rmax4Rmin02Track_201903JFVtx"       :   "xAOD::BTagVertexContainer" ,
  "BTagging_AntiKtVR30Rmax4Rmin02Track_201903JFVtxAux"    :   "xAOD::BTagVertexAuxContainer",
  "BTagging_AntiKtVR30Rmax4Rmin02Track_201903SecVtx"      :   "xAOD::VertexContainer"   ,
  "BTagging_AntiKtVR30Rmax4Rmin02Track_201903SecVtxAux"   :   "xAOD::VertexAuxContainer",
  "AntiKtVR30Rmax4Rmin02Track_201810"                     :   "xAOD::JetContainer"        ,
  "AntiKtVR30Rmax4Rmin02Track_201810Aux"                  :   "xAOD::JetAuxContainer"     ,
  "BTagging_AntiKtVR30Rmax4Rmin02Track_201810"            :   "xAOD::BTaggingContainer"   ,
  "BTagging_AntiKtVR30Rmax4Rmin02Track_201810Aux"         :   "xAOD::BTaggingAuxContainer",
  "BTagging_AntiKtVR30Rmax4Rmin02Track_201810JFVtx"       :   "xAOD::BTagVertexContainer" ,
  "BTagging_AntiKtVR30Rmax4Rmin02Track_201810JFVtxAux"    :   "xAOD::BTagVertexAuxContainer",
  "BTagging_AntiKtVR30Rmax4Rmin02Track_201810SecVtx"      :   "xAOD::VertexContainer"   ,
  "BTagging_AntiKtVR30Rmax4Rmin02Track_201810SecVtxAux"   :   "xAOD::VertexAuxContainer",
  "BTagging_AntiKt2Track"                          :   "xAOD::BTaggingContainer"   ,
  "BTagging_AntiKt2TrackAux"                       :   "xAOD::BTaggingAuxContainer",
  "BTagging_AntiKt2TrackJFVtx"                     :   "xAOD::BTagVertexContainer"   ,
  "BTagging_AntiKt2TrackJFVtxAux"                  :   "xAOD::BTagVertexAuxContainer",
  "BTagging_AntiKt2TrackSecVtx"                    :   "xAOD::VertexContainer"   ,
  "BTagging_AntiKt2TrackSecVtxAux"                 :   "xAOD::VertexAuxContainer",
  "AntiKt8EMPFlowJets"                              :   "xAOD::JetContainer"        ,
  "AntiKt8EMPFlowJetsAux"                           :   "xAOD::JetAuxContainer"     ,
  "AntiKt8EMPFlowExKt2SubJets"                      :   "xAOD::JetContainer"        ,
  "AntiKt8EMPFlowExKt2SubJetsAux"                   :   "xAOD::JetAuxContainer"     ,
  "BTagging_AntiKt8EMPFlowExKt2Sub"                 :   "xAOD::BTaggingContainer"   ,
  "BTagging_AntiKt8EMPFlowExKt2SubAux"              :   "xAOD::BTaggingAuxContainer",
  "AntiKt8EMPFlowExKt3SubJets"                      :   "xAOD::JetContainer"        ,
  "AntiKt8EMPFlowExKt3SubJetsAux"                   :   "xAOD::JetAuxContainer"     ,
  "BTagging_AntiKt8EMPFlowExKt3Sub"                 :   "xAOD::BTaggingContainer"   ,
  "BTagging_AntiKt8EMPFlowExKt3SubAux"              :   "xAOD::BTaggingAuxContainer"
}

#----------------------------------------------------------------------
# Add soft b-tagging containers
excludedVertexAuxData = "-vxTrackAtVertex.-MvfFitInfo.-isInitialized.-VTAV"

StaticContent = []
StaticContent += ["xAOD::VertexContainer#SoftBVrtClusterTool_Tight_Vertices"]
StaticContent += ["xAOD::VertexAuxContainer#SoftBVrtClusterTool_Tight_VerticesAux." + excludedVertexAuxData]
StaticContent += ["xAOD::VertexContainer#SoftBVrtClusterTool_Medium_Vertices"]
StaticContent += ["xAOD::VertexAuxContainer#SoftBVrtClusterTool_Medium_VerticesAux." + excludedVertexAuxData]
StaticContent += ["xAOD::VertexContainer#SoftBVrtClusterTool_Loose_Vertices"]
StaticContent += ["xAOD::VertexAuxContainer#SoftBVrtClusterTool_Loose_VerticesAux." + excludedVertexAuxData]
StaticContent += ["xAOD::VertexContainer#VrtSecInclusive_SoftBtagCandidateVertices"]
StaticContent += ["xAOD::VertexAuxContainer#VrtSecInclusive_SoftBtagCandidateVerticesAux."]

FTAG2SlimmingHelper.StaticContent = StaticContent
#----------------------------------------------------------------------

addJetOutputs(FTAG2SlimmingHelper,["FTAG2"])

FTAG2SlimmingHelper.IncludeMuonTriggerContent = True
FTAG2SlimmingHelper.IncludeEGammaTriggerContent = True
FTAG2SlimmingHelper.IncludeJetTriggerContent = True
FTAG2SlimmingHelper.IncludeEtMissTriggerContent = True
FTAG2SlimmingHelper.IncludeBJetTriggerContent = True

#FTAG2 TrigNav Thinning
FTAG2ThinningHelper = ThinningHelper( "FTAG2ThinningHelper" )
FTAG2ThinningHelper.TriggerChains = 'HLT_.*b.*perf.*|HLT_mu.*|HLT_e.*'
FTAG2ThinningHelper.AppendToStream( FTAG2Stream )

FTAG2SlimmingHelper.AppendContentToStream(FTAG2Stream)
