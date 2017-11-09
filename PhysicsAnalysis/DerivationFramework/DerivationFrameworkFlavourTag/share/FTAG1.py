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
# SKIMMING TOOLS
# (SKIMMING = REMOVING WHOLE EVENTS THAT FAIL CRITERIA)
#====================================================================

# triggers used for skimming:
triggers_jX = ["HLT_j[0-9]*"] 
triggers_bperf = ["HLT_j[1-9][0-9]*_b.*perf.*"]
triggersSkim = triggers_jX + triggers_bperf

FTAG1TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(name = "FTAG1TriggerSkimmingTool",
                                                                    TriggerListOR = triggersSkim)
ToolSvc += FTAG1TriggerSkimmingTool
print FTAG1TriggerSkimmingTool

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

#Add unbiased track parameters to track particles
#FTAG1IPETool = Trk__TrackToVertexIPEstimator(name = "FTAG1IPETool")
#ToolSvc += FTAG1IPETool
#print FTAG1IPETool

#FTAG1TrackToVertexWrapper= DerivationFramework__TrackToVertexWrapper(name = "FTAG1TrackToVertexWrapper",
#        TrackToVertexIPEstimator = FTAG1IPETool,
#        DecorationPrefix = "FTAG1",
#        ContainerName = "InDetTrackParticles")
#ToolSvc += FTAG1TrackToVertexWrapper
#print FTAG1TrackToVertexWrapper

#====================================================================
# CREATE PRIVATE SEQUENCE
#====================================================================

FTAG1Seq = CfgMgr.AthSequencer("FTAG1Sequence");
DerivationFrameworkJob += FTAG1Seq

#====================================================================
# Basic Jet Collections 
#====================================================================

#put custom jet names here
OutputJets["FTAG1"] = ["AntiKtVR30Rmax4Rmin02TrackJets",
                       "AntiKt4EMTopoJets"]

reducedJetList = ["AntiKt2PV0TrackJets",
                  "AntiKt4PV0TrackJets",
                  "AntiKt10LCTopoJets",
                  "AntiKt4TruthJets"] 
replaceAODReducedJets(reducedJetList,FTAG1Seq,"FTAG1")

addDefaultTrimmedJets(FTAG1Seq,"FTAG1",dotruth=True)

#===================================================================
# Variable Radius (VR) Jets 
#===================================================================

# Create variable-R trackjets and dress AntiKt10LCTopo with ghost VR-trkjet 
addVRJets(FTAG1Seq, "AntiKtVR30Rmax4Rmin02Track", "GhostVR30Rmax4Rmin02TrackJet",
          VRJetAlg="AntiKt", VRJetRadius=0.4, VRJetInputs="pv0track", #or should this be lctopo?
          ghostArea = 0 , ptmin = 2000, ptminFilter = 7000,
          variableRMinRadius = 0.02, variableRMassScale = 30000, calibOpt = "none")

# alias for VR
BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR30Rmax4Rmin02Track->AntiKtVR30Rmax4Rmin02Track,AntiKt4EMTopo"]

#===================================================================
# Tag custom or pre-built jet collections
#===================================================================

FlavorTagInit(isFTAG1 = True, JetCollections  = ['AntiKt4EMTopoJets'],Sequencer = FTAG1Seq)

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS
#====================================================================

FTAG1Seq += CfgMgr.DerivationFramework__DerivationKernel("FTAG1Kernel",
                                                         SkimmingTools = [FTAG1TriggerSkimmingTool],
                                                         AugmentationTools = [FTAG1DstarVertexing]
                                                         )

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
FTAG1Stream.AcceptAlgs(["FTAG1Kernel"])

FTAG1SlimmingHelper = SlimmingHelper("FTAG1SlimmingHelper")

# nb: BTagging_AntiKt4EMTopo smart collection includes both AntiKt4EMTopoJets and BTagging_AntiKt4EMTopo
# container variables. Thus BTagging_AntiKt4EMTopo is needed in SmartCollections as well as AllVariables
FTAG1SlimmingHelper.SmartCollections = ["Electrons","Muons",
                                        "PrimaryVertices",
                                        "InDetTrackParticles",
                                        "AntiKt4EMTopoJets", "BTagging_AntiKt4EMTopo",
                                        "MET_Reference_AntiKt4EMTopo"]

FTAG1SlimmingHelper.AllVariables = ["AntiKt4EMTopoJets",
                                    "BTagging_AntiKtVR30Rmax4Rmin02Track",
                                    "BTagging_AntiKtVR30Rmax4Rmin02TrackJFVtx",
                                    "BTagging_AntiKt4EMTopo",
                                    "BTagging_AntiKt2Track",
                                    "BTagging_AntiKt4EMTopoJFVtx",
                                    "BTagging_AntiKt2TrackJFVtx",
                                    "TruthEvents",
                                    "MET_Truth",
                                    "MET_TruthRegions",
                                    "TruthParticles",
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
                                       "AntiKt10LCTopoJets.ConeExclBHadronsFinal",
                                       "AntiKt10LCTopoJets.GhostAntiKt2TrackJet.GhostAntiKt2TrackJetPt.GhostAntiKt2TrackJetCount",
                                       "AntiKt10LCTopoJets.GhostVR30Rmax4Rmin02TrackJet.GhostVR30Rmax4Rmin02TrackJetPt.GhostVR30Rmax4Rmin02TrackJetCount",
                                       "BTagging_AntiKt4EMTopoSecVtx.-vxTrackAtVertex",
                                       "BTagging_AntiKt2TrackSecVtx.-vxTrackAtVertex",
                                       "BTagging_AntiKtVR30Rmax4Rmin02TrackSecVtx.-vxTrackAtVertex",
                                       #"InDetTrackParticles.FTAG1_unbiased_d0.FTAG1_unbiased_z0.FTAG1_unbiased_d0Sigma.FTAG1_unbiased_z0Sigma", 
                                       "CaloCalTopoClusters.calM.calE.calEta.calPhi",
                                       "PrimaryVertices.x.y.trackParticleLinks.vertexType.neutralParticleLinks",
                                       "HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF.vz",
                                       "HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bjet_FTF.vz",
                                       "HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bjet_IDTrig.vz",                        
                                       FTAG1DstarAug+".-vxTrackAtVertex"
]

for FT1_bjetTriggerTracks in FTExtraVars_bjetTriggerTracks:
    FTAG1SlimmingHelper.ExtraVariables.append(FT1_bjetTriggerTracks)

addJetOutputs(FTAG1SlimmingHelper,["FTAG1"],[],[])

#----------------------------------------------------------------------
# Add needed dictionary stuff
FTAG1SlimmingHelper.AppendToDictionary = {
  FTAG1DstarAug                                    :   "xAOD::VertexContainer",
  FTAG1DstarAug+"Aux"                              :   "xAOD::VertexAuxContainer",
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
