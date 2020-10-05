#====================================================================
# FTAG4.py : for single lepton analyses
# It requires the reductionConf flag FTAG4 in Reco_tf.py
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
from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
from DerivationFrameworkFlavourTag.HbbCommon import addVRJets, addExKtCoM, addHbbTagger
from DerivationFrameworkFlavourTag.SoftBtagCommon import applySoftBtagging
from DerivationFrameworkCore.ThinningHelper import ThinningHelper

from DerivationFrameworkCore.LHE3WeightMetadata import *
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
from DerivationFrameworkJetEtMiss.AntiKt4EMTopoJetsCPContent import AntiKt4EMTopoJetsCPContent
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper

#====================================================================
# Create Private Sequence
#====================================================================

FTAG4Seq = CfgMgr.AthSequencer("FTAG4Sequence")

#====================================================================
# SKIMMING TOOLS
# (SKIMMING = REMOVING WHOLE EVENTS THAT FAIL CRITERIA)
# Create skimming tool, and create + add kernel to sequence
#====================================================================

# offline lepton skimming : require at least one lepton
# pt-thresholds were evaluated for lowest single lepton triggers with pt-thresholds of 26 GeV
# 0 == Muons.muonType correspond to Combined muons. DFCommonGoodMuon cuts on type are quality dependent
# we assume that mu-channel events will be requested to have combined muon offline.
# wo type cut, there is a relatively large (+~30%) leakage of mostly CaloTagged (muonType=3) muons
offlineElec = "(Electrons.pt > 24*GeV) && (Electrons.Medium || Electrons.DFCommonElectronsLHMedium)"
offlineMuon = "(Muons.pt > 24*GeV) && (Muons.DFCommonGoodMuon) && (0 == Muons.muonType)"
offlineExpression = "( (count("+offlineElec+") >= 1) ||  (count("+offlineMuon+") >= 1) )"
printfunc ('FTAG4: offline skimming expression : \n', offlineExpression)

FTAG4StringSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "FTAG4StringSkimmingTool",
                                                                      expression = offlineExpression)
ToolSvc += FTAG4StringSkimmingTool
printfunc (FTAG4StringSkimmingTool)

# triggers used for skimming:
# single lepton triggers: we want to include lowest un-prescaled
# we do not hard-code the exact triggers, as these vary with luminosity
# we however assume that the lowest un-prescaled triggers will have
# thresholds of 20 GeV <= pt < 1 TeV
triggers_e = [ "HLT_e[2-9][0-9]_.*", # WARNING: for triggers with 10<=pt<20 GeV, change to HLT_e[1-0][0-9]_.* !
               "HLT_e[1-9][0-9][0-9]_.*" ]
triggers_mu = [ "HLT_mu[2-9][0-9]_.*", # WARNING: for triggers with 10<=pt<20 GeV, change to HLT_mu[1-0][0-9]_.* !
                "HLT_mu[1-9][0-9][0-9]_.*" ]
triggersSkim = triggers_e + triggers_mu

FTAG4TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(name = "FTAG4TriggerSkimmingTool",
                                                                    TriggerListOR = triggersSkim)
ToolSvc += FTAG4TriggerSkimmingTool
printfunc (FTAG4TriggerSkimmingTool)

FTAG4Seq += CfgMgr.DerivationFramework__DerivationKernel("FTAG4SkimKernel",
                                                         SkimmingTools = [FTAG4StringSkimmingTool,FTAG4TriggerSkimmingTool])

#====================================================================
# TRUTH SETUP
#====================================================================
if globalflags.DataSource()!='data':
    from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents, addHFAndDownstreamParticles
    addStandardTruthContents()
    addHFAndDownstreamParticles()

#====================================================================
# Basic Jet Collections
#====================================================================

OutputJets["FTAG4"] = ["AntiKt4EMTopoJets",
                       "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810",
                       "AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2SubJets",
                       "AntiKt4EMPFlowJets"]

reducedJetList = ["AntiKt2PV0TrackJets",
                  "AntiKt4PV0TrackJets",
                  "AntiKt4TruthJets"]

extendedFlag = 1 # --- = 0 for Standard Taggers & =1 for ExpertTaggers
replaceAODReducedJets(reducedJetList,FTAG4Seq,"FTAG4", extendedFlag)

addDefaultTrimmedJets(FTAG4Seq,"FTAG4",dotruth=True)
#
# Adding ExCoM sub-jets for each trimmed large-R jet
#
ExCoMJetCollection__FatJet = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"
ExCoMJetCollection__SubJet = addExKtCoM(FTAG4Seq, ToolSvc, ExCoMJetCollection__FatJet, 2, False, subjetAlgName="CoM")

BTaggingFlags.CalibrationChannelAliases += ["AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2Sub->AntiKt4LCTopo,AntiKt4TopoEM,AntiKt4EMTopo"]

#==============================================================================
# Soft Tagging
#==============================================================================
from InDetVKalVxInJetTool.InDetVKalVxInJetFinder import InDetVKalVxInJetFinder

FTAG4Seq += CfgMgr.BTagVertexAugmenter()

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

FTAG4Seq += softTagAlgLoose

softTagAlgMedium = CfgMgr.SoftBVrt__SoftBVrtClusterTool(  "SoftBVrtClusterToolMedium",
                           OutputLevel=INFO, #DEBUG                                                                                          
                           )

softTagAlgMedium.SVFinderName = 'SoftBJetSVFinder'
softTagAlgMedium.TrackJetCollectionName = 'AntiKt4PV0TrackJets'
softTagAlgMedium.TrackSelectionTool.CutLevel = "LoosePrimary"
softTagAlgMedium.OperatingPoint = 'Medium'

FTAG4Seq += softTagAlgMedium

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

FTAG4Seq += softTagAlgTight

applySoftBtagging("softBtag", FTAG4Seq ) # SV tagger in VrtSecInclusive

#===================================================================
# Variable Radius (VR) Jets
#===================================================================

# Create variable-R trackjets and dress AntiKt10LCTopo with ghost VR-trkjet
addVRJets(FTAG4Seq, doFlipTagger=True)
addVRJets(FTAG4Seq, doFlipTagger=True, training='201903')
addHbbTagger(
    FTAG4Seq, ToolSvc,
    nn_file_name="BoostedJetTaggers/HbbTagger/Summer2018/MulticlassNetwork.json",
    nn_config_file="BoostedJetTaggers/HbbTaggerDNN/MulticlassConfigJune2018.json")

# alias for VR
BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR30Rmax4Rmin02Track->AntiKtVR30Rmax4Rmin02Track,AntiKt4EMTopo"]

#===================================================================
# Tag custom or pre-built jet collections
#===================================================================

FlavorTagInit(scheduleFlipped = True, JetCollections  = ['AntiKt4EMPFlowJets', 'AntiKt4EMTopoJets'],Sequencer = FTAG4Seq)

#====================================================================
# Add sequence (with all kernels needed) to DerivationFrameworkJob
#====================================================================

DerivationFrameworkJob += FTAG4Seq

#====================================================================
# SET UP STREAM
#====================================================================
# The base name (DAOD_FTAG4 here) must match the string in
streamName = derivationFlags.WriteDAOD_FTAG4Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_FTAG4Stream )
FTAG4Stream = MSMgr.NewPoolRootStream( streamName, fileName )
# Only events that pass the filters listed below are written out.
# Name must match that of the kernel above
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
FTAG4Stream.AcceptAlgs(["FTAG4SkimKernel"])

FTAG4SlimmingHelper = SlimmingHelper("FTAG4SlimmingHelper")

# nb: BTagging_AntiKt4EMTopo smart collection includes both AntiKt4EMTopoJets and BTagging_AntiKt4EMTopo
# container variables. Thus BTagging_AntiKt4EMTopo is needed in SmartCollections as well as AllVariables
FTAG4SlimmingHelper.SmartCollections = ["Electrons","Muons",
                                        "InDetTrackParticles",
                                        "AntiKt4EMTopoJets",
                                        "AntiKt4EMPFlowJets",
                                        "MET_Reference_AntiKt4EMPFlow",
                                        "MET_Reference_AntiKt4EMTopo"]

FTAG4SlimmingHelper.AllVariables = [
                                    "BTagging_AntiKtVR30Rmax4Rmin02Track_201810",
                                    "BTagging_AntiKtVR30Rmax4Rmin02Track_201810JFVtx",
                                    "BTagging_AntiKtVR30Rmax4Rmin02Track_201903",
                                    "BTagging_AntiKtVR30Rmax4Rmin02Track_201903JFVtx",
                                    "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2Sub",
                                    "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2SubJFVtx",
                                    "BTagging_AntiKt2Track",
                                    "BTagging_AntiKt2TrackJFVtx",
                                    "TruthVertices",
                                    "TruthEvents",
                                    "TruthHFWithDecayParticles",
                                    "TruthHFWithDecayVertices",
                                    "MET_Truth",
                                    "MET_TruthRegions",
                                    "TruthParticles",
                                    ]

FTAG4SlimmingHelper.ExtraVariables += [AntiKt4EMTopoJetsCPContent[1].replace("AntiKt4EMTopoJetsAux","AntiKt10LCTopoJets"),
                                       "InDetTrackParticles.truthMatchProbability.x.y.z.vx.vy.vz",
                                       "InDetTrackParticles.numberOfInnermostPixelLayerSplitHits.numberOfNextToInnermostPixelLayerSplitHits.numberOfNextToInnermostPixelLayerSharedHits",
                                       "InDetTrackParticles.numberOfPixelSplitHits.numberOfInnermostPixelLayerSharedHits.numberOfContribPixelLayers.hitPattern.radiusOfFirstHit",
                                       "InDetTrackParticles.is_selected.is_associated.is_svtrk_final.pt_wrtSV.eta_wrtSV.phi_wrtSV.d0_wrtSV.z0_wrtSV.errP_wrtSV.errd0_wrtSV.errz0_wrtSV.chi2_toSV",
                                       "PrimaryVertices.x.y.z.neutralWeights.numberDoF.sumPt2.chiSquared.covariance.trackWeights",
                                       "CombinedMuonTrackParticles.vx.vy.vz",
                                       "ExtrapolatedMuonTrackParticles.vx.vy.vz",
                                       "MSOnlyExtrapolatedMuonTrackParticles.vx.vy.vz",
                                       "MuonSpectrometerTrackParticles.vx.vy.vz",
                                       "BTagging_AntiKtVR30Rmax4Rmin02Track_201810SecVtx.-vxTrackAtVertex",
                                       "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2SubSecVtx.-vxTrackAtVertex",
                                       "BTagging_AntiKt2TrackSecVtx.-vxTrackAtVertex"]

FTAG4SlimmingHelper.SmartCollections += \
        ["AntiKt4EMTopoJets_BTagging201810",
         "AntiKt4EMPFlowJets_BTagging201810",
         "AntiKt4EMPFlowJets_BTagging201903",
          "BTagging_AntiKt4EMTopo_201810",
      "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810",
      "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903"]

FTAG4SlimmingHelper.AllVariables += \
        ["BTagging_AntiKt4EMTopo_201810",
         "BTagging_AntiKt4EMPFlow_201810",
         "BTagging_AntiKt4EMPFlow_201903",
         "BTagging_AntiKt4EMTopo_201810JFVtx",
         "BTagging_AntiKt4EMPFlow_201810JFVtx",
         "BTagging_AntiKt4EMPFlow_201903JFVtx",
     "BTagging_AntiKtVR30Rmax4Rmin02Track_201903",
     "BTagging_AntiKtVR30Rmax4Rmin02Track_201810",
         "AntiKt4EMTopoJets_BTagging201810",
         "AntiKt4EMPFlowJets_BTagging201810",
         "AntiKt4EMPFlowJets_BTagging201903"]

FTAG4SlimmingHelper.ExtraVariables += ["BTagging_AntiKt4EMTopo_201810SecVtx.-vxTrackAtVertex",
                                       "BTagging_AntiKt4EMPFlow_201810SecVtx.-vxTrackAtVertex",
                                       "BTagging_AntiKt4EMPFlow_201903SecVtx.-vxTrackAtVertex",
                                       "AntiKt4EMPFlowJets_BTagging201810.EMFrac.HECFrac.LArQuality.HECQuality.FracSamplingMax.NegativeE.AverageLArQF.FracSamplingMaxIndex.HadronConeExclTruthLabelID.GhostTrack",
                                       "AntiKt4EMPFlowJets_BTagging201810.Jvt.JvtRpt.JvtJvfcorr",
                                       "AntiKt4EMPFlowJets_BTagging201810.NumTrkPt1000.NumTrkPt500.SumPtTrkPt500.SumPtTrkPt1000",
                                       "AntiKt4EMPFlowJets_BTagging201903.EMFrac.HECFrac.LArQuality.HECQuality.FracSamplingMax.NegativeE.AverageLArQF.FracSamplingMaxIndex.HadronConeExclTruthLabelID.GhostTrack",
                                       "AntiKt4EMPFlowJets_BTagging201903.Jvt.JvtRpt.JvtJvfcorr",
                                       "AntiKt4EMPFlowJets_BTagging201903.NumTrkPt1000.NumTrkPt500.SumPtTrkPt500.SumPtTrkPt1000"]

#----------------------------------------------------------------------
# Add needed dictionary stuff
FTAG4SlimmingHelper.AppendToDictionary = {
  "TruthHFWithDecayParticles"                  :   "xAOD::TruthParticleContainer",
  "TruthHFWithDecayParticlesAux"               :   "xAOD::TruthParticleAuxContainer",
  "TruthHFWithDecayVertices"                   :   "xAOD::TruthVertexContainer",
  "TruthHFWithDecayVerticesAux"                :   "xAOD::TruthVertexAuxContainer",
  "BTagging_AntiKt2Track"                      :   "xAOD::BTaggingContainer"   ,
  "BTagging_AntiKt2TrackAux"                   :   "xAOD::BTaggingAuxContainer",
  "BTagging_AntiKt2TrackJFVtx"                 :   "xAOD::BTagVertexContainer"   ,
  "BTagging_AntiKt2TrackJFVtxAux"              :   "xAOD::BTagVertexAuxContainer",
  "BTagging_AntiKt2TrackSecVtx"                :   "xAOD::VertexContainer"   ,
  "BTagging_AntiKt2TrackSecVtxAux"             :   "xAOD::VertexAuxContainer",
  "AntiKtVR30Rmax4Rmin02Track"                     :   "xAOD::JetContainer"        ,
  "AntiKtVR30Rmax4Rmin02TrackAux"                  :   "xAOD::JetAuxContainer"     ,
  "BTagging_AntiKt4EMPFlow"                        :   "xAOD::BTaggingContainer",
  "BTagging_AntiKt4EMPFlowAux"                     :   "xAOD::BTaggingAuxContainer",
  "BTagging_AntiKt4EMPFlowJFVtx"                   :   "xAOD::BTagVertexContainer",
  "BTagging_AntiKt4EMPFlowJFVtxAux"                :   "xAOD::BTagVertexAuxContainer",
  "BTagging_AntiKt4EMTopo_201810JFVtx"              :   "xAOD::BTagVertexContainer",
  "BTagging_AntiKt4EMTopo_201810JFVtxAux"           :   "xAOD::BTagVertexAuxContainer",
  "BTagging_AntiKt4EMPFlow_201810JFVtx"             :   "xAOD::BTagVertexContainer",
  "BTagging_AntiKt4EMPFlow_201810JFVtxAux"          :   "xAOD::BTagVertexAuxContainer",
  "BTagging_AntiKt4EMPFlow_201903JFVtx"             :   "xAOD::BTagVertexContainer",
  "BTagging_AntiKt4EMPFlow_201903JFVtxAux"          :   "xAOD::BTagVertexAuxContainer",
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
  "AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2SubJets"                 :   "xAOD::JetContainer"        ,
  "AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2SubJetsAux"              :   "xAOD::JetAuxContainer"     ,
  "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2Sub"            :   "xAOD::BTaggingContainer"   ,
  "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2SubAux"         :   "xAOD::BTaggingAuxContainer",
  "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2SubJFVtx"       :   "xAOD::BTagVertexContainer" ,
  "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2SubJFVtxAux"    :   "xAOD::BTagVertexAuxContainer",
  "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2SubSecVtx"      :   "xAOD::VertexContainer"   ,
  "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2SubSecVtxAux"   :   "xAOD::VertexAuxContainer",
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

FTAG4SlimmingHelper.StaticContent = StaticContent
#----------------------------------------------------------------------

addJetOutputs(FTAG4SlimmingHelper,["FTAG4"])

FTAG4SlimmingHelper.IncludeMuonTriggerContent = True
FTAG4SlimmingHelper.IncludeEGammaTriggerContent = True
FTAG4SlimmingHelper.IncludeJetTriggerContent = True
FTAG4SlimmingHelper.IncludeEtMissTriggerContent = False
FTAG4SlimmingHelper.IncludeBJetTriggerContent = True

#FTAG4 TrigNav Thinning
FTAG4ThinningHelper = ThinningHelper( "FTAG4ThinningHelper" )
FTAG4ThinningHelper.TriggerChains = 'HLT_e[2-9][0-9]_.*|HLT_e[1-9][0-9][0-9]_.*|HLT_mu[2-9][0-9]_.*|HLT_mu[1-9][0-9][0-9]_.*|HLT_mu[2-9][0-9]'
FTAG4ThinningHelper.AppendToStream( FTAG4Stream )

FTAG4SlimmingHelper.AppendContentToStream(FTAG4Stream)
