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
from DerivationFrameworkFlavourTag.HbbCommon import addVRJets
from DerivationFrameworkCore.ThinningHelper import ThinningHelper

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
print 'FTAG4: offline skimming expression : \n', offlineExpression

FTAG4StringSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "FTAG4StringSkimmingTool",
                                                                      expression = offlineExpression)
ToolSvc += FTAG4StringSkimmingTool
print FTAG4StringSkimmingTool

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
print FTAG4TriggerSkimmingTool

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
                       "AntiKtVR30Rmax4Rmin02TrackJets"]

reducedJetList = ["AntiKt2PV0TrackJets",
                  "AntiKt4PV0TrackJets",
                  "AntiKt4TruthJets"]

extendedFlag = 0 # --- = 0 for Standard Taggers & =1 for ExpertTaggers
replaceAODReducedJets(reducedJetList,FTAG4Seq,"FTAG4", extendedFlag)

addDefaultTrimmedJets(FTAG4Seq,"FTAG4",dotruth=True)

#===================================================================
# Variable Radius (VR) Jets
#===================================================================

# Create variable-R trackjets and dress AntiKt10LCTopo with ghost VR-trkjet 
addVRJets(FTAG4Seq, "AntiKtVR30Rmax4Rmin02Track", "GhostVR30Rmax4Rmin02TrackJet",
          VRJetAlg="AntiKt", VRJetRadius=0.4, VRJetInputs="pv0track", #or should this be lctopo?
          ghostArea = 0 , ptmin = 2000, ptminFilter = 7000,
          variableRMinRadius = 0.02, variableRMassScale = 30000, calibOpt = "none")

# alias for VR
BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR30Rmax4Rmin02Track->AntiKtVR30Rmax4Rmin02Track,AntiKt4EMTopo"]

#===================================================================
# Tag custom or pre-built jet collections
#===================================================================

FlavorTagInit(scheduleFlipped = True, JetCollections  = ['AntiKt4EMTopoJets'],Sequencer = FTAG4Seq)

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
                                        "BTagging_AntiKt4EMTopo",
                                        "MET_Reference_AntiKt4EMTopo"]

FTAG4SlimmingHelper.AllVariables = ["AntiKt4EMTopoJets",
                                    "BTagging_AntiKtVR30Rmax4Rmin02Track",
                                    "BTagging_AntiKtVR30Rmax4Rmin02TrackJFVtx",
                                    "BTagging_AntiKt4EMTopo",
                                    "BTagging_AntiKt4EMTopoJFVtx",
                                    "BTagging_AntiKt2Track",
                                    "BTagging_AntiKt2TrackJFVtx", 
                                    "TruthVertices",
                                    "TruthEvents",
                                    "MET_Truth",
                                    "MET_TruthRegions",
                                    "TruthParticles",
                                    ]

FTAG4SlimmingHelper.ExtraVariables += [AntiKt4EMTopoJetsCPContent[1].replace("AntiKt4EMTopoJetsAux","AntiKt10LCTopoJets"),
                                       "InDetTrackParticles.truthMatchProbability.x.y.z.vx.vy.vz",
                                       "InDetTrackParticles.numberOfInnermostPixelLayerSplitHits.numberOfNextToInnermostPixelLayerSplitHits.numberOfNextToInnermostPixelLayerSharedHits",
                                       "InDetTrackParticles.numberOfPixelSplitHits.numberOfInnermostPixelLayerSharedHits.numberOfContribPixelLayers.hitPattern.radiusOfFirstHit",
                                       "PrimaryVertices.x.y.z.neutralWeights.numberDoF.sumPt2.chiSquared.covariance.trackWeights",
                                       "CombinedMuonTrackParticles.vx.vy.vz",
                                       "ExtrapolatedMuonTrackParticles.vx.vy.vz",
                                       "MSOnlyExtrapolatedMuonTrackParticles.vx.vy.vz",
                                       "MuonSpectrometerTrackParticles.vx.vy.vz",
                                       "BTagging_AntiKtVR30Rmax4Rmin02TrackSecVtx.-vxTrackAtVertex",
                                       "BTagging_AntiKt4EMTopoSecVtx.-vxTrackAtVertex",
                                       "BTagging_AntiKt2TrackSecVtx.-vxTrackAtVertex"]

#----------------------------------------------------------------------
# Add needed dictionary stuff
FTAG4SlimmingHelper.AppendToDictionary = {
  "BTagging_AntiKt2Track"                      :   "xAOD::BTaggingContainer"   ,
  "BTagging_AntiKt2TrackAux"                   :   "xAOD::BTaggingAuxContainer",
  "BTagging_AntiKt2TrackJFVtx"                 :   "xAOD::BTagVertexContainer"   ,
  "BTagging_AntiKt2TrackJFVtxAux"              :   "xAOD::BTagVertexAuxContainer",
  "BTagging_AntiKt2TrackSecVtx"                :   "xAOD::VertexContainer"   ,
  "BTagging_AntiKt2TrackSecVtxAux"             :   "xAOD::VertexAuxContainer",
  "AntiKtVR30Rmax4Rmin02Track"                     :   "xAOD::JetContainer"        ,
  "AntiKtVR30Rmax4Rmin02TrackAux"                  :   "xAOD::JetAuxContainer"     ,
  "BTagging_AntiKtVR30Rmax4Rmin02Track"            :   "xAOD::BTaggingContainer"   ,
  "BTagging_AntiKtVR30Rmax4Rmin02TrackAux"         :   "xAOD::BTaggingAuxContainer",
  "BTagging_AntiKtVR30Rmax4Rmin02TrackJFVtx"       :   "xAOD::BTagVertexContainer" ,
  "BTagging_AntiKtVR30Rmax4Rmin02TrackJFVtxAux"    :   "xAOD::BTagVertexAuxContainer",
  "BTagging_AntiKtVR30Rmax4Rmin02TrackSecVtx"      :   "xAOD::VertexContainer"   ,
  "BTagging_AntiKtVR30Rmax4Rmin02TrackSecVtxAux"   :   "xAOD::VertexAuxContainer",

}
#----------------------------------------------------------------------
addJetOutputs(FTAG4SlimmingHelper,["FTAG4"])

FTAG4SlimmingHelper.IncludeMuonTriggerContent = True
FTAG4SlimmingHelper.IncludeEGammaTriggerContent = True
FTAG4SlimmingHelper.IncludeJetTriggerContent = True
FTAG4SlimmingHelper.IncludeEtMissTriggerContent = False
FTAG4SlimmingHelper.IncludeBJetTriggerContent = True

#FTAG4 TrigNav Thinning
FTAG4ThinningHelper = ThinningHelper( "FTAG4ThinningHelper" )
FTAG4ThinningHelper.TriggerChains = 'HLT_e[2-9][0-9]_.*|HLT_e[1-9][0-9][0-9]_.*|HLT_mu[2-9][0-9]_.*|HLT_mu[1-9][0-9][0-9]_.*'
FTAG4ThinningHelper.AppendToStream( FTAG4Stream )

FTAG4SlimmingHelper.AppendContentToStream(FTAG4Stream)

