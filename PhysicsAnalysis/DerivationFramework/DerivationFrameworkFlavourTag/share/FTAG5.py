#====================================================================
# FTAG5.py
# It requires the reductionConf flag FTAG5 in Reco_tf.py
#====================================================================

# Set up common services and job object.

# Framework core
from DerivationFrameworkCore.DerivationFrameworkMaster import (
    DerivationFrameworkJob, buildFileName)
from DerivationFrameworkCore.ThinningHelper import (
    ThinningHelper)
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkCore.DerivationFrameworkCoreConf import (
    DerivationFramework__DerivationKernel)

from DerivationFrameworkTools.DerivationFrameworkToolsConf import (
    DerivationFramework__xAODStringSkimmingTool)

# JetEtMiss
from DerivationFrameworkJetEtMiss.JetCommon import (
    OutputJets, addJetOutputs)
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import (
    addDefaultTrimmedJets, replaceAODReducedJets)
from DerivationFrameworkJetEtMiss.AntiKt4EMTopoJetsCPContent import (
    AntiKt4EMTopoJetsCPContent)

# tracking
from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import (
    Trk__TrackToVertexIPEstimator)

# flavor tagging
from DerivationFrameworkFlavourTag.HbbCommon import addVRJets

#====================================================================
# SKIMMING TOOLS
#====================================================================
skim_expr = 'count(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.pt > 250 * GeV && (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.eta) < 2.0)) > 0'

FTAG5StringSkimmingTool = DerivationFramework__xAODStringSkimmingTool(
    name = "FTAG5StringSkimmingTool",
    expression = skim_expr)

ToolSvc += FTAG5StringSkimmingTool
print FTAG5StringSkimmingTool

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
#====================================================================

#make IPE tool for TrackToVertexWrapper
FTAG5IPETool = Trk__TrackToVertexIPEstimator(name = "FTAG5IPETool")
ToolSvc += FTAG5IPETool
print FTAG5IPETool

#====================================================================
# CREATE PRIVATE SEQUENCE
#====================================================================

FTAG5Seq = CfgMgr.AthSequencer("FTAG5Sequence");
DerivationFrameworkJob += FTAG5Seq

#====================================================================
# Basic Jet Collections
#====================================================================

#put custom jet names here
OutputJets["FTAG5"] = ["AntiKtVR30Rmax4Rmin02TrackJets"]

reducedJetList = ["AntiKt2PV0TrackJets",
                  "AntiKt4PV0TrackJets",
                  "AntiKt10LCTopoJets",
                  "AntiKt4TruthJets"]
replaceAODReducedJets(reducedJetList,FTAG5Seq,"FTAG5")

addDefaultTrimmedJets(FTAG5Seq,"FTAG5",dotruth=True)

#===================================================================
# Variable Radius (VR) Jets
#===================================================================

# Create variable-R trackjets and dress AntiKt10LCTopo with ghost VR-trkjet
addVRJets(FTAG5Seq, "AntiKtVR30Rmax4Rmin02Track", "GhostVR30Rmax4Rmin02TrackJet",
          VRJetAlg="AntiKt", VRJetRadius=0.4, VRJetInputs="pv0track", #or should this be lctopo?
          ghostArea = 0 , ptmin = 2000, ptminFilter = 7000,
          variableRMinRadius = 0.02, variableRMassScale = 30000, calibOpt = "none")

# alias for VR
BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR30Rmax4Rmin02Track->AntiKtVR30Rmax4Rmin02Track,AntiKt4EMTopo"]

#===================================================================
# Tag custom or pre-built jet collections
#===================================================================

# FlavorTagInit(JetCollections  = ['AntiKt4EMPFlowJets',
#                                  'AntiKt4EMTopoJets'], Sequencer = FTAG5Seq)

#==================================================================
# Augment tracks in jets with additional information
#==================================================================
# NOTE: this is commented out until we figure out why the tool can't
# find jet collections.
#
FTAG5Seq += CfgMgr.BTagVertexAugmenter()
for jc in OutputJets["FTAG5"]:
   if 'Truth' in jc:
       continue
   FTAG5Seq += CfgMgr.BTagTrackAugmenter(
       "BTagTrackAugmenter_" + jc,
       OutputLevel=INFO,
       JetCollectionName = jc,
       TrackToVertexIPEstimator = FTAG5IPETool,
       SaveTrackVectors = True,
   )

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS
#====================================================================

FTAG5Seq += CfgMgr.DerivationFramework__DerivationKernel("FTAG5Kernel",
                                                         SkimmingTools = [FTAG5StringSkimmingTool],
                                                         AugmentationTools = []
                                                         )


#====================================================================
# SET UP STREAM
#====================================================================

# The base name (DAOD_FTAG5 here) must match the string in
streamName = derivationFlags.WriteDAOD_FTAG5Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_FTAG5Stream )
FTAG5Stream = MSMgr.NewPoolRootStream( streamName, fileName )
# Only events that pass the filters listed below are written out.
# Name must match that of the kernel above
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
FTAG5Stream.AcceptAlgs(["FTAG5Kernel"])

FTAG5SlimmingHelper = SlimmingHelper("FTAG5SlimmingHelper")

# nb: BTagging_AntiKt4EMTopo smart collection includes both AntiKt4EMTopoJets and BTagging_AntiKt4EMTopo 
# container variables. Thus BTagging_AntiKt4EMTopo is needed in SmartCollections as well as AllVariables
FTAG5SlimmingHelper.SmartCollections = ["Electrons","Muons",
                                        "InDetTrackParticles"]

FTAG5SlimmingHelper.AllVariables = [
    "BTagging_AntiKtVR30Rmax4Rmin02Track",
    "BTagging_AntiKtVR30Rmax4Rmin02TrackJFVtx",
    "PrimaryVertices",
    "TruthEvents",
    "TruthParticles",
    "TruthVertices",
    "MET_Truth",
    "MET_TruthRegions"]

# for FT5_bjetTriggerVtx in FTAllVars_bjetTriggerVtx:
#     FTAG5SlimmingHelper.AllVariables.append(FT5_bjetTriggerVtx)

FTAG5SlimmingHelper.ExtraVariables += [AntiKt4EMTopoJetsCPContent[1].replace("AntiKt4EMTopoJetsAux","AntiKt10LCTopoJets"),
                                       "InDetTrackParticles.truthMatchProbability.x.y.z.vx.vy.vz",
                                       "InDetTrackParticles.numberOfContribPixelLayers.numberOfTRTHits.numberOfInnermostPixelLayerSharedHits.numberOfNextToInnermostPixelLayerSharedHits",
                                       "InDetTrackParticles.numberOfPixelSplitHits.numberOfInnermostPixelLayerSplitHits.numberOfNextToInnermostPixelLayerSplitHits", 
                                       "InDetTrackParticles.hitPattern.radiusOfFirstHit",
                                       #"InDetTrackParticles.FTAG5_unbiased_d0.FTAG5_unbiased_z0.FTAG5_unbiased_d0Sigma.FTAG5_unbiased_z0Sigma",
                                       "CombinedMuonTrackParticles.vx.vy.vz",
                                       "ExtrapolatedMuonTrackParticles.vx.vy.vz",
                                       "MSOnlyExtrapolatedMuonTrackParticles.vx.vy.vz",
                                       "MuonSpectrometerTrackParticles.vx.vy.vz",
                                       "InDetForwardTrackParticles.phi.qOverP.theta",
                                       "BTagging_AntiKtVR30Rmax4Rmin02TrackSecVtx.-vxTrackAtVertex",
                                       "AntiKt10LCTopoJets.GhostVR30Rmax4Rmin02TrackJet.GhostVR30Rmax4Rmin02TrackJetPt.GhostVR30Rmax4Rmin02TrackJetCount",
                                       "InDetTrackParticles.btag_z0.btag_d0.btag_ip_d0.btag_ip_z0.btag_ip_phi.btag_ip_d0_sigma.btag_ip_z0_sigma.btag_track_displacement.btag_track_momentum"]

addJetOutputs(FTAG5SlimmingHelper,["FTAG5"],[],[])

#----------------------------------------------------------------------
# Add needed dictionary stuff
FTAG5SlimmingHelper.AppendToDictionary = {      
  "BTagging_AntiKt4EMPFlow"                        :   "xAOD::BTaggingContainer", 
  "BTagging_AntiKt4EMPFlowAux"                     :   "xAOD::BTaggingAuxContainer", 
  "BTagging_AntiKt4EMPFlowJFVtx"                   :   "xAOD::BTaggingContainer",
  "BTagging_AntiKt4EMPFlowJFVtxAux"                :   "xAOD::BTaggingAuxContainer",
  "AntiKtVR30Rmax4Rmin02Track"                     :   "xAOD::JetContainer"        ,
  "AntiKtVR30Rmax4Rmin02TrackAux"                  :   "xAOD::JetAuxContainer"     ,
  "BTagging_AntiKtVR30Rmax4Rmin02Track"            :   "xAOD::BTaggingContainer"   ,
  "BTagging_AntiKtVR30Rmax4Rmin02TrackAux"         :   "xAOD::BTaggingAuxContainer",
  "BTagging_AntiKtVR30Rmax4Rmin02TrackJFVtx"       :   "xAOD::BTagVertexContainer" ,
  "BTagging_AntiKtVR30Rmax4Rmin02TrackJFVtxAux"    :   "xAOD::BTagVertexAuxContainer",
  "BTagging_AntiKtVR30Rmax4Rmin02TrackSecVtx"      :   "xAOD::VertexContainer"   ,
  "BTagging_AntiKtVR30Rmax4Rmin02TrackSecVtxAux"   :   "xAOD::VertexAuxContainer",
  "BTagging_AntiKt2Track"                          :   "xAOD::BTaggingContainer"   ,
  "BTagging_AntiKt2TrackAux"                       :   "xAOD::BTaggingAuxContainer",
  "BTagging_AntiKt2TrackJFVtx"                     :   "xAOD::BTagVertexContainer"   ,
  "BTagging_AntiKt2TrackJFVtxAux"                  :   "xAOD::BTagVertexAuxContainer",
  "BTagging_AntiKt2TrackSecVtx"                    :   "xAOD::VertexContainer"   ,
  "BTagging_AntiKt2TrackSecVtxAux"                 :   "xAOD::VertexAuxContainer",
}
#----------------------------------------------------------------------

addJetOutputs(FTAG5SlimmingHelper,["FTAG5"])

FTAG5SlimmingHelper.IncludeMuonTriggerContent = False
FTAG5SlimmingHelper.IncludeEGammaTriggerContent = False
FTAG5SlimmingHelper.IncludeJetTriggerContent = False
FTAG5SlimmingHelper.IncludeEtMissTriggerContent = False
FTAG5SlimmingHelper.IncludeBJetTriggerContent = False

#FTAG5 TrigNav Thinning
FTAG5ThinningHelper = ThinningHelper( "FTAG5ThinningHelper" )
FTAG5ThinningHelper.TriggerChains = ''
FTAG5ThinningHelper.AppendToStream( FTAG5Stream )

FTAG5SlimmingHelper.AppendContentToStream(FTAG5Stream)
