#====================================================================
# FTAG1.py
# It requires the reductionConf flag FTAG1 in Reco_tf.py
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
from DerivationFrameworkFlavourTag.HbbCommon import *
from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
from DerivationFrameworkFlavourTag.SoftBtagCommon import applySoftBtagging
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper

from DstarVertexing.DstarVertexingConf import DstarVertexing
from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__TrackToVertexIPEstimator
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
from DerivationFrameworkJetEtMiss.AntiKt4EMTopoJetsCPContent import AntiKt4EMTopoJetsCPContent
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackToVertexWrapper
from DerivationFrameworkFlavourTag.FlavourTagAllVariables import FTAllVars_bjetTriggerVtx
from DerivationFrameworkFlavourTag.FlavourTagExtraVariables import FTExtraVars_bjetTriggerTracks
from DerivationFrameworkFlavourTag.FlavourTagExtraVariables import FTExtraVars_bjetTriggerTracks

#====================================================================
# Create Private Sequence
#====================================================================

FTAG1Seq = CfgMgr.AthSequencer("FTAG1Sequence")

#====================================================================
# SKIMMING TOOLS
# (SKIMMING = REMOVING WHOLE EVENTS THAT FAIL CRITERIA)
# Create skimming tool, and create + add kernel to sequence
#====================================================================

# triggers used for skimming:
triggers_jX = ["HLT_j[0-9]*"]
triggers_bperf = ["HLT_j[1-9][0-9]*_b.*perf.*"]
triggersSkim = triggers_jX + triggers_bperf

FTAG1TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(name = "FTAG1TriggerSkimmingTool",
                                                                    TriggerListOR = triggersSkim)
ToolSvc += FTAG1TriggerSkimmingTool
print FTAG1TriggerSkimmingTool

FTAG1Seq += CfgMgr.DerivationFramework__DerivationKernel("FTAG1SkimKernel",
                                                         SkimmingTools = [FTAG1TriggerSkimmingTool],
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
# Create DStar vertexing tool, and create + add kernel to sequence
#====================================================================

#add D0 augmentation
FTAG1DstarAug="D0VxAug"
FTAG1DstarVertexing = DstarVertexing( name = "FTAG1DstarVertexing",
                                      OutputLevel = INFO,
                                      D0VertexOutputName= FTAG1DstarAug,
                                      InputJetsColletion = "AntiKt4EMTopoJets",
                                      JetPtCut = 20)

ToolSvc += FTAG1DstarVertexing
print "using DstarVertexing package to add D0 vertex information"
print FTAG1DstarVertexing

FTAG1Seq += CfgMgr.DerivationFramework__DerivationKernel("FTAG1AugmentKernel",
                                                         AugmentationTools = [FTAG1DstarVertexing]
                                                        )

#make IPE tool for BTagTrackAugmenter
FTAG1IPETool = Trk__TrackToVertexIPEstimator(name = "FTAG1IPETool")
ToolSvc += FTAG1IPETool
print FTAG1IPETool

#====================================================================
# Basic Jet Collections
#====================================================================

#put custom jet names here
OutputJets["FTAG1"] = ["AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810",
                       "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810GhostTag",
                       "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903",
                       "AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt2SubJets",
                       "AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt3SubJets",
                       "AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2SubJets",
                       "AntiKt4EMTopoJets",
                       "AntiKt4EMPFlowJets"
                       ]


reducedJetList = ["AntiKt2PV0TrackJets",
                  "AntiKt4PV0TrackJets",
                  "AntiKt10LCTopoJets",
                  "AntiKt4TruthJets"]

extendedFlag = 1 # --- = 0 for Standard Taggers & =1 for ExpertTaggers

replaceAODReducedJets(reducedJetList,FTAG1Seq,"FTAG1", extendedFlag)

addDefaultTrimmedJets(FTAG1Seq,"FTAG1",dotruth=True)
#
# Adding ExKt and ExCoM sub-jets for each trimmed large-R jet
#
ExKtJetCollection__FatJet = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"
ExKtJetCollection__SubJet = addExKtCoM(FTAG1Seq, ToolSvc, ExKtJetCollection__FatJet, nSubjets=2, doTrackSubJet=False)
ExKtJetCollection__SubJet3 = addExKtCoM(FTAG1Seq, ToolSvc, ExKtJetCollection__FatJet, nSubjets=3, doTrackSubJet=False)
ExCoMJetCollection__SubJet = addExKtCoM(FTAG1Seq, ToolSvc, ExKtJetCollection__FatJet, nSubjets=2, doTrackSubJet=False, subjetAlgName = "CoM")


BTaggingFlags.CalibrationChannelAliases += ["AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt2Sub->AntiKt4LCTopo,AntiKt4TopoEM,AntiKt4EMTopo",
                                            "AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt3Sub->AntiKt4LCTopo,AntiKt4TopoEM,AntiKt4EMTopo",
                                            "AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2Sub->AntiKt4LCTopo,AntiKt4TopoEM,AntiKt4EMTopo"]

#==============================================================================
# Soft Tagging
#==============================================================================
from DerivationFrameworkFlavourTag.DerivationFrameworkFlavourTagConf import BTagVertexAugmenter
from InDetVKalVxInJetTool.InDetVKalVxInJetFinder import InDetVKalVxInJetFinder

FTAG1Seq += BTagVertexAugmenter("BTagVertexAugmenter")


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

FTAG1Seq += softTagAlgLoose

softTagAlgMedium = CfgMgr.SoftBVrt__SoftBVrtClusterTool(  "SoftBVrtClusterToolMedium",
                           OutputLevel=INFO, #DEBUG
                           )

softTagAlgMedium.SVFinderName = 'SoftBJetSVFinder'
softTagAlgMedium.TrackJetCollectionName = 'AntiKt4PV0TrackJets'
softTagAlgMedium.TrackSelectionTool.CutLevel = "LoosePrimary"
softTagAlgMedium.OperatingPoint = 'Medium'

FTAG1Seq += softTagAlgMedium

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

FTAG1Seq += softTagAlgTight

# use by low pT Xbb
SoftBJetMSVFinderTool      = InDetVKalVxInJetFinder("SoftBJetMSVFinder")
ToolSvc += SoftBJetMSVFinderTool
SoftBJetMSVFinderTool.ConeForTag = 0.8
SoftBJetMSVFinderTool.MultiVertex = True
SoftBJetMSVFinderTool.MultiWithOneTrkVrt = True

softTagAlgMSVTight = CfgMgr.SoftBVrt__SoftBVrtClusterTool(  "SoftBVrtClusterToolMSVTight",
                           OutputLevel=INFO, #DEBUG
                           )

softTagAlgMSVTight.SVFinderName = 'SoftBJetMSVFinder'
softTagAlgMSVTight.TrackJetCollectionName = 'AntiKt4PV0TrackJets'
softTagAlgMSVTight.TrackSelectionTool.CutLevel = "LoosePrimary"
softTagAlgMSVTight.OperatingPoint = 'MSVTight'

FTAG1Seq += softTagAlgMSVTight

# make Pixel and SCT conditions available
include ("InDetRecExample/PixelConditionsAccess.py") # include all pixel condtions avaliable in AOD /DT
include ("InDetRecExample/SCTConditionsAccess.py")


applySoftBtagging("softBtag", FTAG1Seq ) # SV tagger in VrtSecInclusive

#===================================================================
# Add Large-R RC jets w/ ExKt 2 & 3 subjets
#===================================================================

addExKtDoubleTaggerRCJets(FTAG1Seq, ToolSvc)

#===================================================================
# Variable Radius (VR) Jets
#===================================================================

# Create variable-R trackjets and dress AntiKt10LCTopo with ghost VR-trkjet
addVRJets(FTAG1Seq)
addVRJets(FTAG1Seq, do_ghost=True)
addVRJets(FTAG1Seq, training='201903') #new trackjet training!
# Also add Hbb Tagger(s)
addHbbTagger(FTAG1Seq, ToolSvc)
addHbbTagger(
    FTAG1Seq, ToolSvc,
    nn_file_name="BoostedJetTaggers/HbbTagger/Summer2018/MulticlassNetwork.json",
    nn_config_file="BoostedJetTaggers/HbbTaggerDNN/MulticlassConfigJune2018.json")

# alias for VR
BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR30Rmax4Rmin02Track->AntiKtVR30Rmax4Rmin02Track,AntiKt4EMTopo",
                                            "AntiKtVR30Rmax4Rmin02TrackGhostTag->AntiKtVR30Rmax4Rmin02TrackGhostTag,AntiKt4EMTopo"]

#===================================================================
# Tag custom or pre-built jet collections
#===================================================================

FlavorTagInit(scheduleFlipped = True, JetCollections  = ['AntiKt4EMTopoJets', 'AntiKt4EMPFlowJets'],Sequencer = FTAG1Seq)

#====================================================================
# Add sequence (with all kernels needed) to DerivationFrameworkJob
#====================================================================

DerivationFrameworkJob += FTAG1Seq

#====================================================================
# SET UP STREAM
#====================================================================

# The base name (DAOD_FTAG1 here) must match the string in
streamName = derivationFlags.WriteDAOD_FTAG1Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_FTAG1Stream )
FTAG1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
# Only events that pass the filters listed below are written out.
# Name must match that of the kernel above
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
FTAG1Stream.AcceptAlgs(["FTAG1SkimKernel"])
FTAG1SlimmingHelper = SlimmingHelper("FTAG1SlimmingHelper")

# nb: BTagging_AntiKt4EMTopo smart collection includes both AntiKt4EMTopoJets and BTagging_AntiKt4EMTopo
# container variables. Thus BTagging_AntiKt4EMTopo is needed in SmartCollections as well as AllVariables
FTAG1SlimmingHelper.SmartCollections = ["Electrons","Muons",
                                        "PrimaryVertices",
                                        "InDetTrackParticles",
                                        "MET_Reference_AntiKt4EMTopo",
                                        "MET_Reference_AntiKt4EMPFlow",
                                        "AntiKt4EMTopoJets",
                                        "AntiKt4EMPFlowJets",
                                        "AntiKt8EMPFlowJets",
                                        "AntiKt8EMPFlowExKt2SubJets",
                                        "AntiKt8EMPFlowExKt3SubJets",
                                        "BTagging_AntiKt8EMPFlowExKt2Sub",
                                        "BTagging_AntiKt8EMPFlowExKt3Sub",
                                        "AntiKt8EMPFlowExKt2GASubJets",
                                        "AntiKt8EMPFlowExKt3GASubJets",
                                        "BTagging_AntiKt8EMPFlowExKt2GASub",
                                        "BTagging_AntiKt8EMPFlowExKt3GASub",
                                         ]

FTAG1SlimmingHelper.AllVariables = ["AntiKt4EMTopoJets", "AntiKt4EMPFlowJets",
                                    "BTagging_AntiKtVR30Rmax4Rmin02Track_2018GhostTag",
                                    "BTagging_AntiKtVR30Rmax4Rmin02Track_201810",
                                    "BTagging_AntiKtVR30Rmax4Rmin02Track_201810JFVtx",
                                    "BTagging_AntiKtVR30Rmax4Rmin02Track_201903",
                                    "BTagging_AntiKtVR30Rmax4Rmin02Track_201903JFVtx",
                                    "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt2Sub",
                                    "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt2SubJFVtx",
                                    "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt3Sub",
                                    "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt3SubJFVtx",
                                    "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2Sub",
                                    "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2SubJFVtx",
                                    "BTagging_AntiKt2Track",
                                    "BTagging_AntiKt2TrackJFVtx",
                                    "TruthEvents",
                                    "MET_Truth",
                                    "MET_TruthRegions",
                                    "TruthParticles",
                                    "TruthHFWithDecayParticles",
                                    "TruthHFWithDecayVertices",
                                    "TruthVertices",
                                    "CaloCalTopoClusters",
                                    "HLT_xAOD__BTaggingContainer_HLTBjetFex",
                                    "HLT_xAOD__BTagVertexContainer_BjetVertexFex",
                                    "HLT_xAOD__VertexContainer_BjetSecondaryVertexFex",
                                    "HLT_xAOD__VertexContainer_PrimVertexFTKRaw",
                                    "HLT_xAOD__VertexContainer_PrimVertexFTK",
                                    "HLT_xAOD__VertexContainer_PrimVertexFTKRefit",
                                    "HLT_xAOD__VertexContainer_xPrimVx",
                                    "HLT_xAOD__VertexContainer_EFHistoPrmVtx",
                                    "HLT_xAOD__VertexContainer_SecondaryVertex",
                                    "HLT_xAOD__JetContainer_FarawayJet",
                                    "HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bjet_IDTrig",
                                    ]

for FT1_bjetTriggerVtx in FTAllVars_bjetTriggerVtx:
    FTAG1SlimmingHelper.AllVariables.append(FT1_bjetTriggerVtx)

FTAG1SlimmingHelper.ExtraVariables += [AntiKt4EMTopoJetsCPContent[1].replace("AntiKt4EMTopoJetsAux","AntiKt10LCTopoJets"),
                                       "InDetTrackParticles.truthMatchProbability.x.y.z.vx.vy.vz",
                                       "InDetTrackParticles.numberOfInnermostPixelLayerSplitHits.numberOfNextToInnermostPixelLayerSplitHits.numberOfNextToInnermostPixelLayerSharedHits",
                                       "InDetTrackParticles.numberOfPixelSplitHits.numberOfInnermostPixelLayerSharedHits.numberOfContribPixelLayers.hitPattern.radiusOfFirstHit",
                                       "InDetTrackParticles.is_selected.is_associated.is_svtrk_final.pt_wrtSV.eta_wrtSV.phi_wrtSV.d0_wrtSV.z0_wrtSV.errP_wrtSV.errd0_wrtSV.errz0_wrtSV.chi2_toSV",
                                       "InDetTrackParticles.btagIp_d0.btagIp_z0SinTheta.btagIp_d0Uncertainty.btagIp_z0SinThetaUncertainty.btagIp_trackDisplacement.btagIp_trackMomentum",
                                       "PrimaryVertices.neutralWeights.numberDoF.sumPt2.chiSquared.covariance.trackWeights",
                                       "CombinedMuonTrackParticles.vx.vy.vz",
                                       "ExtrapolatedMuonTrackParticles.vx.vy.vz",
                                       "MSOnlyExtrapolatedMuonTrackParticles.vx.vy.vz",
                                       "MuonSpectrometerTrackParticles.vx.vy.vz",
                                       "AntiKt10LCTopoJets.ConeExclBHadronsFinal",
                                       "AntiKt10LCTopoJets.GhostAntiKt2TrackJet.GhostAntiKt2TrackJetPt.GhostAntiKt2TrackJetCount",
                                       "AntiKt10LCTopoJets.GhostVR30Rmax4Rmin02TrackJet_BTagging201810.GhostVR30Rmax4Rmin02TrackJet_BTagging201810Pt.GhostVR30Rmax4Rmin02TrackJet_BTagging201810Count",
                                       "AntiKt10LCTopoJets.GhostVR30Rmax4Rmin02TrackJet_BTagging201903.GhostVR30Rmax4Rmin02TrackJet_BTagging201903Pt.GhostVR30Rmax4Rmin02TrackJet_BTagging201903Count",
                                       "AntiKt10LCTopoJets.GhostVR30Rmax4Rmin02TrackJet_BTagging201810GhostTag.GhostVR30Rmax4Rmin02TrackJet_BTagging201810GhostTagPt.GhostVR30Rmax4Rmin02TrackJet_BTagging201810GhostTagCount",
                                       "BTagging_AntiKt2TrackSecVtx.-vxTrackAtVertex",
                                       "BTagging_AntiKt4EMTopo_201810SecVtx.-vxTrackAtVertex",
                                       "BTagging_AntiKt4EMPFlow_201810SecVtx.-vxTrackAtVertex",
                                       #"BTagging_AntiKtVR30Rmax4Rmin02Track_201810SecVtx.-vxTrackAtVertex",
                                       "BTagging_AntiKt4EMPFlow_201903SecVtx.-vxTrackAtVertex",
                                       "BTagging_AntiKtVR30Rmax4Rmin02Track_201810SecVtx.-vxTrackAtVertex",
                                       "BTagging_AntiKtVR30Rmax4Rmin02Track_201903SecVtx.-vxTrackAtVertex",
                                       "BTagging_AntiKtVR30Rmax4Rmin02Track_201810GhostTagSecVtx.-vxTrackAtVertex",
                                       "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt2SubSecVtx.-vxTrackAtVertex",
                                       "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt3SubSecVtx.-vxTrackAtVertex",
                                       "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2SubSecVtx.-vxTrackAtVertex",
                                       #"InDetTrackParticles.FTAG1_unbiased_d0.FTAG1_unbiased_z0.FTAG1_unbiased_d0Sigma.FTAG1_unbiased_z0Sigma",
                                       "CaloCalTopoClusters.calM.calE.calEta.calPhi",
                                       "PrimaryVertices.x.y.trackParticleLinks.vertexType.neutralParticleLinks",
                                       "HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF.vz",
                                       "HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bjet_FTF.vz",
                                       "HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bjet_IDTrig.vz",
                                       FTAG1DstarAug+".-vxTrackAtVertex"]

for FT1_bjetTriggerTracks in FTExtraVars_bjetTriggerTracks:
    FTAG1SlimmingHelper.ExtraVariables.append(FT1_bjetTriggerTracks)

FTAG1SlimmingHelper.SmartCollections += ["AntiKt4EMTopoJets_BTagging201810",
                                         "AntiKt4EMPFlowJets_BTagging201810",
                                         "AntiKt4EMPFlowJets_BTagging201903"]

FTAG1SlimmingHelper.AllVariables += ["BTagging_AntiKt4EMPFlow_201810",
                                     "BTagging_AntiKt4EMPFlow_201903",
                                     "BTagging_AntiKt4EMPFlow_201810JFVtx",
                                     "BTagging_AntiKt4EMPFlow_201903JFVtx",
                                     "BTagging_AntiKt4EMTopo_201810",
                                     "BTagging_AntiKt4EMTopo_201810JFVtx",
                                     "AntiKt4EMTopoJets_BTagging201810",
                                     "AntiKt4EMPFlowJets_BTagging201810",
                                     "AntiKt4EMPFlowJets_BTagging201903",
                                     "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810",
                                     "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903"]
FTAG1SlimmingHelper.ExtraVariables += ["BTagging_AntiKt4EMTopo_201810SecVtx.-vxTrackAtVertex"]


#--------LF AUGMENTATION-----------------------------------------------
# NOTE: this is commented out until https://its.cern.ch/jira/browse/ATLFTAGDPD-243 is resolved.
#
# for jc in ["AntiKt4EMTopoJets_BTagging201810", "AntiKt4EMPFlowJets_BTagging201903","AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810"]:
# #also add some b-tagging jet-wise information
#     FTAG1Seq += CfgMgr.BTagLFJetAugmenter(
#         "FTAG1LFJetAugmenter_"+jc,
#         OutputLevel=INFO,
#         JetCollectionName=jc
#     )

#----------------------------------------------------------------------
# Add needed dictionary stuff
FTAG1SlimmingHelper.AppendToDictionary = {
  "TruthHFWithDecayParticles"                  :   "xAOD::TruthParticleContainer",
  "TruthHFWithDecayParticlesAux"               :   "xAOD::TruthParticleAuxContainer",
  "TruthHFWithDecayVertices"                   :   "xAOD::TruthVertexContainer",
  "TruthHFWithDecayVerticesAux"                :   "xAOD::TruthVertexAuxContainer",
  FTAG1DstarAug                                    :   "xAOD::VertexContainer",
  FTAG1DstarAug+"Aux"                              :   "xAOD::VertexAuxContainer",
  "AntiKtVR30Rmax4Rmin02Track_BTagging201810GhostTag"             :   "xAOD::JetContainer"        ,
  "AntiKtVR30Rmax4Rmin02Track_BTagging201810GhostTagAux"          :   "xAOD::JetAuxContainer" ,
  "BTagging_AntiKt4EMTopo_201810JFVtx"                   :   "xAOD::BTagVertexContainer",
  "BTagging_AntiKt4EMTopo_201810JFVtxAux"                :   "xAOD::BTagVertexAuxContainer",
  "BTagging_AntiKt4EMPFlow"                        :   "xAOD::BTaggingContainer",
  "BTagging_AntiKt4EMPFlowAux"                     :   "xAOD::BTaggingAuxContainer",
  "BTagging_AntiKt4EMPFlowJFVtx"                   :   "xAOD::BTagVertexContainer",
  "BTagging_AntiKt4EMPFlowJFVtxAux"                :   "xAOD::BTagVertexAuxContainer",
  "BTagging_AntiKt4EMPFlow_201810JFVtx"                   :   "xAOD::BTagVertexContainer",
  "BTagging_AntiKt4EMPFlow_201810JFVtxAux"                :   "xAOD::BTagVertexAuxContainer",
  "BTagging_AntiKt4EMPFlow_201903JFVtx"                   :   "xAOD::BTagVertexContainer",
  "BTagging_AntiKt4EMPFlow_201903JFVtxAux"                :   "xAOD::BTagVertexAuxContainer",
  "BTagging_AntiKt4EMTopo_201810SecVtx"            :   "xAOD::VertexContainer",
  "BTagging_AntiKt4EMTopo_201810SecVtxAux"         :   "xAOD::VertexAuxContainer",
  "BTagging_AntiKt4EMPFlow_201810SecVtx"           :   "xAOD::VertexContainer",
  "BTagging_AntiKt4EMPFlow_201810SecVtxAux"        :   "xAOD::VertexAuxContainer",
  "BTagging_AntiKt4EMPFlow_201903SecVtx"           :   "xAOD::VertexContainer",
  "BTagging_AntiKt4EMPFlow_201903SecVtxAux"        :   "xAOD::VertexAuxContainer",
  "BTagging_AntiKtVR30Rmax4Rmin02Track_201810"            :   "xAOD::BTaggingContainer"   ,
  "BTagging_AntiKtVR30Rmax4Rmin02Track_201810Aux"         :   "xAOD::BTaggingAuxContainer",
  "BTagging_AntiKtVR30Rmax4Rmin02Track_201810JFVtx"       :   "xAOD::BTagVertexContainer" ,
  "BTagging_AntiKtVR30Rmax4Rmin02Track_201810JFVtxAux"    :   "xAOD::BTagVertexAuxContainer",
  "BTagging_AntiKtVR30Rmax4Rmin02Track_201810SecVtx"      :   "xAOD::VertexContainer"   ,
  "BTagging_AntiKtVR30Rmax4Rmin02Track_201810SecVtxAux"   :   "xAOD::VertexAuxContainer",
  "BTagging_AntiKtVR30Rmax4Rmin02Track_201903"            :   "xAOD::BTaggingContainer"   ,
  "BTagging_AntiKtVR30Rmax4Rmin02Track_201903Aux"         :   "xAOD::BTaggingAuxContainer",
  "BTagging_AntiKtVR30Rmax4Rmin02Track_201903JFVtx"       :   "xAOD::BTagVertexContainer" ,
  "BTagging_AntiKtVR30Rmax4Rmin02Track_201903JFVtxAux"    :   "xAOD::BTagVertexAuxContainer",
  "BTagging_AntiKtVR30Rmax4Rmin02Track_201903SecVtx"      :   "xAOD::VertexContainer"   ,
  "BTagging_AntiKtVR30Rmax4Rmin02Track_201903SecVtxAux"   :   "xAOD::VertexAuxContainer",
  "BTagging_AntiKtVR30Rmax4Rmin02Track_201810GhostTag"    :   "xAOD::BTaggingContainer"   ,
  "BTagging_AntiKtVR30Rmax4Rmin02Track_201810GhostTagAux" :   "xAOD::BTaggingAuxContainer",
  "BTagging_AntiKtVR30Rmax4Rmin02Track_201810GhostTagJFVtx"       :   "xAOD::BTagVertexContainer" ,
  "BTagging_AntiKtVR30Rmax4Rmin02Track_201810GhostTagJFVtxAux"    :   "xAOD::BTagVertexAuxContainer",
  "BTagging_AntiKtVR30Rmax4Rmin02Track_201810GhostTagSecVtx"      :   "xAOD::VertexContainer"   ,
  "BTagging_AntiKtVR30Rmax4Rmin02Track_201810GhostTagSecVtxAux"   :   "xAOD::VertexAuxContainer",
  "BTagging_AntiKt2Track"                          :   "xAOD::BTaggingContainer"   ,
  "BTagging_AntiKt2TrackAux"                       :   "xAOD::BTaggingAuxContainer",
  "BTagging_AntiKt2TrackJFVtx"                     :   "xAOD::BTagVertexContainer"   ,
  "BTagging_AntiKt2TrackJFVtxAux"                  :   "xAOD::BTagVertexAuxContainer",
  "BTagging_AntiKt2TrackSecVtx"                    :   "xAOD::VertexContainer"   ,
  "BTagging_AntiKt2TrackSecVtxAux"                 :   "xAOD::VertexAuxContainer",
  "AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt2SubJets"                 :   "xAOD::JetContainer"        ,
  "AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt2SubJetsAux"              :   "xAOD::JetAuxContainer"     ,
  "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt2Sub"            :   "xAOD::BTaggingContainer"   ,
  "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt2SubAux"         :   "xAOD::BTaggingAuxContainer",
  "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt2SubJFVtx"       :   "xAOD::BTagVertexContainer" ,
  "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt2SubJFVtxAux"    :   "xAOD::BTagVertexAuxContainer",
  "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt2SubSecVtx"      :   "xAOD::VertexContainer"   ,
  "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt2SubSecVtxAux"   :   "xAOD::VertexAuxContainer",
  "AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt3SubJets"                 :   "xAOD::JetContainer"        ,
  "AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt3SubJetsAux"              :   "xAOD::JetAuxContainer"     ,
  "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt3Sub"            :   "xAOD::BTaggingContainer"   ,
  "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt3SubAux"         :   "xAOD::BTaggingAuxContainer",
  "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt3SubJFVtx"       :   "xAOD::BTagVertexContainer" ,
  "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt3SubJFVtxAux"    :   "xAOD::BTagVertexAuxContainer",
  "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt3SubSecVtx"      :   "xAOD::VertexContainer"   ,
  "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExKt3SubSecVtxAux"   :   "xAOD::VertexAuxContainer",
  "AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2SubJets"                 :   "xAOD::JetContainer"        ,
  "AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2SubJetsAux"              :   "xAOD::JetAuxContainer"     ,
  "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2Sub"            :   "xAOD::BTaggingContainer"   ,
  "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2SubAux"         :   "xAOD::BTaggingAuxContainer",
  "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2SubJFVtx"       :   "xAOD::BTagVertexContainer" ,
  "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2SubJFVtxAux"    :   "xAOD::BTagVertexAuxContainer",
  "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2SubSecVtx"      :   "xAOD::VertexContainer"   ,
  "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2SubSecVtxAux"   :   "xAOD::VertexAuxContainer",
  "AntiKt8EMPFlowJets"                              :   "xAOD::JetContainer"        ,
  "AntiKt8EMPFlowJetsAux"                           :   "xAOD::JetAuxContainer"     ,
  "AntiKt8EMPFlowExKt2SubJets"                      :   "xAOD::JetContainer"        ,
  "AntiKt8EMPFlowExKt2SubJetsAux"                   :   "xAOD::JetAuxContainer"     ,
  "BTagging_AntiKt8EMPFlowExKt2Sub"                 :   "xAOD::BTaggingContainer"   ,
  "BTagging_AntiKt8EMPFlowExKt2SubAux"              :   "xAOD::BTaggingAuxContainer",
  "AntiKt8EMPFlowExKt3SubJets"                      :   "xAOD::JetContainer"        ,
  "AntiKt8EMPFlowExKt3SubJetsAux"                   :   "xAOD::JetAuxContainer"     ,
  "BTagging_AntiKt8EMPFlowExKt3Sub"                 :   "xAOD::BTaggingContainer"   ,
  "BTagging_AntiKt8EMPFlowExKt3SubAux"              :   "xAOD::BTaggingAuxContainer",
  "AntiKt8EMPFlowExKt2GASubJets"                      :   "xAOD::JetContainer"        ,
  "AntiKt8EMPFlowExKt2GASubJetsAux"                   :   "xAOD::JetAuxContainer"     ,
  "BTagging_AntiKt8EMPFlowExKt2GASub"                 :   "xAOD::BTaggingContainer"   ,
  "BTagging_AntiKt8EMPFlowExKt2GASubAux"              :   "xAOD::BTaggingAuxContainer",
  "AntiKt8EMPFlowExKt3GASubJets"                      :   "xAOD::JetContainer"        ,
  "AntiKt8EMPFlowExKt3GASubJetsAux"                   :   "xAOD::JetAuxContainer"     ,
  "BTagging_AntiKt8EMPFlowExKt3GASub"                 :   "xAOD::BTaggingContainer"   ,
  "BTagging_AntiKt8EMPFlowExKt3GASubAux"              :   "xAOD::BTaggingAuxContainer"
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
StaticContent += ["xAOD::VertexContainer#SoftBVrtClusterTool_MSVTight_Vertices"]
StaticContent += ["xAOD::VertexAuxContainer#SoftBVrtClusterTool_MSVTight_VerticesAux." + excludedVertexAuxData]
StaticContent += ["xAOD::VertexContainer#VrtSecInclusive_SoftBtagCandidateVertices"]
StaticContent += ["xAOD::VertexAuxContainer#VrtSecInclusive_SoftBtagCandidateVerticesAux."]

FTAG1SlimmingHelper.StaticContent = StaticContent
#----------------------------------------------------------------------

addJetOutputs(FTAG1SlimmingHelper,["FTAG1"])

FTAG1SlimmingHelper.IncludeMuonTriggerContent = False
FTAG1SlimmingHelper.IncludeEGammaTriggerContent = False
FTAG1SlimmingHelper.IncludeJetTriggerContent = True
FTAG1SlimmingHelper.IncludeEtMissTriggerContent = False
FTAG1SlimmingHelper.IncludeBJetTriggerContent = True
#----------------------------------------------------------------------

#FTAG1 TrigNav Thinning
FTAG1ThinningHelper = ThinningHelper( "FTAG1ThinningHelper" )
FTAG1ThinningHelper.TriggerChains = 'HLT_j[0-9]*|HLT_j[1-9][0-9]*_b.*perf.*'
FTAG1ThinningHelper.AppendToStream( FTAG1Stream )

FTAG1SlimmingHelper.AppendContentToStream(FTAG1Stream)
