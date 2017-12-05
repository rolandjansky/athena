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
from DerivationFrameworkCore.ThinningHelper import ThinningHelper

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
from DerivationFrameworkJetEtMiss.AntiKt4EMTopoJetsCPContent import AntiKt4EMTopoJetsCPContent
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper


#====================================================================
# SKIMMING TOOLS
# (SKIMMING = REMOVING WHOLE EVENTS THAT FAIL CRITERIA)
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

FTAG4Seq = CfgMgr.AthSequencer("FTAG4Sequence");
DerivationFrameworkJob += FTAG4Seq

#====================================================================
# Basic Jet Collections 
#====================================================================

OutputJets["FTAG4"] = ["AntiKt4EMTopoJets"]

reducedJetList = ["AntiKt2PV0TrackJets",
                  "AntiKt4PV0TrackJets",
                  "AntiKt4TruthJets"]
replaceAODReducedJets(reducedJetList,FTAG4Seq,"FTAG4")

addDefaultTrimmedJets(FTAG4Seq,"FTAG4",dotruth=True)

#===================================================================
# Tag custom or pre-built jet collections
#===================================================================

FlavorTagInit(JetCollections  = ['AntiKt4EMTopoJets'],Sequencer = FTAG4Seq)

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS
#====================================================================
FTAG4Seq += CfgMgr.DerivationFramework__DerivationKernel("FTAG4Kernel",
                                                         SkimmingTools = [FTAG4StringSkimmingTool,FTAG4TriggerSkimmingTool])
                                                                       
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
FTAG4Stream.AcceptAlgs(["FTAG4Kernel"])

FTAG4SlimmingHelper = SlimmingHelper("FTAG4SlimmingHelper")

# nb: BTagging_AntiKt4EMTopo smart collection includes both AntiKt4EMTopoJets and BTagging_AntiKt4EMTopo
# container variables. Thus BTagging_AntiKt4EMTopo is needed in SmartCollections as well as AllVariables
FTAG4SlimmingHelper.SmartCollections = ["Electrons","Muons",
                                        "InDetTrackParticles",
                                        "PrimaryVertices",
                                        "AntiKt4EMTopoJets",
                                        "BTagging_AntiKt4EMTopo",
                                        "MET_Reference_AntiKt4EMTopo"]

FTAG4SlimmingHelper.AllVariables = ["AntiKt4EMTopoJets",
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
                                       "BTagging_AntiKt4EMTopoSecVtx.-vxTrackAtVertex"]

addJetOutputs(FTAG4SlimmingHelper,["FTAG4"],[],[])

#----------------------------------------------------------------------
# Add needed dictionary stuff
FTAG4SlimmingHelper.AppendToDictionary = {
  "BTagging_AntiKt2Track"                      :   "xAOD::BTaggingContainer"   ,
  "BTagging_AntiKt2TrackAux"                   :   "xAOD::BTaggingAuxContainer",
  "BTagging_AntiKt2TrackJFVtx"                 :   "xAOD::BTagVertexContainer"   ,
  "BTagging_AntiKt2TrackJFVtxAux"              :   "xAOD::BTagVertexAuxContainer",
  "BTagging_AntiKt2TrackSecVtx"                :   "xAOD::VertexContainer"   ,
  "BTagging_AntiKt2TrackSecVtxAux"             :   "xAOD::VertexAuxContainer",
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

