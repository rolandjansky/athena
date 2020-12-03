#====================================================================
# STDM13.py : for single lepton analyses
# It requires the reductionConf flag STDM13 in Reco_tf.py
#====================================================================
# Set up common services and job object.
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
from DerivationFrameworkFlavourTag.HbbCommon import addExKtCoM, addHbbTagger
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

STDM13Seq = CfgMgr.AthSequencer("STDM13Sequence")

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
print 'STDM13: offline skimming expression : \n', offlineExpression

STDM13StringSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "STDM13StringSkimmingTool",
                                                                      expression = offlineExpression)
ToolSvc += STDM13StringSkimmingTool
print STDM13StringSkimmingTool

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

STDM13TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(name = "STDM13TriggerSkimmingTool",
                                                                    TriggerListOR = triggersSkim)
ToolSvc += STDM13TriggerSkimmingTool
print STDM13TriggerSkimmingTool

STDM13Seq += CfgMgr.DerivationFramework__DerivationKernel("STDM13SkimKernel",
                                                         SkimmingTools = [STDM13StringSkimmingTool,STDM13TriggerSkimmingTool])


#====================================================================
# TRUTH SETUP
#====================================================================
if globalflags.DataSource()!='data':
    from DerivationFrameworkMCTruth.MCTruthCommon import *
    addStandardTruthContents()
    addHFAndDownstreamParticles()

#====================================================================
# Basic Jet Collections
#====================================================================

OutputJets["STDM13"] = ["AntiKt4EMTopoJets",
                       "AntiKt4EMPFlowJets"]

reducedJetList = ["AntiKt4TruthJets"]

extendedFlag = 1 # --- = 0 for Standard Taggers & =1 for ExpertTaggers
replaceAODReducedJets(reducedJetList,STDM13Seq,"STDM13", extendedFlag)


#===================================================================
# Tag custom or pre-built jet collections
#===================================================================

FlavorTagInit(scheduleFlipped = True, JetCollections  = ['AntiKt4EMPFlowJets', 'AntiKt4EMTopoJets'],Sequencer = STDM13Seq)

#====================================================================
# Add sequence (with all kernels needed) to DerivationFrameworkJob
#====================================================================

DerivationFrameworkJob += STDM13Seq

#====================================================================
# SET UP STREAM
#====================================================================
# The base name (DAOD_STDM13 here) must match the string in
streamName = derivationFlags.WriteDAOD_STDM13Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_STDM13Stream )
STDM13Stream = MSMgr.NewPoolRootStream( streamName, fileName )
# Only events that pass the filters listed below are written out.
# Name must match that of the kernel above
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
STDM13Stream.AcceptAlgs(["STDM13SkimKernel"])

STDM13SlimmingHelper = SlimmingHelper("STDM13SlimmingHelper")

from DerivationFrameworkSM.STDMExtraContent import *
if globalflags.DataSource()=='geant4':
    from DerivationFrameworkSM.STDMCommonTruthTools import *
    STDM13SlimmingHelper.AllVariables += ExtraContainersTruth

# nb: BTagging_AntiKt4EMTopo smart collection includes both AntiKt4EMTopoJets and BTagging_AntiKt4EMTopo
# container variables. Thus BTagging_AntiKt4EMTopo is needed in SmartCollections as well as AllVariables
STDM13SlimmingHelper.SmartCollections = ["Electrons","Muons",
                                        "InDetTrackParticles",
                                        "AntiKt4EMTopoJets",
                                        "AntiKt4EMPFlowJets",
                                        "MET_Reference_AntiKt4EMPFlow",
                                        "MET_Reference_AntiKt4EMTopo"]

STDM13SlimmingHelper.AllVariables = ["BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2Sub",
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

STDM13SlimmingHelper.ExtraVariables += [AntiKt4EMTopoJetsCPContent[1].replace("AntiKt4EMTopoJetsAux","AntiKt10LCTopoJets"),
                                       "InDetTrackParticles.truthMatchProbability.x.y.z.vx.vy.vz",
                                       "InDetTrackParticles.numberOfInnermostPixelLayerSplitHits.numberOfNextToInnermostPixelLayerSplitHits.numberOfNextToInnermostPixelLayerSharedHits",
                                       "InDetTrackParticles.numberOfPixelSplitHits.numberOfInnermostPixelLayerSharedHits.numberOfContribPixelLayers.hitPattern.radiusOfFirstHit",
                                       "InDetTrackParticles.is_selected.is_associated.is_svtrk_final.pt_wrtSV.eta_wrtSV.phi_wrtSV.d0_wrtSV.z0_wrtSV.errP_wrtSV.errd0_wrtSV.errz0_wrtSV.chi2_toSV",
                                       "PrimaryVertices.x.y.z.neutralWeights.numberDoF.sumPt2.chiSquared.covariance.trackWeights",
                                       "CombinedMuonTrackParticles.vx.vy.vz",
                                       "ExtrapolatedMuonTrackParticles.vx.vy.vz",
                                       "MSOnlyExtrapolatedMuonTrackParticles.vx.vy.vz",
                                       "MuonSpectrometerTrackParticles.vx.vy.vz",
                                       "BTagging_AntiKt10LCTopoTrimmedPtFrac5SmallR20ExCoM2SubSecVtx.-vxTrackAtVertex",
                                       "BTagging_AntiKt2TrackSecVtx.-vxTrackAtVertex"]

STDM13SlimmingHelper.SmartCollections += \
        ["AntiKt4EMTopoJets_BTagging201810",
         "AntiKt4EMPFlowJets_BTagging201810",
         "AntiKt4EMPFlowJets_BTagging201903",
          "BTagging_AntiKt4EMTopo_201810"]

STDM13SlimmingHelper.AllVariables += \
        ["BTagging_AntiKt4EMTopo_201810",
         "BTagging_AntiKt4EMPFlow_201810",
         "BTagging_AntiKt4EMPFlow_201903",
         "BTagging_AntiKt4EMTopo_201810JFVtx",
         "BTagging_AntiKt4EMPFlow_201810JFVtx",
         "BTagging_AntiKt4EMPFlow_201903JFVtx",
         "AntiKt4EMTopoJets_BTagging201810",
         "AntiKt4EMPFlowJets_BTagging201810",
         "AntiKt4EMPFlowJets_BTagging201903"]

STDM13SlimmingHelper.ExtraVariables += ["BTagging_AntiKt4EMTopo_201810SecVtx.-vxTrackAtVertex",
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
STDM13SlimmingHelper.AppendToDictionary = {
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


addJetOutputs(STDM13SlimmingHelper,["STDM13"])

STDM13SlimmingHelper.IncludeMuonTriggerContent = True
STDM13SlimmingHelper.IncludeEGammaTriggerContent = True
STDM13SlimmingHelper.IncludeJetTriggerContent = True
STDM13SlimmingHelper.IncludeEtMissTriggerContent = False
STDM13SlimmingHelper.IncludeBJetTriggerContent = True

#STDM13 TrigNav Thinning
STDM13ThinningHelper = ThinningHelper( "STDM13ThinningHelper" )
STDM13ThinningHelper.TriggerChains = 'HLT_e[2-9][0-9]_.*|HLT_e[1-9][0-9][0-9]_.*|HLT_mu[2-9][0-9]_.*|HLT_mu[1-9][0-9][0-9]_.*|HLT_mu[2-9][0-9]'
STDM13ThinningHelper.AppendToStream( STDM13Stream )

STDM13SlimmingHelper.AppendContentToStream(STDM13Stream)
