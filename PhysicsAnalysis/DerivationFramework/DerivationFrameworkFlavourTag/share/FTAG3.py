#====================================================================
# FTAG3.py
# It requires the reductionConf flag FTAG3 in Reco_tf.py
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
from DerivationFrameworkCore.ThinningHelper import ThinningHelper

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper

#====================================================================
# SKIMMING TOOLS
# (SKIMMING = REMOVING WHOLE EVENTS THAT FAIL CRITERIA)
#====================================================================
triggers = []
#mu-jet triggers without online b-tagging information
triggers.append("HLT_mu4_j15_dr05")
triggers.append("HLT_mu4_j25_dr05")
triggers.append("HLT_mu4_j35_dr05")
triggers.append("HLT_mu4_j55_dr05")
triggers.append("HLT_mu6_j85_dr05")
triggers.append("HLT_mu6_j110_dr05")
triggers.append("HLT_mu6_j150_dr05")
triggers.append("HLT_mu6_j175_dr05")
#mu-jet triggers with online b-tagging information
triggers.append("HLT_mu4_j15_bperf_split_dr05_dz02")
triggers.append("HLT_mu4_j25_bperf_split_dr05_dz02")
triggers.append("HLT_mu4_j35_bperf_split_dr05_dz02")
triggers.append("HLT_mu4_j55_bperf_split_dr05_dz02")
triggers.append("HLT_mu6_j85_bperf_split_dr05_dz02")
triggers.append("HLT_mu6_j110_bperf_split_dr05_dz02")
triggers.append("HLT_mu6_j150_bperf_split_dr05_dz02")
triggers.append("HLT_mu6_j175_bperf_split_dr05_dz02")

FTAG3TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(name = "FATG3TriggerSkimmingTool",
                                                                    TriggerListOR = triggers )
ToolSvc += FTAG3TriggerSkimmingTool
print FTAG3TriggerSkimmingTool

#====================================================================
# TRUTH SETUP
#====================================================================
if globalflags.DataSource()!='data':
    from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents, addHFAndDownstreamParticles
    addStandardTruthContents()
    addHFAndDownstreamParticles()   

#====================================================================
# CREATE PRIVATE SEQUENCE
#====================================================================

FTAG3Seq = CfgMgr.AthSequencer("FTAG3Sequence");
DerivationFrameworkJob += FTAG3Seq


#====================================================================
# Basic Jet Collections 
#====================================================================

OutputJets["FTAG3"] = ["AntiKt4EMTopoJets"]

reducedJetList = ["AntiKt2PV0TrackJets",
                  "AntiKt4PV0TrackJets",
                  "AntiKt10TruthWZJets",
                  "AntiKt4TruthJets"]
replaceAODReducedJets(reducedJetList,FTAG3Seq,"FTAG3")

addDefaultTrimmedJets(FTAG3Seq,"FTAG3",dotruth=True)

#===================================================================
# Tag custom or pre-built jet collections 
#===================================================================

FlavorTagInit(JetCollections  = ['AntiKt4EMTopoJets'],Sequencer = FTAG3Seq)

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS
#====================================================================

FTAG3Seq += CfgMgr.DerivationFramework__DerivationKernel("FTAG3Kernel",
                                                         SkimmingTools = [FTAG3TriggerSkimmingTool],
                                                         )


#====================================================================
# SET UP STREAM
#====================================================================

# The base name (DAOD_FTAG3 here) must match the string in
streamName = derivationFlags.WriteDAOD_FTAG3Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_FTAG3Stream )
FTAG3Stream = MSMgr.NewPoolRootStream( streamName, fileName )
FTAG3Stream.AcceptAlgs(["FTAG3Kernel"])

FTAG3SlimmingHelper = SlimmingHelper("FTAG3SlimmingHelper")

# nb: BTagging_AntiKt4EMTopo smart collection includes both AntiKt4EMTopoJets and BTagging_AntiKt4EMTopo
# container variables. Thus BTagging_AntiKt4EMTopo is needed in SmartCollections as well as AllVariables 
FTAG3SlimmingHelper.SmartCollections = ["Electrons","Muons",
                                        "InDetTrackParticles",
                                        "PrimaryVertices",
                                        "AntiKt4EMTopoJets",
                                        "BTagging_AntiKt4EMTopo",
                                        "MET_Reference_AntiKt4EMTopo",]

FTAG3SlimmingHelper.AllVariables =  ["AntiKt4EMTopoJets",
                                     "AntiKt10TruthWZJets",
                                     "BTagging_AntiKt4EMTopo",
                                     "BTagging_AntiKt4EMTopoJFVtx",
                                     "BTagging_AntiKt2Track",
                                     "BTagging_AntiKt2TrackJFVtx", 
                                     "BTagging_AntiKt10TruthWZ",
                                     "BTagging_AntiKt10TruthWZJFVtx",
                                     "TruthEvents",
                                     "TruthParticles",
                                     "MET_Truth",
                                     "MET_TruthRegions",
                                     "HLT_xAOD__TrackParticleContainer_MuonEFInfo_CombTrackParticles",
                                     "HLT_xAOD__TrackParticleContainer_MuonEFInfo_CombTrackParticles_FullScan",
                                     "HLT_xAOD__MuonContainer_MuonEFInfo"
                                     ]

FTAG3SlimmingHelper.ExtraVariables += ["BTagging_AntiKt4EMTopoSecVtx.-vxTrackAtVertex",
                                       "BTagging_AntiKt2TrackSecVtx.-vxTrackAtVertex",
                                       "BTagging_AntiKt10TruthSecVtx.-vxTrackAtVertex"]

addJetOutputs(FTAG3SlimmingHelper,["FTAG3"],[],[])

#----------------------------------------------------------------------
# Add needed dictionary stuff
FTAG3SlimmingHelper.AppendToDictionary = {
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
FTAG3ThinningHelper.TriggerChains = 'HLT_mu*_j.*_dr05|HLT_mu*_j.*_bperf_split_dr05_dz02'
FTAG3ThinningHelper.AppendToStream( FTAG3Stream )


FTAG3SlimmingHelper.AppendContentToStream(FTAG3Stream)


