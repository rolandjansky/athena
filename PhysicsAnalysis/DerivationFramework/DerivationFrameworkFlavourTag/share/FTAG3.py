#====================================================================
# FTAG3.py
# It requires the reductionConf flag FTAG3 in Reco_tf.py
#====================================================================

# Set up common services and job object.
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
#from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets,addDefaultTrimmedJets
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
from DerivationFrameworkCore.ThinningHelper import ThinningHelper

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool,DerivationFramework__xAODStringSkimmingTool
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper

#====================================================================
# Create Private Sequence
#====================================================================

FTAG3Seq = CfgMgr.AthSequencer("FTAG3Sequence");

#====================================================================
# SKIMMING TOOLS
# (SKIMMING = REMOVING WHOLE EVENTS THAT FAIL CRITERIA)
# Create skimming tool, and create + add kernel to sequence
#====================================================================
triggers = []
#2016 mu-jet triggers without online b-tagging information
triggers.append("HLT_mu4_j15_dr05")
triggers.append("HLT_mu4_j25_dr05")
triggers.append("HLT_mu4_j35_dr05")
triggers.append("HLT_mu4_j55_dr05")
triggers.append("HLT_mu6_j85_dr05")
triggers.append("HLT_mu6_j110_dr05")
triggers.append("HLT_mu6_j150_dr05")
triggers.append("HLT_mu6_j175_dr05")
#2016 menu mu-jet triggers with online b-tagging information
triggers.append("HLT_mu4_j15_bperf_split_dr05_dz02")
triggers.append("HLT_mu4_j25_bperf_split_dr05_dz02")
triggers.append("HLT_mu4_j35_bperf_split_dr05_dz02")
triggers.append("HLT_mu4_j55_bperf_split_dr05_dz02")
triggers.append("HLT_mu6_j85_bperf_split_dr05_dz02")
triggers.append("HLT_mu6_j110_bperf_split_dr05_dz02")
triggers.append("HLT_mu6_j150_bperf_split_dr05_dz02")
triggers.append("HLT_mu6_j175_bperf_split_dr05_dz02")
#2018 additional mu-jet triggers
triggers.append("HLT_mu4_j15_boffperf_split_dr05_dz02")
triggers.append("HLT_mu4_j15_gsc35_boffperf_split_dr05_dz02")
triggers.append("HLT_mu4_j25_boffperf_split_dr05_dz02")
triggers.append("HLT_mu4_j35_boffperf_split_dr05_dz02")
triggers.append("HLT_mu4_j35_gsc55_boffperf_split_dr05_dz02")
triggers.append("HLT_mu6_j60_gsc110_boffperf_split_dr05_dz02")
triggers.append("HLT_mu6_j60_gsc85_boffperf_split_dr05_dz02")
triggers.append("HLT_mu6_j110_gsc150_boffperf_split_dr05_dz02") 
triggers.append("HLT_mu6_j110_gsc150_bperf_split_dr05_dz02")
triggers.append("HLT_mu6_j150_gsc175_boffperf_split_dr05_dz02")
triggers.append("HLT_mu6_j175_gsc260_boffperf_split_dr05_dz02")

if globalflags.DataSource()=='data':
    # muon-in-jet triggers are buggy in MC, so only apply them in data
    FTAG3TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(name = "FTAG3TriggerSkimmingTool",
                                                                    TriggerListOR = triggers )
    ToolSvc += FTAG3TriggerSkimmingTool
    printfunc (FTAG3TriggerSkimmingTool)
    FTAG3Seq += CfgMgr.DerivationFramework__DerivationKernel("FTAG3SkimKernel",
                                                         SkimmingTools = [FTAG3TriggerSkimmingTool] )

if globalflags.DataSource()!='data':
    # since we aren't using a trigger skim, apply a muon pT cut to avoid gigantic dijet samples
    FTAG3StringSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "FTAG3StringSkimmingTool",
                                  expression = 'count( (Muons.pt > 4*GeV) && (Muons.DFCommonGoodMuon) )  >= 1')
    ToolSvc += FTAG3StringSkimmingTool
    printfunc (FTAG3StringSkimmingTool)
    FTAG3Seq += CfgMgr.DerivationFramework__DerivationKernel("FTAG3SkimKernel",
                                                             SkimmingTools = [FTAG3StringSkimmingTool] )

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

OutputJets["FTAG3"] = ["AntiKt4EMTopoJets"]

reducedJetList = ["AntiKt2PV0TrackJets",
                  "AntiKt4PV0TrackJets",
                  "AntiKt10TruthWZJets",
                  "AntiKt4TruthJets"]

extendedFlag = 0 # --- = 0 for Standard Taggers & =1 for ExpertTaggers
replaceAODReducedJets(reducedJetList,FTAG3Seq,"FTAG3", extendedFlag)

addDefaultTrimmedJets(FTAG3Seq,"FTAG3",dotruth=True)

#===================================================================
# Tag custom or pre-built jet collections 
#===================================================================

FlavorTagInit(JetCollections  = ['AntiKt4EMTopoJets'],Sequencer = FTAG3Seq)

#====================================================================
# Add sequence (with all kernels needed) to DerivationFrameworkJob 
#====================================================================

DerivationFrameworkJob += FTAG3Seq

#====================================================================
# SET UP STREAM
#====================================================================

# The base name (DAOD_FTAG3 here) must match the string in
streamName = derivationFlags.WriteDAOD_FTAG3Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_FTAG3Stream )
FTAG3Stream = MSMgr.NewPoolRootStream( streamName, fileName )
FTAG3Stream.AcceptAlgs(["FTAG3SkimKernel"])

FTAG3SlimmingHelper = SlimmingHelper("FTAG3SlimmingHelper")

# nb: BTagging_AntiKt4EMTopo smart collection includes both AntiKt4EMTopoJets and BTagging_AntiKt4EMTopo
# container variables. Thus BTagging_AntiKt4EMTopo is needed in SmartCollections as well as AllVariables 
FTAG3SlimmingHelper.SmartCollections = ["Electrons","Muons",
                                        "InDetTrackParticles",
                                        "PrimaryVertices",
                                        "AntiKt4EMTopoJets",
                                        "AntiKt4EMPFlowJets",
                                        "MET_Reference_AntiKt4EMTopo",]

FTAG3SlimmingHelper.AllVariables =  ["AntiKt10TruthWZJets",
                                     "BTagging_AntiKt2Track",
                                     "BTagging_AntiKt2TrackJFVtx", 
                                     "BTagging_AntiKt10TruthWZ",
                                     "BTagging_AntiKt10TruthWZJFVtx",
                                     "TruthEvents",
                                     "TruthVertices",
                                     "TruthParticles",
                                     "TruthHFWithDecayParticles",
                                     "TruthHFWithDecayVertices",
                                     "TruthMuons",
                                     "MuonTruthParticles",
                                     "MET_Truth",
                                     "MET_TruthRegions",
                                     "HLT_xAOD__TrackParticleContainer_MuonEFInfo_CombTrackParticles",
                                     "HLT_xAOD__TrackParticleContainer_MuonEFInfo_CombTrackParticles_FullScan",
                                     "HLT_xAOD__MuonContainer_MuonEFInfo"
                                      ]

FTAG3SlimmingHelper.ExtraVariables += ["BTagging_AntiKt2TrackSecVtx.-vxTrackAtVertex",
                                       "BTagging_AntiKt10TruthSecVtx.-vxTrackAtVertex"]


if BTaggingFlags.Do2019Retraining:
    FTAG3SlimmingHelper.SmartCollections += \
            ["AntiKt4EMTopoJets_BTagging201810"]

    FTAG3SlimmingHelper.AllVariables += \
            ["BTagging_AntiKt4EMTopo_201810",
                    "AntiKt4EMTopoJets_BTagging201810"]
    FTAG3SlimmingHelper.ExtraVariables += ["BTagging_AntiKt4EMTopo_201810SecVtx.-vxTrackAtVertex"]
else:
    FTAG3SlimmingHelper.SmartCollections += \
            ["AntiKt4EMTopoJets"]
    FTAG3SlimmingHelper.AllVariables += \
            ["BTagging_AntiKt4EMTopo",
             "AntiKt4EMTopoJets"]
    FTAG3SlimmingHelper.ExtraVariables += ["BTagging_AntiKt4EMTopoSecVtx.-vxTrackAtVertex"]


#----------------------------------------------------------------------
# Add needed dictionary stuff
FTAG3SlimmingHelper.AppendToDictionary = {
  "TruthHFWithDecayParticles"                  :   "xAOD::TruthParticleContainer",
  "TruthHFWithDecayParticlesAux"               :   "xAOD::TruthParticleAuxContainer",
  "TruthHFWithDecayVertices"                   :   "xAOD::TruthVertexContainer",
  "TruthHFWithDecayVerticesAux"                :   "xAOD::TruthVertexAuxContainer",
  "TruthVertices"                              :   "xAOD::TruthVertexContainer",
  "TruthVerticesAux"                           :   "xAOD::TruthVertexAuxContainer",
  "TruthParticles"                             :   "xAOD::TruthParticleContainer",
  "TruthParticlesAux"                          :   "xAOD::TruthParticleAuxContainer", 
  "TruthMuons"                                 :   "xAOD::TruthParticleContainer",
  "TruthMuonsAux"                              :   "xAOD::TruthParticleAuxContainer",
  "BTagging_AntiKt4EMTopo_201810SecVtx"        :   "xAOD::VertexContainer",
  "BTagging_AntiKt4EMTopo_201810SecVtxAux"     :   "xAOD::VertexAuxContainer",
  "BTagging_AntiKt2Track"                      :   "xAOD::BTaggingContainer"   ,
  "BTagging_AntiKt2TrackAux"                   :   "xAOD::BTaggingAuxContainer",
  "BTagging_AntiKt2TrackJFVtx"                 :   "xAOD::BTagVertexContainer"   ,
  "BTagging_AntiKt2TrackJFVtxAux"              :   "xAOD::BTagVertexAuxContainer",
  "BTagging_AntiKt2TrackSecVtx"                :   "xAOD::VertexContainer"   ,
  "BTagging_AntiKt2TrackSecVtxAux"             :   "xAOD::VertexAuxContainer",
  "BTagging_AntiKt10TruthWZ"                   :   "xAOD::BTaggingContainer"   ,
  "BTagging_AntiKt10TruthWZAux"                :   "xAOD::BTaggingAuxContainer"   ,
  "BTagging_AntiKt10TruthWZJFVtx"              :   "xAOD::BTagVertexContainer"   ,
  "BTagging_AntiKt10TruthWZJFVtxAux"           :   "xAOD::BTagVertexAuxContainer"   ,
  "BTagging_AntiKt10TruthWZSecVtx"             :   "xAOD::VertexContainer"   ,
  "BTagging_AntiKt10TruthWZSecVtxAux"          :   "xAOD::VertexAuxContainer"   ,
}
#----------------------------------------------------------------------
addJetOutputs(FTAG3SlimmingHelper,["FTAG3"])

FTAG3SlimmingHelper.IncludeMuonTriggerContent = True
FTAG3SlimmingHelper.IncludeEGammaTriggerContent = False
FTAG3SlimmingHelper.IncludeJetTriggerContent = True
FTAG3SlimmingHelper.IncludeEtMissTriggerContent = False
FTAG3SlimmingHelper.IncludeBJetTriggerContent = True

#FTAG3 TrigNav Thinning
FTAG3ThinningHelper = ThinningHelper( "FTAG3ThinningHelper" )
FTAG3ThinningHelper.TriggerChains = 'HLT_mu*_j.*_dr05|HLT_mu*_j.*_bperf_split_dr05_dz02|HLT_mu*_j.*_boffperf_split_dr05_dz02|HLT_mu*_j.*_gsc.*_dr05_dz02'
FTAG3ThinningHelper.AppendToStream( FTAG3Stream )

FTAG3SlimmingHelper.AppendContentToStream(FTAG3Stream)

